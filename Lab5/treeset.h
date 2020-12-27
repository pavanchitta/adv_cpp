#include <memory>


struct TreeNode;
using SP_TreeNode = std::shared_ptr<TreeNode>;
struct TreeNode {
    int value;
    SP_TreeNode left{};
    SP_TreeNode right{};
    
    TreeNode(int value) : value{value} {}; 
    TreeNode& operator=(const TreeNode& other) {
        value = other.value;
        if (other.left) {
            left = other.left;
        }
        if (other.right) {
            right = other.right;
        }
        return *this;
    };
    TreeNode& operator=(TreeNode&& other) {
        value = other.value;
        if (other.left) {
            left = std::move(other.left);
        }
        if (other.right) {
            right = std::move(other.right);
        }
        return *this;
    }

    void replaceChild(SP_TreeNode& old_child, SP_TreeNode& n) {
        if (left && left->value == old_child->value) {
            left = n;
        }
        else {
            right = n;
        }
    }

    int findMinChild() {
        auto cur = this;
        while (cur->left) {
            cur = cur->left.get();
        }
        return cur->value;
    }
};




class TreeSet {
private: 
    SP_TreeNode root;
    int size_;
    bool sanity_check(SP_TreeNode n, int minval, int maxval);

public:
    int size();
    bool add(int value);
    bool del(int value);
    bool contains(int value);
    TreeSet& operator=(const TreeSet& other);
    TreeSet(const TreeSet& other);
    TreeSet();
    void printTree();
};
