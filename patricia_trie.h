#include <cstdint>
#include <ipcreator.h>
namespace patricia
{
    struct node
    {
	uint8_t bit;
	uint8_t len;
	uint32_t bit_path;
	node* left;
	node* right;
    };
    class Patricia
    {
    public:
	Patricia()
	{
	    root_ = new node{0,0,0,nullptr,nullptr};
	}
	node* next_node(node* current, uint8_t current_bit, uint32_t next_hop)
	{
	    if(next_hop & (1 <<current_bit))
	    {
		if(current->right && ((next_hop & (0xffffffff << current_bit)) & current->right->bit_path))
		    return current->right;
	    }
	    else
	    {
		if(current->left &&((next_hop & (0xffffffff << current_bit)) & current->left->bit_path))
		    return current->left;
	    }
	    return nullptr;
	}

	bool insert(uint32_t next_hop, uint32_t mask, uint32_t interface)
	{
	    node* n_node = root_;
	    auto size  = __builtin_popcount(mask);
	    node* parent = n_node;
	    uint8_t  i = 0;
	    while(size)
	    {
		if((n_node = next_node(n_node, FIRST_BYTE_OFFSET - i, next_hop)))
		{
		    LLOG_DEBUG() << "Dont make node";
		    parent = n_node;
		    i+=n_node->len;;
		    size-=n_node->len;
		    continue;
		}
		else
		{
		    uint32_t new_nh_path = next_hop &         (0xffffffff << i);
		    uint32_t old_nh_path = parent->bit_path & (0xffffffff << i);
		    if(CHECK_BIT(next_hop, 1 << (FIRST_BYTE_OFFSET - i))){
			parent->right = new node{1,i,new_nh_path,nullptr, nullptr};
			parent->left =  new node{0,i,old_nh_path,nullptr,nullptr};
		    }
		    else{
			parent->left = new node{0,i,new_nh_path,nullptr, nullptr};
			parent->right = new node{1,i,old_nh_path,nullptr, nullptr};
		    }
		    parent->len = size;
		}
	    }
	}
	node* root_;
    };
}
