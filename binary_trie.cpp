#include <cstdint>
#include <bitset>
#include <iostream>
#include <unordered_map>
#include <optional>
#include <vector>
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

	    if(current->bit == (next_hop & (1 <<current_bit)))
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
	node* f_node = first_node(next_hop << FIRST_BYTE_OFFSET);
	node* parent = f_node;
	int i = 1;
	auto size  = __builtin_popcount(mask);
	while(size)
	{
	    if((f_node = next_node(f_node,FIRST_BYTE_OFFSET-i, next_hop )))
	    {
		parent = f_node;
		continue;
	    }
	    else
	    {
		if(CHECK_BIT(next_hop, 1 << (FIRST_BYTE_OFFSET - i))){
		    parent->right = new node{1,nullptr,nullptr, {}};
		    f_node = parent = parent->right;
		}
		else{
		    parent->left  = new node{0,nullptr,nullptr, {}};
		    f_node = parent = parent->left;
		}
	    }
	    i++;
	    size--;
	}
	if(f_node->next_hops.contains(next_hop))
	{
	    return false;
	}
	else
	{
	    f_node->subnet = mask & next_hop;
	    f_node->next_hops[next_hop] = interface;
	    return true;
	}
    }
    std::optional<next_hop_table> lookup(uint32_t dst_ip)
    {
	node* f_node = first_node(dst_ip << FIRST_BYTE_OFFSET);
	node* parent = f_node;
	next_hop_table result;
	int i = 1;
	while(f_node)
	{
	    if((f_node = next_node(f_node, FIRST_BYTE_OFFSET - i, dst_ip)))
	    {
		if(!f_node->next_hops.empty())
		{
		    result = f_node->next_hops;
		}
		parent = f_node;
		continue;
	    }
	}
	if(!result.empty())
	{
	    return {};
	}
	else
	{
	    return result;
	}
    }

    void PrintValues()
    {
	node* f_node1 = root_.left;
	node* f_node2 = root_.right;
	std::vector<node*> collector;
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
	    std::cout << "Subnet:        "<< std::bitset<32>(nd->subnet) << '\n';
	    for(auto& [nexthop, interface]: nd->next_hops)
	    {
		std::cout <<"  Next_hop:    " << std::bitset<32>(nexthop) << " interface: " << interface << '\n';
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
    static constexpr uint32_t omar = 0xffffffff;
    static constexpr uint32_t said = 0xabcdef11;
    static constexpr uint8_t oleg  = 1 << 2;
    static constexpr uint8_t oid  = 2 | 1;
//    static constexpr uint32_t osid  =  1 >>(omar << (10 -1));
    static constexpr size_t sss = __builtin_popcount(0xffffffff << 10);
    std::cout <<sss << "\n";
    std::cout <<std::bitset<32>(1 << 4)  << "\n";
    std::cout <<std::bitset<32>(said)  << "\n";
    std::cout <<std::bitset<32>(~said)  << "\n";
    std::cout <<(said & (1 << 4))  << "\n";
    std::cout <<(~said & (1 << 4))  << "\n";
    std::cout <<std::bitset<32>(1<< 10 )  << "\n";
    std::cout <<std::bitset<32>(said << 10 )  << "\n";
    std::cout <<std::bitset<32>(said  &(said << 10))  << "\n";
    std::cout <<std::bitset<32>(said) << "\n";
    std::cout <<std::bitset<8>(oid) << "\n";
    //  std::cout <<std::bitset<8>(osid) << "\n";
    std::cout << (oleg & oid) << "\n";
    std::cout << "\n\n";
    Btrie trie;
    std::cout << trie.lookup(0xffff1122).has_value() << '\n';
    trie.PrintValues();
    std::cout << trie.insert(0xffff1122,0xffffff00, 12) << '\n';
    std::cout << trie.lookup(0xffff1122).has_value() << '\n';
    trie.PrintValues();
}