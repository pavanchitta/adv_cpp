#include "treeset.h"
#include <assert.h>

#define DEBUG 0
TreeSet::TreeSet() : root{nullptr}, size_{0} {}

bool TreeSet::add(int value) {
    if (DEBUG)
        std::cout << "Adding value: " << value << std::endl;
    assert(sanity_check(root, INT_MIN, INT_MAX));
    if (contains(value)) {
        return false;
    }
    auto cur = root;
    while (true) {
        if (cur == nullptr) {
            root = std::make_shared<TreeNode>(value);
            size_++;
            return true;
        }
        if (value <= cur->value) {
            if (cur->left == nullptr) {
                cur->left = std::make_shared<TreeNode>(value);
                size_++;
                return true;
            }
            cur = cur->left;
        }
        else {
            if (cur->right == nullptr) {
                cur->right = std::make_shared<TreeNode>(value);
                size_++;
                return true;
            }
            cur = cur->right;
        }
    }
}

int TreeSet::size() {
    return size_;
}

bool TreeSet::contains(int value) const {
    if (DEBUG)
        std::cout << "Checking contains: " << value << std::endl; 
    auto cur = root;
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
        if (value <= cur->value) {
            cur = cur->left;
        }
        else {
            cur = cur->right;
        }
    }
}

bool TreeSet::del(int value) {
    if (DEBUG)
        std::cout << "Deleting value " << value << std::endl;
    if (!contains(value)) return false;
    SP_TreeNode cur = root;
    SP_TreeNode parent;
    while (true) {
        assert(cur != nullptr);
        if (cur->value == value) {
            if (DEBUG)
                std::cout << "Found node to delete value: " << value << std::endl;
            break;
        }
        if (value <= cur->value) {
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
        int successor_val = cur->right->findMinChild();
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
            auto null = SP_TreeNode{};
            assert(null == nullptr);
            parent->replaceChild(cur, null);
        }
    }
    size_--;
    return true;
}

TreeSet::TreeSet(const TreeSet& other) {
    size_ = other.size_;
    // Copy assign root node using recursive copy assignment from TreeNode
    if (other.root) {
        auto copy = *other.root;
        root = std::make_shared<TreeNode>(copy);
    }
    else {
        root = SP_TreeNode{};
    }
}
TreeSet& TreeSet::operator=(const TreeSet& other) {
    size_ = other.size_;
    // Copy assign root node using recursive copy assignment from TreeNode
    if (other.root) {
        auto copy = *other.root;
        root = std::make_shared<TreeNode>(copy);
    }
    else {
        root = SP_TreeNode{};
    }
    return *this;
}

bool TreeSet::sanity_check(SP_TreeNode n, int minval, int maxval) {

    if (!n) return true;
    if (n->value > maxval || n->value < minval) {
        std::cerr << "Sanity check failed: " << n->value << " min: " << minval << " max: " << maxval << std::endl;
        return false;
    }
    return (sanity_check(n->left, minval, n->value) && sanity_check(n->right, n->value, maxval));
}

TreeSet::TreeSet(const std::initializer_list<int> list) : TreeSet() {
    //std::cout << "Initializer list constructor" << std::endl;
    for (auto it = list.begin(); it != list.end(); it++) {
        add(*it); 
    }
}

bool TreeSet::operator==(const TreeSet &rhs) const {
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
bool TreeSet::operator!=(const TreeSet &rhs) const {
    return !(*this == rhs);
}
TreeSet TreeSet::plus(const TreeSet &s) const {
    auto res = *this; // copy construct current set
    for (auto it = s.begin(); it != s.end(); it++) {
        if (!res.contains(*it)) {
            res.add(*it);
        }
    }
    return res;
}
TreeSet TreeSet::minus(const TreeSet &s) const {
    auto res = *this; // copy construct current set
    for (auto it = s.begin(); it != s.end(); it++) {
        if (res.contains(*it)) {
            res.del(*it);
        }
    }
    return res;
}
TreeSet TreeSet::intersect(const TreeSet &s) const {
    auto res = TreeSet{};
    for (auto it = s.begin(); it != s.end(); it++) {
        if (contains(*it)) {
            res.add(*it);
        }
    }
    return res;
}

std::ostream& operator<<(std::ostream& os, const TreeSet& s) {
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

TreeSetIter TreeSet::begin() const {
    auto iter = TreeSetIter(*this);
    return iter;
}
TreeSetIter TreeSet::end() const {
    return TreeSetIter{};
}
// ============== TreeSetIter =========== // 
void TreeSetIter::populateNextSequence(SP_TreeNode cur) {
    while (cur != nullptr) {
        entered.push_back(cur);
        cur = cur->left;
    }
}
TreeSetIter::TreeSetIter(const TreeSet& set) : entered{}, cur_node{} {
    auto cur = set.root;
    populateNextSequence(cur);
    if (!entered.empty()) {
        cur_node = entered.back();
    }
}
TreeSetIter &TreeSetIter::operator++() {
    //std::cout << "Pre increment" << std::endl;
    auto cur  = entered.back(); 
    entered.pop_back();
    auto next = cur->right;
    populateNextSequence(next);
    cur_node = entered.empty() ? SP_TreeNode{} : entered.back();
    return *this;
}
TreeSetIter TreeSetIter::operator++(int) {
    //std::cout << "Post increment" << std::endl;
    auto res = *this;
    ++(*this);
    //std::cout << "returning from post increment" << std::endl;
    return res;
}
int TreeSetIter::operator*() {
    assert(cur_node);
    return cur_node->value;
}
bool TreeSetIter::operator==(const TreeSetIter& rhs) const {
    return (cur_node == rhs.cur_node) && (entered == rhs.entered);
}
bool TreeSetIter::operator!=(const TreeSetIter& rhs) const {
    return !(*this == rhs);
}
