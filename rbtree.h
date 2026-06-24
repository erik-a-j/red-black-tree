#ifndef RBTREE_H
#define RBTREE_H

#include <cstddef>
#include <cstdint>
#include <utility>
#include <iostream>
#include <string>
#include <functional>

namespace rb {

using ColorValue = uintptr_t;
struct Color {
    enum : ColorValue {
        Red = 1ULL << 0,
        Black = 1ULL << 1,
        Mask = Red | Black,
    };
};

struct Node;
struct NodePtr {

    static constexpr uintptr_t kNIL{Color::Black | reinterpret_cast<uintptr_t>(nullptr)};

    //template <typename T>
    NodePtr(Node* n) : raw{Color::Red | reinterpret_cast<uintptr_t>(n)} {}
    NodePtr() : raw{kNIL} {}

    uintptr_t raw;
    Node* node() { return reinterpret_cast<Node*>(raw & ~(alignof(Node) - 1)); }
    const Node* node() const { return reinterpret_cast<const Node*>(raw & ~(alignof(Node) - 1)); }
    ColorValue color() const { return raw & Color::Mask; }
    Node* operator->() { return node(); }
    const Node* operator->() const { return node(); }

    explicit operator bool() const noexcept { return node() != nullptr; }
};

//template <typename T = int>
struct Node {
    using T = int;
    NodePtr parent;
    NodePtr left;
    NodePtr right;
    T data;
    Node(const T& data_)
        : parent{}, left{}, right{}, data{data_}
    {
    }
};

//template <typename T = int, typename Cmp = std::less<>>
class Tree {
    using T = int;
    using Cmp = std::less<>;
    static_assert(alignof(Node) >= 4, "Alignment faulty");

public:

    Tree() = default;
    ~Tree() { destroy(m_root); }

    Tree& insert(const T& data)
    {
        Node* node{new Node{data}};
        NodePtr z{node};
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
    NodePtr m_root{};
    Cmp m_cmp{};

    static ColorValue color_of(const NodePtr& n)
    {
        return n ? n.color() : Color::Black;
    }
    static NodePtr& grandparent_of(NodePtr& n)
    {
        return (n && n->parent) ? n->parent->parent : NodePtr{};
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

    void bst_insert(NodePtr& z)
    {
        NodePtr parent{};
        NodePtr curr{m_root};

        while (curr)
        {
            parent = curr;
            curr = m_cmp(z->data, curr->data) ? curr->left : curr->right;
        }

        z->parent = parent;
        if (!parent)
            m_root = z;
        else if (m_cmp(z->data, parent->data))
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

} // namespace rb

#endif /* #ifndef RBTREE_H */