#pragma once

#include "node.h"
#include <memory>
#include <filesystem>
#include <vector>
#include <optional>


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
    const std::unique_ptr <Node>& getRoot() const;
    std::optional <int> singleKey () const noexcept;

    static Tree fromFile ( const std::filesystem::path& path );

    /*  search methods */
    bool searchPath ( const std::unique_ptr <Node>& node, int key, std::vector <int>& path ) const;
    bool sameTree ( const std::unique_ptr <Node>& a, const std::unique_ptr <Node>& b ) const;
    bool containsSubtree ( const std::unique_ptr <Node>& main, const std::unique_ptr <Node>& subtree ) const;

  private:
    std::unique_ptr<Node> root;
    std::size_t size_ = 0;
    bool avl = true;

    bool insert_ ( std::unique_ptr<Node> &node, int key );
    bool printBalance_ ( const Node *node ) const;

    static int height ( const std::unique_ptr<Node> &node );

    static int balanceFactor ( const std::unique_ptr<Node> &node );

    /* file parsing */
    static std::string_view trim ( std::string_view sv );
    static int parseInt (   std::string_view sv,
                            const std::filesystem::path& path,
                            std::size_t lineNr
                        );

};
