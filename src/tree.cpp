#include "tree.h"
#include <iostream>

Node::Node(int key) : key(key), left(nullptr), right(nullptr) {
}

Tree::Tree() : root(nullptr) {
}

void Tree::insert(int key) {
    root = insert(root, key);
}

void Tree::printBalance(Node *node, bool &avl) {
    if (node == nullptr)
        return;

    printBalance(node->right, avl);
    printBalance(node->left, avl);

    int bf = balanceFactor(node);
    std::cout << "bal(" << node->key << ") = " << bf;
    if (bf > 1 || bf < -1) {
        std::cout << " (AVL violation!)";
        avl = false;
    }
    std::cout << std::endl;
}


void Tree::printBalance() const {
    bool avl = true;
    printBalance(root, avl);
    if (avl)
        std::cout << "AVL: yes" << std::endl;
    else
        std::cout << "AVL: no" << std::endl;
}

void Tree::printStats() {
    if (root == nullptr)
        return;
    int count = 0, sum = 0;
    getStats(root, count, sum);
    double avg = (double) sum / count;
    std::cout << "min: " << getMin(root)
            << ", max: " << getMax(root)
            << ", avg: " << avg << std::endl;
}

Node *Tree::insert(Node *node, int key) {
    if (node == nullptr)
        return new Node(key);
    if (key < node->key)
        node->left = insert(node->left, key);
    else if (key > node->key)
        node->right = insert(node->right, key);
    return node;
}

/* insert für AVL Baum mit rotationen.
 *
Node *Tree::insert(Node *node, int key){
    if (node == nullptr)
        return new Node(key);

    if (key < node->key)
        node->left = insert(node->left, key);
    else if (key > node->key)
        node->right = insert(node->right, key);

    int bf = balanceFactor(node);

    if (bf > 1) {
        if (balanceFactor(node->right) < 0)
            node = roteteRightLeft(node);
        else
            node = rotateLeft(node);
    }
    else if (bf < -1) {
        if (balanceFactor(node->left) > 0)
            node = rotateLeftRight(node);
        else
            node = rotateRight(node);
    }

    return node;
}*/
bool Tree::isAVL(Node *node) {
    if (node == nullptr)
        return true;

    int bf = balanceFactor(node);
    if (bf > 1 || bf < -1)
        return false;

    return isAVL(node->left) && isAVL(node->right);
}

bool Tree::isAVL() const {
    return isAVL(root);
}

int Tree::height(Node *node) {
    if (node == nullptr)
        return -1;

    int leftHeight = height(node->left);
    int rightHeight = height(node->right);

    return (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
}


int Tree::balanceFactor(Node *node) {
    return height(node->right) - height(node->left);
}

Node *Tree::rotateLeft(Node *node) {
    Node *newRoot = node->right;
    node->right = newRoot->left;
    newRoot->left = node;
    return newRoot;
}

Node *Tree::rotateRight(Node *node) {
    Node *newRoot = node->left;
    node->left = newRoot->right;
    newRoot->right = node;
    return newRoot;
}


Node *Tree::rotateLeftRight(Node *node) {
    node->left = rotateLeft(node->left);
    return rotateRight(node);
}

Node *Tree::rotateRightLeft(Node *node) {
    node->right = rotateRight(node->right);
    return rotateLeft(node);
}

int Tree::getMin(Node *node) {
    if (node->left == nullptr)
        return node->key;
    return getMin(node->left);
}

int Tree::getMax(Node *node) {
    if (node->right == nullptr)
        return node->key;
    return getMax(node->right);
}

void Tree::getStats(Node *node, int &count, int &sum) {
    if (node == nullptr)
        return;
    count++;
    sum += node->key;
    getStats(node->left, count, sum);
    getStats(node->right, count, sum);
}
