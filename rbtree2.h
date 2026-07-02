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

    bool operator==(NodePtr o) const noexcept { return addr() == o.addr(); }
    bool operator!=(NodePtr o) const noexcept { return addr() != o.addr(); }
    bool operator<(NodePtr o) const noexcept { return addr() < o.addr(); }
    bool operator>(NodePtr o) const noexcept { return addr() > o.addr(); }
    explicit operator bool() const noexcept { return addr() != 0; }
    explicit operator uintptr_t() const noexcept { return value; }

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

    Color color() const noexcept { return static_cast<Color>(value & +Color::BIT); }

    Node* node() noexcept { return reinterpret_cast<Node*>(addr()); }
    const Node* node() const noexcept { return reinterpret_cast<const Node*>(addr()); }
    Node* operator->() noexcept { return node(); }
    const Node* operator->() const noexcept { return node(); }

    uintptr_t operator|(Dir e) const noexcept { return value | +e; }
    uintptr_t operator|(Color e) const noexcept { return value | +e; }
    uintptr_t operator&(Dir e) const noexcept { return value & +e; }
    uintptr_t operator&(Color e) const noexcept { return value & +e; }

private:
    uintptr_t addr() const noexcept { return value & PTR_MASK; }
};

constexpr NodePtr NIL = +Color::BLACK;

/**
 * Overview of struct Node:
 *
 *   `.p` represents:
 *      1. Address of parent Node.
 *      2. Color of parent Node.
 *      3. Dir of this Node relative to parent Node.
 *
 *   `.children[0..1]` represents:
 *      1. Address of child Node.
 *      2. Color of child Node.
 */
struct Node {
    NodePtr p;
    NodePtr children[2];

    Node() : p{NIL}, children{NIL,NIL} {}
    void update(const Node* o) noexcept
    {
        p = o->p;
        children[+Dir::LEFT] = o->children[+Dir::LEFT];
        children[+Dir::RIGHT] = o->children[+Dir::RIGHT];
    }

    NodePtr& child(Dir e) noexcept { return children[+e]; }
    NodePtr& left() noexcept { return children[+Dir::LEFT]; }
    NodePtr& right() noexcept { return children[+Dir::RIGHT]; }

    Dir dir() noexcept { return static_cast<Dir>(p & Dir::BIT); }
};

class TreeBase {
protected:
    NodePtr m_root;

    TreeBase() : m_root{NIL} {}

    static void updateNode(Node* old_node, Node* new_node) noexcept
    {
        new_node->update(old_node);
        new_node->child(Dir::LEFT)->p.setPtr(new_node);
        new_node->child(Dir::RIGHT)->p.setPtr(new_node);
        new_node->p->child(new_node->dir()).setPtr(new_node);
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
                updateNode(curr.node(), node);
                break;
            }
        }
    }
};

} // namespace rb

#endif /* #ifndef RBTREE_H */