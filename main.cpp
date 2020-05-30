#include "server.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <nlohmann/json.hpp>
#include <fifo_map.hpp>
#include <ctime>
//#include <chaiscript/chaiscript>

// TODO: add chaiscripting

//chaiscript::ChaiScript chai;

using namespace nlohmann;

// workaround to preserver order
template<class K, class V, class dummy_compare, class A>
using _fifo_map = nlohmann::fifo_map<K, V, fifo_map_compare<K>, A>;
using _json   = nlohmann::basic_json<_fifo_map>;

std::string exec(std::string command) {
	auto p = popen(command.c_str(), "r");
	std::string ret;
	signed char c;
	while((c = fgetc(p)) != EOF){
		ret += c;
	}
	pclose(p);
	return ret;
};

void send(int fd, const _json& _res) {
	std::string res =
		"HTTP/1.1 " + std::string(_res["status"]) + "\r\n"
		"Content-Length: "+ std::to_string(std::string(_res["content"]).length()) + "\r\n"
		"Content-Type: " + std::string(_res["type"]) + "; charset=utf-8\r\n\r\n"
		+ std::string(_res["content"]);
	write(fd, res.c_str(), res.length());
}

auto get_handler = [](auto handler, auto handle){
	if(handler.find(handle) == handler.end()) {
		return handler["default"];
	} else {
		return handler[handle];
	}
};

int main() {
	std::ifstream _pub("qonnell.key");
	std::string pub = STRING_READ(_pub);
	std::ifstream _head("resources/header.html");
	std::string head = STRING_READ(_head);
	_head.close();
	std::ifstream _jim("resources/jim.html");
	std::string jim = STRING_READ(_jim);
	_jim.close();
	std::ifstream _src2("main.cpp");
	std::ifstream _src1("server.hpp");
	std::string src = "<h2>server.hpp</h2><hr><pre id=\"src\" style=\"border-style:dot-cut\"><" "xmp style=\"border-style:dot-cut; margin: 2 \">\n" + STRING_READ(_src1) + "</xm" "p>\n\n<h2>main.cpp</h1><hr><xm" "p>" + STRING_READ(_src2) + "</" "xmp></pre>";
	_src1.close();
	_src2.close();
	std::ifstream _romanes("resources/romanes_raw.html");
	std::string romanes = STRING_READ(_romanes);
	_romanes.close();
	HTTPServer localhost("80", "logs/"+std::to_string(time(NULL))+".json");
	std::map<std::string, std::function<void(const _json&, _json&)>> handler = {
		{"/" , [&head](const auto& req, auto& res) {
			res["content"] = head + "<h2>Welcome</h2>";
			res["type"] = "text/html";
		}},
		{"default" , [&head](const auto& req, auto& res){
			res["content"] = head + "<h3>404 File Not Found</h3><i>Sorry, your princess is in another village</i>";
			res["status"] = "404 FILE NOT FOUND";
		}},
		{"/logs" , [](const auto& req, auto& res){
			res["type"] = "application/json";
			res["content"] = serverlog.dump(4);
		}},
		{"/src" , [&head, &src](const auto& req, auto& res){
			res["content"] =  head + src;
			//res["type"] = "text/plain";
		}},
		{"/hello", [](const auto& req, auto& res){
			res.merge_patch(json::parse(exec("cgi-bin/helloworld")));
		}},
		{"/romanes" , [&head, &romanes](const auto& req, auto& res){
			res["content"] = head + romanes;
			res["type"] = "text/html";
		}},
		{"/pub", [&pub](const auto& req, auto& res){
			res["content"] = pub;
			res["type"] = "text/plain";
		}},
		{"/chem", [](const auto& req, auto& res){
			res["content"] = "<html><head><style>img{height: 200px;width: 200px;}</style></head><body><center>" + exec("cgi-bin/process.pl resources/chem.txt") + "</body></html>";
			res["type"] = "text/html";
		}},
		{"/chem.txt", [](const auto& req, auto& res) {
			res["content"] = exec("cat resources/chem.txt");
			res["type"] = "text/plain";
		}},
		{"/jim", [&jim](const auto& req, auto& res) {
			res["content"] = jim;
			res["type"] = "text/html";
		}}
	};

	localhost.callback = [&](sockaddr_in client, int clientfd, _json req, unsigned int n){
		std::string addr = std::to_string(client.sin_addr.s_addr)+ ':' +std::to_string(client.sin_port);
		_json res;
		res["status"] = "200 OK";
		res["type"] = "text/html";
		res["content"] = "nothing here yet, go yell at Qonnell to get things working again!\n";
		get_handler(handler, req["path"])(req,res);
		send (clientfd, res);

	};
	localhost.listen(1000);
}
