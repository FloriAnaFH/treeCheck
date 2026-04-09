#ifndef TREE_H
#define TREE_H

class Node{
public: 
int key; 
Node* left;
Node* right;

Node(int key);

};


class Tree{
public: 
    Tree();
    void insert(int key);
    void printBalance();
    void printStats();



    private: 
    Node * root; 
    Node* insert(Node* node, int key);
    int height(Node* node);
    int balanceFactor(Node* node);
    bool printBalance(Node* node);
    int getMin(Node* node);
    int getMax(Node* node);
    void getStats(Node* node, int& count, int& sum);


};

#endif