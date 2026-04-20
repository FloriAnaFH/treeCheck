#include "../include/visualiser.h"
#include "../include/node.h"
#include "../include/stats.h"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace {

/* ANSI colour escape strings used during terminal rendering. */
const std::string RESET  = "\033[0m";
const std::string GREEN  = "\033[1;32m"; // existing balanced node
const std::string YELLOW = "\033[1;33m"; // newly inserted node
const std::string CYAN   = "\033[0;36m"; // connector lines
const std::string RED    = "\033[1;31m"; // AVL violation (|bf| > 1)
const std::string BOLD   = "\033[1m";

/* UTF-8 box-drawing glyphs used for branch connectors. */
const std::string H_LINE = "\xe2\x94\x80"; // U+2500  ─
const std::string C_TL   = "\xe2\x94\x8c"; // U+250C  ┌
const std::string C_TR   = "\xe2\x94\x90"; // U+2510  ┐
const std::string C_BL   = "\xe2\x94\x94"; // U+2514  └
const std::string C_BR   = "\xe2\x94\x98"; // U+2518  ┘
const std::string C_T_UP = "\xe2\x94\xb4"; // U+2534  ┴

/* Terminal canvas that stores one glyph and colour index per cell. */
struct Canvas {
    int numRows;
    int numCols;
    std::vector<std::vector<std::string>> glyphs;
    std::vector<std::vector<int>> colours;

    Canvas(int rows, int cols)
        : numRows(rows)
        , numCols(cols)
        , glyphs(rows, std::vector<std::string>(cols, " "))
        , colours(rows, std::vector<int>(cols, 0)) {}

    void set(int row, int col, const std::string& glyph, int colourIdx = 0) {
        if (row < 0 || row >= numRows || col < 0 || col >= numCols) {
            return;
        }
        glyphs[row][col] = glyph;
        colours[row][col] = colourIdx;
    }

    /* Draw a horizontal line on one row from startCol to endCol (inclusive).
       Complexity: O(k) where k = endCol - startCol + 1 */
    void hline(int row, int startCol, int endCol, const std::string& glyph, int colourIdx) {
        for (int col = startCol; col <= endCol; ++col) {
            set(row, col, glyph, colourIdx);
        }
    }

    /* Print the canvas to stdout while emitting ANSI colours only on changes.
       Complexity: O(rows × cols) */
    void print() const {
        for (int row = 0; row < numRows; ++row) {
            int activeColour = -1;
            for (int col = 0; col < numCols; ++col) {
                const int cellColour = colours[row][col];
                if (cellColour != activeColour) {
                    switch (cellColour) {
                        case 1:
                            std::cout << GREEN;
                            break;
                        case 2:
                            std::cout << YELLOW;
                            break;
                        case 3:
                            std::cout << CYAN;
                            break;
                        case 4:
                            std::cout << RED;
                            break;
                        default:
                            std::cout << RESET;
                            break;
                    }
                    activeColour = cellColour;
                }
                std::cout << glyphs[row][col];
            }
            std::cout << RESET << '\n';
        }
    }
};

struct NodePos {
    int rank;  // in-order rank (horizontal position)
    int depth; // depth from root (vertical position)
};

/* Assign in-order rank and depth to every node.
   Complexity: O(n) — each node is visited exactly once */
void gatherPositions(const Node* node,
                     int depth,
                     int& rank,
                     std::unordered_map<const Node*, NodePos>& positions) {
    if (!node) {
        return;
    }

    gatherPositions(node->left.get(), depth + 1, rank, positions);
    positions[node] = { rank++, depth };
    gatherPositions(node->right.get(), depth + 1, rank, positions);
}

/* Compute balance factor from cached child heights.
   Complexity: O(1) */
int balanceFactorOf(const Node* node) {
    const int leftHeight = node->left ? node->left->height_ : -1;

    const int rightHeight = node->right ? node->right->height_ : -1;
    return rightHeight - leftHeight;
}

/* Render one node and connector row, then recurse into children.
   Complexity: O(1) per call, O(n) total */
void renderNode(Canvas& canvas,
                const Node* node,
                const std::unordered_map<const Node*, NodePos>& positions,
                int cellWidth,
                int rowHeight,
                int newKey) {
    if (!node) {
        return;
    }

    const NodePos& nodePos = positions.at(node);
    const int nodeCol = nodePos.rank * cellWidth + cellWidth / 2;
    const int nodeRow = nodePos.depth * rowHeight;

    int colourIdx = 1;
    if (std::abs(balanceFactorOf(node)) > 1) {
        colourIdx = 4;
    } else if (node->key_ == newKey) {
        colourIdx = 2;
    }

    const std::string label = std::to_string(node->key_);
    const int labelWidth = static_cast<int>(label.size());
    const int labelStart = nodeCol - labelWidth / 2;
    for (int i = 0; i < labelWidth; ++i) {
        canvas.set(nodeRow,
                   labelStart + i,
                   std::string(1, label[static_cast<std::size_t>(i)]),
                   colourIdx);
    }

    const Node* leftChild = node->left.get();
    const Node* rightChild = node->right.get();
    if (!leftChild && !rightChild) {
        return;
    }

    const int connectorRow = nodeRow + 1;
    const int leftChildCol =
        leftChild ? positions.at(leftChild).rank * cellWidth + cellWidth / 2 : nodeCol;
    const int rightChildCol =
        rightChild ? positions.at(rightChild).rank * cellWidth + cellWidth / 2 : nodeCol;

    if (leftChild && rightChild) {
        canvas.set(connectorRow, nodeCol, C_T_UP, 3);
    } else if (leftChild) {
        canvas.set(connectorRow, nodeCol, C_BR, 3);
    } else {
        canvas.set(connectorRow, nodeCol, C_BL, 3);
    }

    if (leftChild) {
        canvas.set(connectorRow, leftChildCol, C_TL, 3);
        canvas.hline(connectorRow, leftChildCol + 1, nodeCol - 1, H_LINE, 3);
    }

    if (rightChild) {
        canvas.hline(connectorRow, nodeCol + 1, rightChildCol - 1, H_LINE, 3);
        canvas.set(connectorRow, rightChildCol, C_TR, 3);
    }

    renderNode(canvas, leftChild, positions, cellWidth, rowHeight, newKey);
    renderNode(canvas, rightChild, positions, cellWidth, rowHeight, newKey);
}

} // namespace

/* Draw the full tree and colour legend in the terminal.
   Complexity: O(n) for traversal/layout plus output cost proportional to canvas size */
void drawTree(const Tree& tree, int newKey, const std::string& header) {
    std::cout << "\033[2J\033[H"; // clear terminal, move cursor to top-left

    if (!header.empty()) {
        std::cout << BOLD << header << RESET << "\n\n";
    }

    const Node* root = tree.getRoot().get();
    if (!root) {
        std::cout << "  (empty tree)\n";
        std::cout.flush();
        return;
    }

    std::unordered_map<const Node*, NodePos> positions;
    int nodeCount = 0;
    gatherPositions(root, 0, nodeCount, positions);

    int maxDepth = 0;
    int maxLabelWidth = 0;
    for (const auto& [nodePtr, nodePos] : positions) {
        maxDepth = std::max(maxDepth, nodePos.depth);
        maxLabelWidth =
            std::max(maxLabelWidth, static_cast<int>(std::to_string(nodePtr->key_).size()));
    }

    const int cellWidth = maxLabelWidth + 2;
    const int rowHeight = 2;
    const int canvasCols = nodeCount * cellWidth + 2;
    const int canvasRows = maxDepth * rowHeight + 1;

    Canvas canvas(canvasRows, canvasCols);
    renderNode(canvas, root, positions, cellWidth, rowHeight, newKey);
    canvas.print();

    std::cout << '\n'
              << YELLOW << "\xe2\x96\xa0" << RESET << " newly inserted"
              << "   "
              << GREEN << "\xe2\x96\xa0" << RESET << " balanced"
              << "   "
              << RED << "\xe2\x96\xa0" << RESET << " AVL violation"
              << '\n';

    std::cout.flush();
}

/* Animate insertions from file, then print final AVL report and stats.
   Complexity: dominated by repeated drawTree calls across all successful inserts */
void animateMode(const std::filesystem::path& file, bool rebalance) {
    const auto keys = Tree::readKeys(file);
    if (keys.empty()) {
        throw std::runtime_error("File contains no keys: " + file.string());
    }

    Tree tree;
    tree.setRebalance(rebalance);

    int inserted = 0;
    const int totalKeys = static_cast<int>(keys.size());
    const auto frameDelay = std::chrono::milliseconds(400);

    const std::string modeLabel = rebalance ? "AVL ON" : "AVL OFF  (violations in red)";

    for (const int key : keys) {
        if (tree.insert(key)) {
            ++inserted;
            const std::string header =
                "  Inserting " + std::to_string(key) +
                "  (" + std::to_string(inserted) +
                " of " + std::to_string(totalKeys) +
                " total keys)  [" + modeLabel + "]";
            drawTree(tree, key, header);
            std::this_thread::sleep_for(frameDelay);
        }
    }

    drawTree(tree,
             -1,
             "  Done \xe2\x80\x94 " + std::to_string(tree.size()) +
                 " unique keys  [" + modeLabel + "]");

    std::cout << '\n';

    const bool isAvl = tree.printBalance();
    tree.setAVL(isAvl);
    tree.printAVL();

    Stats stats;
    stats.getStats(tree.getRoot());
    stats.printStats();
}
