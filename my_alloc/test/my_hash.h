#ifndef MY_HASH_HPP
#define MY_HASH_HPP

#include "my_alloc.hpp"
#include <cstring>
#include <stdexcept>

class HashTable {
private:
    struct Entry {
        const char* key;
        int value;
        bool occupied;
        
        Entry() : key(nullptr), value(0), occupied(false) {}
    };
    
    Entry* entries;
    size_t capacity;
    size_t count;
    
    size_t hash(const char* key) const {
        size_t hash = 5381;
        int c;
        while ((c = *key++)) {
            hash = ((hash << 5) + hash) + c;
        }
        return hash % capacity;
    }
    
    void resize(size_t new_capacity) {
        Entry* new_entries = (Entry*)my_alloc::heap_alloc(new_capacity * sizeof(Entry));
        memset(new_entries, 0, new_capacity * sizeof(Entry));
        
        for (size_t i = 0; i < capacity; i++) {
            if (entries[i].occupied) {
                size_t index = hash(entries[i].key);
                while (new_entries[index].occupied) {
                    index = (index + 1) % new_capacity;
                }
                new_entries[index] = entries[i];
            }
        }
        
        my_alloc::heap_free(entries);
        entries = new_entries;
        capacity = new_capacity;
    }
    
public:
    HashTable(size_t initial_capacity = 16) : capacity(initial_capacity), count(0) {
        entries = (Entry*)my_alloc::heap_alloc(capacity * sizeof(Entry));
        memset(entries, 0, capacity * sizeof(Entry));
    }
    
    ~HashTable() {
        my_alloc::heap_free(entries);
    }
    
    void insert(const char* key, int value) {
        if (count * 2 >= capacity) {
            resize(capacity * 2);
        }
        
        size_t index = hash(key);
        while (entries[index].occupied) {
            if (strcmp(entries[index].key, key) == 0) {
                entries[index].value = value;
                return;
            }
            index = (index + 1) % capacity;
        }
        
        entries[index].key = key;
        entries[index].value = value;
        entries[index].occupied = true;
        count++;
    }
    
    int* get(const char* key) {
        size_t index = hash(key);
        size_t start = index;
        
        while (entries[index].occupied) {
            if (strcmp(entries[index].key, key) == 0) {
                return &entries[index].value;
            }
            index = (index + 1) % capacity;
            if (index == start) break;
        }
        
        return nullptr;
    }
    
    size_t size() const { return count; }
    bool empty() const { return count == 0; }
};

#endif