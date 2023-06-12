#pragma once
#include <cstdint>
#include <bitset>
#include <iostream>
#include <unordered_map>
#include <optional>
#include <vector>
#include "logger.h"
#include "ipcreator.h"
#define BIT_OFFSET 1
#define COMPLETE_FLAG 2

namespace buddy_lpm
{
    using next_hop_table =     std::unordered_map<uint32_t, uint32_t>;
    struct node
    {
	uint8_t bit;
	node* left{};
	node* right{};
	next_hop_table next_hops{};// nexthop-interface
	uint32_t subnet;
    };
    
    class Btrie
    {
    public:
	Btrie()
	    {
		root_.left = new node{0,nullptr,nullptr,{}};
		root_.right = new node{1,nullptr,nullptr,{}};
	    }
    public:
	node* next_node(node* current, uint8_t current_bit, uint32_t next_hop)
	    {

		if(next_hop & (1 <<current_bit))
		{
		    return current->right;
		}
		else
		{
		    return current->left;
		}
	    }
	bool insert(uint32_t next_hop, uint32_t mask, uint32_t interface)
	    {
		node* f_node  =  &root_;
		LLOG_DEBUG_C() <<"\n   Try Insert nexthop " << IPv4::printnum(next_hop) <<'\n';
		node* parent = f_node;
		int i = 0;
		auto size  = __builtin_popcount(mask);
		std::stringstream ss;
		ss << "   Insert Bit path:   ";
		while(size)
		{
		    ss << (bool)f_node->bit <<"";
		    if((f_node = next_node(f_node,FIRST_BYTE_OFFSET-i, next_hop )))
		    {
			LLOG_DEBUG() << "Dont make node";
			parent = f_node;
			i++;
			size--;
			continue;
		    }
		    else
		    {
			if(CHECK_BIT(next_hop, 1 << (FIRST_BYTE_OFFSET - i))){
			    parent->right = new node{1,nullptr,nullptr, {}};
			    f_node = parent = parent->right;
			    LLOG_DEBUG() << "Make node 1 " << i <<" "<< size;
			}
			else{
			    parent->left  = new node{0,nullptr,nullptr, {}};
			    f_node = parent = parent->left;
			    LLOG_DEBUG() << "Make node 0 " << i<<" " << size;
			}
		    }
		    i++;
		    size--;
		}
		LLOG_DEBUG_C() <<ss.str() << '\n';
		LLOG_DEBUG_C() << "\n ";
		if(f_node->next_hops.contains(next_hop))
		{
		    LLOG_DEBUG() << "Already contains";
		    return false;
		}
		else
		{
		    LLOG_DEBUG() << "MAKE INSERT";
		    f_node->subnet = mask & next_hop;
		    f_node->next_hops[next_hop] = interface;
		    LLOG_DEBUG() << "      Subnet:        "<< IPv4::printnumWithDots(mask &next_hop ) << '\n';
		    LLOG_DEBUG() << "      nexthop:       "<< IPv4::printnumWithDots(next_hop) << " IFace:" << interface << '\n';
		    return true;
		}
	    }
	std::optional<next_hop_table> lookup(uint32_t dst_ip)
	    {
		LLOG_INFO() <<"\n   Try loojup";
		node* f_node =&root_;// next_node(&root_, FIRST_BYTE_OFFSET, dst_ip);
		node* parent = f_node;
		next_hop_table result;
		int i = 0;
		if(!f_node)
		{
		    LLOG_INFO() <<"ERROR";
		}
		LLOG_DEBUG_C() << "   Lookup Bit path:   ";
		while(f_node)
		{
		    //std::cout << (bool)f_node->bit <<"";
		    if((f_node = next_node(f_node, FIRST_BYTE_OFFSET - i, dst_ip)))
		    {
			if(!f_node->next_hops.empty())
			{
			    LLOG_DEBUG_C() << " REMEMER " << f_node << " " << parent << " "<< i << " ";
			    LLOG_DEBUG() <<"FOUND";
			    result = f_node->next_hops;
			}
			LLOG_DEBUG() <<"NEXT";
			parent = f_node;
		    }else
		    {

		    }
		    i++;
		    LLOG_DEBUG() <<"NNEXT ";
		}
		LLOG_DEBUG_C() <<"\n";
		if(result.empty())
		{
		    LLOG_DEBUG_C() << "BAD " << f_node << " " << parent << " "<< i << " " << '\n';
		    LLOG_INFO() <<"empty result " << i << " " << (bool)parent->bit << " " << parent;
		    return {};
		}
		else
		{
		    LLOG_INFO() <<"nonempty " << i;
		    return result;
		}
	    }

	void PrintValues()
	    {
		node* f_node1 = root_.left;
		node* f_node2 = root_.right;
		std::vector<node*> collector;
		LLOG_DEBUG() << "Try print values";
		auto foo = [&](auto && foo, node* nd)
		    {
			if(!nd)
			    return;
			if(!nd->next_hops.empty())
			{
			    collector.push_back(nd);
			}
			foo(foo, nd->left);
			foo(foo, nd->right);
		    };
		foo(foo,f_node1);
		foo(foo, f_node2);
		for(auto nd: collector)
		{
		    LLOG_DEBUG_C() << "Subnet:        "<< IPv4::printnumWithDots(nd->subnet) << '\n';
		    for(auto& [nexthop, interface]: nd->next_hops)
		    {
			LLOG_DEBUG_C() <<"  Next_hop:    " << IPv4::printnumWithDots(nexthop) << " interface: " << interface << '\n';
		    }
		}
	    }
    private:
	node root_;
    };
}
