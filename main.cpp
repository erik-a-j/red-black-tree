#include "rbtree.h"

#include <map>
#include <array>

struct C {
    enum : uintptr_t { SAS };
};
int main()
{
    std::to_underlying()
    int a = C::SAS;
    RBTree<int> rb;
    for (int i = 0; i <= 15; ++i)
    {
        rb.insert(i);
    }
    rb.print_inorder();

    return 0;
}