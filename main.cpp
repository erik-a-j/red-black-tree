#include "rbtree.h"

int main()
{
    rb::Tree<int, float> rb;
    for (int i = 1; i <= 7; ++i)
    {
        rb.insert(i, 0.5f * i);
    }
    //rb.print_inorder();
    rb.erase(4);
    rb.insert(4, 0.5f * 4);
    //rb.print_inorder();

    rb.verify(std::cerr);

    return 0;
}