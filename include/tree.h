#pragma once

#include "node.h"
#include <memory>
#include <filesystem>


class Tree {
  public:

    bool insert ( int key );  // change return type to bool
    bool printBalance () const;
    void printAVL () const ;
    void printStats ();

    void setAVL ( bool avl );
    bool isAVL () const { return avl; };
    std::size_t size () const { return size_; }
    void grow ();
    std::unique_ptr <Node>& getRoot();

    static Tree fromFile ( const std::filesystem::path& path );

  private:
    std::unique_ptr<Node> root;
    std::size_t size_ = 0;
    bool avl = true;

    bool insert_ ( std::unique_ptr<Node> &node, int key );
    bool printBalance_ ( const Node *node ) const;

    static int height ( std::unique_ptr<Node> &node );

    static int balanceFactor ( std::unique_ptr<Node> &node );
    static std::string_view trim ( std::string_view sv );
    static int parseInt (   std::string_view sv,
                            const std::filesystem::path& path,
                            std::size_t lineNr
                        );
};
