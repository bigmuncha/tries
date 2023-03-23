#pragma once
#include <b_trie_lpm.h>
#include <ipcreator.h>

using namespace buddy_lpm;
static void test_net_24()
{
    Btrie trie;
    std::array<uint8_t, 4> arr{10, 210,1,1};
    std::array<uint8_t, 4> check_arr = arr;
    for(int i =0; arr[2] < 255; i++)
    {
	IPv4 net(arr);
	uint32_t nets = reinterpret_cast<uint32_t&>(net);
	trie.insert(nets,0xffffff00,i);
	arr[2]++;
    }
    arr = check_arr;
    for(int i=0; arr[2] < 255; i++)
    {
	for(int j =0; arr[3] < 255; j++)
	{
	    IPv4 dst(arr);
	    uint32_t ddst = reinterpret_cast<uint32_t&>(dst);
	    if(!trie.lookup(ddst).has_value())
	    {
		LLOG_ERROR() <<"\n Bad ip               "<< dst.getWithDots() << " i "  << i<<" j" << j << '\n';
		throw std::exception();
	    }
	    arr[3]++;
	}
	arr[3] = 1;
	arr[2]++;
    }
}

static void test_net_16()
{
    Btrie trie;
    std::array<uint8_t, 4> arr{10,1,1,1};
    std::array<uint8_t, 4> check_arr = arr;
    for(int i =0; arr[1] < 255; i++)
    {
	IPv4 net(arr);
	uint32_t nets = reinterpret_cast<uint32_t&>(net);
	trie.insert(nets,0xffff0000,i);
	arr[1]++;
    }
    arr = check_arr;
    for(int k =0; arr[1] < 255; k++)
    {
	for(int i=0; arr[2] < 255; i++)
	{
	    for(int j =0; arr[3] < 255; j++)
	    {
		IPv4 dst(arr);
		uint32_t ddst = reinterpret_cast<uint32_t&>(dst);
		if(!trie.lookup(ddst).has_value())
		{
		    LLOG_ERROR() <<"\n Bad ip               "<< dst.getWithDots() << " i "  << i<<" j" << j << '\n';
		    throw std::exception();
		}
		arr[3]++;
	    }
	    arr[3] = 1;
	    arr[2]++;
	}
	arr[2] = 1;
	arr[1]++;
    }
}
static void test_net_32()
{
    Btrie trie;
    std::array<uint8_t, 4> arr{10, 210,1,1};
    std::array<uint8_t, 4> check_arr = arr;
    for(int i =0; arr[3] < 255; i++)
    {
	IPv4 net(arr);
	uint32_t nets = reinterpret_cast<uint32_t&>(net);
	trie.insert(nets,0xffffff00,i);
	arr[3]++;
    }
    arr = check_arr;
    for(int i=0; arr[2] < 255; i++)
    {
	for(int j =0; arr[3] < 255; j++)
	{
	    IPv4 dst(arr);
	    uint32_t ddst = reinterpret_cast<uint32_t&>(dst);
	    if(!trie.lookup(ddst).has_value())
	    {
		LLOG_ERROR() <<"\n Bad ip               "<< dst.getWithDots() << " i "  << i<<" j" << j << '\n';
		throw std::exception();
	    }
	    arr[3]++;
	}
	arr[3] = 1;
	arr[2]++;
    }
}

static void test_fill_nh_table()
{
    Btrie trie;
    std::array<uint8_t, 4> arr{10,210,10,1};
    std::array<uint8_t, 4> check_arr = arr;
    for(int i =0; arr[3] < 255; i++)
    {
	IPv4 net(arr);
	uint32_t nets = reinterpret_cast<uint32_t&>(net);
	trie.insert(nets,0xffffff00,i);
	arr[3]++;
    }
    arr = check_arr;
    for(int k =0; arr[3] < 255; k++)
    {
	IPv4 dst(arr);
	uint32_t ddst = reinterpret_cast<uint32_t&>(dst);
	auto hash = trie.lookup(ddst);
	if(!hash.has_value())
	{
	    LLOG_ERROR() <<"\n Bad ip               "<< dst.getWithDots() << " i "  << k <<'\n';
	    throw std::exception();
	}
	else
	{
	    auto nh_table = hash.value();
	    if (nh_table[ddst] != k)
	    {
		LLOG_ERROR() <<"\n Bad ip               "<< dst.getWithDots() << " i "  << k <<'\n';
		throw std::exception();
	    }
	}
	arr[3]++;
    }
}

static void test_get_longest_match()
{
    Btrie trie;
    std::array<uint8_t, 4> arr{10,210,10,1};
    std::array<uint8_t, 4> check_arr = arr;
    IPv4 net(arr);
    uint32_t nets = reinterpret_cast<uint32_t&>(net);
    trie.insert(nets, 0xfffffd00, 10);
    trie.insert(nets, 0xffffff00, 12);
    auto hash = trie.lookup(nets);
    if(!hash.has_value())
    {
	LLOG_ERROR() <<"\n Bad ip               "<< net.getWithDots()  <<'\n';
	throw std::exception();
    }
    if(hash.value()[nets] != 12)
    {
	LLOG_ERROR() <<"\n Bad ip               "<< net.getWithDots()  <<'\n';
	throw std::exception();
    }
    LLOG_ERROR() << IPv4::printnumWithDots(IPv4::getNet(nets, 0xffffff00));
    LLOG_ERROR() << IPv4::printnumWithDots(IPv4::getNet(nets, 0xfffffe00));
}
