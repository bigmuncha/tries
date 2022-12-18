#include <iostream>
#include <vector>
template <typename T>
struct node
{
    node (node* parent, T value):
	parent(parent), data(value), left(nullptr), right(nullptr){}
    struct node* parent{};
    struct node* left{};
    struct node* right{};
    T data{};
};

template<typename T>
class Btree
{
public:
    Btree(T* node): root(node){}
    Btree(): root(nullptr){}

    bool insert(T value)
    {
	node<T> *current = root;
	while(current!=nullptr)
	{
	    if(value < current->data )
	    {
		if(current->left){
		    current = current->left;
		}
		else{
		    current->left = new node<T>(current,value);
		    return true;
		}
	    }
	    else if(value > current->data )
	    {
		if(current->right){
		    current = current->right;
		}
		else{
		    current->right = new node<T>(current,value);
		    return true;
		}
	    }
	    else
		return false;
	}
    return true;
    }
    node<T>* findData(T value)
    {
	node<T> *current = root;
	while(current)
	{
	    if(value < current->data)
	    {
		current = current->left;
	    }
	    else if (value > current->data)
	    {
		current = current->right;
	    }
	    else
		return current;
	}
	return nullptr;
    }

    node<T>* getSmallest(node<T>* finder)
    {
	node<T>* current = finder;
	while(current->left)
	{
	    current = current->left;
	}
	return current;
    }
    node<T>* getBiggest(node<T>* finder)
    {
	node<T>* current = finder;
	while(current->right)
	{
	    current = current->right;
	}
	return current;
    }

    bool remove(T value)
    {
	node<T>* removal = findData(value);
	if(!removal)
	    return false;
	if(removal->parent->right == removal){
	    removal->parent->right = removal->left;
	    node<T> * small = getBiggest(removal->left);
	    small->right = removal->right;
	}
	else if(removal->parent->left == removal){
	    removal->parent->left = removal->left;
	    node<T> * small = getBiggest(removal->left);
	    small->right = removal->right;
	}
    }
    bool treeIsCorrect(node<T>* current)
    {
	if(!current)
	    return true;
	if(current->data < current->left->data && current->data < current->right->data)
	{
	    return true * treeIsCorrect(current->left);
	    return true * treeIsCorrect(current->right);
	}
	else
	    return false;
    }
    void printSpace(int numbers)
	{
	    for(int i = 0; i < numbers; i++)
		std::cout <<" ";
	}
    std::vector<node<T>*> getLevel(int level)
    {
	std::vector<node<T>*> resV{};
	node<T>* current = root;
	for(int dep = 0; dep <level; dep++)
	{
	    
	}
    }
    void printTree()
    {
	unsigned int separator = 10;
	unsigned int offset = 20;
	int level = 0;
	printSpace(20);
	std::cout << root->data;
	for()
    }
private:
    node<T>* root;
};
















