#pragma once

#include "tree.h"
#include <filesystem>
#include <string>

/* Draw the current tree state to stdout.
   Highlights the newly inserted key (if any) and marks AVL violations in red.
   Complexity: O(n) for traversal/layout plus output cost proportional to canvas size */
void drawTree(const Tree& tree, int newKey = -1, const std::string& header = "");

/* Build and animate a tree from file by rendering after each successful insert.
   When rebalance is true, AVL rotations are applied after each insertion.
   Prints final balance report and stats after animation completes.
   Complexity: dominated by repeated drawTree calls across all successful inserts */
void animateMode(const std::filesystem::path& file, bool rebalance = false);
