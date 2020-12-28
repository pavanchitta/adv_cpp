#include <memory>
#include <vector>
#include <initializer_list>
#include <iostream>

// Forward declatations
template<typename T>
struct TreeNode;
template<typename T, typename Compare>
class TreeSetIter;

template<typename T>
using SP_TreeNode = std::shared_ptr<TreeNode<T>>;

template<typename T>
struct TreeNode {
    T value;
    SP_TreeNode<T> left{nullptr};
    SP_TreeNode<T> right{nullptr};
    
    TreeNode(T value) : value{value} {}; 
    TreeNode(const TreeNode& other) {
        //std::cout << "In TreeNode copy constructor" << std::endl;
        value = other.value;
        if (other.left) {
            left = std::make_shared<TreeNode<T>>(*other.left);
        }
        if (other.right) {
            right = std::make_shared<TreeNode<T>>(*other.right);
        }
    };
    TreeNode& operator=(const TreeNode& other) {
        //std::cout << "In TreeNode copy assignment" << std::endl;
        value = other.value;
        if (other.left) {
            left = std::make_shared<TreeNode<T>>(*other.left);
        }
        if (other.right) {
            right = std::make_shared<TreeNode<T>>(*other.right);
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

    void replaceChild(SP_TreeNode<T>& old_child, SP_TreeNode<T>& n) {
        if (left && left->value == old_child->value) {
            left = n;
        }
        else {
            right = n;
        }
    }

    T findMinChild() {
        auto cur = this;
        while (cur->left) {
            cur = cur->left.get();
        }
        return cur->value;
    }
};

template <typename T, typename Compare = std::less<T>>
//template <typename T>
class TreeSet {
    friend class TreeSetIter<T, Compare>;
private: 
    SP_TreeNode<T> root;
    int size_;
    bool sanity_check(SP_TreeNode<T> n, T minval, T maxval);

public:
    int size();
    bool add(T value);
    bool del(T value);
    bool contains(T value) const;
    TreeSet& operator=(const TreeSet& other);
    TreeSet(const TreeSet& other);
    TreeSet(const std::initializer_list<T> list);
    TreeSet();
    void printTree();

    bool operator==(const TreeSet &rhs) const;
    bool operator!=(const TreeSet &rhs) const;
    TreeSet plus(const TreeSet &s) const;
    TreeSet minus(const TreeSet &s) const;
    TreeSet intersect(const TreeSet &s) const;
    // Iterator methods
    using iterator = TreeSetIter<T, Compare>;
    TreeSetIter<T, Compare> begin() const;
    TreeSetIter<T, Compare> end() const;
};

template <typename T, typename Compare>
std::ostream& operator<<(std::ostream& os, const TreeSet<T, Compare>& s);

template <typename T, typename Compare>
class TreeSetIter {
private:
    SP_TreeNode<T> cur_node;
    std::vector<SP_TreeNode<T>> entered;
    void populateNextSequence(SP_TreeNode<T> cur);
public:
    TreeSetIter() : cur_node{}, entered{} {};
    TreeSetIter(const TreeSet<T, Compare>& set);
    TreeSetIter &operator++();
    TreeSetIter operator++(int);
    T operator*();
    bool operator==(const TreeSetIter& rhs) const; 
    bool operator!=(const TreeSetIter& rhs) const;
};


/* CPP Code migrated to the .h file */ 

#define DEBUG 0
template <typename T, typename Compare> 
TreeSet<T, Compare>::TreeSet() : root{nullptr}, size_{0} {}

template <typename T, typename Compare> 
bool TreeSet<T, Compare>::add(T value) {
    if (DEBUG)
        std::cout << "Adding value: " << value << std::endl;
    //assert(sanity_check(root, INT_MIN, INT_MAX));
    if (contains(value)) {
        return false;
    }
    auto cur = root;
    Compare cmp;
    while (true) {
        if (cur == nullptr) {
            root = std::make_shared<TreeNode<T>>(value);
            size_++;
            return true;
        }
        if (cmp(value, cur->value)) {
            if (cur->left == nullptr) {
                cur->left = std::make_shared<TreeNode<T>>(value);
                size_++;
                return true;
            }
            cur = cur->left;
        }
        else {
            if (cur->right == nullptr) {
                cur->right = std::make_shared<TreeNode<T>>(value);
                size_++;
                return true;
            }
            cur = cur->right;
        }
    }
}

template<typename T, typename Compare>
int TreeSet<T, Compare>::size() {
    return size_;
}


template<typename T, typename Compare>
bool TreeSet<T, Compare>::contains(T value) const {
    if (DEBUG)
        std::cout << "Checking contains: " << value << std::endl; 
    auto cur = root;
    Compare cmp;
    while (true) {
        if (cur == nullptr) {
            if (DEBUG)
                std::cout << "Couldnt find value" << std::endl;
            return false;
        }
        if (cur->value == value) {
            if (DEBUG)
                std::cout << "Found value!" << std::endl;
            return true;
        }
        if (cmp(value, cur->value)) {
            cur = cur->left;
        }
        else {
            cur = cur->right;
        }
    }
}

template<typename T, typename Compare>
bool TreeSet<T, Compare>::del(T value) {
    if (DEBUG)
        std::cout << "Deleting value " << value << std::endl;
    if (!contains(value)) return false;
    SP_TreeNode<T> cur = root;
    SP_TreeNode<T> parent;
    Compare cmp;
    while (true) {
        assert(cur != nullptr);
        if (cur->value == value) {
            if (DEBUG)
                std::cout << "Found node to delete value: " << value << std::endl;
            break;
        }
        if (cmp(value, cur->value)) {
            parent = cur;
            cur = cur->left;
        }
        else {
            parent = cur;
            cur = cur->right;
        }
    }
    // Found the node to delete and stored its parent. Now
    // have to check cases on how to replace the node we're deleting
    if (cur->left && cur->right) {
        if (DEBUG)
            std::cout << "Found two children" << std::endl;
        T successor_val = cur->right->findMinChild();
        del(successor_val);
        cur->value = successor_val;
    }
    else if (cur->left) {
        if (cur->value == root->value) {
            root = cur->left;
        }
        else {
            // cur only has a left child, so we replace parent's child with cur's left child
            parent->replaceChild(cur,  cur->left);
        }
    }
    else if (cur->right) {
        if (cur->value == root->value) {
            root = cur->right;
        }
        else {
            // cur only has a left child, so we replace parent's child with cur's left child
            parent->replaceChild(cur, cur->right);
        }
    }
    else {
        if (cur->value == root->value) {
            root = nullptr;
        }
        else {
            auto null = SP_TreeNode<T>{};
            assert(null == nullptr);
            parent->replaceChild(cur, null);
        }
    }
    size_--;
    return true;
}
template <typename T, typename Compare>
TreeSet<T, Compare>::TreeSet(const TreeSet& other) {
    size_ = other.size_;
    // Copy assign root node using recursive copy assignment from TreeNode
    if (other.root) {
        auto copy = *other.root;
        root = std::make_shared<TreeNode<T>>(copy);
    }
    else {
        root = SP_TreeNode<T>{};
    }
}

template <typename T, typename Compare>
TreeSet<T, Compare>& TreeSet<T, Compare>::operator=(const TreeSet& other) {
    size_ = other.size_;
    // Copy assign root node using recursive copy assignment from TreeNode
    if (other.root) {
        auto copy = *other.root;
        root = std::make_shared<TreeNode<T>>(copy);
    }
    else {
        root = SP_TreeNode<T>{};
    }
    return *this;
}

template <typename T, typename Compare>
bool TreeSet<T, Compare>::sanity_check(SP_TreeNode<T> n, T minval, T maxval) {

    if (!n) return true;
    if (n->value > maxval || n->value < minval) {
        std::cerr << "Sanity check failed: " << n->value << " min: " << minval << " max: " << maxval << std::endl;
        return false;
    }
    return (sanity_check(n->left, minval, n->value) && sanity_check(n->right, n->value, maxval));
}

template <typename T, typename Compare>
TreeSet<T, Compare>::TreeSet(const std::initializer_list<T> list) : TreeSet() {
    //std::cout << "Initializer list constructor" << std::endl;
    for (auto it = list.begin(); it != list.end(); it++) {
        add(*it); 
    }
}

template <typename T, typename Compare>
bool TreeSet<T, Compare>::operator==(const TreeSet &rhs) const {
    auto it1 = this->begin();
    auto it2 = rhs.begin();
    while (it1 != this->end() && it2 != rhs.end()) {
        if (*it1 != *it2) return false;
        it1++;
        it2++;
    }
    if (it1 != this->end() || it2 != rhs.end()) return false;
    return true;
}

template <typename T, typename Compare>
bool TreeSet<T, Compare>::operator!=(const TreeSet &rhs) const {
    return !(*this == rhs);
}

template <typename T, typename Compare>
TreeSet<T, Compare> TreeSet<T, Compare>::plus(const TreeSet &s) const {
    auto res = *this; // copy construct current set
    for (auto it = s.begin(); it != s.end(); it++) {
        if (!res.contains(*it)) {
            res.add(*it);
        }
    }
    return res;
}

template <typename T, typename Compare>
TreeSet<T, Compare> TreeSet<T, Compare>::minus(const TreeSet &s) const {
    auto res = *this; // copy construct current set
    for (auto it = s.begin(); it != s.end(); it++) {
        if (res.contains(*it)) {
            res.del(*it);
        }
    }
    return res;
}


template <typename T, typename Compare>
TreeSet<T, Compare> TreeSet<T, Compare>::intersect(const TreeSet &s) const {
    auto res = TreeSet{};
    for (auto it = s.begin(); it != s.end(); it++) {
        if (contains(*it)) {
            res.add(*it);
        }
    }
    return res;
}

template <typename T, typename Compare>
std::ostream& operator<<(std::ostream& os, const TreeSet<T, Compare>& s) {
    os << "[";
    auto it = s.begin();
    while (it != s.end()) {
        auto val = *it;
        os << val;
        it++;
        if (it != s.end()) {
            os << ",";
        }
    }
    os << "]";
    return os;
}

template <typename T, typename Compare>
TreeSetIter<T, Compare> TreeSet<T, Compare>::begin() const {
    auto iter = TreeSetIter<T, Compare>(*this);
    return iter;
}
template <typename T, typename Compare>
TreeSetIter<T, Compare> TreeSet<T, Compare>::end() const {
    return TreeSetIter<T, Compare>{};
}



// ============== TreeSetIter =========== // 
template <typename T, typename Compare>
void TreeSetIter<T, Compare>::populateNextSequence(SP_TreeNode<T> cur) {
    while (cur != nullptr) {
        entered.push_back(cur);
        cur = cur->left;
    }
}
template <typename T, typename Compare>
TreeSetIter<T, Compare>::TreeSetIter(const TreeSet<T, Compare>& set) : entered{}, cur_node{} {
    auto cur = set.root;
    populateNextSequence(cur);
    if (!entered.empty()) {
        cur_node = entered.back();
    }
}
template <typename T, typename Compare>
TreeSetIter<T, Compare> &TreeSetIter<T, Compare>::operator++() {
    //std::cout << "Pre increment" << std::endl;
    auto cur  = entered.back(); 
    entered.pop_back();
    auto next = cur->right;
    populateNextSequence(next);
    cur_node = entered.empty() ? SP_TreeNode<T>{} : entered.back();
    return *this;
}
template <typename T, typename Compare>
TreeSetIter<T, Compare> TreeSetIter<T, Compare>::operator++(int) {
    //std::cout << "Post increment" << std::endl;
    auto res = *this;
    ++(*this);
    //std::cout << "returning from post increment" << std::endl;
    return res;
}
template <typename T, typename Compare>
T TreeSetIter<T, Compare>::operator*() {
    assert(cur_node);
    return cur_node->value;
}
template <typename T, typename Compare>
bool TreeSetIter<T, Compare>::operator==(const TreeSetIter& rhs) const {
    return (cur_node == rhs.cur_node) && (entered == rhs.entered);
}
template <typename T, typename Compare>
bool TreeSetIter<T, Compare>::operator!=(const TreeSetIter& rhs) const {
    return !(*this == rhs);
}
