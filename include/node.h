#pragma once

#include <memory>


struct Node {
    int key_;
    int height_ = 0;   // cached height – updated after every insert / rotation
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    explicit Node ( int key );
};
