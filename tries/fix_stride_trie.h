#include <logger.h>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <ipcreator.h>

#define IPV4_HEADER_SIZE 20
constexpr inline unsigned short alignL3MTU(unsigned short mtu)
{
    if((mtu - IPV4_HEADER_SIZE) & 0x7)
	return mtu;
    unsigned short alignMtu = mtu - IPV4_HEADER_SIZE;
    alignMtu &= ~((unsigned short)0x7);
    alignMtu += IPV4_HEADER_SIZE;
    return alignMtu;
}


#define IPV6_BYTE_NUM 16
#include <ipcreator.h>

static constexpr uint16_t omar =  alignL3MTU(512);
namespace fx_trie
{
    static constexpr uint8_t node_flag_intermediate = 1 << 1;
    static constexpr uint8_t node_flag_active_rule =  1 << 2;

    struct node
    {
	uint8_t dep;
	uint8_t flags;
	uint32_t interface;
	ipv6 nexthop;
	char bin_path[IPV6_BYTE_NUM];
	struct node* stride_array;
    };

    template <uint8_t stride_size = 8>
    class Fx_trie
    {
	static constexpr u_int8_t FFull[IPV6_BYTE_NUM] =
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
	static constexpr int stride_tuple_size = std::pow(2,stride_size);
    public:
	Fx_trie()
	{
	    LLOG_DEBUG_C() <<" stride_size: " << stride_tuple_size <<'\n';
	    root = new node;
	    root->stride_array = new node[stride_tuple_size]();
	}
	bool insert(const u_char *next_hop, uint16_t mask_len, uint32_t interface)
	{
	    node* current_node = root;
	    struct ipv6 ip{};
	    std::memcpy(ip.buff,next_hop, sizeof(ip.buff));
	    struct ipv6 nh_path  = applyMask(next_hop, mask_len);

	    uint16_t current_mask = 0;
	    int i = 0;
	    LLOG_DEBUG_C() << "masklen:    " << mask_len;
	    LLOG_DEBUG_C() << "INSERT VAL: " << getStrIpv6(ip);
	    LLOG_DEBUG_C() << "INSERT VAL: " << getStrIpv6(const_cast<u_char*>(next_hop));
	    LLOG_DEBUG_C() << "PATH      : " << getStrIpv6(nh_path);
	    LLOG_DEBUG_C() << "MASK      : " << getStrIpv6(show_mask(mask_len));
	    while(current_mask < mask_len)
	    {
		current_mask+= stride_size;
		u_char stride_num = nh_path.buff[i++];
		LLOG_DEBUG_C() <<"INSERT current stride: " << std::bitset<8>(stride_num)  << " iter: " << i<< " mask: " <<current_mask;
		if(!current_node->stride_array)
		{
		    current_node->stride_array = new node[stride_tuple_size];
		    if (!current_node->stride_array)
		    {
			LLOG_DEBUG_C() <<" alloc errro: " << stride_num <<'\n';
			return 0;
		    }
		}
		current_node->flags |=node_flag_intermediate;
		current_node = &current_node->stride_array[stride_num];
	    }
	    if(current_node->flags & node_flag_active_rule &&
	       current_node->nexthop == ip && current_node->interface == interface)
	    {
		LLOG_DEBUG_C() <<" already set";
		return 0;
	    }
	    current_node->interface = interface;
	    current_node->nexthop = ip;
	    current_node->flags |=node_flag_active_rule;
	    LLOG_DEBUG_C() <<" SUCCESS";
	    return true;
	}
	bool lookup(const u_char *ip_add)
	{
	    node* current_node = root;
	    int i = 0;
	    LLOG_DEBUG_C() << "Lookup VAL: " << getStrIpv6(const_cast<u_char*>(ip_add));
	    while(true)
	    {
		u_char current_stride = ip_add[i++];
		LLOG_DEBUG_C() <<"LOOKUP current stride: " << std::bitset<8>(current_stride)  << " iter: " << i;
		if(current_node->stride_array[current_stride].flags & node_flag_intermediate)
		{
		    current_node = &current_node->stride_array[current_stride];
		}
		else
		{
		    LLOG_DEBUG_C() <<" Finnaly rich ";
		    for(int j = 0; j < 8; j++)
		    {
			current_stride &= (0xff << j);
			LLOG_DEBUG_C() <<" current final stride: " << std::bitset<8>(current_stride)  << " jiter: " << j;
			if(current_node->stride_array[current_stride].flags & node_flag_active_rule)
			{
			    LLOG_DEBUG_C() << "FOUND";
			    return true;
			}
		    }
		    LLOG_DEBUG_C() <<" Bad lookup"<<'\n';
		    return 0;
		}
	    }
	}
    private:
	node* root;
    };
};

