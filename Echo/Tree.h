#pragma once
#include <string>
#include <vector>

using namespace std;
using uint = unsigned int;

template<typename T>
class TrieNode {
public:
    TrieNode* children[256];
    bool isEnd;
    T value;

    TrieNode() : isEnd(false) {
        for (int i = 0; i < 256; i++) children[i] = nullptr;
    }

    ~TrieNode() {
        for (int i = 0; i < 256; i++)
            if (children[i]) delete children[i];
    }
};

template<typename T>
class Trie {
private:
    TrieNode<T>* root;
    uint count;

    void insertRec(TrieNode<T>* node, const string& key, const T& value, size_t index) {
        if (index == key.length()) {
            node->isEnd = true;
            node->value = value;
            count++;
            return;
        }
        unsigned char c = static_cast<unsigned char>(key[index]);
        if (!node->children[c])
            node->children[c] = new TrieNode<T>();
        insertRec(node->children[c], key, value, index + 1);
    }

    TrieNode<T>* searchNodeRec(TrieNode<T>* node, const string& key, size_t index) const {
        if (!node) return nullptr;
        if (index == key.length()) return node;
        unsigned char c = static_cast<unsigned char>(key[index]);
        return searchNodeRec(node->children[c], key, index + 1);
    }

    void collectRec(TrieNode<T>* node, vector<T>& results) const {
        if (!node) return;
        if (node->isEnd) results.push_back(node->value);
        for (int i = 0; i < 256; i++) {
            if (node->children[i])
                collectRec(node->children[i], results);
        }
    }

public:
    Trie() : root(new TrieNode<T>()), count(0) {}

    // Destructor limpia todo el arbol
    ~Trie() { delete root; }

    // Evitar copia accidental
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;

    void insert(const string& key, const T& value) {
        insertRec(root, key, value, 0);
    }

    vector<T> searchPrefix(const string& prefix) const {
        vector<T> results;
        TrieNode<T>* node = searchNodeRec(root, prefix, 0);
        if (node) collectRec(node, results);
        return results;
    }

    T* searchExact(const string& key) const {
        TrieNode<T>* node = searchNodeRec(root, key, 0);
        return (node && node->isEnd) ? &(node->value) : nullptr;
    }

    void clear() {
        delete root;
        root = new TrieNode<T>();
        count = 0;
    }

    uint getSize() const { return count; }
};