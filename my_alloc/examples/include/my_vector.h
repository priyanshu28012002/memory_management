#ifndef MY_VECTOR_HPP
#define MY_VECTOR_HPP

#include <my_alloc/my_alloc.hpp>
#include <cstddef>
#include <new> 
#include <stdexcept>

template<typename T>
class Vector {
private:
    T* data;
    size_t capacity;
    size_t length;
    
public:
    Vector() : data(nullptr), capacity(0), length(0) {}
    
    ~Vector() {
        clear();
        my_alloc::heap_free(data);
    }
    
    void push_back(const T& value) {
        if (length >= capacity) {
            reserve(capacity == 0 ? 1 : capacity * 2);
        }
        
        new (&data[length++]) T(value);
    }
    
    void pop_back() {
        if (length == 0) throw std::out_of_range("Vector is empty");
        data[--length].~T();
    }
    
    void clear() {
        for (size_t i = 0; i < length; i++) {
            data[i].~T();
        }
        length = 0;
    }
    
    void reserve(size_t new_capacity) {
        if (new_capacity <= capacity) return;
        
        T* new_data = (T*)my_alloc::heap_alloc(new_capacity * sizeof(T));
        
        for (size_t i = 0; i < length; i++) {
            new (&new_data[i]) T(data[i]);
            data[i].~T();
        }
        
        my_alloc::heap_free(data);
        data = new_data;
        capacity = new_capacity;
    }
    
    size_t size() const { return length; }
    size_t cap() const { return capacity; }
    bool empty() const { return length == 0; }
    
    T& operator[](size_t index) { 
        if (index >= length) throw std::out_of_range("Index out of range");
        return data[index]; 
    }
    
    const T& operator[](size_t index) const { 
        if (index >= length) throw std::out_of_range("Index out of range");
        return data[index]; 
    }
};

#endif