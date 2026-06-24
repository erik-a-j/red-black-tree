#include "rbtree.h"
struct C {
    enum : uintptr_t { SAS };
};
int main()
{
    int a = C::SAS;
    RBTree<int> rb;
    for (int i = 0; i <= 15; ++i)
    {
        rb.insert(i);
    }
    rb.print_inorder();

    return 0;
}