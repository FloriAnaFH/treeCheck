#include "../include/modes.h"
#include "../include/stats.h"
#include "../include/tree.h"
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>

/* Build a tree from file, print per-node balance factors, AVL status, and stats.
   Complexity: dominated by file load + insertions + full traversals for reporting */
void analysisMode ( const std::filesystem::path &file ) {
    Tree tree = Tree::fromFile ( file );
    if ( !tree.size() ) {
        throw std::runtime_error ( "File contains no keys: " + file.string() );
    }

    bool avl = tree.printBalance();
    tree.setAVL ( avl );
    tree.printAVL();

    Stats stats;
    stats.getStats ( tree.getRoot() );
    stats.printStats();
}

/* Compare main/query trees from files:
   - single-key query -> search path in main tree
   - multi-key query  -> subtree containment check
   Complexity: dominated by file load + insertions, then search/subtree check */
void searchMode ( const std::filesystem::path &mainFile, const std::filesystem::path &queryFile ) {
    const Tree mainTree = Tree::fromFile ( mainFile );
    const Tree queryTree = Tree::fromFile ( queryFile );

    /* Format visited keys as "a, b, c".
       Complexity: O(k) where k = number of keys in the path */
    auto keys = [] ( const std::vector<int> &keys ) -> std::string {
        if ( keys.empty() )
            return "";
        std::ostringstream out;
        std::copy ( keys.begin(), keys.end() - 1, std::ostream_iterator<int> ( out, ", " ) );
        out << keys.back();
        return out.str();
    };

    if ( queryTree.size() == 0 ) {
        throw std::runtime_error ( "Query file empty - must contain at least one key: " + queryFile.string() );
    }

    if ( queryTree.size() == 1 ) {
        const int key = *queryTree.singleKey();
        std::vector<int> path;

        if ( mainTree.searchPath ( mainTree.getRoot(), key, path ) ) {
            std::cout << key << " found " << keys ( path ) << "\n";
        } else {
            std::cout << key << " not found!\n";
        }
        return;
    }
    std::cout << ( ( mainTree.containsSparseSubtree ( mainTree.getRoot(), queryTree.getRoot() ) )
                       ? "Subtree found\n"
                       : "Subtree not found!\n" );
}
