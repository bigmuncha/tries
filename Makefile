all:
	g++ -g -std=gnu++2b -DLOG_DEBUG -o main main.cpp binary_trie.h -I./helpers/

