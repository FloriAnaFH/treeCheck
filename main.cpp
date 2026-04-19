#include "include/modes.h"
#include "include/visualiser.h"
#include <exception>
#include <iostream>
#include <string_view>

int main ( int argc, char* argv [] ) {
    try {
        bool rebalance    = false;
        bool animate      = false;
        int  firstFileArg = 1;   // index of the first non-flag argument

        while ( firstFileArg < argc ) {
            std::string_view currentArg = argv[firstFileArg];
            if      ( currentArg == "--rebalance" ) { rebalance = true; ++firstFileArg; }
            else if ( currentArg == "--animate"   ) { animate   = true; ++firstFileArg; }
            else break;
        }

        const int fileCount = argc - firstFileArg;

        if ( animate ) {
            if ( fileCount != 1 ) {
                std::cerr << "Usage: treecheck [--rebalance] --animate <file>\n";
                return 1;
            }
            animateMode( argv[firstFileArg], rebalance );
            return 0;
        }

        switch ( fileCount ) {
            case 1: {
                analysisMode( argv[firstFileArg], rebalance );
                return 0;
            }
            case 2: {
                searchMode( argv[firstFileArg], argv[firstFileArg + 1], rebalance );
                return 0;
            }
            default: {
                std::cerr << "Usage:\n"
                          << "  treecheck <file>                        analyse tree\n"
                          << "  treecheck --rebalance <file>            analyse with AVL rebalancing\n"
                          << "  treecheck <main> <query>                search / subtree check\n"
                          << "  treecheck --rebalance <main> <query>    search with AVL rebalancing\n"
                          << "  treecheck --animate <file>              animate insertions (plain BST)\n"
                          << "  treecheck --rebalance --animate <file>  animate insertions (AVL)\n";
                return 1;
            }
        }
    } catch ( std::exception& ex ) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}
