#ifndef RBTREE_INTERNAL_H
#define RBTREE_INTERNAL_H

#include <cstddef>
#include <cstdint>
#include <utility>

namespace rb {

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

enum class Rules {
    R1, // Every node is either red or black.
    R2, // The root is black.
    R3, // Every leaf (NIL) is black.
    R4, // If a node is red, then both its children are black.
    R5, // For each node, all simple paths from the node to descendant leaves contain the same number of black nodes.
};

class NodeBase {
protected:
    NodeBase() : p{nullptr}, child{nullptr, nullptr}, color{Color::RED} {}
public:
    NodeBase* p;
    NodeBase* child[2];
    Color::Value color;

    NodeBase*& left() noexcept { return child[Dir::LEFT]; }
    NodeBase*& right() noexcept { return child[Dir::RIGHT]; }
    NodeBase* const& left() const noexcept { return child[Dir::LEFT]; }
    NodeBase* const& right() const noexcept { return child[Dir::RIGHT]; }
};

class TreeBase {
    TreeBase(const TreeBase& o) = delete;
protected:
    using Callback = void(*)(const NodeBase*, void*);

    NodeBase* m_root;

    TreeBase() noexcept : m_root{nullptr} {}
    TreeBase(TreeBase&& o) noexcept : m_root{o.m_root} { o.m_root = nullptr; }

    void move_from(TreeBase&& o) noexcept
    {
        m_root = std::exchange(o.m_root, nullptr);
    }

    static bool is_black(NodeBase* n) noexcept
    {
        return !n || n->color;
    }
    static bool is_red(NodeBase* n) noexcept
    {
        return n && !n->color;
    }

    template <typename N>  // N deduces to NodeBase or const NodeBase
    static N* rb_minimum(N* n)
    {
        while (n->left())
            n = n->left();
        return n;
    }
    template <typename N>
    static N* rb_maximum(N* n)
    {
        while (n->right())
            n = n->right();
        return n;
    }

    void rb_rotate(NodeBase* x, Dir::Value dir);
    void rb_rotate_left(NodeBase* x) { rb_rotate(x, Dir::LEFT); }
    void rb_rotate_right(NodeBase* x) { rb_rotate(x, Dir::RIGHT); }

    void rb_insert_fixup(NodeBase* z);

    void rb_transplant(NodeBase* u, NodeBase* v);
    void rb_delete_fixup(NodeBase* p, NodeBase* x);
    void rb_delete(NodeBase* z);

    static void rb_inorder(NodeBase* root, Callback cb, void* usrdata);

    /* Properties of red-black trees
    *
    *   1. Every node is either red or black.
    *   2. The root is black.
    *   3. Every leaf (NIL) is black.
    *   4. If a node is red, then both its children are black.
    *   5. For each node, all simple paths from the node to descendant leaves contain the
    *   same number of black nodes.
    */
    /* template <typename F>
    int rb_check_subtree(NodeBase* n, F&& cb)
    {
        if (n == NIL()) return 1;

        // rule 4
        if (n->color == Color::RED && (n->left()->color == Color::RED || n->right()->color == Color::RED))
            cb(Rules::R4, n, 0, 0);

        int lh = rb_check_subtree(n->left(), std::forward<F>(cb));
        int rh = rb_check_subtree(n->right(), std::forward<F>(cb));

        // rule 5
        if (lh != rh)
            cb(Rules::R5, n, lh, rh);

        return lh + static_cast<int>(n->color);
    } */

};


} // namespace rb

#endif /* #ifndef RBTREE_INTERNAL_H */
