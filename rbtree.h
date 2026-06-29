#ifndef RBTREE_H
#define RBTREE_H

#include <cstddef>
#include <cstdint>
#include <utility>
#include <iostream>
#include <string>
#include <functional>
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

namespace Color {
enum Value : uint8_t {
    RED = 0, BLACK = 1
};
}
namespace Dir {
enum Value : uint8_t {
    LEFT = 0, RIGHT = 1
};
}

class NodeBase {
    struct NIL_tag {};
    NodeBase(NIL_tag)
        : p{this}, child{this, this}, color{Color::BLACK}
    {
    }
protected:
    NodeBase() : p{NIL()}, child{NIL(), NIL()}, color{Color::RED} {}

public:
    NodeBase* p;
    NodeBase* child[2];
    Color::Value color;

    NodeBase*& left() noexcept { return child[Dir::LEFT]; }
    NodeBase*& right() noexcept { return child[Dir::RIGHT]; }
    NodeBase*& uncle() noexcept { return p->p->child[p == p->p->left()]; }
    NodeBase* const& left() const noexcept { return child[Dir::LEFT]; }
    NodeBase* const& right() const noexcept { return child[Dir::RIGHT]; }
    NodeBase* const& uncle() const noexcept { return p->p->child[p == p->p->left()]; }

    static NodeBase* NIL()
    {
        static NodeBase nil{NIL_tag{}};
        return &nil;
    }
};

template <typename K, typename V>
struct Node : NodeBase {
    K key;
    V value;

    Node(const K& k, const V& v)
        : NodeBase{}, key{k}, value{v}
    {
    }

};

template <typename K, typename V, typename Cmp = std::less<>, typename Alloc = std::allocator<V>>
class Tree {
    //using K = int;
    //using V = float;
    //using Cmp = std::less<>;
    //using Alloc = std::allocator<V>;
    using Node = rb::Node<K, V>;
    using NodeAllocator = std::allocator_traits<Alloc>::template rebind_alloc<Node>;
    using NodeTraits = std::allocator_traits<Alloc>::template rebind_traits<Node>;

    NodeAllocator m_alloc;
    NodeBase* m_root;
    Cmp m_cmp;
public:

    Tree() : m_alloc{}, m_root{Node::NIL()}, m_cmp{} {}
    ~Tree()
    {
        destroy(m_root);
    }

    Tree& insert(const K& key, const V& value)
    {
        rb_fix_insert(rb_insert(key, value));
        return *this;
    }
    void print_inorder() const
    {
        inorder(m_root);
        std::cout << '\n';
    }
private:
    void rotate(NodeBase* x, Dir::Value dir)
    {
        NodeBase* y = x->child[!dir];    // set y
        x->child[!dir] = y->child[dir];  // turn y's dir subtree into x's !dir subtree
        if (y->child[dir] != NodeBase::NIL())
            y->child[dir]->p = x;

        y->p = x->p;                     // link x's parent to y
        if (x->p == NodeBase::NIL())
            m_root = y;
        else if (x == x->p->child[dir])
            x->p->child[dir] = y;
        else
            x->p->child[!dir] = y;

        y->child[dir] = x;               // put x on y's dir
        x->p = y;
    }

    NodeBase* rb_insert(const K& k, const V& v)
    {
        NodeBase* p = Node::NIL();
        NodeBase* curr{m_root};

        while (curr != Node::NIL())
        {
            const K& curr_k = static_cast<Node*>(curr)->key;
            if (m_cmp(k, curr_k))      curr = curr->left();
            else if (m_cmp(curr_k, k)) curr = curr->right();
            else
            {
                static_cast<Node*>(curr)->value = v;
                return curr;
            }
            p = curr;
        }

        Node* z = NodeTraits::allocate(m_alloc, 1);
        NodeTraits::construct(m_alloc, z, k, v);
        z->p = p;

        const K& p_key = static_cast<Node*>(p)->key;
        if (p == Node::NIL())          m_root = z;
        else if (m_cmp(z->key, p_key)) p->left() = z;
        else                           p->right() = z;

        return z;
    }

    void rb_fix_insert(NodeBase* z)
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
                    rotate(z, Dir::LEFT);
                }
                z->p->color = Color::BLACK;
                z->p->p->color = Color::RED;
                rotate(z->p->p, Dir::RIGHT);
            }
            else
            {
                if (z == z->p->left())
                {
                    z = z->p;
                    rotate(z, Dir::RIGHT);
                }
                z->p->color = Color::BLACK;
                z->p->p->color = Color::RED;
                rotate(z->p->p, Dir::LEFT);
            }
        }
        m_root->color = Color::BLACK;
    }

    void inorder(const NodeBase* n) const
    {
        if (n == NodeBase::NIL()) return;
        inorder(n->left());
        const Node* node = static_cast<const Node*>(n);
        std::cout << (node->color == Color::RED ? "(\x1b[38;5;196mR\x1b[0m)" : "(\x1b[38;5;8mB\x1b[0m)") << node->key << ":" << node->value
            << " ";
        inorder(n->right());
    }

    void destroy(NodeBase* n)
    {
        if (n == NodeBase::NIL()) return;
        destroy(n->left());
        destroy(n->right());
        Node* node = static_cast<Node*>(n);
        NodeTraits::destroy(m_alloc, node);
        NodeTraits::deallocate(m_alloc, node, 1);
    }
};

} // namespace rb

#endif /* #ifndef RBTREE_H */