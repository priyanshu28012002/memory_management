#include <my_alloc/my_alloc.hpp>
#include <iostream>
#include <unistd.h>
#include <ds.hpp>
#define N 10

int main() {
    void* current_brk = sbrk(0);
    std::cout << "Starting Pointer of the heap is " << current_brk << std::endl;

    void* ptrs[N] = {0};

    for (int i = 0; i < N; i++) {
        ptrs[i] = my_alloc::heap_alloc(i);
    }

    for (int i = 0; i < N; i++) {
        my_alloc::heap_free(ptrs[i]);
    }

    void* p = my_alloc::heap_alloc(100);
    if (p == NULL) {
        std::cout << "Failed to allocate memory\n";
    }

    my_alloc::heap_dump_stats();
    return 0;
}

