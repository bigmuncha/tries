#include "ipcreator.h"
#include <cstdint>
#include <stdint.h>
#include <sys/types.h>
#include <rte_lpm6.h>
#include <rte_timer.h>
#define ITERATIONS (1 << 10)
#define BATCH_SIZE 100000
#define NUMBER_TBL8S                                           (1 << 16)


/* check if IPv6 address ip[] match the rule with IPv6 address ip_rule[]
 * and depth. if matched, return 0, else return -1.
 */
static inline int check_lpm6_rule(uint8_t *ip,
	const uint8_t *ip_rule, uint8_t depth)
{
	int k;
	uint8_t mask;

	for (k = 0; k < 16; k++) {
		if (depth >= 8) {
			if (ip[k] != ip_rule[k])
				return -1;
		} else if (depth > 0) {
			mask = (uint8_t)((unsigned int)(-1) << (8 - depth));
			if ((ip[k] & mask) == (ip_rule[k] & mask))
				return 0;
			else
				return -1;
		} else
			return 0;

		depth -= 8;
	}

	return 0;
}

/* check input IPv6 address ip[] with each one in rule[] and
 * output the *next_hop from the matched item in rule[] with
 * longest depth. The count of items in rule[ ] is rule_num.
 * if found that some item in rule[] is matched return 0,
 * else return -1;
 */
static int get_next_hop(uint8_t *ip, uint32_t *next_hop,
	const struct rules_tbl_entry *rule, int rule_num)
{
	int i;
	int result;
	uint8_t max_depth = 0;

	for (i = 0; i < rule_num; i++) {
		if (rule[i].depth >= max_depth) {
			result = check_lpm6_rule(ip, rule[i].ip, rule[i].depth);
			if (result == 0) {
				*next_hop = rule[i].next_hop;
				max_depth = rule[i].depth;
			}
		}
	}

	if (max_depth > 0)
		return 0;
	else
		return -1;
}

/** Number of elements in the array. */
#define	RTE_DIM(a)	(sizeof (a) / sizeof ((a)[0]))

#define  NUM_ROUTE_ENTRIES RTE_DIM(large_route_table) 

#define  NUM_IPS_ENTRIES (NUM_ROUTE_ENTRIES * 100)
static struct ips_tbl_entry large_ips_table[NUM_IPS_ENTRIES*2*2];
#include <stdlib.h>
/* the implementation of algorithm to generate large IPS table
 * at run time. if gen_expected_nex_hop is non-zero, the expected
 * next_hop of the IPv6 address of each item in IPS table is computed.
 */
static void generate_large_ips_table(int gen_expected_next_hop)
{
	uint32_t i, j, k;

	for (i = 0; i < NUM_IPS_ENTRIES; i++) {
	    for (j = 0; j < 16; j++){
			large_ips_table[i].ip[j] = lrand48();
	    }
	}

	for (k = j = 0, i = 0; i < NUM_IPS_ENTRIES; i++) {
		mask_ip6_prefix(large_ips_table[i].ip,
			large_route_table[j].ip, large_route_table[j].depth);
		k++;
		if (k == (NUM_IPS_ENTRIES*2*2 / NUM_ROUTE_ENTRIES)) {
			j++;
			k = 0;
		}
		if (j == NUM_ROUTE_ENTRIES)
			j = 0;
	}

	if (gen_expected_next_hop == 0)
		return;

	for (k = 0; k < NUM_IPS_ENTRIES*2*2; k++)
		get_next_hop(large_ips_table[k].ip,
				&(large_ips_table[k].next_hop),
				large_route_table,
				NUM_ROUTE_ENTRIES);

}


static void generate_long_ip_table(int size)
{
    for(int i = 0; i < NUM_ROUTE_ENTRIES; i++)
    {
	if(large_route_table[i].depth < 80){
	    if(large_route_table[i].depth +30 < 128)
	    {
		large_route_table[i].depth+=30;
		if(large_route_table[i].depth < 80 && large_route_table[i].depth + 30 < 128)
		{
		    large_route_table[i].depth+=30;
		}
	    }
	}
    }
}
#include <vector>
#include <rte_random.h>

#include "fix_stride_trie.h"
#define MAX_DEPTH                                                    128
#define MAX_RULES                                                1000000
#define NUMBER_TBL8S                                           ((1 << 16) * 8 )
#define MAX_NUM_TBL8S                                          (1 << 21)
#define PASS 0
#define SOCKET_ID_ANY -1 /**< Any NUMA socket. */

#define TEST_LPM_ASSERT(cond) do {                                            \
	if (!(cond)) {                                                        \
		printf("Error at line %d: \n", __LINE__);                     \
		return -1;                                                    \
	}                                                                     \
} while(0)
#include <rte_random.h>
#include <rte_common.h>


static int
test_lpm6_perf(std::vector<rules_tbl_entry> &vector_rules, std::vector<ips_tbl_entry> &vector_ips)
{
	struct rte_lpm6 *lpm = NULL;
	struct rte_lpm6_config config;
	uint64_t begin, total_time;
	unsigned i, j;
	uint32_t next_hop_add = 0xAA, next_hop_return = 0;
	int status = 0;
	int64_t count = 0;

	config.max_rules = 1000000;
	config.number_tbl8s = NUMBER_TBL8S;
	config.flags = 0;

	/* Only generate IPv6 address of each item in large IPS table,
	 * here next_hop is not needed.
	 */

        lpm = rte_lpm6_create(__func__, SOCKET_ID_ANY, &config);
	TEST_LPM_ASSERT(lpm != NULL);

	/* Measure add. */
	begin = rte_rdtsc();
	for (i =0; i < vector_rules.size(); i++) {
	    if (rte_lpm6_add(lpm, vector_rules[i].ip,
				      vector_rules[i].depth, vector_rules[i].next_hop) == 0)
		status++;
	}
	double percent = 0; 
	/* End Timer. */
	total_time = rte_rdtsc() - begin;

	printf("Unique added entries = %d\n", status);
	printf("Average LPM Add: %g cycles\n",
	       (double)total_time / vector_rules.size());

	/* Measure single Lookup */
	total_time = 0;
	count = 0;

	for (i = 0; i < ITERATIONS; i ++) {
		begin = rte_rdtsc();
		for (j = 0; j < vector_ips.size(); j ++) {
			if (rte_lpm6_lookup(lpm, vector_ips[j].ip,
					&next_hop_return) != 0)
				count++;
		}

		total_time += rte_rdtsc() - begin;

	}
	printf("Average LPM Lookup: %.1f cycles (fails = %.1f%%)\n",
	       (double)total_time / ((double)ITERATIONS * vector_ips.size()),
	       (count * 100.0) / (double)(ITERATIONS * vector_ips.size()));

	/* Measure bulk Lookup */
	total_time = 0;
	count = 0;

	uint8_t ip_batch[vector_ips.size()][16];
	int32_t next_hops[vector_ips.size()];

	for (i = 0; i < vector_ips.size();i++)
	{
	    memcpy(ip_batch[i],   vector_ips[i].ip, 16);
	    //memcpy(ip_batch[i+2],   large_ips_table[j].ip, 16);
	}
	for (i = 0; i < ITERATIONS; i ++) {
	    begin = rte_rdtsc();
	    for (j = 0; j < vector_ips.size(); j ++) {
		if (rte_lpm6_lookup(lpm, ip_batch[j],
				    &next_hops[j]) != 0)
		    count++;
	    }
	    total_time += rte_rdtsc() - begin;
	}
	printf("Average LPM Lookup: %.1f cycles (fails = %.1f%%)\n",
	       (double)total_time / ((double)ITERATIONS * vector_ips.size()),
	       (count * 100.0) / (double)(ITERATIONS * vector_ips.size()));

	total_time = 0;
	count = 0;

	for (i = 0; i < ITERATIONS; i ++) {
		/* Lookup per batch */
		begin = rte_rdtsc();
		rte_lpm6_lookup_bulk_func(lpm, ip_batch, next_hops, vector_ips.size());
		total_time += rte_rdtsc() - begin;

		for (j = 0; j < vector_ips.size(); j++)
			if (next_hops[j] != vector_ips[j].next_hop)
				count++;
	}
	printf("BULK LPM Lookup: %.1f cycles (fails = %.1f%%)\n",
	       (double)total_time / ((double)ITERATIONS * vector_ips.size()),
	       (count * 100.0) / (double)(ITERATIONS * vector_ips.size()));

	/* Delete */
	status = 0;
	begin = rte_rdtsc();

	for (i = 0; i < vector_rules.size(); i++) {
		/* rte_lpm_delete(lpm, ip, depth) */
		status += rte_lpm6_delete(lpm, vector_rules[i].ip,
				vector_rules[i].depth);
	}

	total_time += rte_rdtsc() - begin;

	printf("Average LPM Delete: %g cycles\n",
	       (double)total_time / vector_rules.size());


	rte_lpm6_delete_all(lpm);
	rte_lpm6_free(lpm);

	return 0;
}


#include <multibit_dpdk.h>
#include <vector>
static int
test_multibit_perf(std::vector<rules_tbl_entry> &vector_rules, std::vector<ips_tbl_entry> &vector_ips)
{
	struct multibit_rte_lpm6 *lpm = NULL;
	struct multibit_rte_lpm6_config config;
	uint64_t begin, total_time;
	unsigned i, j;
	uint32_t next_hop_add = 0xAA, next_hop_return = 0;
	int status = 0;
	int64_t count = 0;

	config.max_rules = 1000000;
	config.number_tbl8s = 1 << 19 ;
	config.flags = 0;

	rte_srand(rte_rdtsc());

	/* Only generate IPv6 address of each item in large IPS table,
	 * here next_hop is not needed.
	 */

        lpm = multibit_rte_lpm6_create(__func__, SOCKET_ID_ANY, &config);
	TEST_LPM_ASSERT(lpm != NULL);

	/* Measure add. */
	begin = rte_rdtsc();
	for (i =0; i < vector_rules.size(); i++) {
	    if (multibit_rte_lpm6_add(lpm, vector_rules[i].ip,
				      vector_rules[i].depth, vector_rules[i].next_hop) == 0)
		status++;
	}
	/* End Timer. */
	total_time = rte_rdtsc() - begin;

	printf("Unique added entries = %d\n", status);
	printf("Average LPM Add: %g cycles\n",
	       (double)total_time / vector_rules.size());

	/* Measure single Lookup */
	total_time = 0;
	count = 0;

	for (i = 0; i < ITERATIONS; i ++) {
		begin = rte_rdtsc();
		for (j = 0; j < vector_ips.size(); j ++) {
			if (multibit_rte_lpm6_lookup(lpm, vector_ips[j].ip,
					&next_hop_return) != 0)
				count++;
		}

		total_time += rte_rdtsc() - begin;

	}
	printf("Average LPM Lookup: %.1f cycles (fails = %.1f%%)\n",
	       (double)total_time / ((double)ITERATIONS * vector_ips.size()),
	       (count * 100.0) / (double)(ITERATIONS * vector_ips.size()));

	/* Measure bulk Lookup */
	total_time = 0;
	count = 0;

	 uint8_t ip_batch[vector_ips.size()][16];
	 int32_t next_hops[vector_ips.size()];

	for (i = 0; i < vector_ips.size();i++)
	{
	    memcpy(ip_batch[i],   vector_ips[i].ip, 16);
	    //memcpy(ip_batch[i+2],   large_ips_table[j].ip, 16);
	}

	for (i = 0; i < ITERATIONS; i ++) {
		/* Lookup per batch */
		begin = rte_rdtsc();
		multibit_rte_lpm6_lookup_bulk_func(lpm, ip_batch, next_hops, vector_ips.size());
		total_time += rte_rdtsc() - begin;

		for (j = 0; j < vector_ips.size(); j++)
		    if (next_hops[j] != vector_ips[j].next_hop)
				count++;
	}
	printf("BULK LPM Lookup: %.1f cycles (fails = %.1f%%)\n",
	       (double)total_time / ((double)ITERATIONS * vector_ips.size()),
	       (count * 100.0) / (double)(ITERATIONS * vector_ips.size()));

	memset(next_hops, 0, sizeof(next_hops));
	/* Measure bulk Lookup */
	total_time = 0;
	count = 0;

	for (i = 0; i < ITERATIONS; i ++) {
		/* Lookup per batch */
		begin = rte_rdtsc();
		multibit_rte_lpm6_lookup_bulk_func_branch(lpm, ip_batch, next_hops, vector_ips.size());
		total_time += rte_rdtsc() - begin;

		for (j = 0; j < vector_ips.size(); j++)
			if (next_hops[j] != vector_ips[j].next_hop)
				count++;
	}
	printf("BULK BRANCH LPM Lookup: %.1f cycles (fails = %.1f%%)\n",
	       (double)total_time / ((double)ITERATIONS * vector_ips.size()),
	       (count * 100.0) / (double)(ITERATIONS * vector_ips.size()));

		/* Measure bulk Lookup */
	total_time = 0;
	count = 0;

	memset(next_hops, 0, sizeof(next_hops));
	for (i = 0; i < ITERATIONS; i ++) {
		/* Lookup per batch */
		begin = rte_rdtsc();
		multibit_rte_lpm6_lookup_bulk_func_prefetch(lpm, ip_batch, next_hops, vector_ips.size());
		total_time += rte_rdtsc() - begin;

		for (j = 0; j < vector_ips.size(); j++)
		    if (next_hops[j] != vector_ips[j].next_hop)
				count++;
	}
	printf("BULK Prefetch LPM Lookup: %.1f cycles (fails = %.1f%%)\n",
	       (double)total_time / ((double)ITERATIONS * vector_ips.size()),
	       (count * 100.0) / (double)(ITERATIONS * vector_ips.size()));

	/* Measure bulk Lookup */
	total_time = 0;
	count = 0;

	memset(next_hops, 0, sizeof(next_hops));
	for (i = 0; i < ITERATIONS; i ++) {
		/* Lookup per batch */
		begin = rte_rdtsc();
		multibit_rte_lpm6_lookup_bulk_func_cache(lpm, ip_batch, next_hops, vector_ips.size());
		total_time += rte_rdtsc() - begin;

		for (j = 0; j < vector_ips.size(); j++)
		    if (next_hops[j] != vector_ips[j].next_hop)
				count++;
	}
	printf("BULK Cache LPM Lookup: %.1f cycles (fails = %.1f%%)\n",
	       (double)total_time / ((double)ITERATIONS * vector_ips.size()),
	       (count * 100.0) / (double)(ITERATIONS * vector_ips.size()));

	/* Delete */
	status = 0;
	begin = rte_rdtsc();

	for (i = 0; i < vector_rules.size(); i++) {
		/* rte_lpm_delete(lpm, ip, depth) */
		status += multibit_rte_lpm6_delete(lpm, vector_rules[i].ip,
				vector_rules[i].depth);
	}

	total_time += rte_rdtsc() - begin;

	printf("Average LPM Delete: %g cycles\n",
	       (double)total_time / vector_rules.size());

	multibit_rte_lpm6_delete_all(lpm);
	multibit_rte_lpm6_free(lpm);

	return 0;
}



/*
 * Creates two different LPM tables. Tries to create a third one with the same
 * name as the first one and expects the create function to return the same
 * pointer.
 */
static int32_t test1(void)
{
	struct rte_lpm6 *lpm1 = NULL, *lpm2 = NULL, *lpm3 = NULL;
	struct rte_lpm6_config config;

	config.max_rules = MAX_RULES;
	config.number_tbl8s = NUMBER_TBL8S;
	config.flags = 0;

	/* rte_lpm6_create: lpm name == LPM1 */
	lpm1 = rte_lpm6_create("LPM1", SOCKET_ID_ANY, &config);
	TEST_LPM_ASSERT(lpm1 != NULL);

	/* rte_lpm6_create: lpm name == LPM2 */
	lpm2 = rte_lpm6_create("LPM2", SOCKET_ID_ANY, &config);
	TEST_LPM_ASSERT(lpm2 != NULL);

	/* /\* rte_lpm6_create: lpm name == LPM2 *\/ */
	/* lpm3 = rte_lpm6_create("LPM1", SOCKET_ID_ANY, &config); */
	/* TEST_LPM_ASSERT(lpm3 == NULL); */

	rte_lpm6_free(lpm1);
	rte_lpm6_free(lpm2);

	return PASS;
}
#include <multibit_dpdk.h>
//#include <OMAR_dpdk_lpm6.h>
/*
 * Creates two different LPM tables. Tries to create a third one with the same
 * name as the first one and expects the create function to return the same
 * pointer.
 */
static int32_t test2(void)
{
        struct multibit_rte_lpm6 *lpm1 = NULL;
	struct multibit_rte_lpm6_config config;

	config.max_rules = MAX_RULES;
	config.number_tbl8s = NUMBER_TBL8S;
	config.flags = 0;

	/* rte_lpm6_create: lpm name == LPM1 */
	lpm1 = multibit_rte_lpm6_create("LPM1", SOCKET_ID_ANY, &config);
	TEST_LPM_ASSERT(lpm1 != NULL);
	uint8_t routes[][16]=
	    {
		{0xa,0xb,0xc,0x0,0xf,0,0,0,0,0,0,0,0,0,0,0},
		{0xa,0xb,0xc,0x0,0xf,0,0,0,0,0,0,0,0,0,0,0},
		{0xa,0xb,0xc,0x0,0xf,0xff,0,0,0,0,0,0,0,0,0,0},
		{0xc,0xd,0xc,0xd,0xf,0xff,0,0,0,0,0,0,0,0,0,0},
		{0xc,0xc,0xc,0xd,0xf,0xff,0,0,0,0,0,0,0,0,0,0}
	    };

        //rte_lpm6_add(lpm1, routes[0], 42, 10);
//	rte_lpm6_add(lpm1, routes[0], 43, 20);

	multibit_rte_lpm6_add(lpm1, routes[0], 44, 90);
	multibit_rte_lpm6_add(lpm1, routes[0], 45, 80);
	/* rte_lpm6_add(lpm1, routes[0], 27, 120); */
	/* rte_lpm6_add(lpm1, routes[0], 28, 120); */
	//rte_lpm6_add(lpm1, routes[4], 49, 120);
	/* rte_lpm6_add(lpm1, routes[0], 46, 70); */
	/* rte_lpm6_add(lpm1, routes[0], 47, 60); */
	/* rte_lpm6_add(lpm1, routes[0], 48, 50); */
	/* rte_lpm6_add(lpm1, routes[3], 48, 50); */
	/* rte_lpm6_add(lpm1, routes[4], 48, 50); */
	/* rte_lpm6_add(lpm1, routes[2], 47, 50); */
	/* rte_lpm6_add(lpm1, routes[2], 18, 18); */
	uint8_t lookups[2][16]=
	    {
		{0xa,0xb,0xc,0x0,0xf,8,0,0,0,0,0,0,0,0,0,0},
		{0xa,0xb,0xc,0x0,0xf,4,0,0,0,0,0,0,0,0,0,0},
	    };
	uint32_t nexthop[2];
	multibit_rte_lpm6_lookup(lpm1, lookups[0], &nexthop[0]);
	multibit_rte_lpm6_lookup(lpm1, lookups[1], &nexthop[1]);
	std::cout <<"Lookup first: \n"
		  << getStrIpv6(lookups[0]) <<'\n'
		  << nexthop[0] <<'\n'
		  << "Lookup second: \n"
		  << getStrIpv6(lookups[1]) <<'\n'
		  << nexthop[1] <<'\n';

	multibit_rte_lpm6_free(lpm1);

	return PASS;
}


static inline uint8_t
bits_in_nh(uint8_t nh_sz)
{
	return 8 * (1 << nh_sz);
}

static inline uint64_t
get_max_nh(uint8_t nh_sz)
{
	return ((1ULL << (bits_in_nh(nh_sz) - 1)) - 1);
}
#include <fib6/OMAR_dpdk_fib6.h>
#define TEST_FIB_ASSERT(cond) do {				\
	if (!(cond)) {						\
		printf("Error at line %d:\n", __LINE__);	\
		return -1;					\
	}							\
} while (0)

static int
tests_fib6_perf(std::vector<rules_tbl_entry> &vector_rules, std::vector<ips_tbl_entry> &vector_ips)
{
	struct rte_fib6 *fib = NULL;
	struct rte_fib6_conf conf;
	uint64_t begin, total_time;
	unsigned int i, j;
	uint64_t next_hop_add;
	int status = 0;
	int64_t count = 0;

	conf.type = RTE_FIB6_TRIE;
	conf.default_nh = 0;
	conf.max_routes = 20000;
	conf.rib_ext_sz = 0;
	conf.trie.nh_sz = RTE_FIB6_TRIE_4B;
	conf.trie.num_tbl8 = RTE_MIN(get_max_nh(conf.trie.nh_sz), 2000U);

	rte_srand(rte_rdtsc());
	fib = rte_fib6_create(__func__, SOCKET_ID_ANY, &conf);
	TEST_FIB_ASSERT(fib != NULL);

	begin = rte_rdtsc();
	for (i =0; i < vector_rules.size(); i++) {
	    if (rte_fib6_add(fib, vector_rules[i].ip,
				      vector_rules[i].depth, vector_rules[i].next_hop) == 0)
		status++;
	}
	total_time = rte_rdtsc() - begin;

	printf("Unique added entrsies = %d\n", status);
	printf("Average LPM Add: %g cycles\n",
	       (double)total_time / vector_rules.size());

	/* Measure single Lookup */
	total_time = 0;
	count = 0;

	uint8_t ip_batch[vector_ips.size()][16];
	int64_t next_hops[vector_ips.size()];

	for (i = 0; i < vector_ips.size();i++)
	{
	    memcpy(ip_batch[i],   vector_ips[i].ip, 16);
	    //memcpy(ip_batch[i+2],   large_ips_table[j].ip, 16);
	}

/* Measure single Lookup */
	total_time = 0;
	count = 0;

	for (i = 0; i < ITERATIONS; i ++) {
		/* Lookup per batch */
		begin = rte_rdtsc();
		rte_fib6_lookup_bulk(fib, ip_batch, next_hops, vector_ips.size());
		total_time += rte_rdtsc() - begin;

		for (j = 0; j < vector_ips.size(); j++)
		    if (next_hops[j] != vector_ips[j].next_hop)
				count++;
	}
	printf("BULK LPM Lookup: %.1f cycles (fails = %.1f%%)\n",
	       (double)total_time / ((double)ITERATIONS * vector_ips.size()),
	       (count * 100.0) / (double)(ITERATIONS * vector_ips.size()));

	/* Delete */
	status = 0;
	begin = rte_rdtsc();

	for (i = 0; i < vector_rules.size(); i++) {
		/* rte_lpm_delete(lpm, ip, depth) */
		status += rte_fib6_delete(fib, vector_rules[i].ip,
				vector_rules[i].depth);
	}
	total_time = rte_rdtsc() - begin;

	printf("Average FIB Delete: %g cycles\n",
			(double)total_time / NUM_ROUTE_ENTRIES);

	rte_fib6_free(fib);
	
	return 0;
}
