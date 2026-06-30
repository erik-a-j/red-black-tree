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
static inline void rb_rotate_left(TreeBase t, NodeBase* x)
{
    rb_rotate(t, x, Dir::LEFT);
}
static inline void rb_rotate_right(TreeBase t, NodeBase* x)
{
    rb_rotate(t, x, Dir::RIGHT);
}

void rb_insert_fixup(TreeBase t, NodeBase* z)
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
                rb_rotate(t, z, dir);
            }
            z->p->color = Color::BLACK;      // case 3
            z->p->p->color = Color::RED;
            rb_rotate(t, z->p->p, !dir);
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

    if (y_color == Color::BLACK)
        rb_delete_fixup(t, x);
}

void rb_delete_fixup(TreeBase t, NodeBase* x)
{
    while (x != t.root && x->color == Color::BLACK)
    {
        Dir::Value dir = x == x->p->right();      // side x is on
        NodeBase* w = x->p->child[!dir];          // sibling

        if (w->color == Color::RED)               // case 1
        {
            w->color = Color::BLACK;
            x->p->color = Color::RED;
            rb_rotate(t, x->p, dir);
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
                rb_rotate(t, w, !dir);
                w = x->p->child[!dir];                    //   refetch sibling
            }
            w->color = x->p->color;                       // case 4
            x->p->color = Color::BLACK;
            w->child[!dir]->color = Color::BLACK;         //   far nephew → black
            rb_rotate(t, x->p, dir);
            x = t.root;
        }
    }
    x->color = Color::BLACK;
}

} // namespace rb


