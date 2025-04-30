#include <iostream>
#include <unistd.h> // For getpagesize()
#include <sys/resource.h> // For getrusage()

long getHeapSize() {
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        return usage.ru_maxrss * 1024; // Convert kilobytes to bytes
    } else {
        return -1; // Error occurred
    }
}

int main() {
    long heapSize = getHeapSize();

    if (heapSize != -1) {
        std::cout << "Approximate heap size: " << heapSize << " bytes" << std::endl;
    } else {
        std::cerr << "Failed to retrieve heap size." << std::endl;
    }

    // Example of allocating some heap memory (for demonstration)
    int* myArray = new int[100000];
    if(myArray == nullptr){
        std::cerr << "Memory allocation failed." << std::endl;
        return 1;
    }
    long heapSizeAfterAllocation = getHeapSize();
    if (heapSizeAfterAllocation != -1) {
        std::cout << "Approximate heap size after allocation: " << heapSizeAfterAllocation << " bytes" << std::endl;
    }
    delete[] myArray; // Free the allocated memory.

    return 0;
}


/*


Explanation and Important Considerations:

getrusage():
This function from the sys/resource.h header is used to retrieve resource usage statistics for the current process.
RUSAGE_SELF specifies that we want statistics for the calling process itself.
ru_maxrss is the maximum resident set size used during the process's lifetime, measured in kilobytes. This is the closest metric to heap memory usage that getrusage() provides.
It is important to understand that this is the maximum resident set size, and not the exact heap size. There are other things, besides the heap, that can contribute to the resident set size.
Kilobytes to Bytes Conversion:
ru_maxrss is in kilobytes, so we multiply it by 1024 to get the size in bytes.
Error Handling:
The getrusage() function returns 0 on success and -1 on error. The code checks for this error and prints an error message if necessary.
Heap Allocation Example:
The new int[100000] line demonstrates how to allocate memory on the heap.
delete[] myArray frees the allocated memory, preventing memory leaks.
Limitations:
getrusage() provides an approximation of the heap size. It might include other memory usage besides the heap.
The accuracy of ru_maxrss can vary depending on the operating system.
This method is not portable to windows. For windows, you would need to use windows specific API calls.
Portability:
This code is written for POSIX-compliant systems (like Linux and macOS). It will not work on Windows without significant modification.

*/