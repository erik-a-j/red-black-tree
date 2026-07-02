#ifndef RBTREE_H
#define RBTREE_H

#include <cstddef>
#include <cstdint>
//#include <iostream>
#include <memory>
#include <utility>
#include <type_traits>
#include <concepts>

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

enum class Dir : uintptr_t {
    BIT = 1ULL << 0,
    LEFT = 0,    // 0
    RIGHT = BIT, // 1
};
enum class Color : uintptr_t {
    BIT = 1ULL << 1,
    RED = 0,     // 0
    BLACK = BIT, // 2
};

constexpr uintptr_t operator+(Dir e) noexcept { return static_cast<uintptr_t>(e); }
constexpr uintptr_t operator+(Color e) noexcept { return static_cast<uintptr_t>(e); }
constexpr uintptr_t operator~(Dir e) noexcept { return ~static_cast<uintptr_t>(e); }
constexpr uintptr_t operator~(Color e) noexcept { return ~static_cast<uintptr_t>(e); }

constexpr uintptr_t TAG_MASK = +Dir::BIT | +Color::BIT;
constexpr uintptr_t PTR_MASK = ~TAG_MASK;

struct Node;
struct NodePtr {
    uintptr_t value;

    constexpr NodePtr(uintptr_t v) : value{v} {}
    constexpr NodePtr(Node* n) : value{reinterpret_cast<uintptr_t>(n)} {}

    NodePtr& setPtr(Node* n) noexcept
    {
        value = (value & TAG_MASK) | reinterpret_cast<uintptr_t>(n);
        return *this;
    }
    NodePtr& setDir(Dir e) noexcept
    {
        value = (value & ~Dir::BIT) | +e;
        return *this;
    }
    NodePtr& setColor(Color e) noexcept
    {
        value = (value & ~Color::BIT) | +e;
        return *this;
    }

    Node* node() noexcept { return reinterpret_cast<Node*>(value & PTR_MASK); }
    Node* operator->() noexcept { return node(); }

    explicit operator uintptr_t() const noexcept { return value; }

    uintptr_t operator|(Dir e) const noexcept { return value | +e; }
    uintptr_t operator|(Color e) const noexcept { return value | +e; }
    uintptr_t operator&(Dir e) const noexcept { return value & +e; }
    uintptr_t operator&(Color e) const noexcept { return value & +e; }

};

constexpr NodePtr NIL = +Color::BLACK;

struct Node {
    NodePtr p;
    NodePtr child[2];

    Node() : p{NIL}, child{NIL,NIL} {}

    NodePtr& left() noexcept { return child[+Dir::LEFT]; }
    NodePtr& right() noexcept { return child[+Dir::RIGHT]; }

    NodePtr& as_child() noexcept { p->child[+dir()]; }
    NodePtr& as_parent(Dir child_dir) noexcept { child[+child_dir]->p; }


    Color color_left() noexcept { return static_cast<Color>(child[+Dir::LEFT] & Color::BIT); }
    Color color_right() noexcept { return static_cast<Color>(child[+Dir::RIGHT] & Color::BIT); }
    Color color_p() noexcept { return static_cast<Color>(p & Color::BIT); }

    Dir dir() noexcept { return static_cast<Dir>(p & Dir::BIT); }

};

class TreeBase {
protected:
    NodePtr m_root;

    TreeBase() : m_root{NIL} {}

    static void replaceNode(Node* old_node, Node* new_node) noexcept
    {
        old_node->as_parent(Dir::LEFT).setPtr(new_node);
        old_node->as_parent(Dir::RIGHT).setPtr(new_node);
        old_node->p->child[+old_node->dir()].setPtr(new_node);
    }
};


template <typename Cmp>
class Tree : TreeBase {

    [[no_unique_address]] Cmp m_cmp;
public:

    Tree() = default;

    void insert(Node* node)
    {
        NodePtr p{NIL};
        NodePtr curr{m_root};

        while (curr != NIL)
        {
            p = curr;
            int dir = m_cmp(node, toNode(curr));
            if (dir > 0) curr = toNode(curr)->child[+Dir::RIGHT];
            else if (dir < 0) curr = toNode(curr)->child[+Dir::LEFT];
            else
            {
                replaceNode(curr, node);
                break;
            }
        }
    }
};

} // namespace rb

#endif /* #ifndef RBTREE_H */