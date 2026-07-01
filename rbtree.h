#ifndef RBTREE_H
#define RBTREE_H

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include "rbtree_internal.h"

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

template <typename K, typename V, typename Cmp = std::less<>, typename Alloc = std::allocator<V>>
class Tree : private TreeBase {
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

    using NodeAllocatorType = std::allocator_traits<Alloc>::template rebind_alloc<Node>;
    using NodeTraits = std::allocator_traits<Alloc>::template rebind_traits<Node>;

    struct NodeAllocator : NodeAllocatorType {
        Node* create(NodeBase* nil, const K& k, const V& v)
        {
            Node* node = NodeTraits::allocate(*this, 1);
            try { NodeTraits::construct(*this, node, nil, k, v); }
            catch (...) { NodeTraits::deallocate(*this, node, 1); throw; }
            return node;
        }
        void destroy(Node* n)
        {
            NodeTraits::destroy(*this, n);
            NodeTraits::deallocate(*this, n, 1);
        }
    };

    NodeAllocator m_alloc{};
    Cmp m_cmp{};
    size_t m_size{0};
public:

    Tree() : TreeBase{} {}

    Tree(const Tree&) = delete;
    Tree& operator=(const Tree&) = delete;
    Tree(Tree&&) = delete;
    Tree& operator=(Tree&&) = delete;

    ~Tree()
    {
        destroy(m_root);
    }

    size_t size() const noexcept { return m_size; }

    Tree& insert(const K& key, const V& value)
    {
        insert_impl(key, value);
        return *this;
    }
    bool erase(const K& key)
    {
        return erase_impl(key);
    }

    bool verify(std::ostream& os)
    {
        bool ok{true};
        if (m_root != NIL())
        {
            int bh = 0;
            if (m_root->color == Color::RED)
            {
                os << "VIOLATION: root is red\n";
                ok = false;
            }
            else
            {
                bh = rb_check_subtree(m_root, [&ok, &os](Rules r, NodeBase* n, int lh, int rh) {
                    ok = false;
                    if (r == Rules::R4)
                        os << "VIOLATION: consecutive reds at key " << key_of(n) << '\n';
                    else if (r == Rules::R5)
                        os << "VIOLATION: black-height mismatch at key " << key_of(n)
                        << "  (left=" << lh << ", right=" << rh << ")\n";
                    });
            }
            if (ok)
                os << "All RB properties satisfied. BH=" << bh << '\n';
        }
        return ok;
    }

    void print_inorder() const
    {
        inorder(m_root);
        std::cout << '\n';
    }
private:
    static const K& key_of(const Node* n) { return n->key; }
    static const K& key_of(const NodeBase* n) { return key_of(static_cast<const Node*>(n)); }
    static const V& value_of(const Node* n) { return n->value; }
    static const V& value_of(const NodeBase* n) { return value_of(static_cast<const Node*>(n)); }
    static V& value_of(Node* n) { return n->value; }
    static V& value_of(NodeBase* n) { return value_of(static_cast<Node*>(n)); }

    NodeBase* find_node(const K& k)
    {
        NodeBase* curr = m_root;
        while (curr != NIL())
        {
            if (m_cmp(k, key_of(curr)))      curr = curr->left();
            else if (m_cmp(key_of(curr), k)) curr = curr->right();
            else                             break;
        }
        return curr;
    }

    void insert_impl(const K& k, const V& v)
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
                return;
            }
        }

        Node* z = m_alloc.create(NIL(), k, v);
        z->p = p;

        if (p == NIL())                    m_root = z;
        else if (m_cmp(z->key, key_of(p))) p->left() = z;
        else                               p->right() = z;

        rb_insert_fixup(z);
        ++m_size;
    }

    bool erase_impl(const K& k)
    {
        NodeBase* rem = find_node(k);
        if (rem == NIL()) return false;
        rb_delete(rem);
        m_alloc.destroy(static_cast<Node*>(rem));
        --m_size;
        return true;
    }

    void inorder(const NodeBase* n) const
    {
        if (n == NIL()) return;
        inorder(n->left());
        std::cout << (n->color == Color::RED ? "(\x1b[38;5;196mR\x1b[0m)" : "(\x1b[38;5;8mB\x1b[0m)")
            << key_of(n) << " ";
        inorder(n->right());
    }

    void destroy(NodeBase* n)
    {
        if (n == NIL()) return;
        destroy(n->left());
        destroy(n->right());
        m_alloc.destroy(static_cast<Node*>(n));
    }
};

} // namespace rb

#endif /* #ifndef RBTREE_H */