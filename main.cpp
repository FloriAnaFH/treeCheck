#include "include/modes.h"
#include <exception>
#include <iostream>
#include <string_view>

int main ( int argc, char* argv [] ) {
    try {
        const bool rebalance = ( argc >= 2 && std::string_view( argv[1] ) == "--rebalance" );
        const int  shift     = rebalance ? 1 : 0;   // how many args the flag consumed

        switch ( argc - shift ) {
            case 2: {
                analysisMode( argv[1 + shift], rebalance );
                return 0;
            }
            case 3: {
                searchMode( argv[1 + shift], argv[2 + shift], rebalance );
                return 0;
            }
        default: {
            std::cerr << "Usage:\n"
                      << "  treecheck <file>                      analyse tree\n"
                      << "  treecheck --rebalance <file>          analyse with AVL rebalancing\n"
                      << "  treecheck <main> <query>              search / subtree check\n"
                      << "  treecheck --rebalance <main> <query>  search with AVL rebalancing\n";
            return 1;
        }
        }
    } catch ( std::exception &ex ) {
        std::cerr << "Error: " << ex.what () << "\n";
        return 1;
    }

}
