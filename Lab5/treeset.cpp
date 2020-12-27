#include "treeset.h"
#include <assert.h>
#include <iostream>

#define DEBUG 0
TreeSet::TreeSet() : root{}, size_{0} {}

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

bool TreeSet::contains(int value) {
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
    root = other.root;
}
TreeSet& TreeSet::operator=(const TreeSet& other) {
    size_ = other.size_;
    // Copy assign root node using recursive copy assignment from TreeNode
    root = other.root;
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

