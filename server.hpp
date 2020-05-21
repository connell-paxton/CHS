#include <sys/types.h>
#include <sys/socket.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <sstream>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <functional>
#include <nlohmann/json.hpp>
#include <fifo_map.hpp>
#include <fstream>

#define READ_STRING(f) std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>())

using namespace nlohmann;

// workaround to preserve order
template<class K, class V, class dummy_compare, class A>
using _fifo_map = nlohmann::fifo_map<K, V, fifo_map_compare<K>, A>;
using _json	= nlohmann::basic_json<_fifo_map>;

typedef union ip {
	unsigned int p;
	unsigned char b[4];
} ip;

_json getJSONHeaders(sockaddr_in client, std::string _header) {
	ip _ {client.sin_addr.s_addr};
	std::string addr = std::to_string(_.b[0]) + '.' + std::to_string(_.b[1]) + '.' + std::to_string(_.b[2]) + '.' + std::to_string(_.b[3]);
	std::stringstream req(_header);
	_json header;
	std::string temp;
	req >> temp;
	header["address"] = addr;
	req >> temp;
	header["path"] = temp;
	req >> temp;
	header["method"] = temp;
	req >> temp;
	header["version"] = temp;
	while(std::getline(req, temp, ':')){
		if(temp == "\r\n") break;
		std::string key = temp;
		std::getline(req, temp);
		temp.pop_back();
		header[key] = temp;
	}
	return header;
}



_json serverlog;
struct HTTPServer {
	int fd = -1;
	unsigned int n = 1;
	std::function <void(sockaddr_in, int, _json, unsigned int)> callback;
	std::ofstream logfile;

	HTTPServer(std::string port, std::string _logfile) {
		logfile.open(_logfile.c_str());
		struct addrinfo hint, *res, *i;
		std::memset(&hint, 0, sizeof(hint));
		hint.ai_family   = AF_INET;
		hint.ai_flags    = AI_PASSIVE;
		hint.ai_socktype = SOCK_STREAM;

		getaddrinfo(NULL, port.c_str(), &hint, &res);
		for(i = res; res != NULL; i= res->ai_next){
			int opt = 1;
			fd = socket(i->ai_family, i->ai_socktype, 0);
			setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
			if(fd == -1)
				continue;
			if(bind(fd, i->ai_addr, i->ai_addrlen) == 0)
				break;
		}
		if(i == NULL) {
			perror("issue with socket() or bind()");
			exit(1);
		}
	}

	void listen(int n_backlog) {
		if(::listen(fd, n_backlog) != 0) {
			perror("listen() error");
			exit(1);
		}
		struct sockaddr_in client;
		size_t addrlen = sizeof(client);
		try {
			for(;;) {
				int clientfd = accept(fd, (struct sockaddr*) &client, &addrlen);
				if(clientfd < 0) {
					perror("listen() error");
					exit(1);
				}

				char* buff = (char*)malloc(65535);
				int rcvd = recv(clientfd, buff, 65535, 0);

				if(rcvd < 0)
					write(2, "recv() failed\n", 13);
				else if (rcvd == 0)
					write(2, "Client disconnected unexpectedly.\n", 34);
				else {
					auto headers = getJSONHeaders(client, std::string(buff,rcvd));
					serverlog[std::to_string(n)] = headers;
					n++;
					if(fork() == 0) {
						callback(client, clientfd, headers, n);
						close(clientfd);
						exit(0);
					} else {
						close(clientfd);
					}
				}
			}
		}
		catch (...) {
			logfile << serverlog;
			logfile.close();
			exit(1);
		}
	}
};
