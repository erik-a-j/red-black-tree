#include "rbtree.h"

namespace rb {

void TreeBase::rb_insert_fixup(NodeBase* z)
{
    while (z->p->color == Color::RED)
    {
        Dir::Value dir = z->p == z->p->p->right();
        NodeBase* y = z->p->p->child[!dir];

        if (y->color == Color::RED)          // case 1
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

void TreeBase::rb_delete(NodeBase* z)
{
    NodeBase* x;
    NodeBase* y = z;
    Color::Value y_color = y->color;

    if (z->left() == NIL())
    {
        x = z->right();
        rb_transplant(z, z->right());
    }
    else if (z->right() == NIL())
    {
        x = z->left();
        rb_transplant(z, z->left());
    }
    else
    {
        y = rb_minimum(z->right());
        y_color = y->color;
        x = y->right();
        if (y->p == z)
        {
            x->p = y;
        }
        else
        {
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
        rb_delete_fixup(x);
}

void TreeBase::rb_delete_fixup(NodeBase* x)
{
    while (x != m_root && x->color == Color::BLACK)
    {
        Dir::Value dir = x == x->p->right();      // side x is on
        NodeBase* w = x->p->child[!dir];          // sibling

        if (w->color == Color::RED)               // case 1
        {
            w->color = Color::BLACK;
            x->p->color = Color::RED;
            rb_rotate(x->p, dir);
            w = x->p->child[!dir];                // new sibling
        }

        if (w->left()->color == Color::BLACK &&   // case 2
            w->right()->color == Color::BLACK)
        {
            w->color = Color::RED;
            x = x->p;
        }
        else
        {
            if (w->child[!dir]->color == Color::BLACK)    // case 3: far nephew black
            {
                w->child[dir]->color = Color::BLACK;      //   near nephew → black
                w->color = Color::RED;
                rb_rotate(w, !dir);
                w = x->p->child[!dir];                    //   refetch sibling
            }
            w->color = x->p->color;                       // case 4
            x->p->color = Color::BLACK;
            w->child[!dir]->color = Color::BLACK;         //   far nephew → black
            rb_rotate(x->p, dir);
            x = m_root;
        }
    }
    x->color = Color::BLACK;
}




} // namespace rb


