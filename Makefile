PKGCONF ?= pkg-config

multibit.o: tries/multibit_dpdk.c
	g++-11  -c tries/multibit_dpdk.c -o multibit.o -DLOG_DEBUG $(shell $(PKGCONF) --libs libdpdk) -fpermissive  -march=native -I./helpers -g -O2

lpm6.o: tries/OMAR_dpdk_lpm6.c
	g++-11  -c tries/OMAR_dpdk_lpm6.c -o lpm6.o -DLOG_DEBUG $(shell $(PKGCONF) --libs libdpdk) -fpermissive  -march=native -I./helpers -g -O2

# trie_avx512.o: tries/fib6/trie_avx512.c
# 	g++ -c tries/fib6/trie_avx512.c -o trie_avx512.o $(shell $(PKGCONF) --libs libdpdk) -fpermissive -mavx512f

trie.o: tries/fib6/trie.c
	g++-11 -c tries/fib6/trie.c -o trie.o $(shell $(PKGCONF) --libs libdpdk) -fpermissive -march=native -g -O2 

fib6.o: tries/fib6/OMAR_dpdk_fib6.c
	g++-11 -c tries/fib6/OMAR_dpdk_fib6.c -o fib6.o $(shell $(PKGCONF) --libs libdpdk) -fpermissive -march=native  -I./helpers -g -O2 -fsanitize=undefined

all: lpm6.o   multibit.o fib6.o trie.o
	g++-11 -std=gnu++2b  -DRTE_MAX_LCORE=8 -DLOG_DEBUG -o main main.cpp lpm6.o multibit.o  fib6.o trie.o  -I./tries/fib6 -I./tries/ -I./helpers/ -I./test/  $(shell $(PKGCONF) --libs libdpdk) -fpermissive   -march=native -g -O2 -fsanitize=undefined

