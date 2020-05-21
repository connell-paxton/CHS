#include "server.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <nlohmann/json.hpp>
#include <fifo_map.hpp>
#include <ctime>

// TODO: add chaiscripting

using namespace nlohmann;

// workaround to preserver order
template<class K, class V, class dummy_compare, class A>
using _fifo_map = nlohmann::fifo_map<K, V, fifo_map_compare<K>, A>;
using _json   = nlohmann::basic_json<_fifo_map>;

std::string exec(std::string command) {
	auto p = popen(command.c_str(), "r");
	char buffer[4096];
	int len = fread(buffer, 4096, 4096, p);
	pclose(p);
	return std::string(buffer, len);
};

void send(int fd, const _json& _res) {
	std::string res =
		"HTTP/1.1" + std::string(_res["status"]) + "\r\n"
		"Content-Length: "+ std::to_string(std::string(_res["content"]).length()) + "\r\n"
		"Content-Type: " + std::string(_res["type"]) + "\r\n\r\n"
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
	std::ifstream _head("resources/header.html");
	std::string head = STRING_READ(_head);
	_head.close();
	std::ifstream _src("main.cpp");
	std::string src = STRING_READ(_src);
	_src.close();
	std::ifstream _romanes("resources/romanes_raw.html");
	std::string romanes = STRING_READ(_romanes);
	_romanes.close();
	HTTPServer localhost("80", "logs/"+std::to_string(time(NULL))+".json");
	std::map<std::string, std::function<void(const _json&, _json&)>> handler = {
		{"/" , [&head](const auto& req, auto& res) {
			res["content"] = head + "<h2>Welcome</h2>";
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
			auto script = R"(<script>
					document.getElementById('src').innerHTML = (document.getElementById('src').innerHTML.replace(/&/g, '&amp;').replace(/>/g, '&gt;').replace(/</g, '&lt;'))
					;</script>)";
			res["content"] = script + head + "<pre><code id=\"src\">" +  src + "</code></pre>";
		}},
		{"/romanes" , [&head, &romanes](const auto& req, auto& res){
			res["content"] = head + romanes;
		}},
	};

	localhost.callback = [&](sockaddr_in client, int clientfd, _json req, unsigned int n){
		std::string addr = std::to_string(client.sin_addr.s_addr)+ ':' +std::to_string(client.sin_port);
		_json res;
		res["status"] = "200 OK";
		res["type"] = "text/html";
		res["content"] = "nothing here yet, go yell at Qonnell to get things working again!";
		get_handler(handler, req["path"])(req,res);
		send (clientfd, res);

	};
	localhost.listen(1000);
}
