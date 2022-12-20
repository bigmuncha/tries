
struct node
{
    bool bit{};
    node* left{};
    node* right{};
    unsigned int prefix{};
};

class Btrie
{
    bool insert(unsigned int prefix)
    {
	
    }

    struct root{
	root()
	    {
		left = new node{0,nullptr,nullptr,0};
		right = new node{1,nullptr,nullptr,1};
	    }
	node* left{};
	node* right{};
    } root_;
};
