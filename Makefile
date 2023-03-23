all:
	g++ -g -std=gnu++2b -DLOG_DEBUG -o main main.cpp -I./tries/ -I./helpers/ -I./test/

