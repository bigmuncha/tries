#pragma once
#include <iostream>
#include <cstdint>
#include <array>
#include <bitset>
#include <sstream>
#include <span>
#include <logger.h>
#include <cstring>

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
static std::string getStrIpv6(u_char *ip)
{
    std::stringstream str;
    for(int i=0; i < 16; i++)
    {
	str << std::bitset<8>(ip[i]) << ".";
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
