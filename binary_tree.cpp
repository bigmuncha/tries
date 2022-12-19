#include <iostream>
#include <vector>
#include <source_location>
#include <string_view>

void log(const std::string_view message,
         const std::source_location location = 
               std::source_location::current())
{
     std::cout << "file: "
              << location.file_name() << "("
              << location.line() << ":"
              << location.column() << ") `"
              << location.function_name() << "`: "
              << message << '\n';
}
template <typename T>
struct node
{
    node (node* parent, T value):
	parent(parent), data(value), left(nullptr), right(nullptr){}
    node(T value):
	parent(nullptr), left(nullptr), right(nullptr), data(value){}
    node<T>* getLeft()
	{
	    if(left == nullptr)
		return nullptr;
	    return left;
	}
    node<T>* getRight()
	{
	    if(!right)
		return nullptr;
	    return right;
	}
    struct node* parent{};
    struct node* left{};
    struct node* right{};
    T data{};
};
template <typename T>
std::ostream& operator<<(std::ostream& os, const node<T>* obj)
{
    std::string str = obj? std::to_string(obj->data) : "*";
    os <<str;
    return os;
}

template<typename T>
class Btree
{
public:
    Btree(T* node): root(node){}
    Btree(): root(nullptr){}

    bool insert(T value)
    {
	if(!root){
	    root = new node<T>(value);
	    return true;
	}
	node<T> *current = root;
	while(current!=nullptr)
	{
	    if(value < current->data )
	    {
		if(current->left){
		    current = current->left;
		}
		else{
		    log("INSERT Left");
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
		    log("INSERT Right");
		    current->right = new node<T>(current,value);
		    return true;
		}
	    }
	    else{
		log("NO Insert");
		return false;
	    }
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
	while(current && current->left)
	{
	    current = current->left;
	}
	return current;
    }
    node<T>* getBiggest(node<T>* finder)
    {
	node<T>* current = finder;
	while(current && current->right)
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
	if(!removal->parent)
	{
	    node<T>* small = getBiggest(removal->left);
	    small = removal->right;
	    root = removal->left;
	    delete removal;
	}
	else if(removal->parent->right == removal){
	    removal->parent->right = removal->left;
	    node<T> * small = getBiggest(removal->left);
	    small = removal->right;
	    delete removal;
	}
	else if(removal->parent->left == removal){
	    removal->parent->left = removal->left;
	    node<T> * small = getBiggest(removal->left);
	    small = removal->right;
	    delete removal;
	}
	return true;
    }
    bool treeIsCorrect()
    {
	return treeIsCorrect(root);
    }

    bool treeIsCorrect(node<T>* current)
    {

	if(!current || !current->left && !current->right)
	    return true;
	if(current->left && current->data > current->left->data
	    && current->right  && current->data < current->right->data)
	{
	    return treeIsCorrect(current->left) * treeIsCorrect(current->right);
	}
	if(current->right  && current->data < current->right->data)
	{
	    return true * treeIsCorrect(current->right);
	}
	if(current->left && current->data > current->left->data)
	{
	    return true * treeIsCorrect(current->left);
	}
	std::cout <<current <<"SSS";
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
    int getDepth(node<T>* current, int dep)
    {
	if(!current)
	{
	    return dep;
	}
	return std::max(getDepth(current->left, dep+ 1), getDepth(current->right, dep + 1));
    }
    int getDepth()
    {
	return getDepth(root, 0);
    }
    void printNode(node<T>* nnode)
	{
	    if(!nnode)
		return;
	    std::cout << nnode->data << " ";
	    printNode(nnode->left);
	    printNode(nnode->right);
	}
    void printTree()
    {
	int trailer = 20;
	node<T>* current = root;
	if(!current)
	{
	    return;
	}
	printSpace(trailer);
	std::cout << current << "\n\n";

	std::vector<node<T>*>  vec{ current->getLeft(), current->getRight()};
	printSpace(trailer - 2);
	std::cout<< vec[0] << "  " <<  vec[1] << "\n\n";
	vec.clear();
	vec.assign({current->left->left, current->left->right,current->right->left, current->right->right});
	printSpace(trailer - 3 - 3);
	std::cout<< vec[0] << "  " <<  vec[1] << "  "<< vec[2] << "  " <<  vec[3] << "\n\n";
    }
private:
    node<T>* root;
};

int main()
{
    Btree<int> omar;
    std::cout << omar.treeIsCorrect() << "\n";
    omar.printTree();
    omar.insert(12);
    omar.insert(15);
    omar.insert(22);
    omar.insert(3);
    std::cout << omar.treeIsCorrect() << "\n";
    omar.printTree();
    omar.remove(15);
    omar.insert(39);
    omar.insert(-14);
    omar.insert(0);
    omar.insert(18);
    
    omar.remove(0);
    omar.printTree() ;
    for(int i =0; i < 20; i++)
    {
	omar.insert(i);
    }
    for(int i =0; i < 20; i+=2)
    {
	omar.remove(i);
    }
    for(int i =1; i < 2000; i*=2)
    {
	omar.insert(i);
	std::cout <<"Try insert " << i << "\n";
    }

    for(int i =2000; i > 0; i-=2)
    {
	//	omar.remove(i);
    }

    std::cout << "\n" << omar.treeIsCorrect() << "\n";
    std::cout << omar.getDepth() << "\n";
}
















