#include <memory>
#include <vector>
#include <initializer_list>
#include <iostream>

// Forward declatations
struct TreeNode;
class TreeSetIter;


using SP_TreeNode = std::shared_ptr<TreeNode>;
struct TreeNode {
    int value;
    SP_TreeNode left{nullptr};
    SP_TreeNode right{};
    
    TreeNode(int value) : value{value} {}; 
    TreeNode(const TreeNode& other) {
        //std::cout << "In TreeNode copy constructor" << std::endl;
        value = other.value;
        if (other.left) {
            left = std::make_shared<TreeNode>(*other.left);
        }
        if (other.right) {
            right = std::make_shared<TreeNode>(*other.right);
        }
    };
    TreeNode& operator=(const TreeNode& other) {
        //std::cout << "In TreeNode copy assignment" << std::endl;
        value = other.value;
        if (other.left) {
            left = std::make_shared<TreeNode>(*other.left);
        }
        if (other.right) {
            right = std::make_shared<TreeNode>(*other.right);
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
    friend class TreeSetIter;
private: 
    SP_TreeNode root;
    int size_;
    bool sanity_check(SP_TreeNode n, int minval, int maxval);

public:
    int size();
    bool add(int value);
    bool del(int value);
    bool contains(int value) const;
    TreeSet& operator=(const TreeSet& other);
    TreeSet(const TreeSet& other);
    TreeSet(const std::initializer_list<int> list);
    TreeSet();
    void printTree();

    bool operator==(const TreeSet &rhs) const;
    bool operator!=(const TreeSet &rhs) const;
    TreeSet plus(const TreeSet &s) const;
    TreeSet minus(const TreeSet &s) const;
    TreeSet intersect(const TreeSet &s) const;
    // Iterator methods
    using iterator = TreeSetIter;
    TreeSetIter begin() const;
    TreeSetIter end() const;
};


std::ostream& operator<<(std::ostream& os, const TreeSet& s);
class TreeSetIter {
private:
    SP_TreeNode cur_node;
    std::vector<SP_TreeNode> entered;
    void populateNextSequence(SP_TreeNode cur);
public:
    TreeSetIter() : cur_node{}, entered{} {};
    TreeSetIter(const TreeSet& set);
    TreeSetIter &operator++();
    TreeSetIter operator++(int);
    int operator*();
    bool operator==(const TreeSetIter& rhs) const; 
    bool operator!=(const TreeSetIter& rhs) const;
};
