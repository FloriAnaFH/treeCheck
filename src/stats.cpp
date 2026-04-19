#include "../include/stats.h"
#include <iostream>
#include <algorithm>

/* Complexity: O(1) */
void Stats::add ( int key ) {
    min = std::min ( min, key );
    max = std::max ( max, key );
    sum += key;
    ++count;
}

/* Complexity: O(1) */
double Stats::average () const {
    return count == 0 ? 0.0 : static_cast<double>(sum) / static_cast<double>(count);
}

/* Complexity: O(n) — pre-order traversal visits every node exactly once */
void Stats::getStats ( const std::unique_ptr <Node>& node) {
    if ( !node ) return;

    add ( node->key_ );
    getStats ( node->left);
    getStats ( node->right);
}

/* Complexity: O(1) */
void Stats::printStats () const {
    std::cout   << "min: " << min
                << " | max: " << max
                << " | avg: " << average()
                << "\n";
}
