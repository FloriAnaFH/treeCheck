#include "../include/modes.h"
#include "../include/tree.h"
#include "../include/stats.h"
#include <stdexcept>

void analysisMode( const std::filesystem::path &file ) {
    Tree tree = Tree::fromFile ( file );
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

void searchMode () {

}
