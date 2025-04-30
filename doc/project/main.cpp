// #include "my_alloc.h"
// #include <iostream>

// #define N 10

// int main() {
//     void* current_brk = sbrk(0);
//     std::cout << "Starting Pointer of the heap is " << current_brk << std::endl;

//     void* ptrs[N] = {0};

//     for (int i = 0; i < N; i++) {
//         ptrs[i] = heap_alloc(i);
//     }

//     for (int i = 0; i < N; i++) {
//         heap_free(ptrs[i]);
//     }

//     void* p = heap_alloc(10);
//     if (p == NULL) {
//         printf("Failed to allocate memory\n");
//     }

//     heap_dump_stats();
//     return 0;
// }


// #include "my_alloc.h"
// #include <iostream>
// #include <cstring> // for memset

// #define N 10

// // Simple vector implementation using custom allocator
// template<typename T>
// class Vector {
// private:
//     T* data;
//     size_t capacity;
//     size_t length;
    
// public:
//     Vector() : data(nullptr), capacity(0), length(0) {}
    
//     ~Vector() {
//         if (data) {
//             for (size_t i = 0; i < length; i++) {
//                 data[i].~T();
//             }
//             heap_free(data);
//         }
//     }
    
//     void push_back(const T& value) {
//         if (length >= capacity) {
//             size_t new_capacity = capacity == 0 ? 1 : capacity * 2;
//             T* new_data = (T*)heap_alloc(new_capacity * sizeof(T));
            
//             for (size_t i = 0; i < length; i++) {
//                 new (&new_data[i]) T(data[i]);
//                 data[i].~T();
//             }
            
//             if (data) heap_free(data);
//             data = new_data;
//             capacity = new_capacity;
//         }
        
//         new (&data[length]) T(value);
//         length++;
//     }
    
//     size_t size() const { return length; }
//     T& operator[](size_t index) { return data[index]; }
// };

// // Simple hash table implementation
// class HashTable {
// private:
//     struct Entry {
//         const char* key;
//         int value;
//         bool occupied;
        
//         Entry() : key(nullptr), value(0), occupied(false) {}
//     };
    
//     Entry* entries;
//     size_t capacity;
//     size_t count;
    
//     size_t hash(const char* key) const {
//         size_t hash = 5381;
//         int c;
//         while ((c = *key++)) {
//             hash = ((hash << 5) + hash) + c;
//         }
//         return hash % capacity;
//     }
    
// public:
//     HashTable(size_t initial_capacity = 16) : capacity(initial_capacity), count(0) {
//         entries = (Entry*)heap_alloc(capacity * sizeof(Entry));
//         memset(entries, 0, capacity * sizeof(Entry));
//     }
    
//     ~HashTable() {
//         heap_free(entries);
//     }
    
//     void insert(const char* key, int value) {
//         if (count * 2 >= capacity) {
//             resize(capacity * 2);
//         }
        
//         size_t index = hash(key);
//         while (entries[index].occupied) {
//             if (strcmp(entries[index].key, key) == 0) {
//                 entries[index].value = value;
//                 return;
//             }
//             index = (index + 1) % capacity;
//         }
        
//         entries[index].key = key;
//         entries[index].value = value;
//         entries[index].occupied = true;
//         count++;
//     }
    
//     int* get(const char* key) {
//         size_t index = hash(key);
//         size_t start = index;
        
//         while (entries[index].occupied) {
//             if (strcmp(entries[index].key, key) == 0) {
//                 return &entries[index].value;
//             }
//             index = (index + 1) % capacity;
//             if (index == start) break;
//         }
        
//         return nullptr;
//     }
    
// private:
//     void resize(size_t new_capacity) {
//         Entry* new_entries = (Entry*)heap_alloc(new_capacity * sizeof(Entry));
//         memset(new_entries, 0, new_capacity * sizeof(Entry));
        
//         for (size_t i = 0; i < capacity; i++) {
//             if (entries[i].occupied) {
//                 size_t index = hash(entries[i].key);
//                 while (new_entries[index].occupied) {
//                     index = (index + 1) % new_capacity;
//                 }
//                 new_entries[index] = entries[i];
//             }
//         }
        
//         heap_free(entries);
//         entries = new_entries;
//         capacity = new_capacity;
//     }
// };

// // Stack implementation
// template<typename T>
// class Stack {
// private:
//     T* data;
//     size_t capacity;
//     size_t top;
    
// public:
//     Stack(size_t initial_capacity = 16) : capacity(initial_capacity), top(0) {
//         data = (T*)heap_alloc(capacity * sizeof(T));
//     }
    
//     ~Stack() {
//         heap_free(data);
//     }
    
//     void push(const T& value) {
//         if (top >= capacity) {
//             capacity *= 2;
//             T* new_data = (T*)heap_alloc(capacity * sizeof(T));
//             memcpy(new_data, data, top * sizeof(T));
//             heap_free(data);
//             data = new_data;
//         }
//         data[top++] = value;
//     }
    
//     T pop() {
//         if (top == 0) throw std::out_of_range("Stack is empty");
//         return data[--top];
//     }
    
//     bool empty() const { return top == 0; }
// };

// // Queue implementation
// template<typename T>
// class Queue {
// private:
//     T* data;
//     size_t capacity;
//     size_t head;
//     size_t tail;
//     size_t count;
    
// public:
//     Queue(size_t initial_capacity = 16) : capacity(initial_capacity), head(0), tail(0), count(0) {
//         data = (T*)heap_alloc(capacity * sizeof(T));
//     }
    
//     ~Queue() {
//         heap_free(data);
//     }
    
//     void enqueue(const T& value) {
//         if (count >= capacity) {
//             size_t new_capacity = capacity * 2;
//             T* new_data = (T*)heap_alloc(new_capacity * sizeof(T));
            
//             for (size_t i = 0; i < count; i++) {
//                 new_data[i] = data[(head + i) % capacity];
//             }
            
//             heap_free(data);
//             data = new_data;
//             head = 0;
//             tail = count;
//             capacity = new_capacity;
//         }
        
//         data[tail] = value;
//         tail = (tail + 1) % capacity;
//         count++;
//     }
    
//     T dequeue() {
//         if (count == 0) throw std::out_of_range("Queue is empty");
//         T value = data[head];
//         head = (head + 1) % capacity;
//         count--;
//         return value;
//     }
    
//     bool empty() const { return count == 0; }
// };



// int main() {
//     void* current_brk = sbrk(0);
//     std::cout << "Starting Pointer of the heap is " << current_brk << std::endl;

//     // Original test
//     void* ptrs[N] = {0};
//     for (int i = 0; i < N; i++) {
//         ptrs[i] = heap_alloc(i);
//     }
//     for (int i = 0; i < N; i++) {
//         heap_free(ptrs[i]);
//     }
//     void* p = heap_alloc(10);
//     if (p == NULL) {
//         printf("Failed to allocate memory\n");
//     }

//     // Vector example
//     std::cout << "\nVector Example:\n";
//     Vector<int> vec;
//     for (int i = 0; i < 20; i++) {
//         vec.push_back(i);
//     }
//     std::cout << "Vector size: " << vec.size() << std::endl;
//     std::cout << "Vector[5]: " << vec[5] << std::endl;

//     // Hash table example
//     std::cout << "\nHash Table Example:\n";
//     HashTable ht;
//     ht.insert("apple", 5);
//     ht.insert("banana", 7);
//     ht.insert("orange", 9);
    
//     int* val = ht.get("banana");
//     if (val) {
//         std::cout << "banana: " << *val << std::endl;
//     }

//     // Stack example
//     std::cout << "\nStack Example:\n";
//     Stack<int> stack;
//     for (int i = 0; i < 10; i++) {
//         stack.push(i);
//     }
//     while (!stack.empty()) {
//         std::cout << stack.pop() << " ";
//     }
//     std::cout << std::endl;

//     // Queue example
//     std::cout << "\nQueue Example:\n";
//     Queue<int> queue;
//     for (int i = 0; i < 10; i++) {
//         queue.enqueue(i);
//     }
//     while (!queue.empty()) {
//         std::cout << queue.dequeue() << " ";
//     }
//     std::cout << std::endl;

//     heap_dump_stats();
//     return 0;
// }



#include "my_vector.h"
#include "my_hash.h"
#include "my_stack.h"
#include "my_queue.h"
#include <iostream>

int main() {
    // Vector demo
    Vector<int> vec;
    for (int i = 0; i < 10; i++) {
        vec.push_back(i);
    }
    std::cout << "Vector contents: ";
    for (size_t i = 0; i < vec.size(); i++) {
        std::cout << vec[i] << " ";
    }
    std::cout << "\n\n";

    // Hash table demo
    HashTable ht;
    ht.insert("apple", 5);
    ht.insert("banana", 7);
    ht.insert("orange", 9);
    
    std::cout << "Hash table contents:\n";
    int* val = ht.get("apple");
    if (val) std::cout << "apple: " << *val << "\n";
    val = ht.get("banana");
    if (val) std::cout << "banana: " << *val << "\n";
    val = ht.get("orange");
    if (val) std::cout << "orange: " << *val << "\n\n";

    // Stack demo
    Stack<int> stack;
    for (int i = 0; i < 5; i++) {
        stack.push(i);
    }
    std::cout << "Stack contents (LIFO order): ";
    while (!stack.empty()) {
        std::cout << stack.pop() << " ";
    }
    std::cout << "\n\n";

    // Queue demo
    Queue<int> queue;
    for (int i = 0; i < 5; i++) {
        queue.enqueue(i);
    }
    std::cout << "Queue contents (FIFO order): ";
    while (!queue.empty()) {
        std::cout << queue.dequeue() << " ";
    }
    std::cout << "\n";

    my_alloc::heap_dump_stats();
    return 0;
}

// #include "my_alloc.hpp"
// #include <iostream>
// #include <unistd.h> // for sbrk

// #define N 10

// int main() {
//     void* current_brk = sbrk(0);
//     std::cout << "Starting Pointer of the heap is " << current_brk << std::endl;

//     void* ptrs[N] = {0};

//     for (int i = 0; i < N; i++) {
//         ptrs[i] = my_alloc::heap_alloc(i);
//     }

//     for (int i = 0; i < N; i++) {
//         my_alloc::heap_free(ptrs[i]);
//     }

//     void* p = my_alloc::heap_alloc(10);
//     if (p == NULL) {
//         std::cout << "Failed to allocate memory\n";
//     }

//     my_alloc::heap_dump_stats();
//     return 0;
// }
