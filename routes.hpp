#pragma once
#include <string>
#include <fstream>



std::string exec(const std::string& command);

/* file contents */
namespace routes {
	std::string head;
	std::string pub;
	std::string src;
	std::string jim;
	std::string romanes;

	void setup() {
		std::ifstream _head("resources/header.html");
		head = STRING_READ(_head);
		_head.close();
		std::ifstream _pub("qonnell.key");
		pub = STRING_READ(_pub);
		_pub.close();
		std::ifstream _src("routes.hpp"); /* hey look, you can see this! */
		src = STRING_READ(_src);
		_src.close();
		std::ifstream _jim("resources/jim.html");
		jim = STRING_READ(_jim);
		_jim.close();
		std::ifstream _romanes();
	}

	const std::map<std::string, std::function<void(const _json&, _json&)>> handler = {
	{"/", [](const auto& req, auto& res) {
		res["content"] = head + R"(
		<marquee behavior="alternate" scrolldelay="70" direction="right" width="700">
  			<marquee scrolldelay="70" behavior="alternate" direction="down">
				<h1>
					hello
				</h1>
			</marquee>
		</marquee>
		)";
		res["type"] = "text/html";
	}},
	{"default", [](const auto& req, auto& res){
		res["content"] = head + "<h3>404 File Not Found</h3><i>Sorry, your princess is in another village</i>";
		res["status"] = "404 FILE NOT FOUND";
	}},
	{"/log", [](const auto& req, auto& res){
		res["type"] = "application/json";
		res["content"] = serverlog.dump(4);
	}},
	{"/logs", [](const auto& _, auto& res){
		res["type"] = "text/plain";
		res["content"] = exec("cat logs/*");
	}},
	{"/src" , [](const auto& req, auto& res){
		//res["content"] =  head + src;
		res["type"] = "text/plain";
		res["content"] = src;
	}},
	{"/hello", [](const auto& req, auto& res){
		res.merge_patch(json::parse(exec("cgi-bin/helloworld")));
	}},
	{"/romanes", [](const auto& req, auto& res){
		res["content"] = head + romanes;
		res["type"] = "text/html";
	}},
	{"/pub", [](const auto& req, auto& res){
		res["content"] = pub;
		res["type"] = "text/plain";
	}},
	{"/chem", [](const auto& req, auto& res){
		res["content"] = "<html><head><style>img{height: 200px;width: 200px;}</style></head><body><center>" + exec("cgi-bin/process.pl resources/chem.txt") + "</body></html>";
		res["type"] = "text/html";
	}},
	{"/chem.txt", [](const auto& req, auto& res) {
		res["content"] = ::exec("cat resources/chem.txt");
		res["type"] = "text/plain";
	}},
	{"/jim", [](const auto& req, auto& res) {
		res["content"] = jim;
		res["type"] = "text/html";
	}}};
}

