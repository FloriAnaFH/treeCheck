#pragma once

#include "node.h"
#include <memory>
#include <filesystem>


class Tree {
  public:

    bool insert ( int key );  // change return type to bool
    int printBalance () ;
    static void printAVL ();
    void printStats ();

    void setAVL ( bool avl ) { this->avl = avl; }
    bool isAVL () const { return avl; }

    static Tree fromFile ( const std::filesystem::path& path );

  private:
    std::unique_ptr<Node> root;
    std::size_t size = 0;
    bool avl = true;

    bool insert_ ( std::unique_ptr<Node> &node, int key );
    int printBalance_ ( const Node *node );

    static int height ( std::unique_ptr<Node> &node );

    static int balanceFactor ( std::unique_ptr<Node> &node );
    static std::string_view trim ( std::string_view sv );
    static int parseInt (   std::string_view sv,
                            const std::filesystem::path& path,
                            std::size_t lineNr
                        );
};
