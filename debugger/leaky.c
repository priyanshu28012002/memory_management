#include <stdlib.h>
#include <stdio.h>

void leak_memory() {
    int* data = malloc(100 * sizeof(int));  // allocated but not freed
    data[0] = 42;
    printf("Leaked memory with value: %d\n", data[0]);
}

int main() {
    leak_memory();
    return 0;
}
