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
    NodeBase*& uncle() noexcept { return p->p->child[p == p->p->left()]; }
    NodeBase* const& left() const noexcept { return child[Dir::LEFT]; }
    NodeBase* const& right() const noexcept { return child[Dir::RIGHT]; }
    NodeBase* const& uncle() const noexcept { return p->p->child[p == p->p->left()]; }
};

struct TreeBase {
    NodeBase*& root;
    NodeBase* NIL;
};

void rb_rotate(TreeBase t, NodeBase* x, Dir::Value dir);
void rb_insert_fixup(TreeBase t, NodeBase* z);
void rb_delete(TreeBase t, NodeBase* z);
void rb_delete_fixup(TreeBase t, NodeBase* x);

template <typename K, typename V, typename Cmp = std::less<>, typename Alloc = std::allocator<V>>
class Tree {
    //using K = int;
    //using V = float;
    //using Cmp = std::less<>;
    //using Alloc = std::allocator<V>;
    //using Node = rb::Node<K, V>;

    struct Node : NodeBase {
        K key;
        V value;

        Node(NodeBase* nil, const K& k, const V& v)
            : NodeBase{nil}, key{k}, value{v}
        {
        }
    };

    using NodeAllocator = std::allocator_traits<Alloc>::template rebind_alloc<Node>;
    using NodeTraits = std::allocator_traits<Alloc>::template rebind_traits<Node>;

    NodeBase m_nil;
    NodeBase* m_root;
    NodeAllocator m_alloc{};
    Cmp m_cmp{};
public:

    Tree() : m_nil{NodeBase::NIL_tag{}}, m_root{&m_nil} {}
    ~Tree()
    {
        destroy(m_root);
    }

    Tree& insert(const K& key, const V& value)
    {
        auto [z, need_fix] = rb_insert(key, value);
        if (need_fix) rb_fix_insert(m_root, NIL(), z);
        return *this;
    }
    void print_inorder() const
    {
        inorder(m_root);
        std::cout << '\n';
    }
private:
    Node* make_node(const K& k, const V& v)
    {
        Node* node = NodeTraits::allocate(m_alloc, 1);
        try { NodeTraits::construct(m_alloc, node, NIL(), k, v); }
        catch (...) { NodeTraits::deallocate(m_alloc, node, 1); throw; }
        return node;
    }
    static const K& key_of(const Node* n) { return n->key; }
    static const K& key_of(const NodeBase* n) { return key_of(static_cast<const Node*>(n)); }
    static const V& value_of(const Node* n) { return n->value; }
    static const V& value_of(const NodeBase* n) { return value_of(static_cast<const Node*>(n)); }
    static V& value_of(Node* n) { return n->value; }
    static V& value_of(NodeBase* n) { return value_of(static_cast<Node*>(n)); }
    NodeBase* NIL() noexcept { return &m_nil; }
    const NodeBase* NIL() const noexcept { return &m_nil; }

    std::pair<NodeBase*, bool> rb_insert(const K& k, const V& v)
    {
        NodeBase* p = NIL();
        NodeBase* curr{m_root};

        while (curr != NIL())
        {
            p = curr;
            if (m_cmp(k, key_of(curr)))      curr = curr->left();
            else if (m_cmp(key_of(curr), k)) curr = curr->right();
            else
            {
                value_of(curr) = v;
                return {curr, false};
            }
        }

        Node* z = make_node(k, v);
        z->p = p;

        if (p == NIL())                    m_root = z;
        else if (m_cmp(z->key, key_of(p))) p->left() = z;
        else                               p->right() = z;

        return {z, true};
    }


    void inorder(const NodeBase* n) const
    {
        if (n == NIL()) return;
        inorder(n->left());
        std::cout << (n->color == Color::RED ? "(\x1b[38;5;196mR\x1b[0m)" : "(\x1b[38;5;8mB\x1b[0m)") << key_of(n) << ":" << value_of(n)
            << " ";
        inorder(n->right());
    }

    void destroy(NodeBase* n)
    {
        if (n == NIL()) return;
        destroy(n->left());
        destroy(n->right());
        Node* node = static_cast<Node*>(n);
        NodeTraits::destroy(m_alloc, node);
        NodeTraits::deallocate(m_alloc, node, 1);
    }
};

} // namespace rb

#endif /* #ifndef RBTREE_H */