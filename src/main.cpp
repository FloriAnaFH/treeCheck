#include <iostream>
#include <fstream>
#include "../include/tree.h"


int main (int argc, char* argv[]){
     if (argc < 2) {
        std::cerr << "Usage: treecheck <filename>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);

    if (!file.is_open()) {
        std::cerr << "Error: could not open file " << argv[1] << std::endl;
        return 1;
    }
    Tree tree;
    int key;

    while (file >> key) {
        tree.insert(key);
    }

    file.close();

    tree.printBalance();
    tree.printStats();

    return 0;
}
