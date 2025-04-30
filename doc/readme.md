## 🚀 Custom Heap Memory Allocator in C++

This project demonstrates a basic **heap memory manager** implemented in C/C++. The allocator manages memory from a statically defined `heap[]` array and uses custom logic to **allocate**, **free**, and **merge memory chunks**.

---

## 🔧 Components Overview

### 1. **Chunk & Chunk_List**
- `Chunk` stores a pointer (`start`) and the size of a block.
- `Chunk_List` is an array of `Chunk` with metadata to track count.

### 2. **Heap Buffers**
- `heap[]`: The static memory pool (640KB)
- `alloced_chunks`: Tracks currently allocated memory blocks.
- `freed_chunks`: Tracks freed memory blocks.
- `temp_chunks`: A temporary list used to merge and defragment memory.

---

## 📦 Core Functions

### `heap_alloc(size_t size)`
- Finds a suitable freed memory chunk that fits the requested size.
- Splits large chunks into used + remaining free space.
- Merges free chunks before allocation to minimize fragmentation.

### `heep_free(void *ptr)`
- Frees memory by moving it from `alloced_chunks` to `freed_chunks`.
- Does **not** immediately merge — it's done during the next allocation.

### `chunk_list_merge(dst, src)`
- Combines adjacent memory blocks in a sorted list to reduce fragmentation.

### `chunks_list_insert` & `chunks_list_remove`
- Insert or remove a `Chunk` from a `Chunk_List`, maintaining sorted order.

---

## 🧪 Example Usage

```cpp
for (int i = 0; i < N; i++) {
    ptrs[i] = heap_alloc(i);
}
for (int i = 0; i < N; i++) {
    if (i % 2 == 0) {
        heep_free(ptrs[i]);
    }
}
heap_alloc(10);
```

---

## 📈 Output

Dumps currently allocated and freed chunks. Example:
```
Count 5
Start 0x12345 Size 3
...
```

---

## 📹 Reference

This project was inspired by the YouTube tutorial:  
[🔗 https://youtu.be/sZ8GJ1TiMdk?si=gCTcsVBstArhKSR4](https://youtu.be/sZ8GJ1TiMdk?si=gCTcsVBstArhKSR4)

---

## ✅ Summary

This allocator is a great introduction to low-level memory management, including:
- Manual allocation and deallocation
- Avoiding memory leaks
- Fragmentation handling

---

# 📄 Code 

Below is your code with added inline documentation for understanding logic, control flow, and behavior:

```cpp
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

// Constants defining heap limits
#define HEAP_CUP 640000               // Total heap capacity (640KB)
#define HEAP_ALLOCED_CAP 1024        // Max number of allocated chunks
#define HEAP_FREED_CAP 1024          // Max number of freed chunks
#define CHUNKS_LIST_CUP 1024         // Max number of total chunks in a list

// Chunk represents a memory block with a start pointer and size
typedef struct {
    char *start;
    size_t size;
} Chunk;

// List of Chunks with a count for how many blocks are currently in use
typedef struct {
    size_t count;
    Chunk chunks[HEAP_ALLOCED_CAP];
} Chunk_List;

// Static memory pool
char heap[HEAP_CUP] = {0};
size_t heap_size = 0;

// Lists to track allocated and freed memory blocks
Chunk_List alloced_chunks = {0}; // Currently allocated chunks
Chunk_List temp_chunks = {0};    // Temporary list used for merging
Chunk_List freed_chunks = {
    .count = 1,
    .chunks = {[0] = {.start = heap, .size = sizeof(heap)}}, // Initially, whole heap is free
};

// Inserts a chunk into a Chunk_List in sorted order
int chunks_list_insert(Chunk_List *list, void *start, size_t size)
{
    assert(list->count < CHUNKS_LIST_CUP);

    list->chunks[list->count].start = (char *)start;
    list->chunks[list->count].size = size;

    // Maintain sorted order by start address
    for (size_t i = list->count;
         i > 0 && list->chunks[i].start < list->chunks[i - 1].start;
         --i)
    {
        Chunk t = list->chunks[i];
        list->chunks[i] = list->chunks[i - 1];
        list->chunks[i - 1] = t;
    }

    list->count++;
    return 0;
}

// Prints all chunks in a given list
void chunks_list_dump(const Chunk_List *list)
{
    std::cout << "Count " << list->count << std::endl;
    for (size_t i = 0; i < list->count; i++)
    {
        std::cout << "Start " << (void *)list->chunks[i].start
                  << " Size " << list->chunks[i].size << std::endl;
    }
}

// Compares two chunks by their start address
int chunk_start_compar(const void *a, const void *b)
{
    const Chunk *a_chunk = (const Chunk *)a;
    const Chunk *b_chunk = (const Chunk *)b;
    return (a_chunk->start > b_chunk->start) - (a_chunk->start < b_chunk->start);
}

// Finds index of chunk starting at a specific pointer
int chunks_list_find(Chunk_List *list, void *ptr)
{
    for(size_t i = 0; i < list->count; ++i){
        if(list->chunks[i].start == ptr){
            return (int) i;
        }
    }
    return -1;
}

// Removes a chunk at a specific index from the list
void chunks_list_remove(Chunk_List *list, size_t index)
{
    assert(index < list->count);
    for (size_t i = index; i < list->count - 1; i++)
    {
        list->chunks[i] = list->chunks[i + 1];
    }
    list->count--;
}

// Frees a previously allocated block by moving it to freed_chunks
void heep_free(void *ptr)
{
    if (ptr != nullptr)
    {
        const int index = chunks_list_find(&alloced_chunks, ptr);
        assert(index >= 0);

        // Move from alloced -> freed
        chunks_list_insert(&freed_chunks,
                           alloced_chunks.chunks[index].start,
                           alloced_chunks.chunks[index].size);
        chunks_list_remove(&alloced_chunks, (size_t)index);
    }
}

// Merges adjacent free chunks to minimize fragmentation
void chunk_list_merge(Chunk_List *dst, const Chunk_List *src)
{
    dst->count = 0;

    for (size_t i = 0; i < src->count; ++i)
    {
        const Chunk chunk = src->chunks[i];

        if (dst->count > 0)
        {
            Chunk *top_chunk = &dst->chunks[dst->count - 1];

            // Merge if adjacent in memory
            if (top_chunk->start + top_chunk->size == chunk.start)
            {
                top_chunk->size += chunk.size;
            }
            else
            {
                chunks_list_insert(dst, chunk.start, chunk.size);
            }
        }
        else
        {
            chunks_list_insert(dst, chunk.start, chunk.size);
        }
    }
}

// Allocates a memory block from the heap
void *heap_alloc(size_t size)
{
    if (size < 1)
    {
        return nullptr;
    }

    // Merge freed chunks before allocation
    chunk_list_merge(&temp_chunks, &freed_chunks);
    freed_chunks = temp_chunks;

    for (size_t i = 0; i < freed_chunks.count; ++i)
    {
        const Chunk chunk = freed_chunks.chunks[i];

        if (chunk.size >= size)
        {
            chunks_list_remove(&freed_chunks, i);
            const size_t tail_size = chunk.size - size;

            chunks_list_insert(&alloced_chunks, chunk.start, size);

            // If there's leftover, return to free list
            if (tail_size > 0)
            {
                chunks_list_insert(&freed_chunks, chunk.start + size, tail_size);
            }

            return chunk.start;
        }
    }

    return nullptr;
}

// Test program
#define N 10
void *ptrs[N] = {0};

int main()
{
    // Allocate N blocks
    for(int i= 0 ; i<N ; i++){
        ptrs[i] = heap_alloc(i);
    }

    // Free even-indexed blocks
    for(int i= 0 ; i<N ; i++){


        if(i%2==0){
            heep_free(ptrs[i]);
        }
    }

    // Additional allocation to test merge
    heap_alloc(10);

    // Dump final state
    chunks_list_dump(&alloced_chunks);
    chunks_list_dump(&freed_chunks);
    return 0;
}
```

---

