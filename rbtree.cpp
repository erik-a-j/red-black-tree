#include "rbtree.h"

namespace rb {

NodeBase* rb_minimum(NodeBase* n, NodeBase* NIL)
{
    while (n->left() != NIL)
        n = n->left();
    return n;
}

void rb_rotate(TreeBase t, NodeBase* x, Dir::Value dir)
{
    NodeBase* y = x->child[!dir];    // set y
    x->child[!dir] = y->child[dir];  // turn y's dir subtree into x's !dir subtree
    if (y->child[dir] != t.NIL)
        y->child[dir]->p = x;

    y->p = x->p;                     // link x's parent to y
    if (x->p == t.NIL)
        t.root = y;
    else if (x == x->p->child[dir])
        x->p->child[dir] = y;
    else
        x->p->child[!dir] = y;

    y->child[dir] = x;               // put x on y's dir
    x->p = y;
}

void rb_fix_insert(TreeBase t, NodeBase* z)
{
    while (z->p->color == Color::RED)
    {
        if (z->uncle()->color == Color::RED)
        {
            z->p->color = Color::BLACK;
            z->uncle()->color = Color::BLACK;
            z->p->p->color = Color::RED;
            z = z->p->p;
        }
        else if (z->p == z->p->p->left())
        {
            if (z == z->p->right())
            {
                z = z->p;
                rb_rotate(t, z, Dir::LEFT);
            }
            z->p->color = Color::BLACK;
            z->p->p->color = Color::RED;
            rb_rotate(t, z->p->p, Dir::RIGHT);
        }
        else
        {
            if (z == z->p->left())
            {
                z = z->p;
                rb_rotate(t, z, Dir::RIGHT);
            }
            z->p->color = Color::BLACK;
            z->p->p->color = Color::RED;
            rb_rotate(t, z->p->p, Dir::LEFT);
        }
    }
    t.root->color = Color::BLACK;
}

void rb_transplant(TreeBase t, NodeBase* u, NodeBase* v)
{
    if (u->p == t.NIL)
        t.root = v;
    else if (u == u->p->left())
        u->p->left() = v;
    else
        u->p->right() = v;
    v->p = u->p;
}

void rb_delete(TreeBase t, NodeBase* z)
{
    NodeBase* x;
    NodeBase* y = z;
    Color::Value y_color = y->color;

    if (z->left() == t.NIL)
    {
        x = z->right();
        rb_transplant(t, z, z->right());
    }
    else if (z->right() == t.NIL)
    {
        x = z->left();
        rb_transplant(t, z, z->left());
    }
    else
    {
        y = rb_minimum(z->right(), t.NIL);
        y_color = y->color;
        x = y->right();
        if (y->p == z)
            x->p = y;
        else
        {
            rb_transplant(t, y, y->right());
            y->right() = z->right();
            y->right()->p = y;
        }
        rb_transplant(t, z, y);
        y->left() = z->left();
        y->left()->p = y;
        y->color = z->color;
    }

    if (y_color == Color::BLACK) {} // fix
}

} // namespace rb