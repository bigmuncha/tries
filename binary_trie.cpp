#include <cstdint>
#include <bitset>
#include <iostream>
#include <unordered_map>
#include <optional>
#include <vector>
#include "helpers/logger.h"
#include "helpers/ipcreator.h"
#define BIT_OFFSET 1
#define COMPLETE_FLAG 2
#define FIRST_BYTE_OFFSET 31
//#define ROOT_FLAG 3

#define CHECK_BIT(num, bit)  ((num) & (bit))

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
    node* first_node(uint32_t first_bit)
	{
	    return CHECK_BIT(first_bit, 1)? root_.right: root_.left;
	}
    bool insert(uint32_t next_hop, uint32_t mask, uint32_t interface)
    {
	node* f_node = first_node(next_hop >> FIRST_BYTE_OFFSET);
	std::cout <<"\n   Try Insert nexthop " << IPv4::printnum(next_hop) <<'\n';
	node* parent = f_node;
	int i = 1;
	auto size  = __builtin_popcount(mask) -1;
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
	std::cout <<ss.str() << '\n';
	std::cout << "\n ";
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
	//LLOG_DEBUG() <<"\n   Try loojup";
	node* f_node = first_node(dst_ip >> FIRST_BYTE_OFFSET);
	node* parent = f_node;
	next_hop_table result;
	int i = 0;
	if(!f_node)
	{
	    LLOG_INFO() <<"ERROR";
	}
	std::cout << "   Lookup Bit path:   ";
	while(f_node)
	{
	    i++;
	    std::cout << (bool)f_node->bit <<"";
	    if((f_node = next_node(f_node, FIRST_BYTE_OFFSET - i, dst_ip)))
	    {
		//LLOG_DEBUG() <<"FIRST DEP " << (bool)f_node->bit << " ";
		if(!f_node->next_hops.empty())
		{
		    std::cout << " REMEMER " << f_node << " " << parent << " "<< i << " ";
		    //  LLOG_DEBUG() <<"FOUND";
		    result = f_node->next_hops;
		}
//		LLOG_DEBUG() <<"NEXT";
		parent = f_node;
	    }else
	    {

	    }

//	    LLOG_DEBUG() <<"NNEXT ";
	}
	std::cout <<"\n";
	if(result.empty())
	{
	    std::cout << "BAD " << f_node << " " << parent << " "<< i << " " << '\n';
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
	    std::cout << "Subnet:        "<< IPv4::printnumWithDots(nd->subnet) << '\n';
	    for(auto& [nexthop, interface]: nd->next_hops)
	    {
		std::cout <<"  Next_hop:    " << IPv4::printnumWithDots(nexthop) << " interface: " << interface << '\n';
	    }
	}
    }
private:
    struct root{
	root()
	    {
		left = new node{0,nullptr,nullptr,{}};
		right = new node{1,nullptr,nullptr,{}};
	    }
	node* left{};
	node* right{};
    } root_;
};

int main ()
{
/* //    LLOG_DEBUG() << "OGOR"; */
/*     static constexpr uint32_t omar = 0xffffffff; */
/*     static constexpr uint32_t said = 0xabcdef11; */
/*     static constexpr uint8_t oleg  = 1 << 2; */
/*     static constexpr uint8_t oid  = 2 | 1; */
/* //    static constexpr uint32_t osid  =  1 >>(omar << (10 -1)); */
/*     static constexpr size_t sss = __builtin_popcount(0xffffffff << 10); */
/*     std::cout <<sss << "\n"; */
/*     std::cout <<std::bitset<32>(1 << 4)  << "\n"; */
/*     std::cout <<std::bitset<32>(said)  << "\n"; */
/*     std::cout <<std::bitset<32>(~said)  << "\n"; */
/*     std::cout <<(said & (1 << 4))  << "\n"; */
/*     std::cout <<(~said & (1 << 4))  << "\n"; */
/*     std::cout <<std::bitset<32>(1<< 10 )  << "\n"; */
/*     std::cout <<std::bitset<32>(said << 10 )  << "\n"; */
/*     std::cout <<std::bitset<32>(said  &(said << 10))  << "\n"; */
/*     std::cout <<std::bitset<32>(said) << "\n"; */
/*     std::cout <<std::bitset<8>(oid) << "\n"; */
/*     //  std::cout <<std::bitset<8>(osid) << "\n"; */
/*     std::cout << (oleg & oid) << "\n"; */
/*     std::cout << "\n\n"; */
     Btrie trie; 
/*     std::cout << trie.lookup(0xffff1122).has_value() << '\n'; */
/*     trie.PrintValues(); */
/*     std::cout << trie.insert(0xffff1122,0xffffff00, 12) << '\n'; */
/*     std::cout << trie.insert(0xffff3333,0xffffff00, 15) << '\n'; */
/*     std::cout << trie.insert(0xffff1144,0xffffff00, 18) << '\n'; */
/*     std::cout << trie.lookup(0xffff1122).has_value() << '\n'; */
//    trie.PrintValues();
    LLOG_DEBUG() << "OLEG";
    /* std::cout <<getLogLevel() << '\n'; */

    /* uint32_t subnet= 0xffff3322; */
    /* std::array<uint8_t,4> net{0xff, 0xff, 0x1,0x1}; */
    /* for(int i = 0; i < 256; i++ ) */
    /* { */
    /* 	if(!trie.insert(subnet++,0xffffff00,i)) */
    /* 	{ */
    /* 	    LLOG_ERROR() << "ERROR OCUR " << i; */
    /* 	    throw std::exception(); */
    /* 	} */
    /* } */
    /* uint32_t other_subnet  = 0x19232224; */
    /* for(int i = 0; i< 500; i++) */
    /* { */
    /* 	if(!trie.lookup(other_subnet)) */
    /* 	{ */
    /* 	    LLOG_ERROR() << "ERROR OCUR " << i; */
    /* 	    throw std::exception(); */
    /* 	} */
    /* } */
    std::array<uint8_t, 4> arr{10, 210,1,1};
    std::array<uint8_t, 4> check_arr = arr;
    for(int i =0; arr[2] < 254; i++)
    {
	IPv4 net(arr);
	uint32_t nets = reinterpret_cast<uint32_t&>(net);
	trie.insert(nets,0xffffff00,i);
	arr[2]++;
    }
    arr = check_arr;
    for(int i=0; arr[2] < 252; i++)
    {
	for(int j =0; arr[3] < 252; j++)
	{
	    IPv4 dst(arr);
	    uint32_t ddst = reinterpret_cast<uint32_t&>(dst);
	    if(!trie.lookup(ddst).has_value())
	    {
		std::cout <<"\n Bad ip               "<< dst.getWithDots() << " i "  << i<<" j" << j << '\n';
		throw std::exception();
	    }
	    arr[3]++;
	}
	arr[3] = 1;
	arr[2]++;
    }
    IPv4 num(1);
    IPv4 bum(std::array<uint8_t,4>{0xff, 0xff,0xcc,0xdd});
    IPv4 sum(0xff, 0xff, 0xcc, 0xdd);
    std::cout <<'\n';
    std::cout << num.getWithDots() << " " <<num.getWithoutDots() << '\n';
    std::cout << bum.getWithDots() << " " <<bum.getWithoutDots() << '\n';
    std::cout << sum.getWithDots() << " " <<sum.getWithoutDots() << '\n';
    uint32_t oamr = reinterpret_cast<uint32_t&>(num);
    std::cout<<IPv4::printnumWithDots(1 <<8) <<" " << IPv4::printnum(oamr << (31-24)) << '\n';
    //show_nets();
    //    trie.PrintValues();

}
