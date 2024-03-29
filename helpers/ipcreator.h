#pragma once
#include <iostream>
#include <cstdint>
#include <array>
#include <bitset>
#include <sstream>
#include <span>
#include <logger.h>
#include <cstring>
#include "ipsaver.h"
#include <rte_random.h>
#include <rte_timer.h>
#define FIRST_BYTE_OFFSET 31
//#define ROOT_FLAG 3

#define CHECK_BIT(num, bit)  ((num) & (bit))

struct IPv4
{
    static uint32_t getNet(uint32_t net, uint32_t mask)
	{
	    return net & mask;
	}
    static std::string printnum(uint32_t num)
	{
	    std::stringstream ss;
	    ss << std::bitset<32>(num);
	    return ss.str();
	}
    static std::string printnumWithDots(uint32_t num)
	{
	    std::stringstream ss;
	    uint8_t* arr;
	    arr = (uint8_t*)&num;
	    ss << std::bitset<8>(arr[3]) << "."
		<< std::bitset<8>(arr[2]) << "."
		<< std::bitset<8>(arr[1]) << "."
		<< std::bitset<8>(arr[0]) ;
	    return ss.str();

	}
    static std::string printnum(uint8_t* num)
	{
	    std::stringstream ss;
	    uint8_t* arr = num;
	    ss << std::bitset<8>(arr[3])
		<< std::bitset<8>(arr[2])
		<< std::bitset<8>(arr[1])
	       << std::bitset<8>(arr[0]);
	    return ss.str();
	}
    static std::string printnumWithDots(uint8_t* num)
	{
	    std::stringstream ss;
	    uint8_t* arr = num;
	    ss << std::bitset<8>(arr[0]) << "."
		<< std::bitset<8>(arr[1]) << "."
		<< std::bitset<8>(arr[2]) << "."
		<< std::bitset<8>(arr[3]) << ".";
	    return ss.str();

	}

    IPv4(uint32_t num)
	{
	    u.num = num;
	}
    IPv4(std::array<uint8_t,sizeof(uint32_t)> arr)
	{// Litle andian style
	    u.a = arr[3];
	    u.b = arr[2];
	    u.c = arr[1];
	    u.d = arr[0];
	}
    IPv4(uint8_t n0, uint8_t n1, uint8_t n2, uint8_t n3)
	{
	    u.a = n3;
	    u.b = n2;
	    u.c = n1;
	    u.d = n0;
	}
    std::string getWithDots()
	{
	    std::stringstream ss;
	    ss << std::bitset<8>(u.d) << "."
		<< std::bitset<8>(u.c) << "."
		<< std::bitset<8>(u.b) << "."
		<< std::bitset<8>(u.a) ;
	    return ss.str();
	}
    std::string getWithoutDots()
	{
	    std::stringstream ss;
	    ss << std::bitset<32>(u.num);
	    return ss.str();
	}
    union
    {
	unsigned int num;
	struct
	{
	    uint8_t a;
	    uint8_t b;
	    uint8_t c;
	    uint8_t d;
	};
    }u;
};

struct ipv6
{
    bool operator==(struct ipv6 cmp)
	{
	    if(std::memcmp(cmp.buff, buff, sizeof(buff)) == 0)
		return true;
	    return false;
	}
    bool operator==(char* cmp)
	{
	    if(std::memcmp(cmp, buff, sizeof(buff)) == 0)
		return true;
	    return false;
	}

    uint8_t buff[16];
};
#include <memory>

static ipv6 applyMask( uint8_t const  *arr, int mask)
{
    struct ipv6 ret_val;
    uint8_t * buff = ret_val.buff;
    for(int i =0; i < 16; i++)
    {
	if(mask >=8)
	{
	    buff[i] = arr[i];
	    mask-=8;
	}
	else if(mask < 8 && mask != 0)
	{
	    buff[i] = arr[i] & (0xff << (8 - mask));
	    mask = 0;
	}
	else
	{
	    buff[i] = 0;
	}
    }
    return ret_val;
}

static ipv6 show_mask( int mask)
{
    struct ipv6 ret_val;
    uint8_t * buff = ret_val.buff;
    std::memset(buff,0, sizeof(ret_val.buff));
    for(int i =0; i < 16; i++)
    {
	if(mask >=8)
	{
	    buff[i] = 0xff;
	    mask-=8;
	}
	else if(mask < 8 && mask != 0)
	{
	    buff[i] =(0xff << (8 - mask));
	    mask = 0;
	}
	else
	{
	    buff[i] = 0;
	}
    }
    return ret_val;
}

static std::string getStrIpv6(ipv6 ip)
{
    std::stringstream str;
    for(int i=0; i < 16; i++)
    {
	str << std::bitset<8>(ip.buff[i]) << ".";
    }
    return str.str();
}
static std::string getStrIpv6Hex(ipv6 ip)
{
    std::stringstream str;
    for(int i=0; i < 16; i++)
    {
	str << std::hex << (int)(ip.buff[i]) << ":";
    }
    return str.str();
}

static std::string getStrIpv6(u_char *ip)
{
    std::stringstream str;
    for(int i=0; i < 16; i++)
    {
	str << std::bitset<8>(ip[i]) << ".";
    }
    return str.str();
}

static std::string getStrIpv6Hex(u_char *ip)
{
    std::stringstream str;
    for(int i=0; i < 16; i++)
    {
	str << std::hex << (int)(ip[i]) << ":";
    }
    return str.str();
}
static std::string getStrIpv6Dec(u_char *ip)
{
    std::stringstream str;
    for(int i=0; i < 16; i++)
    {
	str << (int)(ip[i]) << ":";
    }
    return str.str();
}

struct ipv6_addr
{
    uint64_t first;
    uint64_t second;
};

//char [16] omar ()

static void show_nets([[maybe_unused]]uint8_t mask)
{
    std::array<uint8_t, 4> arr{101, 210,10,1};
    for(int i =0; arr[3] < 254; i++)
    {
	IPv4 net(arr);
	uint32_t nets = reinterpret_cast<uint32_t&>(net);
	LLOG_DEBUG_C() <<std::hex << nets <<" " << IPv4::printnumWithDots(IPv4::getNet(nets,0xffff0000)) << '\n';
	arr[3]++;
    }
}

static inline void mask_ip6_prefix(uint8_t *ip_out,
	const uint8_t *ip_in, uint8_t depth)
{
	int k;
	uint8_t mask_in, mask_out;

	for (k = 0; k < 16; k++) {
		if (depth >= 8)
			ip_out[k] = ip_in[k];
		else if (depth > 0) {
			mask_in = (uint8_t)((unsigned int)(-1) << (8 - depth));
			mask_out = ~mask_in;
			ip_out[k] = (ip_in[k] & mask_in)
					| (ip_out[k] & mask_out);
		} else
			return;

		depth -= 8;
	}
}

#include <vector>

template <typename PREFIX>
static void generate_long_ip_table_vec(std::vector<PREFIX> &vec, int num, char average_len, PREFIX* base, int base_max)
{
    for(int i = 0,j =0; i < num; i++, j++)
    {
	if(j >= base_max)
	    j=0;
	auto temp = lrand48();
	PREFIX rule = base[j];
	rule.depth = temp % 128;
	rule.ip[temp%16] = temp %255;
	vec.push_back(rule);
    }
}
#include <cmath>
template <typename PREFIX>
static void generate_long_ip_table_vec(std::vector<PREFIX> &vec, int num, char average_len, char second_len =0, char third_len = 0)
{
    for(int i = 0; i < num; i++)
    {
	PREFIX rule;
	for(int j =0; j < 16; j++)
	{
	    rule.ip[j] = rte_rand_max(256);
	}
	auto temp = rte_rand_max(129);
	rule.depth = temp ;
	auto dep = rte_rand_max(6);
	switch(dep)
	{
	case 1:
	    rule.depth = average_len;
	    break;
	case 2:
	    rule.depth = second_len;
	    break;
	case 3:
	    rule.depth = third_len;
	    break;
	default:
	    break;
	}
	vec.push_back(rule);
    }
}
template <typename PREFIX>
static void set_new_mask_ip_table_vec(std::vector<PREFIX> &vec, int num, uint8_t average_len, uint8_t second_len =0, uint8_t third_len = 0)
{
    for(int i =0; i < vec.size();i++)
    {
	vec[i].depth = rte_rand_max(129);
	auto dep = rte_rand_max(6);
	switch(dep)
	{
	case 1:
	    vec[i].depth = average_len;
	    break;
	case 2:
	    vec[i].depth = second_len;
	    break;
	case 3:
	    vec[i].depth = third_len;
	    break;
	default:
	    break;
	}
    }
}

template <typename PREFIX, typename IPS>
static void generate_large_ips_table_vec(const std::vector<PREFIX> &from,
					 std::vector<IPS> &to, int ips_size,  int reps_number, int pace, int failure_pace = 0)
{

    int current_pace = pace;
    int current_fail = failure_pace;
    for(int i =0, from_index =0; i< ips_size;i++)
    {
	if(from_index >= from.size())
	    from_index = 0;

	IPS ips;
	for(int j =0; j < 16; j++)
	    ips.ip[j] = rte_rand_max(256);

	mask_ip6_prefix(ips.ip,
			from[from_index].ip, from[from_index].depth);
	ips.next_hop = from[from_index].next_hop;
	if(current_pace >= pace)
	{
	    for(int f =0; f < reps_number; f++)
	    {
		//std::cout <<"pace create\n";
		to.push_back(ips);

	    }
	    current_pace = 0;
	    continue;
	}
	//std::cout <<"pace increment\n";
	to.push_back(ips);
	current_pace++;
	//if(failure_pace)
    }
    to.push_back({});
}

static void
print_route_distribution(const struct rules_tbl_entry *table, uint32_t n)
{
	unsigned int i, j;

	printf("Route distribution per prefix width:\n");
	printf("DEPTH    QUANTITY (PERCENT)\n");
	printf("---------------------------\n");

	/* Count depths. */
	for (i = 1; i <= 128; i++) {
		unsigned int depth_counter = 0;
		double percent_hits =0 ;

		for (j = 0; j < n; j++)
			if (table[j].depth == (uint8_t) i)
				depth_counter++;

		percent_hits = ((double)depth_counter)/((double)n) * 100;
		printf("%.2u%15u (%.2f)\n", i, depth_counter, percent_hits);
	}
	printf("\n");
}
