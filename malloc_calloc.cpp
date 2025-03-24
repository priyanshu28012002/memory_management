#include <iostream>
#include <cstdlib> 

struct MyStruct {
    int a;
    double b;
};

int main() {
    // 1. Using malloc for int array
    int *ptr = (int*)malloc(sizeof(int) * 100);
    if (!ptr) {
        std::cerr << "Memory allocation failed!\n";
        return 1;
    }

    std::cout << "Using malloc with int array:\n";
    int *temp = ptr; // Store original pointer
    for (int i = 0; i < 10; i++) {
        *temp = i;
        std::cout << "Address: " << temp << " Value: " << *temp << std::endl;
        temp++; // Pointer arithmetic
    }

    // 2. Reallocating memory
    ptr = (int*)realloc(ptr, sizeof(int) * 200); // Expand memory
    if (!ptr) {
        std::cerr << "Reallocation failed!\n";
        return 1;
    }
    std::cout << "\nAfter reallocating to 200 integers:\n";
    for (int i = 10; i < 20; i++) {
        ptr[i] = i;
        std::cout << "Address: " << &ptr[i] << " Value: " << ptr[i] << std::endl;
    }

    free(ptr); // Free allocated memory

    // 3. Using calloc (zero-initialized)
    int *ptr_calloc = (int*)calloc(10, sizeof(int));
    if (!ptr_calloc) {
        std::cerr << "Calloc failed!\n";
        return 1;
    }

    std::cout << "\nUsing calloc (zero-initialized memory):\n";
    for (int i = 0; i < 10; i++) {
        std::cout << "Address: " << &ptr_calloc[i] << " Value: " << ptr_calloc[i] << std::endl;
    }
    free(ptr_calloc); // Free allocated memory

    // 4. Using malloc for struct


    
    MyStruct *structPtr = (MyStruct*)malloc(sizeof(MyStruct));
    if (!structPtr) {
        std::cerr << "Memory allocation for struct failed!\n";
        return 1;
    }
    structPtr->a = 42;
    structPtr->b = 3.14;
    std::cout << "\nUsing malloc for struct:\n";
    std::cout << "Struct Address: " << structPtr << " a: " << structPtr->a << " b: " << structPtr->b << std::endl;
    free(structPtr); // Free allocated memory

    // 5. Using new (C++ style)
    int *cppPtr = new int[10];
    std::cout << "\nUsing new (C++ style):\n";
    for (int i = 0; i < 10; i++) {
        cppPtr[i] = i * 2;
        std::cout << "Address: " << &cppPtr[i] << " Value: " << cppPtr[i] << std::endl;
    }
    delete[] cppPtr; // Free allocated memory

    return 0;
}
