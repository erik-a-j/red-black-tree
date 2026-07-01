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
    NodeBase(NodeBase* nil) : p{nil}, child{nil, nil}, color{Color::RED} {}
public:
    struct NIL_tag {};
    NodeBase(NIL_tag) : p{this}, child{this, this}, color{Color::BLACK} {}

    NodeBase* p;
    NodeBase* child[2];
    Color::Value color;

    NodeBase*& left() noexcept { return child[Dir::LEFT]; }
    NodeBase*& right() noexcept { return child[Dir::RIGHT]; }
    NodeBase* const& left() const noexcept { return child[Dir::LEFT]; }
    NodeBase* const& right() const noexcept { return child[Dir::RIGHT]; }
};

class TreeBase {
    NodeBase m_nil;
protected:
    NodeBase* m_root;

    TreeBase() : m_nil{NodeBase::NIL_tag{}}, m_root{&m_nil} {}

    NodeBase* rb_minimum(NodeBase* n)
    {
        while (n->left() != NIL())
            n = n->left();
        return n;
    }
    NodeBase* rb_maximum(NodeBase* n)
    {
        while (n->right() != NIL())
            n = n->right();
        return n;
    }

    void rb_rotate(NodeBase* x, Dir::Value dir)
    {
        NodeBase* y = x->child[!dir];
        x->child[!dir] = y->child[dir]; // turn y's dir subtree into x's !dir subtree
        if (y->child[dir] != NIL())
            y->child[dir]->p = x;       // x becomes parent of the subtree's root

        y->p = x->p;                    // x's parent becomes y's parent
        if (x->p == NIL())
            m_root = y;                 // y becomes the root
        else if (x == x->p->child[dir])
            x->p->child[dir] = y;       // y becomes a dir child
        else
            x->p->child[!dir] = y;      // y becomes a !dir child

        y->child[dir] = x;              // x becomes y's dir child
        x->p = y;
    }
    void rb_rotate_left(NodeBase* x) { rb_rotate(x, Dir::LEFT); }
    void rb_rotate_right(NodeBase* x) { rb_rotate(x, Dir::RIGHT); }

    void rb_insert_fixup(NodeBase* z);

    void rb_transplant(NodeBase* u, NodeBase* v)
    {
        if (u->p == NIL())
            m_root = v;
        else if (u == u->p->left())
            u->p->left() = v;
        else
            u->p->right() = v;
        v->p = u->p;
    }
    void rb_delete_fixup(NodeBase* x);
    void rb_delete(NodeBase* z);

    /* Properties of red-black trees
    *
    *   1. Every node is either red or black.
    *   2. The root is black.
    *   3. Every leaf (NIL) is black.
    *   4. If a node is red, then both its children are black.
    *   5. For each node, all simple paths from the node to descendant leaves contain the
    *   same number of black nodes.
    */
    template <typename F>
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
    }


    NodeBase* NIL() noexcept { return &m_nil; }
    const NodeBase* NIL() const noexcept { return &m_nil; }
};


} // namespace rb

#endif /* #ifndef RBTREE_INTERNAL_H */
