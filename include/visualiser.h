#pragma once

#include "tree.h"
#include <filesystem>
#include <string>


// Draw the current state of the tree to stdout.
//
//   newKey  – the key just inserted, highlighted in yellow; pass -1 if none.
//   header  – optional status line printed above the tree (cleared each frame).
//
// Nodes whose balance factor falls outside [-1, 1] are highlighted in red.
//
void drawTree ( const Tree& tree, int newKey = -1, const std::string& header = "" );


// Build a tree from file, animating each insertion step in the terminal.
//
//   rebalance – when true the tree self-balances via AVL rotations after
//               every insertion (mirrors the --rebalance CLI flag).
//               Defaults to false (plain BST, violations shown in red).
//
// After all keys are inserted the final balance check and stats are printed.
//
void animateMode ( const std::filesystem::path& file, bool rebalance = false );
