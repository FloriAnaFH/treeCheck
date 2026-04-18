#ifndef TREE_H
#define TREE_H

class Node {
public:
    int key;
    Node *left;
    Node *right;

    Node(int key);

};


class Tree {
public:
    Tree();
    void insert(int key);
    static void printBalance(Node *node, bool &avl);
    void printBalance() const;
    void printStats();
    bool isAVL() const;

private:
    Node *root;

    static Node *insert(Node *node, int key);

    static bool isAVL(Node *node);

    static int height(Node *node);

    static int balanceFactor(Node *node);
//die brauchen wir nicht
    static Node *rotateLeft(Node *node);
    static Node *rotateRight(Node *node);
    static Node *rotateLeftRight(Node *node);
    static Node *rotateRightLeft(Node *node);

    int getMin(Node *node);

    int getMax(Node *node);

    static void getStats(Node *node, int &count, int &sum);
};

#endif
