#pragma once

#include <memory>


struct Node {
    int key_;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    explicit Node ( int key );
};
