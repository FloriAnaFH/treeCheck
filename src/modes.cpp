#include "../include/modes.h"
#include "../include/tree.h"
#include "../include/stats.h"
#include <sstream>
#include <stdexcept>
#include <string>
#include <iostream>
#include <iterator>

void analysisMode( const std::filesystem::path &file, bool rebalance ) {
    Tree tree = Tree::fromFile ( file, rebalance );
    if ( !tree.size () )  {
        throw std::runtime_error ( "File contains no keys: " + file.string () );
    }

    bool avl = tree.printBalance();
    tree.setAVL ( avl );
    tree.printAVL();

    Stats stats;
    stats.getStats (tree.getRoot() );
    stats.printStats();
}

void searchMode ( const std::filesystem::path& mainFile,
                const std::filesystem::path& queryFile, bool rebalance ) {
    const Tree mainTree = Tree::fromFile ( mainFile, rebalance );
    const Tree queryTree = Tree::fromFile ( queryFile, rebalance );

    auto formatPath = [] (const std::vector <int>& pathKeys ) -> std::string {
                    if ( pathKeys.empty() ) return "";
                    std::ostringstream out;
                    std::copy ( pathKeys.begin(), pathKeys.end () -1,
                        std::ostream_iterator <int> ( out, ", ") );
                    out << pathKeys.back();
                    return out.str ();
                };

    if ( queryTree.size() == 0 ) {
        throw std::runtime_error ( "Query file empty - must contain at least one key: " + queryFile.string () );
    }

    if ( queryTree.size() == 1 ) {
        const int key = *queryTree.singleKey();
        std::vector <int> path;

        if ( mainTree.searchPath ( mainTree.getRoot(), key, path )) {
            std::cout << key << " found " << formatPath ( path ) << "\n";
        } else {
            std::cout << key << " not found!\n";
        }
        return;
    }
    std::cout << (( mainTree.containsSubtree ( mainTree.getRoot(), queryTree.getRoot()) )? "Subtree found\n" : "Subtree not found!\n");

}
