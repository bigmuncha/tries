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
	next_hop_table table;
    };
    class Patricia
    {
    public:
	Patricia()
	{
	    root_ = new node{0,0,0,nullptr,nullptr};
	}
	node* next_node(node* current, uint8_t current_bit, uint32_t next_hop_net)
	{
	    if((next_hop_net & (1 <<current_bit)) && current->right)
	    {
		LLOG_DEBUG() << " Try return right    " <<(uint32_t )current_bit <<" " << (uint32_t) current->right->len;
		return current->right;
		LLOG_DEBUG() << " Try return right    " <<IPv4::printnumWithDots(current->right->bit_path) <<" " << (uint32_t) current->right->len;
		if(current->right && (((next_hop_net & (0xffffffff << current->right->len)) & current->right->bit_path)) == current->right->bit_path)
		{

		    return current->right;
		}
	    }
	    else if(current->left)
	    {
		LLOG_DEBUG() << " Try return left    " <<(uint32_t )current_bit <<" " << (uint32_t) current->left->len;
		return current->left;
		LLOG_DEBUG() << " Try return left    " <<IPv4::printnumWithDots(current->left->bit_path) <<" " << (uint32_t) current->left->len;
		if(current->left && (((next_hop_net & (0xffffffff << current->left->len)) & current->left->bit_path)) == current->right->bit_path)
		{
		    return current->left;
		}

	    }
	    LLOG_DEBUG() << " Next dont match";
	    return nullptr;
	}

	bool insert(uint32_t next_hop, uint32_t mask, uint32_t interface)
	{
	    node* n_node = root_;
	    std::cout << '\n';
	    LLOG_DEBUG() << "Try insert node:      " << IPv4::printnumWithDots(next_hop) << " mask: " << IPv4::printnumWithDots(mask);
	    uint8_t size  = __builtin_popcount(mask);
	    uint8_t ssize = size;
	    uint8_t rest_bites= size;
	    node* parent = n_node;
	    uint8_t  i = 0;
	    uint32_t nh_net = next_hop &  mask;
	    next_hop_table nh;
	    nh[next_hop] = interface;
	    while(size)
	    {
		if((n_node = next_node(n_node, FIRST_BYTE_OFFSET - i, nh_net)))
		{
		    uint32_t nh_substr = nh_net & (0xffffffff << (FIRST_BYTE_OFFSET-n_node->len + 1));
		    LLOG_DEBUG() << "Dont make node        " <<  IPv4::printnumWithDots(n_node->bit_path)
				 << " " << IPv4::printnumWithDots(nh_substr) <<" "
				 <<(uint32_t) rest_bites << " cur_node len " <<(uint32_t) n_node->len;
		    if(nh_substr == n_node->bit_path)
		    {
			LLOG_DEBUG() << "continue node addr " << n_node;
			parent = n_node;
			i=n_node->len;
			size = rest_bites- n_node->len;
			continue;
		    }
		    else
		    {
		    CREATE:
			if(n_node != root_){
			    uint32_t find_notmatch_bit = next_hop ^ n_node->bit_path;
			    uint8_t bit_number = get_first_nonzero(find_notmatch_bit);
			    uint32_t old_nh_path = n_node->bit_path & (0xffffffff << (32 - bit_number +1));
			    if(CHECK_BIT(next_hop, 1 << (FIRST_BYTE_OFFSET - bit_number))){
				n_node->right = new node{1,ssize,nh_net,nullptr, nullptr, nh};
				n_node->left =  new node{0,n_node->len,n_node->bit_path,nullptr,nullptr, n_node->table};
				LLOG_DEBUG() << "Add node right:       "  << IPv4::printnumWithDots(n_node->right->bit_path)
					     <<" len: " << (uint32_t)n_node->right->len << " addr " << n_node->right;
				LLOG_DEBUG() << "Expand node left:     " << IPv4::printnumWithDots(n_node->left->bit_path)
					     <<" len: " << (uint32_t)n_node->left->len << " addr " << n_node->left;
			    }
			    else{
				n_node->left = new node{0,ssize,nh_net,nullptr, nullptr, nh};
				n_node->right = new node{1,n_node->len,n_node->bit_path,nullptr, nullptr, n_node->table};
				LLOG_DEBUG() << "Add node left:        " << IPv4::printnumWithDots(n_node->left->bit_path)
					     <<" len: " << (uint32_t)n_node->left->len  << " addr " << n_node->left;
				LLOG_DEBUG() << "Expand node right:    " << IPv4::printnumWithDots(n_node->right->bit_path)
					     <<" len: " << (uint32_t)n_node->right->len  << " addr " << n_node->right;
			    }
			    n_node->len  = bit_number - 1;
			    n_node->bit_path = old_nh_path;
			    LLOG_DEBUG() << "Trim parrent node:    " << IPv4::printnumWithDots(old_nh_path)
					 <<" len: " << (uint32_t)n_node->len << " addr " << n_node;;
			    return true;
			}
			else
			{
			    if(CHECK_BIT(next_hop, 1 << (FIRST_BYTE_OFFSET)))
			    {
				root_->right = new node{1,ssize,nh_net, nullptr,nullptr, nh};
				LLOG_DEBUG() << "Add right root node:   " << IPv4::printnumWithDots(nh_net)
					     <<" len: " << (uint32_t)root_->right->len << " addr " << root_->right;
			    }
			    else
			    {
				root_->left = new node{0,ssize,nh_net, nullptr,nullptr, nh};
				LLOG_DEBUG() << "Add left root node:   " << IPv4::printnumWithDots(root_->left->bit_path)
					     <<" len: " << (uint32_t) root_->left->len << " addr " << root_->left;;
			    }
			}
		    }
		}
		else
		{
		    if(CHECK_BIT(next_hop, 1 << (FIRST_BYTE_OFFSET)))
		    {
			root_->right = new node{1,ssize,nh_net, nullptr,nullptr, nh};
			LLOG_DEBUG() << "Add right root node: " << IPv4::printnumWithDots(nh_net)
				     <<" len: " << (uint32_t)root_->right->len << " addr " << root_->right;
		    }
		    else
		    {
			root_->left = new node{0,ssize,nh_net, nullptr,nullptr, nh};
			LLOG_DEBUG() << "Add left root node: " << IPv4::printnumWithDots(root_->left->bit_path)
				     <<" len: " << (uint32_t) root_->left->len << " addr " << root_->left;
		    }
		}
		break;
	    }
	    if(n_node)
	    {
		n_node->table[next_hop] = interface;
		LLOG_DEBUG() << "Add another next_hop in ptree: " << IPv4::printnumWithDots(n_node->bit_path) <<" len: " << (uint32_t) n_node->len ;
	    }
	    return true;
	}
	node* root_;
    };
}
