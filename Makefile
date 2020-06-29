all: httpd

httpd: *.cpp *.hpp
	g++ main.cpp -Ijson/single_include -Ififo_map/src -o httpd -std=c++17
run: httpd
	./httpd
