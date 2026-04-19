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

// ── ANSI colour strings ───────────────────────────────────────────────────────

const std::string RESET  = "\033[0m";
const std::string GREEN  = "\033[1;32m";   // colour index 1 — existing balanced node
const std::string YELLOW = "\033[1;33m";   // colour index 2 — just-inserted node
const std::string CYAN   = "\033[0;36m";   // colour index 3 — connector lines
const std::string RED    = "\033[1;31m";   // colour index 4 — AVL violation (|bf| > 1)
const std::string BOLD   = "\033[1m";

// ── UTF-8 box-drawing glyph strings ──────────────────────────────────────────

const std::string H_LINE = "\xe2\x94\x80";   // U+2500  ─
const std::string C_TL   = "\xe2\x94\x8c";   // U+250C  ┌
const std::string C_TR   = "\xe2\x94\x90";   // U+2510  ┐
const std::string C_BL   = "\xe2\x94\x94";   // U+2514  └
const std::string C_BR   = "\xe2\x94\x98";   // U+2518  ┘
const std::string C_T_UP = "\xe2\x94\xb4";   // U+2534  ┴

// ── Canvas ────────────────────────────────────────────────────────────────────

struct Canvas {
    int nr, nc;
    std::vector<std::vector<std::string>> g;   // glyph per cell
    std::vector<std::vector<int>>         k;   // colour index per cell (0–4)

    Canvas(int rows, int cols)
        : nr(rows), nc(cols),
          g(rows, std::vector<std::string>(cols, " ")),
          k(rows, std::vector<int>(cols, 0))
    {}

    void set(int r, int c, const std::string& glyph, int col = 0) {
        if (r < 0 || r >= nr || c < 0 || c >= nc) return;
        g[r][c] = glyph;
        k[r][c] = col;
    }

    void hline(int r, int c0, int c1, const std::string& glyph, int col) {
        for (int c = c0; c <= c1; ++c) {
            set(r, c, glyph, col);
        }
    }

    void print() const {
        for (int r = 0; r < nr; ++r) {
            int curCol = -1;
            for (int c = 0; c < nc; ++c) {
                int idx = k[r][c];
                if (idx != curCol) {
                    switch (idx) {
                        case 0:  std::cout << RESET;  break;
                        case 1:  std::cout << GREEN;  break;
                        case 2:  std::cout << YELLOW; break;
                        case 3:  std::cout << CYAN;   break;
                        case 4:  std::cout << RED;    break;
                        default: std::cout << RESET;  break;
                    }
                    curCol = idx;
                }
                std::cout << g[r][c];
            }
            std::cout << RESET << '\n';
        }
    }
};

// ── Pos ───────────────────────────────────────────────────────────────────────

struct Pos { int rank, depth; };

// ── gatherPos ─────────────────────────────────────────────────────────────────

void gatherPos(const Node* n, int depth, int& rank,
               std::unordered_map<const Node*, Pos>& out)
{
    if (!n) return;
    gatherPos(n->left.get(),  depth + 1, rank, out);
    out[n] = { rank++, depth };
    gatherPos(n->right.get(), depth + 1, rank, out);
}

// ── bfOf ─────────────────────────────────────────────────────────────────────

int bfOf(const Node* n) {
    int l = n->left  ? n->left->height_  : -1;
    int r = n->right ? n->right->height_ : -1;
    return r - l;
}

// ── renderNode ────────────────────────────────────────────────────────────────

void renderNode(Canvas& cv, const Node* n,
                const std::unordered_map<const Node*, Pos>& pos,
                int cellW, int rowH, int newKey)
{
    if (!n) return;

    const Pos& p   = pos.at(n);
    int nodeCol    = p.rank * cellW + cellW / 2;
    int nodeRow    = p.depth * rowH;

    // Colour selection (priority order)
    int colour;
    if (std::abs(bfOf(n)) > 1) {
        colour = 4;                        // RED — AVL violation
    } else if (n->key_ == newKey) {
        colour = 2;                        // YELLOW — just inserted
    } else {
        colour = 1;                        // GREEN — existing balanced
    }

    // Write key label centred on nodeCol
    std::string label    = std::to_string(n->key_);
    int         lblW     = static_cast<int>(label.size());
    int         lblStart = nodeCol - lblW / 2;
    for (int i = 0; i < lblW; ++i) {
        cv.set(nodeRow, lblStart + i, std::string(1, label[static_cast<std::size_t>(i)]), colour);
    }

    if (!n->left && !n->right) return;

    int connRow = nodeRow + 1;

    const Node* lc = n->left.get();
    const Node* rc = n->right.get();

    int lCol = lc ? pos.at(lc).rank * cellW + cellW / 2 : nodeCol;
    int rCol = rc ? pos.at(rc).rank * cellW + cellW / 2 : nodeCol;

    // Junction glyph at the parent column
    if (lc && rc) {
        cv.set(connRow, nodeCol, C_T_UP, 3);
    } else if (lc) {
        cv.set(connRow, nodeCol, C_BR, 3);
    } else {
        cv.set(connRow, nodeCol, C_BL, 3);
    }

    // Left branch
    if (lc) {
        cv.set(connRow, lCol, C_TL, 3);
        cv.hline(connRow, lCol + 1, nodeCol - 1, H_LINE, 3);
    }

    // Right branch
    if (rc) {
        cv.hline(connRow, nodeCol + 1, rCol - 1, H_LINE, 3);
        cv.set(connRow, rCol, C_TR, 3);
    }

    renderNode(cv, lc, pos, cellW, rowH, newKey);
    renderNode(cv, rc, pos, cellW, rowH, newKey);
}

} // anonymous namespace

// ── drawTree ──────────────────────────────────────────────────────────────────

void drawTree(const Tree& tree, int newKey, const std::string& header)
{
    // Clear terminal
    std::cout << "\033[2J\033[H";

    if (!header.empty()) {
        std::cout << BOLD << header << RESET << "\n\n";
    }

    const Node* root = tree.getRoot().get();
    if (!root) {
        std::cout << "  (empty tree)\n";
        std::cout.flush();
        return;
    }

    std::unordered_map<const Node*, Pos> posMap;
    int rankCounter = 0;
    gatherPos(root, 0, rankCounter, posMap);

    int maxDepth = 0;
    int maxLblW  = 0;
    for (const auto& [node, p] : posMap) {
        if (p.depth > maxDepth) maxDepth = p.depth;
        int lblW = static_cast<int>(std::to_string(node->key_).size());
        if (lblW > maxLblW) maxLblW = lblW;
    }

    int n     = rankCounter;
    int cellW = maxLblW + 2;
    int rowH  = 2;
    int ncols = n * cellW + 2;
    int nrows = maxDepth * rowH + 1;

    Canvas cv(nrows, ncols);
    renderNode(cv, root, posMap, cellW, rowH, newKey);
    cv.print();

    // Colour legend
    std::cout << '\n'
              << YELLOW << "\xe2\x96\xa0" << RESET << " newly inserted"
              << "   "
              << GREEN  << "\xe2\x96\xa0" << RESET << " balanced"
              << "   "
              << RED    << "\xe2\x96\xa0" << RESET << " AVL violation"
              << '\n';

    std::cout.flush();
}

// ── animateMode ───────────────────────────────────────────────────────────────

void animateMode(const std::filesystem::path& file, bool rebalance)
{
    auto keys = Tree::readKeys(file);
    if (keys.empty()) {
        throw std::runtime_error("File contains no keys: " + file.string());
    }

    Tree tree;
    tree.setRebalance(rebalance);

    int        inserted = 0;
    const int  total    = static_cast<int>(keys.size());
    const auto delay    = std::chrono::milliseconds(400);

    const std::string mode = rebalance
        ? "AVL ON"
        : "AVL OFF  (violations in red)";

    for (int key : keys) {
        if (tree.insert(key)) {
            ++inserted;
            std::string header =
                "  Inserting " + std::to_string(key) +
                "  (" + std::to_string(inserted) +
                " of " + std::to_string(total) +
                " total keys)  [" + mode + "]";
            drawTree(tree, key, header);
            std::this_thread::sleep_for(delay);
        }
    }

    drawTree(tree, -1,
        "  Done \xe2\x80\x94 " + std::to_string(tree.size()) +
        " unique keys  [" + mode + "]");

    std::cout << '\n';

    bool avl = tree.printBalance();
    tree.setAVL(avl);
    tree.printAVL();

    Stats s;
    s.getStats(tree.getRoot());
    s.printStats();
}
