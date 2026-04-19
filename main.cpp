#include "include/modes.h"
#include <exception>
#include <iostream>

int main ( int argc, char* argv [] ) {
    try {
        switch ( argc ) {
            case 2: {
                analysisMode( argv[1]);
                return 0;
            }
            case 3: {
                searchMode ( argv [1], argv [2] );
                return 0;
        }
        default: {
            std::cerr << "Usage:\n";
            return 1;
        }
        }
    } catch ( std::exception &ex ) {
        std::cerr << "Error: " << ex.what () << "\n";
        return 1;
    }

}
