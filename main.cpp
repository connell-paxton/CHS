#include "server.hpp"
#include "routes.hpp"
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

std::string exec(const std::string& command) {
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
	HTTPServer localhost("80", "logs/"+std::to_string(time(NULL))+".json");

	// set up the file reads for some of the routes
	routes::setup();

	localhost.callback = [&](sockaddr_in client, int clientfd, _json req, unsigned int n){
		std::string addr = std::to_string(client.sin_addr.s_addr)+ ':' +std::to_string(client.sin_port);
		_json res;
		res["status"] = "200 OK";
		res["type"] = "text/html";
		res["content"] = "nothing here yet, go yell at Qonnell to get things working again!\n";
		get_handler(routes::handler, req["path"])(req,res);
		send (clientfd, res);

	};
	localhost.listen(1000);
}
