#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct fstnode FSTnode;
typedef struct n_entry entry;
typedef struct ipv4_prefix ipv4_pfx;

struct fstnode{
	entry *entries;
};

struct n_entry{
	int pfx[32];
	int next_hop[4];
	FSTnode *child;
};

struct ipv4_prefix {
	int pfx[32];
	int next_hop[4];
	uint8_t netmask;
};
