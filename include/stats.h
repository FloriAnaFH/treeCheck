#pragma once

#include "node.h"
#include <memory>
#include <limits>

struct Stats {
    int min = std::numeric_limits <int>::max();
    int max = std::numeric_limits <int>::min();
    long long sum = 0;
    std::size_t count = 0;

    void add ( int key );
    double average () const;
    void getStats ( const Node* node );
    void printStats () const;

};
