//  https://youtu.be/sZ8GJ1TiMdk?si=gCTcsVBstArhKSR4

#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#define HEAP_CUP 640000
#define HEAP_ALLOCED_CAP 1024
#define HEAP_FREED_CAP 1024
#define CHUNKS_LIST_CUP 1024

typedef struct
{
    char *start;
    size_t size;
} Chunk;

typedef struct
{
    size_t count;
    Chunk chunks[HEAP_ALLOCED_CAP];
} Chunk_List;

char heap[HEAP_CUP] = {0};
size_t heap_size = 0;

Chunk_List alloced_chunks = {0};
Chunk_List temp_chunks = {0};

Chunk_List freed_chunks = {
    .count = 1,
    .chunks = {[0] = {.start = heap, .size = sizeof(heap)}},
};

int chunks_list_insert(Chunk_List *list, void *start, size_t size)
{
    // std::cout << "heap_alloced_size " << list->count << std::endl;
    assert(list->count < CHUNKS_LIST_CUP);
    list->chunks[list->count].start = (char *)start;
    list->chunks[list->count].size = size;

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

void chunks_list_dump(const Chunk_List *list)
{
    std::cout << "Count " << list->count << std::endl;
    for (size_t i = 0; i < list->count; i++)
    {
        std::cout << "Start " << (void *)list->chunks[i].start
                  << " Size " << list->chunks[i].size << std::endl;
    }
}

int chunk_start_compar(const void *a, const void *b)
{
    const Chunk *a_chunk = (const Chunk *)a;
    const Chunk *b_chunk = (const Chunk *)b;
    return (a_chunk->start > b_chunk->start) - (a_chunk->start < b_chunk->start);
}

int chunks_list_find(Chunk_List *list, void *ptr)
{

    for(size_t i=0; i<list->count;++i){
        if(list->chunks[i].start==ptr){
            return (int) i;
        }
    }
    return -1;    

    //////////////////
    // Chunk key = {.start = (char *)ptr};

    // void *result = bsearch(&key, list->chunks, list->count,
    //                        sizeof(list->chunks[0]), chunk_start_compar);
    // if (result == nullptr)
    //     return -1;
    // return (Chunk *)result - list->chunks;
}

void chunks_list_remove(Chunk_List *list, size_t index)
{
    assert(index < list->count);
    for (size_t i = index; i < list->count - 1; i++)
    {
        list->chunks[i] = list->chunks[i + 1];
    }
    list->count--;
}

void heep_free(void *ptr)
{
    if (ptr != nullptr)
    {
        const int index = chunks_list_find(&alloced_chunks, ptr);
        assert(index >= 0);
        // assert(ptr == alloced_chunks.chunks[index].start)
        chunks_list_insert(&freed_chunks,
                           alloced_chunks.chunks[index].start,
                           alloced_chunks.chunks[index].size);
        chunks_list_remove(&alloced_chunks, (size_t)index);
    }
}

void chunk_list_merge(Chunk_List *dst, const Chunk_List *src)
{
    dst->count = 0;
    for (size_t i = 0; i < src->count; ++i)
    {
        const Chunk chunk = src->chunks[i];

        if (dst->count > 0)
        {
            Chunk *top_chunk = &dst->chunks[dst->count - 1];

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

void *heap_alloc(size_t size)
{
    if (size < 1)
    {
        return nullptr;
    }

    for (size_t i = 0; i < freed_chunks.count; ++i)

    {
        chunk_list_merge(&temp_chunks, &freed_chunks);
        freed_chunks = temp_chunks;
        const Chunk chunk = freed_chunks.chunks[i];

        if (chunk.size >= size)
        {
            chunks_list_remove(&freed_chunks, i);

            const size_t tail_size = chunk.size - size;

            chunks_list_insert(&alloced_chunks, chunk.start, size);

            if (tail_size > 0)
            {
                chunks_list_insert(&freed_chunks, chunk.start + size, tail_size);
            }

            return chunk.start;
        }
    }

    return nullptr;

    // assert(heap_size + size <= HEAP_CUP);

    // void *ptr = heap + heap_size;
    // heap_size += size;

    // chunks_list_insert(&alloced_chunks, ptr, size);
    // return ptr;
}

#define N 10

void *ptrs[N] = {0};
int main()
{
    // for (int i = 0; i <= 10; i++) {
    //     void *p = heap_alloc(i);
    //     if (p == nullptr) {
    //         std::cout << "Fail to allocate the memory " << std::endl;
    //     } else if (i % 2 == 0) {
    //         heep_free(p);
    //     }
    // }

    // for (int i = 0; i <= 10; i++)
    // {
    //     void *p = heap_alloc(i);
    //     if (p == nullptr)
    //     {
    //         std::cout << "Fail to allocate the memory " << std::endl;
    //     }
    //     else
    //     {
    //         heep_free(p);
    //     }
    // }
    // heap_alloc(10);


    // char* root = (char *)heap_alloc(26);
    // for (int i = 0; i < 26; i++) {
    //     root[i] = i + 'a';
    // }

    // for (int i = 0; i < 26; i++) {
    //     std::cout << root[i];
    // }
    // std::cout << std::endl;




    // 3 example

    for(int i= 0 ; i<N ; i++){
        ptrs[i] = heap_alloc(i);
    }

    for(int i= 0 ; i<N ; i++){
        if(i%2==0){
      heep_free(ptrs[i]);

        }
    }

    heap_alloc(10);



    chunks_list_dump(&alloced_chunks);
    chunks_list_dump(&freed_chunks);
    return 0;
}


