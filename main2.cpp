#include "rbtree2.h"

struct Node : rb::Node {};

struct Cmp {
    int compare(rb::Node* a, rb::Node* b);
};

int main()
{

    return 0;
}