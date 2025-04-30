// memory leak 
// is memory is get freed or only rewriteable
// Buffer overflow protection
// Original heap size
// use my_allocation in the problams like dp and back propogation 



#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <iostream>

#define HEAP_ALLOCED_CAP 5611520
#define N 10

typedef struct {
    void *start;
    size_t size;
} Chunk;

typedef struct {
    size_t count;
    Chunk chunks[HEAP_ALLOCED_CAP];
} Chunk_List;

void *heap_start = NULL;
size_t heap_size = 0;

Chunk_List alloced_chunks = {0};
Chunk_List temp_chunks = {0};
Chunk_List freed_chunks = {0};

int chunks_list_insert(Chunk_List *list, void *start, size_t size) {
    assert(list->count < HEAP_ALLOCED_CAP);
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
    printf("Count %zu\n", list->count);
    for (size_t i = 0; i < list->count; i++) {
        printf("Start %p Size %zu\n", list->chunks[i].start, list->chunks[i].size);
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

void heap_free(void *ptr) {
    if (ptr != NULL) {
        const int index = chunks_list_find(&alloced_chunks, ptr);
        assert(index >= 0);
        chunks_list_insert(&freed_chunks,
                          alloced_chunks.chunks[index].start,
                          alloced_chunks.chunks[index].size);
        chunks_list_remove(&alloced_chunks, (size_t)index);
    }
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

void *heap_alloc(size_t size) {
    if (size < 1) {
        return NULL;
    }

    // Initialize heap if not already done
    if (heap_start == NULL) {
        heap_start = sbrk(0);
        if (heap_start == (void*)-1) {
            return NULL;
        }
    }

    // First try to find a suitable freed chunk
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

    return new_mem;
}



void *ptrs[N] = {0};

int main() {

    void *current_brk = sbrk(0);
    std::cout<<"Starting Pointer of the heap is "<<current_brk<<std::endl;

    for(int i = 0; i < N; i++) {
        ptrs[i] = heap_alloc(i);
    }

    for(int i = 0; i < N; i++) {
            heap_free(ptrs[i]);
    }

    void *p = heap_alloc(10);
    if (p == NULL) {
        printf("Failed to allocate memory\n");
    }

   
    chunks_list_dump(&alloced_chunks);
    chunks_list_dump(&freed_chunks);
    return 0;
}