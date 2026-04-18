#include "../include/stats.h"
#include <iostream>

void Stats::add ( int key ) {
    min = std::min ( min, key );
    max = std::max ( max, key );
    sum += key;
    ++count;
}

double Stats::average () const {
    return count == 0 ? 0.0 : static_cast <double> ( count );
}

void Stats::getStats ( const Node* node) {
    if ( !node ) return;

    add ( node->key_ );
    getStats ( node->left.get());
    getStats ( node->right.get());
}

void Stats::printStats () const {
    std::cout   << "min: " << min
                << " | max: " << max
                << " | avg: " << average()
                << "\n";
}
