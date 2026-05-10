#pragma once
#include <vector>
#include <functional>

using uint = unsigned int;

template<typename T>
class BSTNode {
public:
    T data;
    BSTNode<T>* left;
    BSTNode<T>* right;
    BSTNode(T d) : data(d), left(nullptr), right(nullptr) {}
};

template<typename T>
class BinarySearchTree {
private:
    BSTNode<T>* root;
    uint count;
    std::function<bool(const T&, const T&)> comparator;

    BSTNode<T>* insertRec(BSTNode<T>* node, const T& value) {
        if (!node) {
            count++;
            return new BSTNode<T>(value);
        }
        if (comparator(value, node->data))
            node->left = insertRec(node->left, value);
        else
            node->right = insertRec(node->right, value);
        return node;
    }

    void inorderRec(BSTNode<T>* node, std::vector<T*>& out) const {
        if (!node) return;
        inorderRec(node->left, out);
        out.push_back(const_cast<T*>(&(node->data)));
        inorderRec(node->right, out);
    }

    BSTNode<T>* searchRec(BSTNode<T>* node, const T& value) const {
        if (!node) return nullptr;
        bool less = comparator(value, node->data);
        bool greater = comparator(node->data, value);
        if (!less && !greater) return node;
        if (less) return searchRec(node->left, value);
        return searchRec(node->right, value);
    }

    void clearRec(BSTNode<T>* node) {
        if (!node) return;
        clearRec(node->left);
        clearRec(node->right);
        delete node;
    }

public:
    BinarySearchTree(std::function<bool(const T&, const T&)> comp)
        : root(nullptr), count(0), comparator(comp) {
    }

    // Destructor: libera toda la memoria del arbol
    ~BinarySearchTree() { clear(); }

    // Evitar copia accidental (evita double-free)
    BinarySearchTree(const BinarySearchTree&) = delete;
    BinarySearchTree& operator=(const BinarySearchTree&) = delete;

    void insert(const T& value) { root = insertRec(root, value); }

    std::vector<T*> inorder() const {
        std::vector<T*> result;
        inorderRec(root, result);
        return result;
    }

    T* search(const T& value) const {
        BSTNode<T>* res = searchRec(root, value);
        return res ? &(res->data) : nullptr;
    }

    void clear() {
        clearRec(root);
        root = nullptr;
        count = 0;
    }

    uint getSize() const { return count; }
    bool isEmpty() const { return root == nullptr; }
};