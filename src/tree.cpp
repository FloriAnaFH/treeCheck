#include "../include/tree.h"
#include "../include/node.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string_view>
#include <charconv>

Node::Node ( int key ) : key_ ( key ), left ( nullptr ), right ( nullptr ) {}


/* public functions */

bool Tree::insert ( int key ) { return insert_ ( root, key ); }

bool Tree::printBalance () const {
    return printBalance_ ( root.get() );
}

static Tree fromFile ( std::filesystem::path& path ) {
    Tree tree;

    std::ifstream input ( path );
    if ( !input ) {
        throw std::runtime_error ( "Can't open file: " + path.string() );
    }

    std::string line;
    std::size_t lineNr = 0;

    while ( std::getline ( input, line )) {
        ++lineNr;

    }
    return tree;
}

void Tree::printAVL() const {
    std::cout << "AVL: " << ( isAVL()  ? "YES" : "NO" );
}

void Tree::grow () { ++size_; }
std::unique_ptr <Node>& Tree::getRoot() { return root; }

/* private functions */

bool Tree::insert_ ( std::unique_ptr<Node> &node, int key ) {
    /* insert a key into the search tree - check for and ignore duplicate keys. Complexity: avg: O(log n), worst: O(n)*/

    if ( !node ) {
        node = std::make_unique<Node> ( key );
        grow();
        return true;
    }
    if ( key < node->key_ ) {
        return insert_ ( node->left, key );
    }
    if ( key > node->key_ ) {
        return insert_ ( node->right, key );
    }
    return false;  // key already exists
}

static std::string_view trim ( std::string_view sv ) {
    /* remove whitespace from both ends */
    while ( !sv.empty () && std::isspace ( static_cast <unsigned char> ( sv.front() ))) {
        sv.remove_suffix ( 1 );
    }

    while ( !sv.empty () && std::isspace ( static_cast <unsigned char> ( sv.back () ) )) {
        sv.remove_suffix ( 1 );
    }
    return sv;
}

static int parseInt ( std::string_view sv, const std::filesystem::path& path, std::size_t lineNr ) {
    /* tries to parse an integer - throws an error if parsing produces an error or not all characters were read */
    int value{};
    const char* begin = sv.data();
    const char* end = begin + sv.size ();
    const auto [ ptr, ec ] = std::from_chars ( begin, end, value );

    if ( ec != std::errc{} || ptr != end ) {
        std::ostringstream msg;
        msg << "Invalid integer in " << path.string () << " at line " << lineNr << " : '" << sv << "'";
        throw std::runtime_error( msg.str () );
    }

    return value;
}

bool Tree::printBalance_ ( const Node *node ) const {
    /* compute subtree height and AVL information in one pass to avoid recomputation of subtree heights */
    if ( !node )
        return false;

    const int rightHeight = printBalance_ ( node->right.get() );
    const int leftHeight = printBalance_ ( node->left.get() );
    const int bf = rightHeight - leftHeight;

    std::cout << "bal(" << node->key_ << ") = " << bf;
    if ( bf > 1 || bf < -1 ) {
        std::cout << " (AVL violation)";
        return  false;
    }
    std::cout << "\n";

    return std::max ( leftHeight, rightHeight ) + 1;
}

int Tree::height ( std::unique_ptr<Node> &node ) {
    if ( !node )
        return -1;

    int leftHeight = height ( node->left );
    int rightHeight = height ( node->right );

    return ( leftHeight > rightHeight ? leftHeight : rightHeight ) + 1;
}

int Tree::balanceFactor ( std::unique_ptr<Node> &node ) { return height ( node->right ) - height ( node->left ); }

/* insert für AVL Baum mit rotationen.
 *
Node *Tree::insert(Node *node, int key){
    if (node == nullptr)
        return new Node(key);

    if (key < node->key)
        node->left = insert(node->left, key);
    else if (key > node->key)
        node->right = insert(node->right, key);

    int bf = balanceFactor(node);

    if (bf > 1) {
        if (balanceFactor(node->right) < 0)
            node = roteteRightLeft(node);
        else
            node = rotateLeft(node);
    }
    else if (bf < -1) {
        if (balanceFactor(node->left) > 0)
            node = rotateLeftRight(node);
        else
            node = rotateRight(node);
    }

    return node;
}*/
