all:
	clang++ main.cpp -Ijson/single_include -Ififo_map/src -o httpd -std=c++17
run: all
	./httpd
