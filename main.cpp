#include "include/modes.h"
#include "include/visualiser.h"
#include <exception>
#include <iostream>
#include <string_view>

int main ( int argc, char* argv [] ) {
    try {
        bool rebalance = false;
        bool animate   = false;
        int  argStart  = 1;   // index of the first non-flag argument

        while ( argStart < argc ) {
            std::string_view arg = argv[argStart];
            if      ( arg == "--rebalance" ) { rebalance = true; ++argStart; }
            else if ( arg == "--animate"   ) { animate   = true; ++argStart; }
            else break;
        }

        const int nfiles = argc - argStart;

        if ( animate ) {
            if ( nfiles != 1 ) {
                std::cerr << "Usage: treecheck [--rebalance] --animate <file>\n";
                return 1;
            }
            animateMode( argv[argStart], rebalance );
            return 0;
        }

        switch ( nfiles ) {
            case 1: {
                analysisMode( argv[argStart], rebalance );
                return 0;
            }
            case 2: {
                searchMode( argv[argStart], argv[argStart + 1], rebalance );
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
