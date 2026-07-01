#include "rbtree.h"

namespace rb {

void TreeBase::rb_rotate(NodeBase* x, Dir::Value dir)
{
    NodeBase* y = x->child[!dir];
    x->child[!dir] = y->child[dir]; // turn y's dir subtree into x's !dir subtree
    if (y->child[dir])
        y->child[dir]->p = x;       // x becomes parent of the subtree's root

    y->p = x->p;                    // x's parent becomes y's parent
    if (!x->p)
        m_root = y;                 // y becomes the root
    else if (x == x->p->child[dir])
        x->p->child[dir] = y;       // y becomes a dir child
    else
        x->p->child[!dir] = y;      // y becomes a !dir child

    y->child[dir] = x;              // x becomes y's dir child
    x->p = y;
}

void TreeBase::rb_insert_fixup(NodeBase* z)
{
    while (is_red(z->p))
    {
        Dir::Value dir = z->p == z->p->p->right();
        NodeBase* y = z->p->p->child[!dir];

        if (is_red(y))                       // case 1
        {
            z->p->color = Color::BLACK;
            y->color = Color::BLACK;
            z->p->p->color = Color::RED;
            z = z->p->p;
        }
        else
        {
            if (z == z->p->child[!dir])      // case 2: z is the "inner" grandchild
            {
                z = z->p;
                rb_rotate(z, dir);
            }
            z->p->color = Color::BLACK;      // case 3
            z->p->p->color = Color::RED;
            rb_rotate(z->p->p, !dir);
        }
    }
    m_root->color = Color::BLACK;
}
void TreeBase::rb_transplant(NodeBase* u, NodeBase* v)
{
    if (!u->p)                  m_root = v;         // was: u->p == NIL()
    else if (u == u->p->left()) u->p->left() = v;
    else                        u->p->right() = v;
    if (v) v->p = u->p;                             // guard: v may be null
}

void TreeBase::rb_delete(NodeBase* z)
{
    NodeBase* x;
    NodeBase* x_parent;              // threaded to fixup; x itself may be null
    NodeBase* y = z;
    Color::Value y_color = y->color;

    if (!z->left())                                 // was: z->left() == NIL()
    {
        x = z->right();
        x_parent = z->p;
        rb_transplant(z, z->right());
    }
    else if (!z->right())
    {
        x = z->left();
        x_parent = z->p;
        rb_transplant(z, z->left());
    }
    else
    {
        y = rb_minimum(z->right());
        y_color = y->color;
        x = y->right();
        if (y->p == z)
        {
            x_parent = y;                           // was: x->p = y; (only for NIL)
        }
        else
        {
            x_parent = y->p;                        // capture BEFORE rb_transplant(z,y) clobbers y->p
            rb_transplant(y, y->right());
            y->right() = z->right();
            y->right()->p = y;
        }
        rb_transplant(z, y);
        y->left() = z->left();
        y->left()->p = y;
        y->color = z->color;
    }

    if (y_color == Color::BLACK)
        rb_delete_fixup(x_parent, x);
}

void TreeBase::rb_delete_fixup(NodeBase* p, NodeBase* x)   // x may be null
{
    while (x != m_root && is_black(x))
    {
        Dir::Value dir = (x == p->right());   // NOT x->p; p passed in
        NodeBase* w = p->child[!dir];         // sibling — still guaranteed non-null

        if (w->color == Color::RED)
        {              // case 1
            w->color = Color::BLACK; p->color = Color::RED;
            rb_rotate(p, dir);
            w = p->child[!dir];
        }
        if (is_black(w->left()) && is_black(w->right()))
        {   // case 2
            w->color = Color::RED;
            x = p; p = x->p;             // advance BOTH, not just x
        }
        else
        {
            if (is_black(w->child[!dir]))
            {         // case 3
                w->child[dir]->color = Color::BLACK;  // near nephew: provably red ⇒ non-null
                w->color = Color::RED;
                rb_rotate(w, !dir);
                w = p->child[!dir];
            }
            w->color = p->color;               // case 4
            p->color = Color::BLACK;
            w->child[!dir]->color = Color::BLACK;    // far nephew: provably red ⇒ non-null
            rb_rotate(p, dir);
            x = m_root;
        }
    }
    if (x) x->color = Color::BLACK;                  // x may be null
}

template <typename N>
void TreeBase::rb_inorder(N* x, TraversalCB<N> cb)
{
    if (!x) return;
    rb_inorder(x->left(), cb);
    cb(x);
    rb_inorder(x->right(), cb);
}
template <typename N>
void TreeBase::rb_preorder(N* x, TraversalCB<N> cb)
{
    if (!x) return;
    cb(x);
    rb_inorder(x->left(), cb);
    rb_inorder(x->right(), cb);
}
template <typename N>
void TreeBase::rb_postorder(N* x, TraversalCB<N> cb)
{
    if (!x) return;
    rb_inorder(x->left(), cb);
    rb_inorder(x->right(), cb);
    cb(x);
}

template void TreeBase::rb_inorder<NodeBase>(NodeBase*, TraversalCB<NodeBase>);
template void TreeBase::rb_preorder<NodeBase>(NodeBase*, TraversalCB<NodeBase>);
template void TreeBase::rb_postorder<NodeBase>(NodeBase*, TraversalCB<NodeBase>);

template void TreeBase::rb_inorder<const NodeBase>(const NodeBase*, TraversalCB<const NodeBase>);
template void TreeBase::rb_preorder<const NodeBase>(const NodeBase*, TraversalCB<const NodeBase>);
template void TreeBase::rb_postorder<const NodeBase>(const NodeBase*, TraversalCB<const NodeBase>);

} // namespace rb


