all:
	g++ -g -std=gnu++2b -DLOG_DEBUG binary_tree.cpp -o binary_tree ./helpers/logger.h
	g++ -g -std=gnu++2b -DLOG_DEBUG binary_trie.cpp -o binary_trie ./helpers/logger.h
