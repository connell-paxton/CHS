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
#include <cstdlib>
#include <signal.h>

#define STRING_READ(f) std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>())

std::ofstream logfile;

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
	header["method"] = temp;
	req >> temp;
	header["path"] = temp;
	req >> temp;
	header["version"] = temp;
	while(std::getline(req, temp, ':')){
		std::string key = temp;
		if(temp == "\r\n") break;
		std::getline(req, temp);
		if(key[0] == '\r') {
			key = key.substr(2, temp.size()-2);
		}
		temp = temp.substr(1, temp.size()-2);
		header[key] = temp;
	}
	return header;
}


_json serverlog;

void exit_handler(int s) {
	logfile << serverlog.dump(4);
	logfile.close();
	exit(0);
}

struct HTTPServer {
	int fd = -1;
	unsigned int n = 1;
	std::function <void(sockaddr_in, int, _json, unsigned int)> callback;

	HTTPServer(std::string port, std::string _logfile) {
		struct sigaction sigIntHandler;
		sigIntHandler.sa_handler = exit_handler;
		sigemptyset(&sigIntHandler.sa_mask);
		sigIntHandler.sa_flags = 0;
		sigaction(SIGINT, &sigIntHandler, NULL);

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
		socklen_t addrlen = sizeof(client);

		for(;;) {
			int clientfd = accept(fd, (struct sockaddr*) &client, &addrlen);
			if(clientfd < 0) {
				perror("listen() error");
				exit(1);
			}

			char buff[8*1024];
			int rcvd = recv(clientfd, buff, 8*1024, 0);
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
};

