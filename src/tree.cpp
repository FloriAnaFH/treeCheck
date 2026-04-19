#include "../include/tree.h"
#include "../include/node.h"
#include <charconv>
#include <cctype>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string_view>

Node::Node(int key) : key_(key) {}

/* public functions */

bool Tree::insert(int key) { return insert_(root, key); }

bool Tree::printBalance() const {
    return printBalance_(root.get());
}

void Tree::setRebalance(bool enabled) {
    rebalance_ = enabled;
}

std::vector<int> Tree::readKeys(const std::filesystem::path& path) {
    /* Parses every non-blank line of the file into an integer without
       inserting anything into a tree.
       Complexity: O(n) where n = number of lines in the file */
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("Can't open file: " + path.string());
    }

    std::vector<int> keys;
    std::string line;
    std::size_t lineNr = 0;

    while (std::getline(input, line)) {
        ++lineNr;
        std::string_view sv = trim(line);
        if (sv.empty()) continue;
        keys.push_back(parseInt(sv, path, lineNr));
    }

    return keys;
}

Tree Tree::fromFile(const std::filesystem::path& path, bool rebalance) {
    /* Reads all keys from the file and inserts them into a new tree.
       Complexity: avg O(n log n),
                   worst O(n log n) [AVL on] / O(n²) [AVL off]
                   where n = number of unique keys */
    Tree tree;
    tree.setRebalance(rebalance);

    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("Can't open file: " + path.string());
    }

    std::string line;
    std::size_t lineNr = 0;

    while (std::getline(input, line)) {
        ++lineNr;

        std::string_view sv = trim(line);
        if (sv.empty()) {
            continue;
        }

        const int key = parseInt(sv, path, lineNr);
        tree.insert(key); // duplicates are ignored by insert_
    }

    return tree;
}

void Tree::printAVL() const {
    std::cout << "AVL: " << (isAVL() ? "YES" : "NO") << "\n";
}

void Tree::printStats() {
    // Intentionally left empty for now.
    // Stats functionality is implemented via the separate Stats type.
}

void Tree::setAVL(bool value) {
    avl = value;
}

void Tree::grow() { ++size_; }

const std::unique_ptr<Node>& Tree::getRoot() const { return root; }

std::optional<int> Tree::singleKey() const noexcept {
    if (size_ == 1 && root) {
        return root->key_;
    }
    return std::nullopt;
}

/* private functions */

bool Tree::insert_(std::unique_ptr<Node>& node, int key) {
    /* Insert a key into the search tree; duplicate keys are silently ignored.
       height_ is always kept up to date so printBalance_ has O(1) lookups.
       Rotations are only applied when rebalance_ is true.
       Complexity: avg O(log n),
                   worst O(log n) [AVL on] / O(n) [AVL off] */
    if (!node) {
        node = std::make_unique<Node>(key);
        grow();
        return true;
    }

    bool inserted = false;
    if (key < node->key_) {
        inserted = insert_(node->left, key);
    } else if (key > node->key_) {
        inserted = insert_(node->right, key);
    } else {
        return false; // key already exists
    }

    if (inserted) {
        updateHeight(node);
        if (rebalance_) rebalance(node);
    }
    return inserted;
}

std::string_view Tree::trim(std::string_view sv) {
    /* Remove leading and trailing whitespace.
       Complexity: O(k) where k = string length */
    while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.front()))) {
        sv.remove_prefix(1);
    }

    while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.back()))) {
        sv.remove_suffix(1);
    }
    return sv;
}

int Tree::parseInt(std::string_view sv,
                   const std::filesystem::path& path,
                   std::size_t lineNr) {
    /* Parse a decimal integer; throw if the input contains non-digit
       characters or does not consume the entire string.
       Complexity: O(k) where k = string length */
    int value{};
    const char* begin = sv.data();
    const char* end = begin + sv.size();
    const auto [ptr, ec] = std::from_chars(begin, end, value);

    if (ec != std::errc{} || ptr != end) {
        std::ostringstream msg;
        msg << "Invalid integer in " << path.string() << " at line " << lineNr
            << " : '" << sv << "'";
        throw std::runtime_error(msg.str());
    }

    return value;
}

bool Tree::printBalance_(const Node* node) const {
    /* Print the balance factor of every node in pre-order and return
       true iff all balance factors lie in [-1, 1].
       Complexity: O(n) — visits every node exactly once */
    if (!node) {
        return true;
    }

    const int rightHeight = height(node->right);
    const int leftHeight = height(node->left);
    const int bf = rightHeight - leftHeight;

    std::cout << "bal(" << node->key_ << ") = " << bf;
    const bool okHere = (bf >= -1 && bf <= 1);
    if (!okHere) {
        std::cout << " (AVL violation)";
    }
    std::cout << "\n";

    const bool leftOk = printBalance_(node->left.get());
    const bool rightOk = printBalance_(node->right.get());
    return okHere && leftOk && rightOk;
}

int Tree::height(const std::unique_ptr<Node>& node) {
    /* O(1) — reads the value cached in the node by updateHeight */
    return node ? node->height_ : -1;
}

void Tree::updateHeight(const std::unique_ptr<Node>& node) {
    /* O(1) — two O(1) height lookups plus arithmetic */
    const int leftHeight  = height(node->left);
    const int rightHeight = height(node->right);
    node->height_ = (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
}

int Tree::balanceFactor(const std::unique_ptr<Node>& node) {
    /* O(1) — two O(1) height lookups */
    return height(node->right) - height(node->left);
}

/* ── Rotations ──────────────────────────────────────────────────────────────
 *
 *  rotateLeft  – fixes a Right-Right imbalance:
 *
 *      x                y
 *       \      →       / \
 *        y            x   B
 *       / \            \
 *      A   B            A
 *
 *  rotateRight – fixes a Left-Left imbalance (mirror image).
 *
 *  Complexity: O(1) — pointer rewiring and two updateHeight calls
 */

std::unique_ptr<Node> Tree::rotateLeft(std::unique_ptr<Node> oldRoot) {
    std::unique_ptr<Node> newRoot = std::move(oldRoot->right);
    oldRoot->right = std::move(newRoot->left);
    updateHeight(oldRoot);
    newRoot->left = std::move(oldRoot);
    updateHeight(newRoot);
    return newRoot;
}

std::unique_ptr<Node> Tree::rotateRight(std::unique_ptr<Node> oldRoot) {
    std::unique_ptr<Node> newRoot = std::move(oldRoot->left);
    oldRoot->left = std::move(newRoot->right);
    updateHeight(oldRoot);
    newRoot->right = std::move(oldRoot);
    updateHeight(newRoot);
    return newRoot;
}

/* ── Rebalance ──────────────────────────────────────────────────────────────
 *
 *  Four cases (bf = height(right) – height(left)):
 *
 *  bf < -1  →  Left-heavy
 *    bf(left) <= 0  →  Left-Left  : single right rotation
 *    bf(left)  > 0  →  Left-Right : left rotation on child, then right
 *
 *  bf >  1  →  Right-heavy
 *    bf(right) >= 0  →  Right-Right : single left rotation
 *    bf(right)  < 0  →  Right-Left  : right rotation on child, then left
 *
 *  Complexity: O(1) — at most two rotations, each O(1)
 */

void Tree::rebalance(std::unique_ptr<Node>& node) {
    const int bf = balanceFactor(node);

    if (bf < -1) {
        if (balanceFactor(node->left) > 0)
            node->left = rotateLeft(std::move(node->left));   // LR → LL
        node = rotateRight(std::move(node));
    } else if (bf > 1) {
        if (balanceFactor(node->right) < 0)
            node->right = rotateRight(std::move(node->right)); // RL → RR
        node = rotateLeft(std::move(node));
    }
}

/* search methods */

bool Tree::searchPath(const std::unique_ptr<Node>& node, int key, std::vector<int>& path) const {
    /* Walk from the given node toward the target key, recording every
       visited key in path (whether or not the key is found).
       Complexity: avg O(log n),
                   worst O(log n) [AVL on] / O(n) [AVL off] */
    const Node* current = node.get();
    while (current) {
        path.push_back(current->key_);
        if (key == current->key_) {
            return true;
        }
        current = (key < current->key_) ? current->left.get() : current->right.get();
    }
    return false;
}

bool Tree::sameTree(const std::unique_ptr<Node>& a,
                    const std::unique_ptr<Node>& b) const {
    /* Return true iff both trees have identical structure and keys.
       Complexity: avg O(1) if root keys differ (fails immediately),
                   worst O(min(|a|, |b|)) for structurally identical trees */
    if (!a && !b) return true;
    if (!a || !b) return false;

    return a->key_ == b->key_ &&
           sameTree(a->left, b->left) &&
           sameTree(a->right, b->right);
}

bool Tree::containsSubtree(const std::unique_ptr<Node>& main,
                           const std::unique_ptr<Node>& subtree) const {
    /* Return true iff subtree appears as a structurally identical
       connected subgraph anywhere within main.
       Complexity: avg O(n) — sameTree fails fast on key mismatches,
                   worst O(n × m) where n = |main|, m = |subtree| */
    if (!subtree) return true;
    if (!main) return false;
    if (sameTree(main, subtree)) return true;

    return containsSubtree(main->left, subtree) ||
           containsSubtree(main->right, subtree);
}
