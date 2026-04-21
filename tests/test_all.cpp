#include "../include/tree.h"
#include "../include/stats.h"
#include "../include/node.h"

#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// ─── Minimal test framework ───────────────────────────────────────────────────

static int s_passed = 0;
static int s_failed = 0;

static void run_test(const char* name, const std::function<void()>& fn) {
    try {
        fn();
        std::cout << "  [PASS] " << name << "\n";
        ++s_passed;
    } catch (const std::exception& e) {
        std::cerr << "  [FAIL] " << name << ": " << e.what() << "\n";
        ++s_failed;
    } catch (...) {
        std::cerr << "  [FAIL] " << name << ": (unknown exception)\n";
        ++s_failed;
    }
}

#define TEST(name, ...) run_test(name, [&]() { __VA_ARGS__ })

#define ASSERT(cond) \
    do { \
        if (!(cond)) { \
            std::ostringstream _oss; \
            _oss << "ASSERT(" #cond ") failed at line " << __LINE__; \
            throw std::runtime_error(_oss.str()); \
        } \
    } while(0)

#define ASSERT_EQ(a, b) \
    do { \
        auto _a = (a); auto _b = (b); \
        if (_a != _b) { \
            std::ostringstream _oss; \
            _oss << "ASSERT_EQ failed at line " << __LINE__ \
                 << ": " << _a << " != " << _b; \
            throw std::runtime_error(_oss.str()); \
        } \
    } while(0)

#define ASSERT_NEAR(a, b, eps) \
    do { \
        double _a = static_cast<double>(a); \
        double _b = static_cast<double>(b); \
        if (std::abs(_a - _b) > (eps)) { \
            std::ostringstream _oss; \
            _oss << "ASSERT_NEAR failed at line " << __LINE__ \
                 << ": " << _a << " !~= " << _b; \
            throw std::runtime_error(_oss.str()); \
        } \
    } while(0)

#define ASSERT_THROWS(expr) \
    do { \
        bool _threw = false; \
        try { expr; } catch (...) { _threw = true; } \
        if (!_threw) { \
            std::ostringstream _oss; \
            _oss << "Expected exception not thrown: " #expr \
                 << " at line " << __LINE__; \
            throw std::runtime_error(_oss.str()); \
        } \
    } while(0)

// Redirect std::cout to a sink for the duration of the scope.
// Useful for methods that print as a side-effect (printBalance, printAVL, …).
struct SilentOut {
    std::streambuf* saved;
    std::ostringstream sink;
    SilentOut()  { saved = std::cout.rdbuf(sink.rdbuf()); }
    ~SilentOut() { std::cout.rdbuf(saved); }
};

// ─── Helpers ──────────────────────────────────────────────────────────────────

static Tree make_tree(std::initializer_list<int> keys) {
    Tree t;
    for (int k : keys) t.insert(k);
    return t;
}

// ─── Tree: insert / size / getRoot ───────────────────────────────────────────

static void test_insert() {
    std::cout << "\n── Tree::insert / size / getRoot ──\n";

    TEST("empty tree has size 0", {
        Tree t;
        ASSERT_EQ(t.size(), std::size_t(0));
    });

    TEST("empty tree has null root", {
        Tree t;
        ASSERT(t.getRoot() == nullptr);
    });

    TEST("single insert increases size to 1", {
        Tree t;
        ASSERT(t.insert(5) == true);
        ASSERT_EQ(t.size(), std::size_t(1));
    });

    TEST("root holds the first-inserted key", {
        Tree t;
        t.insert(42);
        ASSERT(t.getRoot() != nullptr);
        ASSERT_EQ(t.getRoot()->key_, 42);
    });

    TEST("inserting N distinct keys yields size N", {
        Tree t = make_tree({5, 3, 7, 1, 4, 6, 8});
        ASSERT_EQ(t.size(), std::size_t(7));
    });

    TEST("duplicate key is silently ignored and returns false", {
        Tree t;
        ASSERT(t.insert(10) == true);
        ASSERT(t.insert(10) == false);
        ASSERT_EQ(t.size(), std::size_t(1));
    });

    TEST("multiple duplicates do not grow the tree", {
        Tree t = make_tree({5, 5, 5, 3, 3});
        ASSERT_EQ(t.size(), std::size_t(2));
    });

    TEST("BST ordering: smaller keys go left", {
        Tree t;
        t.insert(10);
        t.insert(5);
        ASSERT(t.getRoot()->left != nullptr);
        ASSERT_EQ(t.getRoot()->left->key_, 5);
    });

    TEST("BST ordering: larger keys go right", {
        Tree t;
        t.insert(10);
        t.insert(15);
        ASSERT(t.getRoot()->right != nullptr);
        ASSERT_EQ(t.getRoot()->right->key_, 15);
    });
}

// ─── Tree::singleKey ─────────────────────────────────────────────────────────

static void test_single_key() {
    std::cout << "\n── Tree::singleKey ──\n";

    TEST("singleKey returns nullopt for empty tree", {
        Tree t;
        ASSERT(!t.singleKey().has_value());
    });

    TEST("singleKey returns the key when size is 1", {
        Tree t;
        t.insert(99);
        auto opt = t.singleKey();
        ASSERT(opt.has_value());
        ASSERT_EQ(*opt, 99);
    });

    TEST("singleKey returns nullopt for multi-node tree", {
        Tree t = make_tree({1, 2, 3});
        ASSERT(!t.singleKey().has_value());
    });
}

// ─── Tree::searchPath ────────────────────────────────────────────────────────

static void test_search_path() {
    std::cout << "\n── Tree::searchPath ──\n";

    //        5
    //       / \
    //      3   7
    Tree t = make_tree({5, 3, 7});

    TEST("search on empty tree returns false with empty path", {
        Tree empty;
        std::vector<int> path;
        ASSERT(!empty.searchPath(empty.getRoot(), 5, path));
        ASSERT(path.empty());
    });

    TEST("search finds root; path contains only root", {
        std::vector<int> path;
        ASSERT(t.searchPath(t.getRoot(), 5, path));
        ASSERT_EQ(path.size(), std::size_t(1));
        ASSERT_EQ(path[0], 5);
    });

    TEST("search finds left child; path is [root, target]", {
        std::vector<int> path;
        ASSERT(t.searchPath(t.getRoot(), 3, path));
        ASSERT_EQ(path.size(), std::size_t(2));
        ASSERT_EQ(path[0], 5);
        ASSERT_EQ(path[1], 3);
    });

    TEST("search finds right child; path is [root, target]", {
        std::vector<int> path;
        ASSERT(t.searchPath(t.getRoot(), 7, path));
        ASSERT_EQ(path.size(), std::size_t(2));
        ASSERT_EQ(path[0], 5);
        ASSERT_EQ(path[1], 7);
    });

    TEST("search returns false for missing key; path is non-empty", {
        std::vector<int> path;
        ASSERT(!t.searchPath(t.getRoot(), 99, path));
        ASSERT(!path.empty());
    });

    TEST("deep search returns correct ordered path", {
        //         10
        //        /  \
        //       5    15
        //      / \
        //     2   7
        Tree deep = make_tree({10, 5, 15, 2, 7});
        std::vector<int> path;
        ASSERT(deep.searchPath(deep.getRoot(), 7, path));
        ASSERT_EQ(path.size(), std::size_t(3));
        ASSERT_EQ(path[0], 10);
        ASSERT_EQ(path[1], 5);
        ASSERT_EQ(path[2], 7);
    });

    TEST("search for deepest node returns full path", {
        //  1 -> 2 -> 3 -> 4 -> 5 (right-skewed)
        Tree skewed = make_tree({1, 2, 3, 4, 5});
        std::vector<int> path;
        ASSERT(skewed.searchPath(skewed.getRoot(), 5, path));
        ASSERT_EQ(path.size(), std::size_t(5));
        ASSERT_EQ(path[4], 5);
    });
}

// ─── Tree::sameTree ──────────────────────────────────────────────────────────

static void test_same_tree() {
    std::cout << "\n── Tree::sameTree ──\n";

    TEST("two empty trees are the same", {
        Tree a, b;
        ASSERT(a.sameTree(a.getRoot(), b.getRoot()));
    });

    TEST("identical single-node trees are the same", {
        Tree a = make_tree({5});
        Tree b = make_tree({5});
        ASSERT(a.sameTree(a.getRoot(), b.getRoot()));
    });

    TEST("single-node trees with different keys are not the same", {
        Tree a = make_tree({5});
        Tree b = make_tree({7});
        ASSERT(!a.sameTree(a.getRoot(), b.getRoot()));
    });

    TEST("structurally identical multi-node trees are the same", {
        Tree a = make_tree({5, 3, 7});
        Tree b = make_tree({5, 3, 7});
        ASSERT(a.sameTree(a.getRoot(), b.getRoot()));
    });

    TEST("same values but different BST structure are not the same", {
        // a: root=5, left=3, right=7
        // b: root=3, right=5, right->right=7  (root differs)
        Tree a = make_tree({5, 3, 7});
        Tree b = make_tree({3, 5, 7});
        ASSERT(!a.sameTree(a.getRoot(), b.getRoot()));
    });

    TEST("extra node on one side makes trees different", {
        Tree a = make_tree({5, 3, 7});
        Tree b = make_tree({5, 3, 7, 1}); // b has an extra left-left node
        ASSERT(!a.sameTree(a.getRoot(), b.getRoot()));
    });

    TEST("empty vs non-empty is not the same", {
        Tree a;
        Tree b = make_tree({5});
        ASSERT(!a.sameTree(a.getRoot(), b.getRoot()));
    });
}

// ─── Tree::containsSubtree ───────────────────────────────────────────────────

static void test_contains_subtree() {
    std::cout << "\n── Tree::containsSubtree ──\n";

    //        5
    //       / \
    //      3   7
    //     / \
    //    1   4
    Tree main_tree = make_tree({5, 3, 7, 1, 4});

    TEST("a tree contains itself as a subtree", {
        ASSERT(main_tree.containsSubtree(main_tree.getRoot(), main_tree.getRoot()));
    });

    TEST("a tree contains an exact left subtree match", {
        // left subtree of main rooted at 3 with children 1 and 4
        Tree sub = make_tree({3, 1, 4});
        ASSERT(main_tree.containsSubtree(main_tree.getRoot(), sub.getRoot()));
    });

    TEST("a tree contains a single-node subtree that is a leaf", {
        Tree sub = make_tree({7});
        ASSERT(main_tree.containsSubtree(main_tree.getRoot(), sub.getRoot()));
    });

    TEST("a tree contains a single-node subtree that is an internal node", {
        // Node 3 in main has children; a single-node sub {3} won't match
        // because sameTree requires exact structural match (3's children differ)
        Tree sub = make_tree({3});
        ASSERT(!main_tree.containsSubtree(main_tree.getRoot(), sub.getRoot()));
    });

    TEST("subtree with wrong child structure is not found", {
        // sub = {3, 1} only (no right child 4) — doesn't match node 3 in main
        Tree sub = make_tree({3, 1});
        ASSERT(!main_tree.containsSubtree(main_tree.getRoot(), sub.getRoot()));
    });

    TEST("subtree with key not in tree is not found", {
        Tree sub = make_tree({99});
        ASSERT(!main_tree.containsSubtree(main_tree.getRoot(), sub.getRoot()));
    });

    TEST("empty subtree is always found in any tree", {
        Tree empty_sub;
        ASSERT(main_tree.containsSubtree(main_tree.getRoot(), empty_sub.getRoot()));
    });

    TEST("empty subtree is found in an empty main tree", {
        Tree empty_main, empty_sub;
        ASSERT(empty_main.containsSubtree(empty_main.getRoot(), empty_sub.getRoot()));
    });

    TEST("non-empty subtree is not found in an empty main tree", {
        Tree empty_main;
        Tree sub = make_tree({5});
        ASSERT(!empty_main.containsSubtree(empty_main.getRoot(), sub.getRoot()));
    });
}

// ─── Tree::printBalance / isAVL / setAVL ─────────────────────────────────────

static void test_balance() {
    std::cout << "\n── Tree::printBalance / setAVL / isAVL ──\n";

    TEST("empty tree printBalance returns true", {
        Tree t;
        SilentOut _;
        ASSERT(t.printBalance());
    });

    TEST("single-node tree is AVL (bf=0)", {
        Tree t = make_tree({42});
        SilentOut _;
        ASSERT(t.printBalance());
    });

    TEST("two-node tree (root + right child) is AVL (bf=1)", {
        Tree t = make_tree({5, 7});
        SilentOut _;
        ASSERT(t.printBalance());
    });

    TEST("two-node tree (root + left child) is AVL (bf=-1)", {
        Tree t = make_tree({5, 3});
        SilentOut _;
        ASSERT(t.printBalance());
    });

    TEST("perfectly balanced 7-node tree is AVL", {
        //       4
        //      / \
        //     2   6
        //    / \ / \
        //   1  3 5  7   — all bf = 0
        Tree t = make_tree({4, 2, 6, 1, 3, 5, 7});
        SilentOut _;
        ASSERT(t.printBalance());
    });

    TEST("right-skewed 5-node tree is not AVL", {
        // 1->2->3->4->5  root bf=4
        Tree t = make_tree({1, 2, 3, 4, 5});
        SilentOut _;
        ASSERT(!t.printBalance());
    });

    TEST("left-skewed 5-node tree is not AVL", {
        // 5->4->3->2->1
        Tree t = make_tree({5, 4, 3, 2, 1});
        SilentOut _;
        ASSERT(!t.printBalance());
    });

    TEST("three-node right-skewed tree is not AVL (bf=2)", {
        // 1 -> 2 -> 3
        Tree t = make_tree({1, 2, 3});
        SilentOut _;
        ASSERT(!t.printBalance());
    });

    TEST("setAVL(true) makes isAVL() return true", {
        Tree t;
        t.setAVL(true);
        ASSERT(t.isAVL());
    });

    TEST("setAVL(false) makes isAVL() return false", {
        Tree t;
        t.setAVL(false);
        ASSERT(!t.isAVL());
    });

    TEST("printAVL prints YES for AVL tree (no crash)", {
        Tree t = make_tree({4, 2, 6, 1, 3, 5, 7});
        t.setAVL(true);
        SilentOut _;
        t.printAVL(); // must not throw
    });

    TEST("printAVL prints NO for non-AVL tree (no crash)", {
        Tree t = make_tree({1, 2, 3, 4, 5});
        t.setAVL(false);
        SilentOut _;
        t.printAVL(); // must not throw
    });
}

// ─── Tree::fromFile ───────────────────────────────────────────────────────────

#ifndef TEST_DATA_DIR
#define TEST_DATA_DIR "tests"
#endif

static void test_from_file() {
    std::cout << "\n── Tree::fromFile ──\n";

    TEST("input1.txt loads 10 unique keys", {
        Tree t = Tree::fromFile(TEST_DATA_DIR "/input1.txt");
        ASSERT_EQ(t.size(), std::size_t(10));
    });

    TEST("input1.txt root is 5 (first line)", {
        Tree t = Tree::fromFile(TEST_DATA_DIR "/input1.txt");
        ASSERT(t.getRoot() != nullptr);
        ASSERT_EQ(t.getRoot()->key_, 5);
    });

    TEST("input1.txt is not AVL", {
        Tree t = Tree::fromFile(TEST_DATA_DIR "/input1.txt");
        SilentOut _;
        ASSERT(!t.printBalance());
    });

    TEST("avl_tree.txt loads 7 unique keys", {
        Tree t = Tree::fromFile(TEST_DATA_DIR "/avl_tree.txt");
        ASSERT_EQ(t.size(), std::size_t(7));
    });

    TEST("avl_tree.txt is AVL", {
        Tree t = Tree::fromFile(TEST_DATA_DIR "/avl_tree.txt");
        SilentOut _;
        ASSERT(t.printBalance());
    });

    TEST("non_avl_tree.txt is not AVL", {
        Tree t = Tree::fromFile(TEST_DATA_DIR "/non_avl_tree.txt");
        SilentOut _;
        ASSERT(!t.printBalance());
    });

    TEST("single_key.txt loads exactly 1 key", {
        Tree t = Tree::fromFile(TEST_DATA_DIR "/single_key.txt");
        ASSERT_EQ(t.size(), std::size_t(1));
    });

    TEST("single_key.txt singleKey() returns 42", {
        Tree t = Tree::fromFile(TEST_DATA_DIR "/single_key.txt");
        auto opt = t.singleKey();
        ASSERT(opt.has_value());
        ASSERT_EQ(*opt, 42);
    });

    TEST("duplicates.txt de-duplicates to 3 unique keys", {
        // file contains: 5 3 5 7 3  → unique: {3, 5, 7}
        Tree t = Tree::fromFile(TEST_DATA_DIR "/duplicates.txt");
        ASSERT_EQ(t.size(), std::size_t(3));
    });

    TEST("missing file throws std::runtime_error", {
        ASSERT_THROWS(Tree::fromFile("__no_such_file__.txt"));
    });

    TEST("file with invalid integer throws std::runtime_error", {
        ASSERT_THROWS(Tree::fromFile(TEST_DATA_DIR "/invalid.txt"));
    });
}

// ─── Stats ───────────────────────────────────────────────────────────────────

static void test_stats() {
    std::cout << "\n── Stats ──\n";

    TEST("default-constructed Stats: count is 0", {
        Stats s;
        ASSERT_EQ(s.count, std::size_t(0));
    });

    TEST("default-constructed Stats: average() returns 0.0", {
        Stats s;
        ASSERT_NEAR(s.average(), 0.0, 1e-9);
    });

    TEST("add single value: min == max == value, count == 1", {
        Stats s;
        s.add(7);
        ASSERT_EQ(s.min, 7);
        ASSERT_EQ(s.max, 7);
        ASSERT_EQ(s.count, std::size_t(1));
        ASSERT_NEAR(s.average(), 7.0, 1e-9);
    });

    TEST("add multiple values: min and max are correct", {
        Stats s;
        s.add(3);
        s.add(1);
        s.add(5);
        s.add(2);
        s.add(4);
        ASSERT_EQ(s.min, 1);
        ASSERT_EQ(s.max, 5);
    });

    TEST("add multiple values: count and average are correct", {
        Stats s;
        s.add(3);
        s.add(1);
        s.add(5);
        s.add(2);
        s.add(4);
        ASSERT_EQ(s.count, std::size_t(5));
        ASSERT_NEAR(s.average(), 3.0, 1e-9);  // (1+2+3+4+5)/5 = 3
    });

    TEST("add negative values: min and max are tracked correctly", {
        Stats s;
        s.add(-10);
        s.add(0);
        s.add(10);
        ASSERT_EQ(s.min, -10);
        ASSERT_EQ(s.max, 10);
        ASSERT_NEAR(s.average(), 0.0, 1e-9);
    });

    TEST("getStats on nullptr does nothing", {
        Stats s;
        s.getStats(std::unique_ptr<Node>{});
        ASSERT_EQ(s.count, std::size_t(0));
    });

    TEST("getStats on single-node tree", {
        Tree t = make_tree({99});
        Stats s;
        s.getStats(t.getRoot());
        ASSERT_EQ(s.count, std::size_t(1));
        ASSERT_EQ(s.min, 99);
        ASSERT_EQ(s.max, 99);
        ASSERT_NEAR(s.average(), 99.0, 1e-9);
    });

    TEST("getStats traverses all 7 nodes of a balanced tree", {
        //       4
        //      / \
        //     2   6
        //    / \ / \
        //   1  3 5  7    sum = 28, avg = 4
        Tree t = make_tree({4, 2, 6, 1, 3, 5, 7});
        Stats s;
        s.getStats(t.getRoot());
        ASSERT_EQ(s.count, std::size_t(7));
        ASSERT_EQ(s.min, 1);
        ASSERT_EQ(s.max, 7);
        ASSERT_NEAR(s.average(), 4.0, 1e-9);
    });

    TEST("getStats on right-skewed tree visits all nodes", {
        // 1->2->3->4->5
        Tree t = make_tree({1, 2, 3, 4, 5});
        Stats s;
        s.getStats(t.getRoot());
        ASSERT_EQ(s.count, std::size_t(5));
        ASSERT_EQ(s.min, 1);
        ASSERT_EQ(s.max, 5);
        ASSERT_NEAR(s.average(), 3.0, 1e-9);
    });

    TEST("printStats output contains expected min and max values", {
        Stats s;
        s.add(1);
        s.add(9);
        s.add(5);
        std::ostringstream captured;
        auto* saved = std::cout.rdbuf(captured.rdbuf());
        s.printStats();
        std::cout.rdbuf(saved);
        const std::string out = captured.str();
        ASSERT(out.find("1") != std::string::npos);  // min=1
        ASSERT(out.find("9") != std::string::npos);  // max=9
    });
}

// ─── Entry point ─────────────────────────────────────────────────────────────

int main() {
    std::cout << "Running treeCheck test suite\n";
    std::cout << "════════════════════════════════════════\n";

    test_insert();
    test_single_key();
    test_search_path();
    test_same_tree();
    test_contains_subtree();
    test_balance();
    test_from_file();
    test_stats();

    std::cout << "\n════════════════════════════════════════\n";
    const int total = s_passed + s_failed;
    std::cout << "Results: " << s_passed << "/" << total
              << " passed";
    if (s_failed > 0) {
        std::cout << ", " << s_failed << " FAILED";
    }
    std::cout << "\n";

    return s_failed == 0 ? 0 : 1;
}
