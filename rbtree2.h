#ifndef RBTREE_H
#define RBTREE_H

#include <cstddef>
#include <cstdint>
#include <utility>
#include <iostream>
#include <string>
#include <functional>
#include <type_traits>
#include <memory>

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

enum Dir : uint8_t {
    DIR_BIT = 1U << 0, LEFT = 0, RIGHT = DIR_BIT
};
enum Color : uint8_t {
    COLOR_BIT = 1U << 1, BLACK = 0, RED = COLOR_BIT
};

class Node;
static inline uintptr_t ptr_conv(Node* v) noexcept { return reinterpret_cast<uintptr_t>(v); }
static inline Node* ptr_conv(uintptr_t v) noexcept { return reinterpret_cast<Node*>(v); }

class Node {
    struct NIL_tag {};
    Node(NIL_tag)
        : parent{ptr_conv(this)}, child{ptr_conv(this), ptr_conv(this)}
    {}
public:
    uintptr_t parent;
    uintptr_t child[2];

    Node() : parent{ptr_conv(NIL()) | Color::RED}, child{ptr_conv(NIL()), ptr_conv(NIL())} {}


    Node* p() noexcept { return ptr_conv(addr(child[Dir::LEFT])); }
    Node* left() noexcept { return ptr_conv(addr(child[Dir::LEFT])); }
    Node* right() noexcept { return ptr_conv(addr(child[Dir::RIGHT])); }
    Node* uncle() noexcept { return p->p->child[p == p->p->left()]; }

    static Node* NIL()
    {
        static Node nil{NIL_tag{}};
        return &nil;
    }
    static uintptr_t addr(uintptr_t v) noexcept { return v & ~3ULL; }
};

inline Node* const NIL = Node::NIL();


//template <typename T = int, typename Cmp = std::less<>>
class Tree {
    using K = int;
    using V = float;
    using Cmp = std::less<>;
    using Alloc = std::allocator<V>;
    using Node = rb::Node<K, V>;
    using NodeAllocator = std::allocator_traits<Alloc>::template rebind_alloc<Node>;
    using NodeTraits = std::allocator_traits<Alloc>::template rebind_traits<Node>;

    NodeBase* m_root;
    Cmp m_cmp{};
public:

    Tree() : m_root{Node::NIL()} {}
    ~Tree() { destroy(m_root); }

    Tree& insert(const K& key, const V& value)
    {

        bst_insert(key, value);
        fix_insert(z);
        return *this;
    }
    void print_inorder() const
    {
        inorder(m_root);
        std::cout << '\n';
    }
private:
    void rotate_left(NodeT* x)
    {
        NodeT* y = x->right;

        x->right = y->left;
        if (y->left) y->left->parent = x;

        y->parent = x->parent;
        if (!x->parent)                m_root = y;
        else if (x == x->parent->left) x->parent->left = y;
        else                           x->parent->right = y;

        y->left = x;
        x->parent = y;
    }
    void rotate_right(NodeT* x)
    {
        NodeT* y = x->left;

        x->left = y->right;
        if (y->right) y->right->parent = x;

        y->parent = x->parent;
        if (!x->parent)                 m_root = y;
        else if (x == x->parent->right) x->parent->right = y;
        else                            x->parent->left = y;

        y->right = x;
        x->parent = y;
    }


    NodeBase* bst_insert(const K& k, const V& v)
    {
        NodeBase* p = NIL;
        NodeBase* curr{m_root};

        while (curr != NIL)
        {
            const K& curr_k = static_cast<Node*>(curr)->key;
            if (m_cmp(k, curr_k))      curr = curr->left();
            else if (m_cmp(curr_k, k)) curr = curr->right();
            else
            {
                curr->value = v;
                return;
            }
            p = curr;
        }

        Node* z{new Node{k, v}};
        z->p = p;

        if (p == Node::NIL())          m_root = z;
        else if (m_cmp(z->key, p->key)) p->left() = z;
        else                            p->right() = z;
    }

    void fix_insert(Node* z)
    {

        while (z->p->color == Color::RED)
        {
            Node* p = z->p;
            Node* g = z->p->p;
            Node* u = z->uncle();

            if (z->uncle()->color == Color::RED)
            {
                z->p->color = Color::BLACK;
                z->uncle()->color = Color::BLACK;
                z->p->p->color = Color::RED;
                z = z->p->p;
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

                p->color = Color::BLACK;
                g->color = Color::RED;
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

                p->color = Color::BLACK;
                g->color = Color::RED;
                rotate_left(g);
            }
        }
        m_root->color = Color::BLACK;
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