all:
	g++ -g -std=gnu++2b -DLOG_DEBUG binary_tree.cpp -o binary_tree -I./helpers/
	g++ -g -std=gnu++2b -DLOG_DEBUG binary_trie.cpp -o binary_trie -I./helpers/
