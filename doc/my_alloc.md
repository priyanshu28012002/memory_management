# Ensuring Memory is Freed and Preventing Memory Leaks

To ensure memory is properly freed and prevent memory leaks in your allocator, you need to implement several safeguards and verification mechanisms. Here's how to modify your code to achieve this:

## 1. Track All Allocations

First, let's add tracking of all active allocations:

```c
size_t total_allocated = 0;
size_t currently_allocated = 0;
```

## 2. Modified Allocation/Free Functions

Update your `heap_alloc` and `heap_free` functions to track memory usage:

```c
void *heap_alloc(size_t size) {
    if (size < 1) {
        return NULL;
    }

    // ... [existing allocation logic] ...

    if (new_mem != (void*)-1) {
        chunks_list_insert(&alloced_chunks, new_mem, size);
        heap_size += size;
        total_allocated += size;
        currently_allocated += size;
        return new_mem;
    }

    return NULL;
}

void heap_free(void *ptr) {
    if (ptr != NULL) {
        const int index = chunks_list_find(&alloced_chunks, ptr);
        if (index >= 0) {
            currently_allocated -= alloced_chunks.chunks[index].size;
            chunks_list_insert(&freed_chunks,
                              alloced_chunks.chunks[index].start,
                              alloced_chunks.chunks[index].size);
            chunks_list_remove(&alloced_chunks, (size_t)index);
        }
    }
}
```

## 3. Add Memory Verification Functions

Add these functions to check for memory leaks:

```c
void verify_no_leaks() {
    if (currently_allocated != 0) {
        fprintf(stderr, "MEMORY LEAK DETECTED: %zu bytes still allocated\n", 
                currently_allocated);
        chunks_list_dump(&alloced_chunks);
        exit(EXIT_FAILURE);
    }
}

void verify_heap_integrity() {
    // Check for overlapping chunks
    for (size_t i = 1; i < alloced_chunks.count; i++) {
        void *prev_end = (char*)alloced_chunks.chunks[i-1].start + 
                        alloced_chunks.chunks[i-1].size;
        if (alloced_chunks.chunks[i].start < prev_end) {
            fprintf(stderr, "HEAP CORRUPTION: Overlapping chunks detected\n");
            exit(EXIT_FAILURE);
        }
    }
    
    // Similar checks for freed chunks
    for (size_t i = 1; i < freed_chunks.count; i++) {
        void *prev_end = (char*)freed_chunks.chunks[i-1].start + 
                        freed_chunks.chunks[i-1].size;
        if (freed_chunks.chunks[i].start < prev_end) {
            fprintf(stderr, "HEAP CORRUPTION: Overlapping freed chunks\n");
            exit(EXIT_FAILURE);
        }
    }
}
```

## 4. Add Boundary Checkers

To detect buffer overflows/underflows:

```c
#define CANARY_SIZE 16
static const unsigned char CANARY_VALUE = 0xAA;

void *heap_alloc_with_canary(size_t size) {
    // Allocate extra space for canaries
    void *ptr = heap_alloc(size + 2*CANARY_SIZE);
    if (!ptr) return NULL;
    
    // Set canaries at start and end
    memset(ptr, CANARY_VALUE, CANARY_SIZE);
    memset((char*)ptr + CANARY_SIZE + size, CANARY_VALUE, CANARY_SIZE);
    
    // Return pointer after start canary
    return (char*)ptr + CANARY_SIZE;
}

void heap_free_with_canary(void *ptr) {
    if (!ptr) return;
    
    // Get actual allocation start
    void *real_ptr = (char*)ptr - CANARY_SIZE;
    
    // Check canaries
    for (int i = 0; i < CANARY_SIZE; i++) {
        if (((unsigned char*)real_ptr)[i] != CANARY_VALUE || 
            ((unsigned char*)real_ptr)[CANARY_SIZE + ((char*)ptr - (char*)real_ptr) + i] != CANARY_VALUE) {
            fprintf(stderr, "BUFFER OVERFLOW/UNDERFLOW DETECTED\n");
            exit(EXIT_FAILURE);
        }
    }
    
    heap_free(real_ptr);
}
```

## 5. Final Main Function with Verification

```c
int main() {
    // Test allocations
    void *ptrs[5];
    for (int i = 0; i < 5; i++) {
        ptrs[i] = heap_alloc_with_canary(100 * (i+1));
        if (!ptrs[i]) {
            fprintf(stderr, "Allocation failed\n");
            return EXIT_FAILURE;
        }
    }

    // Free some allocations
    heap_free_with_canary(ptrs[1]);
    heap_free_with_canary(ptrs[3]);

    // Verify heap integrity
    verify_heap_integrity();

    // Allocate more
    void *p = heap_alloc(500);
    if (!p) {
        fprintf(stderr, "Allocation failed\n");
        return EXIT_FAILURE;
    }

    // Free remaining
    heap_free_with_canary(ptrs[0]);
    heap_free_with_canary(ptrs[2]);
    heap_free_with_canary(ptrs[4]);
    heap_free(p);

    // Check for leaks
    verify_no_leaks();

    printf("All tests passed successfully\n");
    return EXIT_SUCCESS;
}
```

## Key Improvements:

1. **Memory Leak Detection**: The `verify_no_leaks()` function checks if all allocated memory was freed.

2. **Heap Integrity Checks**: `verify_heap_integrity()` ensures no overlapping chunks exist.

3. **Boundary Checking**: Canary values help detect buffer overflows/underflows.

4. **Usage Tracking**: The `total_allocated` and `currently_allocated` counters track memory usage.

5. **Safe Wrappers**: `heap_alloc_with_canary` and `heap_free_with_canary` provide additional protection.

To use this in production:
1. Call `verify_no_leaks()` before program exit
2. Use the canary versions for debugging
3. Regularly call `verify_heap_integrity()` during development
4. Consider adding more sophisticated checks (like tracking allocation call sites)

This approach will help ensure memory is properly freed and detect many common memory management errors.

integrate all this stuff in the original code 


# Complete Integrated Memory Allocator with Leak Detection

Here's the full integration of all the memory safety features into your original code:

```c
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define HEAP_ALLOCED_CAP 1024
#define HEAP_FREED_CAP 1024
#define CHUNKS_LIST_CUP 1024
#define CANARY_SIZE 16
static const unsigned char CANARY_VALUE = 0xAA;

typedef struct {
    void *start;
    size_t size;
} Chunk;

typedef struct {
    size_t count;
    Chunk chunks[HEAP_ALLOCED_CAP];
} Chunk_List;

// Global memory tracking
void *heap_start = NULL;
size_t heap_size = 0;
size_t total_allocated = 0;
size_t currently_allocated = 0;

Chunk_List alloced_chunks = {0};
Chunk_List temp_chunks = {0};
Chunk_List freed_chunks = {0};

// Helper functions
int chunks_list_insert(Chunk_List *list, void *start, size_t size) {
    assert(list->count < CHUNKS_LIST_CUP);
    list->chunks[list->count].start = start;
    list->chunks[list->count].size = size;

    for (size_t i = list->count;
         i > 0 && list->chunks[i].start < list->chunks[i - 1].start;
         --i) {
        Chunk t = list->chunks[i];
        list->chunks[i] = list->chunks[i - 1];
        list->chunks[i - 1] = t;
    }

    list->count++;
    return 0;
}

void chunks_list_dump(const Chunk_List *list) {
    printf("Chunk List Dump (%zu entries):\n", list->count);
    for (size_t i = 0; i < list->count; i++) {
        printf("  %p: %zu bytes\n", list->chunks[i].start, list->chunks[i].size);
    }
}

int chunks_list_find(Chunk_List *list, void *ptr) {
    for(size_t i = 0; i < list->count; ++i) {
        if(list->chunks[i].start == ptr) {
            return (int)i;
        }
    }
    return -1;
}

void chunks_list_remove(Chunk_List *list, size_t index) {
    assert(index < list->count);
    for (size_t i = index; i < list->count - 1; i++) {
        list->chunks[i] = list->chunks[i + 1];
    }
    list->count--;
}

void chunk_list_merge(Chunk_List *dst, const Chunk_List *src) {
    dst->count = 0;
    for (size_t i = 0; i < src->count; ++i) {
        const Chunk chunk = src->chunks[i];

        if (dst->count > 0) {
            Chunk *top_chunk = &dst->chunks[dst->count - 1];

            if ((char*)top_chunk->start + top_chunk->size == chunk.start) {
                top_chunk->size += chunk.size;
            } else {
                chunks_list_insert(dst, chunk.start, chunk.size);
            }
        } else {
            chunks_list_insert(dst, chunk.start, chunk.size);
        }
    }
}

// Core allocator functions
void *heap_alloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    // Initialize heap if not already done
    if (heap_start == NULL) {
        heap_start = sbrk(0);
        if (heap_start == (void*)-1) {
            return NULL;
        }
    }

    // Try to find a suitable freed chunk first
    chunk_list_merge(&temp_chunks, &freed_chunks);
    freed_chunks = temp_chunks;
    
    for (size_t i = 0; i < freed_chunks.count; ++i) {
        const Chunk chunk = freed_chunks.chunks[i];

        if (chunk.size >= size) {
            chunks_list_remove(&freed_chunks, i);

            const size_t tail_size = chunk.size - size;
            chunks_list_insert(&alloced_chunks, chunk.start, size);

            if (tail_size > 0) {
                chunks_list_insert(&freed_chunks, (char*)chunk.start + size, tail_size);
            }

            total_allocated += size;
            currently_allocated += size;
            return chunk.start;
        }
    }

    // If no suitable freed chunk, request more memory from the system
    void *current_brk = sbrk(0);
    if (current_brk == (void*)-1) {
        return NULL;
    }

    void *new_mem = sbrk(size);
    if (new_mem == (void*)-1) {
        return NULL;
    }

    chunks_list_insert(&alloced_chunks, new_mem, size);
    heap_size += size;
    total_allocated += size;
    currently_allocated += size;

    return new_mem;
}

void heap_free(void *ptr) {
    if (ptr != NULL) {
        const int index = chunks_list_find(&alloced_chunks, ptr);
        if (index >= 0) {
            currently_allocated -= alloced_chunks.chunks[index].size;
            chunks_list_insert(&freed_chunks,
                             alloced_chunks.chunks[index].start,
                             alloced_chunks.chunks[index].size);
            chunks_list_remove(&alloced_chunks, (size_t)index);
        }
    }
}

// Safety functions
void verify_no_leaks() {
    if (currently_allocated != 0) {
        fprintf(stderr, "\nMEMORY LEAK DETECTED: %zu bytes still allocated\n", 
                currently_allocated);
        fprintf(stderr, "Allocated chunks:\n");
        chunks_list_dump(&alloced_chunks);
        exit(EXIT_FAILURE);
    }
    printf("No memory leaks detected\n");
}

void verify_heap_integrity() {
    // Check for overlapping allocated chunks
    for (size_t i = 1; i < alloced_chunks.count; i++) {
        void *prev_end = (char*)alloced_chunks.chunks[i-1].start + 
                        alloced_chunks.chunks[i-1].size;
        if (alloced_chunks.chunks[i].start < prev_end) {
            fprintf(stderr, "HEAP CORRUPTION: Overlapping allocated chunks\n");
            chunks_list_dump(&alloced_chunks);
            exit(EXIT_FAILURE);
        }
    }
    
    // Check for overlapping freed chunks
    for (size_t i = 1; i < freed_chunks.count; i++) {
        void *prev_end = (char*)freed_chunks.chunks[i-1].start + 
                        freed_chunks.chunks[i-1].size;
        if (freed_chunks.chunks[i].start < prev_end) {
            fprintf(stderr, "HEAP CORRUPTION: Overlapping freed chunks\n");
            chunks_list_dump(&freed_chunks);
            exit(EXIT_FAILURE);
        }
    }
    printf("Heap integrity verified\n");
}

// Canary-protected allocation
void *heap_alloc_protected(size_t size) {
    // Allocate extra space for canaries
    void *ptr = heap_alloc(size + 2*CANARY_SIZE);
    if (!ptr) return NULL;
    
    // Set canaries at start and end
    memset(ptr, CANARY_VALUE, CANARY_SIZE);
    memset((char*)ptr + CANARY_SIZE + size, CANARY_VALUE, CANARY_SIZE);
    
    // Return pointer after start canary
    return (char*)ptr + CANARY_SIZE;
}

void heap_free_protected(void *ptr) {
    if (!ptr) return;
    
    // Get actual allocation start
    void *real_ptr = (char*)ptr - CANARY_SIZE;
    
    // Check canaries
    for (int i = 0; i < CANARY_SIZE; i++) {
        if (((unsigned char*)real_ptr)[i] != CANARY_VALUE) {
            fprintf(stderr, "BUFFER UNDERFLOW DETECTED\n");
            exit(EXIT_FAILURE);
        }
        size_t user_size = ((char*)ptr - (char*)real_ptr - CANARY_SIZE);
        if (((unsigned char*)real_ptr)[CANARY_SIZE + user_size + i] != CANARY_VALUE) {
            fprintf(stderr, "BUFFER OVERFLOW DETECTED\n");
            exit(EXIT_FAILURE);
        }
    }
    
    heap_free(real_ptr);
}

// Memory usage reporting
void report_memory_usage() {
    printf("\nMemory Usage Report:\n");
    printf("Total heap size: %zu bytes\n", heap_size);
    printf("Total allocated: %zu bytes\n", total_allocated);
    printf("Currently allocated: %zu bytes\n", currently_allocated);
    printf("Allocated chunks: %zu\n", alloced_chunks.count);
    printf("Free chunks: %zu\n", freed_chunks.count);
}

// Test function
void run_tests() {
    printf("Running memory allocator tests...\n");
    
    // Test 1: Basic allocation and free
    void *p1 = heap_alloc_protected(100);
    void *p2 = heap_alloc_protected(200);
    heap_free_protected(p1);
    heap_free_protected(p2);
    
    // Test 2: Verify no leaks
    verify_no_leaks();
    
    // Test 3: Verify heap integrity
    verify_heap_integrity();
    
    // Test 4: Test canary protection
    p1 = heap_alloc_protected(50);
    // Simulate buffer overflow (comment out for normal operation)
    // memset(p1, 0, 60); // This should trigger overflow detection
    heap_free_protected(p1);
    
    printf("All tests passed successfully\n");
}

int main() {
    run_tests();
    
    // Example usage
    void *ptrs[5];
    for (int i = 0; i < 5; i++) {
        ptrs[i] = heap_alloc_protected(100 * (i+1));
        printf("Allocated %d bytes at %p\n", 100*(i+1), ptrs[i]);
    }
    
    // Free some allocations
    heap_free_protected(ptrs[1]);
    heap_free_protected(ptrs[3]);
    
    // Allocate more
    void *p = heap_alloc(500);
    printf("Allocated 500 bytes at %p\n", p);
    
    // Free remaining
    heap_free_protected(ptrs[0]);
    heap_free_protected(ptrs[2]);
    heap_free_protected(ptrs[4]);
    heap_free(p);
    
    // Final checks
    verify_heap_integrity();
    verify_no_leaks();
    report_memory_usage();
    
    return 0;
}
```

## Key Features of the Integrated Solution:

1. **Memory Leak Detection**: The `verify_no_leaks()` function checks if all memory was freed.

2. **Heap Integrity Verification**: `verify_heap_integrity()` ensures no memory corruption.

3. **Canary Protection**: `heap_alloc_protected()` and `heap_free_protected()` add buffer overflow/underflow detection.

4. **Comprehensive Tracking**:
   - Tracks total allocated memory
   - Currently allocated memory
   - All allocated and freed chunks

5. **Diagnostic Tools**:
   - Memory usage reporting
   - Chunk list dumping
   - Automated test suite

6. **Safety Checks**:
   - Overlap detection between chunks
   - Boundary checking with canaries
   - Proper initialization checks

## Usage Recommendations:

1. For production code:
   - Use `heap_alloc()` and `heap_free()` for normal operations
   - Call `verify_no_leaks()` before program exit

2. For debugging:
   - Use `heap_alloc_protected()` and `heap_free_protected()`
   - Call `verify_heap_integrity()` periodically
   - Enable memory usage reporting

3. For testing:
   - Use the built-in test suite
   - Add your own test cases to `run_tests()`

This integrated solution provides robust memory management with comprehensive safety checks while maintaining the original functionality of your allocator.