#ifndef MY_QUEUE_HPP
#define MY_QUEUE_HPP

#include "my_alloc.hpp"
#include <cstring>
#include <stdexcept>

template<typename T>
class Queue {
private:
    T* data;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t count;
    
    void resize(size_t new_capacity) {
        T* new_data = (T*)my_alloc::heap_alloc(new_capacity * sizeof(T));
        
        for (size_t i = 0; i < count; i++) {
            new_data[i] = data[(head + i) % capacity];
        }
        
        my_alloc::heap_free(data);
        data = new_data;
        head = 0;
        tail = count;
        capacity = new_capacity;
    }
    
public:
    Queue(size_t initial_capacity = 16) : capacity(initial_capacity), head(0), tail(0), count(0) {
        data = (T*)my_alloc::heap_alloc(capacity * sizeof(T));
    }
    
    ~Queue() {
        my_alloc::heap_free(data);
    }
    
    void enqueue(const T& value) {
        if (count >= capacity) {
            resize(capacity * 2);
        }
        
        data[tail] = value;
        tail = (tail + 1) % capacity;
        count++;
    }
    
    T dequeue() {
        if (count == 0) throw std::out_of_range("Queue is empty");
        T value = data[head];
        head = (head + 1) % capacity;
        count--;
        return value;
    }
    
    T& front() {
        if (count == 0) throw std::out_of_range("Queue is empty");
        return data[head];
    }
    
    size_t size() const { return count; }
    bool empty() const { return count == 0; }
};

#endif