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

Tree Tree::fromFile(const std::filesystem::path& path) {
    Tree tree;

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
    /* insert a key into the search tree - check for and ignore duplicate keys.
       Complexity: avg: O(log n), worst: O(n) */
    if (!node) {
        node = std::make_unique<Node>(key);
        grow();
        return true;
    }

    if (key < node->key_) {
        return insert_(node->left, key);
    }
    if (key > node->key_) {
        return insert_(node->right, key);
    }
    return false; // key already exists
}

std::string_view Tree::trim(std::string_view sv) {
    /* remove whitespace from both ends */
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
    /* tries to parse an integer - throws an error if parsing produces an error
       or not all characters were read */
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
    if (!node) {
        return -1;
    }

    int leftHeight = height(node->left);
    int rightHeight = height(node->right);

    return (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
}

int Tree::balanceFactor(const std::unique_ptr<Node>& node) {
    return height(node->right) - height(node->left);
}

/* search methods */

bool Tree::searchPath(const std::unique_ptr<Node>& node, int key, std::vector<int>& path) const {
    const Node* node_ = node.get();
    while (node_) {
        path.push_back(node_->key_);
        if (key == node_->key_) {
            return true;
        }
        node_ = (key < node_->key_) ? node_->left.get() : node_->right.get();
    }
    return false;
}

bool Tree::sameTree(const std::unique_ptr<Node>& a,
                    const std::unique_ptr<Node>& b) const {
    if (!a && !b) return true;
    if (!a || !b) return false;

    return a->key_ == b->key_ &&
           sameTree(a->left, b->left) &&
           sameTree(a->right, b->right);
}

bool Tree::containsSubtree(const std::unique_ptr<Node>& main,
                           const std::unique_ptr<Node>& subtree) const {
    if (!subtree) return true;
    if (!main) return false;
    if (sameTree(main, subtree)) return true;

    return containsSubtree(main->left, subtree) ||
           containsSubtree(main->right, subtree);
}
