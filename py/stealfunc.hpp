#include <strings.h>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <memory>

#define STRING_READ(file) std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
std::string exec(std::string cmd){
	auto p = popen(cmd.c_str(), "r");
	char buff[4098];
	auto rd = fread(buff, 1, 4098, p);
	buff[rd] = 0;
	pclose(p);
	return buff;
}
int server_init(int clientfd, std::string req) {
/*	std::ifstream headerfile("../header.html");
	std::string header = STRING_READ(headerfile);
	std::ifstream rawfile("../romanes_raw.html");
	std::ifstream romfile("../romanes.html");
	std::string raw = STRING_READ(rawfile);
	std::string rom = STRING_READ(romfile);
	std::ifstream comfile("../colormagic.txt");
	std::ifstream comhtmlfile("../colormagic.html");
	std::string com = STRING_READ(comfile);
	std::string comhtml = STRING_READ(comhtmlfile);
	std::ifstream discfile("../disclaimer.html");
	std::string disc = STRING_READ(discfile);
	headerfile.close();
	rawfile.close();
	romfile.close();
	comfile.close();
	comhtmlfile.close();
	discfile.close();
		// flags based off what they request
		bool _com = false;
		bool _rom = false;
		bool _fav = false;
		bool _home = false;

		std::string ftype = "text/html";

		if(req[0] == 'G') {
			std::string content = header;
			std::string status_code = "200 OK";
			if(req.substr(0,12)=="GET /fav.ico") {
				_fav = true;
				status_code = "404 ERROR NOT FOUND";
			} else {
					if(req.substr(5, 8) == "romanes ") {
						_rom = true;
						content += raw;
						content +=  rom;
					} else if(req.substr(5, 7) == "com.txt" || req.substr(5, 4) == "com ") {
						ftype = (req.substr(5,7) == "com.txt")?"text/plain":"text/html";
						_com = true;
						if(ftype == "text/plain"){
							content = com;
						} else {
							content += "<pre>" + comhtml + "<pre>";
						}
					} else {
						_home = true;
						content += disc;
						std::string opts[]  = {" ascii-art ", "humorists", "-l"};
						for(auto &i : opts) {
							content += std::string("<pre style='border-style: dotted;'>")
							+ exec("fortune "+i) + "</pre>";
						}
					}
*/			}
			std::content = "hello!";
			std::string res =
				"HTTP/1.1 200\r\n"
				"Content-Length: " + std::to_string(content.size()) + "\n"
				"Content-Type: "+ftype+"; charset=utf-8\r\n\r\n"
				+ content;
			if(clientfd > 2){
				write(clientfd, res.c_str(), res.length());
			}
/*			std::cout << "REQ:\n"<< req;
			std::cout <<
			((_rom)?
				"ROMANES EUNT DOMUS"
				:
					((_com)?
						"Arghh! Another Pirate out for GOLD! (or dragons!)"
					:
						((_home)?
						 	"A visitor to the homeland!"
						:
							((_fav)?
								"FAVICO"
							:
								"Something else entirely"
							)
						)
					)
			) << std::endl; */
	}
}

