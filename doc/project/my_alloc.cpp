// #include "my_alloc.h"
// #include <stdbool.h>
// #include <assert.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>

// #define HEAP_ALLOCED_CAP 5611520

// typedef struct {
//     void* start;
//     size_t size;
// } Chunk;

// typedef struct {
//     size_t count;
//     Chunk chunks[HEAP_ALLOCED_CAP];
// } Chunk_List;

// static void* heap_start = NULL;
// static size_t heap_size = 0;
// static size_t total_allocated = 0;
// static size_t currently_allocated = 0;

// static Chunk_List alloced_chunks = {0};
// static Chunk_List temp_chunks = {0};
// static Chunk_List freed_chunks = {0};

// static int chunks_list_insert(Chunk_List* list, void* start, size_t size) {
//     assert(list->count < HEAP_ALLOCED_CAP);
//     list->chunks[list->count].start = start;
//     list->chunks[list->count].size = size;

//     for (size_t i = list->count;
//          i > 0 && list->chunks[i].start < list->chunks[i - 1].start;
//          --i) {
//         Chunk t = list->chunks[i];
//         list->chunks[i] = list->chunks[i - 1];
//         list->chunks[i - 1] = t;
//     }

//     list->count++;
//     return 0;
// }

// static void chunks_list_dump(const Chunk_List* list) {
//     printf("Count %zu\n", list->count);
//     for (size_t i = 0; i < list->count; i++) {
//         printf("Start %p Size %zu\n", list->chunks[i].start, list->chunks[i].size);
//     }
// }

// static int chunks_list_find(Chunk_List* list, void* ptr) {
//     for (size_t i = 0; i < list->count; ++i) {
//         if (list->chunks[i].start == ptr) {
//             return (int)i;
//         }
//     }
//     return -1;
// }

// static void chunks_list_remove(Chunk_List* list, size_t index) {
//     assert(index < list->count);
//     for (size_t i = index; i < list->count - 1; i++) {
//         list->chunks[i] = list->chunks[i + 1];
//     }
//     list->count--;
// }

// static void chunk_list_merge(Chunk_List* dst, const Chunk_List* src) {
//     dst->count = 0;
//     for (size_t i = 0; i < src->count; ++i) {
//         const Chunk chunk = src->chunks[i];

//         if (dst->count > 0) {
//             Chunk* top_chunk = &dst->chunks[dst->count - 1];

//             if ((char*)top_chunk->start + top_chunk->size == chunk.start) {
//                 top_chunk->size += chunk.size;
//             }
//             else {
//                 chunks_list_insert(dst, chunk.start, chunk.size);
//             }
//         }
//         else {
//             chunks_list_insert(dst, chunk.start, chunk.size);
//         }
//     }
// }

// void* heap_alloc(size_t size) {
//     if (size < 1) {
//         return NULL;
//     }

//     if (heap_start == NULL) {
//         heap_start = sbrk(0);
//         if (heap_start == (void*)-1) {
//             return NULL;
//         }
//     }

//     chunk_list_merge(&temp_chunks, &freed_chunks);
//     freed_chunks = temp_chunks;

//     for (size_t i = 0; i < freed_chunks.count; ++i) {
//         const Chunk chunk = freed_chunks.chunks[i];

//         if (chunk.size >= size) {
//             chunks_list_remove(&freed_chunks, i);

//             const size_t tail_size = chunk.size - size;
//             chunks_list_insert(&alloced_chunks, chunk.start, size);
//             total_allocated += size;
//             currently_allocated += size;

//             if (tail_size > 0) {
//                 chunks_list_insert(&freed_chunks, (char*)chunk.start + size, tail_size);
//             }

//             return chunk.start;
//         }
//     }

//     void* current_brk = sbrk(0);
//     if (current_brk == (void*)-1) {
//         return NULL;
//     }

//     void* new_mem = sbrk(size);
//     if (new_mem == (void*)-1) {
//         return NULL;
//     }

//     chunks_list_insert(&alloced_chunks, new_mem, size);
//     heap_size += size;
//     total_allocated += size;
//     currently_allocated += size;

//     return new_mem;
// }

// void heap_free(void* ptr) {
//     if (ptr != NULL) {
//         const int index = chunks_list_find(&alloced_chunks, ptr);
//         if (index >= 0) {
//             currently_allocated -= alloced_chunks.chunks[index].size;
//             chunks_list_insert(&freed_chunks,
//                 alloced_chunks.chunks[index].start,
//                 alloced_chunks.chunks[index].size);
//             chunks_list_remove(&alloced_chunks, (size_t)index);
//         }
//     }
// }

// void heap_dump_stats() {
//     printf("\nHeap Statistics:\n");
//     printf("Total heap size: %zu bytes\n", heap_size);
//     printf("Total allocated: %zu bytes\n", total_allocated);
//     printf("Currently allocated: %zu bytes\n", currently_allocated);
//     printf("Allocated chunks: %zu\n", alloced_chunks.count);
//     printf("Free chunks: %zu\n", freed_chunks.count);
// }


// #ifndef MY_ALLOC_H
// #define MY_ALLOC_H

// #include <stdbool.h>
// #include <assert.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <iostream>

// #define HEAP_ALLOCED_CAP 5611520

// class my_alloc {
// private:
//     struct Chunk {
//         void* start;
//         size_t size;
//     };

//     struct Chunk_List {
//         size_t count;
//         Chunk chunks[HEAP_ALLOCED_CAP];
//     };

//     static void* heap_start;
//     static size_t heap_size;
//     static size_t total_allocated;
//     static size_t currently_allocated;

//     static Chunk_List alloced_chunks;
//     static Chunk_List temp_chunks;
//     static Chunk_List freed_chunks;

//     static int chunks_list_insert(Chunk_List* list, void* start, size_t size) {
//         assert(list->count < HEAP_ALLOCED_CAP);
//         list->chunks[list->count].start = start;
//         list->chunks[list->count].size = size;

//         for (size_t i = list->count;
//              i > 0 && list->chunks[i].start < list->chunks[i - 1].start;
//              --i) {
//             Chunk t = list->chunks[i];
//             list->chunks[i] = list->chunks[i - 1];
//             list->chunks[i - 1] = t;
//         }

//         list->count++;
//         return 0;
//     }

//     static void chunks_list_dump(const Chunk_List* list) {
//         printf("Count %zu\n", list->count);
//         for (size_t i = 0; i < list->count; i++) {
//             printf("Start %p Size %zu\n", list->chunks[i].start, list->chunks[i].size);
//         }
//     }

//     static int chunks_list_find(Chunk_List* list, void* ptr) {
//         for (size_t i = 0; i < list->count; ++i) {
//             if (list->chunks[i].start == ptr) {
//                 return (int)i;
//             }
//         }
//         return -1;
//     }

//     static void chunks_list_remove(Chunk_List* list, size_t index) {
//         assert(index < list->count);
//         for (size_t i = index; i < list->count - 1; i++) {
//             list->chunks[i] = list->chunks[i + 1];
//         }
//         list->count--;
//     }

//     static void chunk_list_merge(Chunk_List* dst, const Chunk_List* src) {
//         dst->count = 0;
//         for (size_t i = 0; i < src->count; ++i) {
//             const Chunk chunk = src->chunks[i];

//             if (dst->count > 0) {
//                 Chunk* top_chunk = &dst->chunks[dst->count - 1];

//                 if ((char*)top_chunk->start + top_chunk->size == chunk.start) {
//                     top_chunk->size += chunk.size;
//                 }
//                 else {
//                     chunks_list_insert(dst, chunk.start, chunk.size);
//                 }
//             }
//             else {
//                 chunks_list_insert(dst, chunk.start, chunk.size);
//             }
//         }
//     }

// public:
//     static void* heap_alloc(size_t size) {
//         if (size < 1) {
//             return NULL;
//         }

//         if (heap_start == NULL) {
//             heap_start = sbrk(0);
//             if (heap_start == (void*)-1) {
//                 return NULL;
//             }
//         }

//         chunk_list_merge(&temp_chunks, &freed_chunks);
//         freed_chunks = temp_chunks;

//         for (size_t i = 0; i < freed_chunks.count; ++i) {
//             const Chunk chunk = freed_chunks.chunks[i];

//             if (chunk.size >= size) {
//                 chunks_list_remove(&freed_chunks, i);

//                 const size_t tail_size = chunk.size - size;
//                 chunks_list_insert(&alloced_chunks, chunk.start, size);
//                 total_allocated += size;
//                 currently_allocated += size;

//                 if (tail_size > 0) {
//                     chunks_list_insert(&freed_chunks, (char*)chunk.start + size, tail_size);
//                 }

//                 return chunk.start;
//             }
//         }

//         void* current_brk = sbrk(0);
//         if (current_brk == (void*)-1) {
//             return NULL;
//         }

//         void* new_mem = sbrk(size);
//         if (new_mem == (void*)-1) {
//             return NULL;
//         }

//         chunks_list_insert(&alloced_chunks, new_mem, size);
//         heap_size += size;
//         total_allocated += size;
//         currently_allocated += size;

//         return new_mem;
//     }

//     static void heap_free(void* ptr) {
//         if (ptr != NULL) {
//             const int index = chunks_list_find(&alloced_chunks, ptr);
//             if (index >= 0) {
//                 currently_allocated -= alloced_chunks.chunks[index].size;
//                 chunks_list_insert(&freed_chunks,
//                     alloced_chunks.chunks[index].start,
//                     alloced_chunks.chunks[index].size);
//                 chunks_list_remove(&alloced_chunks, (size_t)index);
//             }
//         }
//     }

//     static void heap_dump_stats() {
//         printf("\nHeap Statistics:\n");
//         printf("Total heap size: %zu bytes\n", heap_size);
//         printf("Total allocated: %zu bytes\n", total_allocated);
//         printf("Currently allocated: %zu bytes\n", currently_allocated);
//         printf("Allocated chunks: %zu\n", alloced_chunks.count);
//         printf("Free chunks: %zu\n", freed_chunks.count);
//     }
// };

// // Initialize static members
// void* my_alloc::heap_start = NULL;
// size_t my_alloc::heap_size = 0;
// size_t my_alloc::total_allocated = 0;
// size_t my_alloc::currently_allocated = 0;

// my_alloc::Chunk_List my_alloc::alloced_chunks = {0};
// my_alloc::Chunk_List my_alloc::temp_chunks = {0};
// my_alloc::Chunk_List my_alloc::freed_chunks = {0};

// #endif



#include "my_alloc.hpp"


// Initialize static members
void* my_alloc::heap_start = nullptr;
size_t my_alloc::heap_size = 0;
size_t my_alloc::total_allocated = 0;
size_t my_alloc::currently_allocated = 0;

my_alloc::Chunk_List my_alloc::alloced_chunks = {0};
my_alloc::Chunk_List my_alloc::temp_chunks = {0};
my_alloc::Chunk_List my_alloc::freed_chunks = {0};

int my_alloc::chunks_list_insert(Chunk_List* list, void* start, size_t size) {
    assert(list->count < 5611520);
    list->chunks[list->count].start = start;
    list->chunks[list->count].size = size;

    for (size_t i = list->count; i > 0 && list->chunks[i].start < list->chunks[i - 1].start; --i) {
        Chunk t = list->chunks[i];
        list->chunks[i] = list->chunks[i - 1];
        list->chunks[i - 1] = t;
    }

    list->count++;
    return 0;
}

void my_alloc::chunks_list_dump(const Chunk_List* list) {
    printf("Count %zu\n", list->count);
    for (size_t i = 0; i < list->count; i++) {
        printf("Start %p Size %zu\n", list->chunks[i].start, list->chunks[i].size);
    }
}

int my_alloc::chunks_list_find(Chunk_List* list, void* ptr) {
    for (size_t i = 0; i < list->count; ++i) {
        if (list->chunks[i].start == ptr) {
            return (int)i;
        }
    }
    return -1;
}

void my_alloc::chunks_list_remove(Chunk_List* list, size_t index) {
    assert(index < list->count);
    for (size_t i = index; i < list->count - 1; i++) {
        list->chunks[i] = list->chunks[i + 1];
    }
    list->count--;
}

void my_alloc::chunk_list_merge(Chunk_List* dst, const Chunk_List* src) {
    dst->count = 0;
    for (size_t i = 0; i < src->count; ++i) {
        const Chunk chunk = src->chunks[i];

        if (dst->count > 0) {
            Chunk* top_chunk = &dst->chunks[dst->count - 1];

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

void* my_alloc::heap_alloc(size_t size) {
    if (size < 1) {
        return nullptr;
    }

    if (heap_start == nullptr) {
        heap_start = sbrk(0);
        if (heap_start == (void*)-1) {
            return nullptr;
        }
    }

    chunk_list_merge(&temp_chunks, &freed_chunks);
    freed_chunks = temp_chunks;

    for (size_t i = 0; i < freed_chunks.count; ++i) {
        const Chunk chunk = freed_chunks.chunks[i];

        if (chunk.size >= size) {
            chunks_list_remove(&freed_chunks, i);

            const size_t tail_size = chunk.size - size;
            chunks_list_insert(&alloced_chunks, chunk.start, size);
            total_allocated += size;
            currently_allocated += size;

            if (tail_size > 0) {
                chunks_list_insert(&freed_chunks, (char*)chunk.start + size, tail_size);
            }

            return chunk.start;
        }
    }

    void* current_brk = sbrk(0);
    if (current_brk == (void*)-1) {
        return nullptr;
    }

    void* new_mem = sbrk(size);
    if (new_mem == (void*)-1) {
        return nullptr;
    }

    chunks_list_insert(&alloced_chunks, new_mem, size);
    heap_size += size;
    total_allocated += size;
    currently_allocated += size;

    return new_mem;
}

void my_alloc::heap_free(void* ptr) {
    if (ptr != nullptr) {
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

void my_alloc::heap_dump_stats() {
    printf("\nHeap Statistics:\n");
    printf("Total heap size: %zu bytes\n", heap_size);
    printf("Total allocated: %zu bytes\n", total_allocated);
    printf("Currently allocated: %zu bytes\n", currently_allocated);
    printf("Allocated chunks: %zu\n", alloced_chunks.count);
    chunks_list_dump(&alloced_chunks);
    printf("Free chunks: %zu\n", freed_chunks.count);
    chunks_list_dump(&freed_chunks);
}
