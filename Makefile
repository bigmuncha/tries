all:
	g++ -g -std=gnu++2b  -DRTE_MAX_LCORE=8 -DLOG_DEBUG -o main main.cpp -I./tries/ -I./helpers/ -I./test/

