#ifndef RBTREE_H
#define RBTREE_H

#include <cstddef>
#include <cstdint>
#include <utility>
#include <iostream>
#include <string>

template <typename T>
class RBTree {
    enum class Color : bool {
        Red = false, Black = true
    };
    struct Node {
        T data;
        Color color{Color::Red};
        Node* parent{nullptr};
        Node* left{nullptr};
        Node* right{nullptr};
    };

public:

    RBTree() = default;
    ~RBTree() { destroy(m_root); }

    RBTree& insert(const T& data)
    {
        Node* z = new Node{data};
        bst_insert(z);
        fix_insert(z);
        return *this;
    }
    void print_inorder() const
    {
        inorder(m_root);
        std::cout << '\n';
    }
private:
    Node* m_root{};

    static Color color_of(const Node* n)
    {
        return n ? n->color : Color::Black;
    }
    static Node* grandparent_of(const Node* n)
    {
        return (n && n->parent) ? n->parent->parent : nullptr;
    }
    static Node* uncle_of(const Node* n)
    {
        Node* g = grandparent_of(n);
        if (!g) return nullptr;
        return (n->parent == g->left) ? g->right : g->left;
    }

    void rotate_left(Node* x)
    {
        Node* y = x->right;

        x->right = y->left;
        if (y->left) y->left->parent = x;

        y->parent = x->parent;
        if (!x->parent)                m_root = y;
        else if (x == x->parent->left) x->parent->left = y;
        else                           x->parent->right = y;

        y->left = x;
        x->parent = y;
    }
    void rotate_right(Node* x)
    {
        Node* y = x->left;

        x->left = y->right;
        if (y->right) y->right->parent = x;

        y->parent = x->parent;
        if (!x->parent)                 m_root = y;
        else if (x == x->parent->right) x->parent->right = y;
        else                            x->parent->left = y;

        y->right = x;
        x->parent = y;
    }

    void bst_insert(Node* z)
    {
        Node* parent{nullptr};
        Node* curr{m_root};

        while (curr)
        {
            parent = curr;
            curr = (z->data < curr->data) ? curr->left : curr->right;
        }

        z->parent = parent;
        if (!parent)
            m_root = z;
        else if (z->data < parent->data)
            parent->left = z;
        else
            parent->right = z;
    }
    void fix_insert(Node* z)
    {
        while (color_of(z->parent) == Color::Red)
        {
            Node* p = z->parent;
            Node* g = grandparent_of(z);
            Node* u = uncle_of(z);

            if (color_of(u) == Color::Red)
            {
                p->color = Color::Black;
                u->color = Color::Black;
                g->color = Color::Red;
                z = g;
            }
            else if (p == g->left)
            {
                if (z == p->right)
                {
                    z = p;
                    rotate_left(z);
                    p = z->parent;
                    g = grandparent_of(z);
                }

                p->color = Color::Black;
                g->color = Color::Red;
                rotate_right(g);
            }
            else
            {
                if (z == p->left)
                {
                    z = p;
                    rotate_right(z);
                    p = z->parent;
                    g = grandparent_of(z);
                }

                p->color = Color::Black;
                g->color = Color::Red;
                rotate_left(g);
            }
        }
        m_root->color = Color::Black;
    }

    void inorder(const Node* n) const
    {
        if (!n) return;
        inorder(n->left);
        std::cout << (n->color == Color::Red ? "(\x1b[38;5;196mR\x1b[0m)" : "(\x1b[38;5;8mB\x1b[0m)") << n->data
            << " ";
        inorder(n->right);
    }

    void destroy(Node* n)
    {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }
};

#endif /* #ifndef RBTREE_H */