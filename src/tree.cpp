#include "tree.h"
#include <iostream>

Node::Node(int key){
    this->key = key;
    this->left = nullptr;
    this->right = nullptr;
}

Tree::Tree(){
    this->root = nullptr;
}

Node *Tree::insert(Node *node, int key){
    if (node == nullptr)
        return new Node(key);
    

    if (key < node->key)
        node->left = insert(node->left, key);
    

    else if (key > node->key)
        node->right = insert(node->right, key);

    return node;
}


void Tree::insert(int key){ root= insert(root, key);}

int Tree::height(Node* node){
    if(node == nullptr)
        return  0;
    
    int leftHeight = height(node->left);
    int reightHeight =  height(node->right);

    if(leftHeight > reightHeight)
        return leftHeight + 1;
     else
        return reightHeight +1;
}