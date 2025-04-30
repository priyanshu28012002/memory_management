#ifndef MY_STACK_HPP
#define MY_STACK_HPP

#include <my_alloc/my_alloc.hpp>
#include <cstring>
#include <stdexcept>

template<typename T>
class Stack {
private:
    T* data;
    size_t capacity;
    size_t top;
    
    void resize(size_t new_capacity) {
        T* new_data = (T*)my_alloc::heap_alloc(new_capacity * sizeof(T));
        memcpy(new_data, data, top * sizeof(T));
        my_alloc::heap_free(data);
        data = new_data;
        capacity = new_capacity;
    }
    
public:
    Stack(size_t initial_capacity = 16) : capacity(initial_capacity), top(0) {
        data = (T*)my_alloc::heap_alloc(capacity * sizeof(T));
    }
    
    ~Stack() {
        my_alloc::heap_free(data);
    }
    
    void push(const T& value) {
        if (top >= capacity) {
            resize(capacity * 2);
        }
        data[top++] = value;
    }
    
    T pop() {
        if (top == 0) throw std::out_of_range("Stack is empty");
        return data[--top];
    }
    
    T& peek() {
        if (top == 0) throw std::out_of_range("Stack is empty");
        return data[top - 1];
    }
    
    size_t size() const { return top; }
    bool empty() const { return top == 0; }
};

#endif