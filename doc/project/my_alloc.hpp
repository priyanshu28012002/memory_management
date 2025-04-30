// #ifndef MY_ALLOC_H
// #define MY_ALLOC_H

// #include <stdbool.h>
// #include <assert.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h> 
// #include <iostream>

// void* heap_alloc(size_t size);
// void heap_free(void* ptr);
// void heap_dump_stats();

// #endif



#ifndef MY_ALLOC_HPP
#define MY_ALLOC_HPP
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstddef>

class my_alloc {
private:
    struct Chunk {
        void* start;
        size_t size;
    };

    struct Chunk_List {
        size_t count;
        Chunk chunks[5611520];
    };

    static void* heap_start;
    static size_t heap_size;
    static size_t total_allocated;
    static size_t currently_allocated;

    static Chunk_List alloced_chunks;
    static Chunk_List temp_chunks;
    static Chunk_List freed_chunks;

    static int chunks_list_insert(Chunk_List* list, void* start, size_t size);
    static void chunks_list_dump(const Chunk_List* list);
    static int chunks_list_find(Chunk_List* list, void* ptr);
    static void chunks_list_remove(Chunk_List* list, size_t index);
    static void chunk_list_merge(Chunk_List* dst, const Chunk_List* src);

public:
    static void* heap_alloc(size_t size);
    static void heap_free(void* ptr);
    static void heap_dump_stats();
};

#endif // MY_ALLOC_HPP
