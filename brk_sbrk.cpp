#include <iostream>
#include <unistd.h> 

int main() {
    // Get the current program break
    int *initial_break = (int *)sbrk(0);
    std::cout << "Initial heap break: " << initial_break << std::endl;

    // Allocate space for 10 integers
    int *ptr = (int *)sbrk(sizeof(int) * 10);
    std::cout << "New heap break after allocation: " << sbrk(0) << std::endl;

    int *starting = ptr;

    // Store values and print memory locations
    for (int i = 0; i < 10; i++) {
        ptr[i] = i * 10;
    }

    std::cout << "\nStored values in allocated memory:\n";
    for (int i = 0; i < 10; i++) {
        std::cout << "Address: " << &ptr[i] << " Value: " << ptr[i] << std::endl;
    }

    // Experiment 1: Check memory alignment
    std::cout << "\nMemory Alignment Test:\n";
    if ((uintptr_t)ptr % alignof(int) == 0) {
        std::cout << "Memory is properly aligned for integers.\n";
    } else {
        std::cout << "Memory alignment issue detected!\n";
    }

    // Experiment 2: Change heap break using brk()
    void *new_break = (void *)((char *)starting + sizeof(int) * 5); // Shrink heap to 5 elements
    int brk_status = brk(new_break);
    if (brk_status == 0) {
        std::cout << "\nHeap successfully reduced using brk().\n";
    } else {
        std::cerr << "\nbrk() failed!\n";
    }

    std::cout << "New heap break after brk(): " << sbrk(0) << std::endl;

    return 0;
}
