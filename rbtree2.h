#ifndef RBTREE_H
#define RBTREE_H

#include <cstddef>
#include <cstdint>
//#include <iostream>
#include <memory>
#include <utility>

namespace rb {

/* Properties of red-black trees
 *
 *   1. Every node is either red or black.
 *   2. The root is black.
 *   3. Every leaf (NIL) is black.
 *   4. If a node is red, then both its children are black.
 *   5. For each node, all simple paths from the node to descendant leaves contain the
 *   same number of black nodes.
 */

namespace Color {
enum : bool {
    RED = false, BLACK = true
};
using Value = bool;
}
namespace Dir {
enum : bool {
    LEFT = false, RIGHT = true
};
using Value = bool;
}

struct Node {
    Node* p;
    Node* child[2];
    Color::Value color;

    Node() : p{nullptr}, child{nullptr,nullptr}, color{Color::RED} {}
};

class TreeBase {
protected:

};


template <typename Cmp>
class Tree : TreeBase {

    [[no_unique_address]] Cmp m_cmp;
public:

    bool insert(Node* node);
};

} // namespace rb

#endif /* #ifndef RBTREE_H */