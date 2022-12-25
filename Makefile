all:
	g++ -g -std=gnu++2b -DLOG_ERROR -o main main.cpp b_trie_lpm.h patricia_trie.h -I./helpers/ -I./test/

