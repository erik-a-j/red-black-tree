#include "rbtree.h"

int main()
{
    rb::Tree<int, float> rb;
    for (int i = 0; i <= 15; ++i)
    {
        rb.insert(i, 0.5f * i);
    }
    rb.print_inorder();

    return 0;
}