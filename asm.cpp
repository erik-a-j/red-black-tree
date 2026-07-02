#include <cstdint>

struct A {
    int a;
};
constexpr uintptr_t A_PTRMASK = ~(alignof(A) - 1);

struct tagged {
    uintptr_t v;

    A* operator->() noexcept { return reinterpret_cast<A*>(v & A_PTRMASK); }
};

int get_a1(uintptr_t p)
{
    return reinterpret_cast<A*>(p & A_PTRMASK)->a;
}

int get_a2(tagged p)
{
    return p->a;
}
