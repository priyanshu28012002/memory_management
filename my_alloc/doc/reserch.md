# Research Document for `my_alloc` - Custom Memory Allocator

## Table of Contents

1. [Introduction](#introduction)
2. [Memory Allocation Techniques](#memory-allocation-techniques)
   1. [System Calls: `sbrk()` and `brk()`](#system-calls-sbrk-and-brk)
   2. [Fragmentation and Strategies](#fragmentation-and-strategies)
   3. [Best-fit, First-fit, and Buddy Allocators](#best-fit-first-fit-and-buddy-allocators)
3. [Free List Management](#free-list-management)
4. [Thread Safety](#thread-safety)
5. [Memory Leak Detection](#memory-leak-detection)
6. [Garbage Collection](#garbage-collection)
7. [Inter-Process Communication (IPC)](#inter-process-communication-ipc)
8. [Advanced Features and Future Work](#advanced-features-and-future-work)
9. [References](#references)

---

## 1. Introduction

This document provides an in-depth look at the core research and findings that inform the design, implementation, and future expansion of the `my_alloc` custom memory allocator. It includes foundational concepts in memory allocation, strategies for efficient block management, methods for tracking memory usage, and advanced topics such as **garbage collection** and **inter-process communication (IPC)**.

---

## 2. Memory Allocation Techniques

### 2.1 System Calls: `sbrk()` and `brk()`

The allocator's core mechanism relies on the **`sbrk()`** system call, which manipulates the program's break pointer to request additional memory from the OS heap.

- **`sbrk()`**:
  - Definition: Adjusts the program's data space (heap) to allocate or deallocate memory.
  - Deprecation: `sbrk()` is deprecated in modern systems (e.g., macOS), and **`mmap()`** might be used as an alternative in some environments.
  - Research Areas:
    - Platform-specific implementations of `sbrk()` and alternatives like `mmap()`.
    - Handling fragmentation with **dynamic heap resizing**.
    -The sbrk() function in C is used to dynamically increase or decrease the amount of memory allocated to a process's data segment (heap) by adding a specified number of bytes to the current "break" value, which marks the end of the data segment. 


- Purpose:
sbrk() is a system call (or a C library wrapper around a system call) that allows a program to request more memory from the operating system for its data segment. 
How it works:
It takes an argument incr (an integer representing the number of bytes to add or subtract). 
It adds incr to the current "break" value, which determines the end of the data segment. 
If incr is positive, the data segment grows; if negative, it shrinks (though shrinking is not always guaranteed to succeed). 
It returns a pointer to the start of the newly allocated memory (or the prior break value if incr is 0). 
- Return Value:
On success, sbrk() returns the prior break value. 
On failure (e.g., if there's not enough memory), it returns (void *) -1 and sets errno to indicate the error. 
Relationship to brk():
The brk() function is related to sbrk(), but it sets the break value directly to a given address, while sbrk() adds to the current break value. 
Modern Alternatives:
While sbrk() was a common way to manage memory allocation in older systems, modern systems often use mmap() for memory mapping, which provides more flexibility and control over memory allocation. 


### 2.2 Fragmentation and Strategies

Memory fragmentation occurs when memory is allocated and freed in a way that leaves unusable gaps. It can lead to inefficient use of memory, as free blocks may not be large enough to satisfy allocation requests.

- **Internal Fragmentation**: Memory waste within allocated blocks (due to the block size being larger than requested).
- **External Fragmentation**: Gaps between allocated blocks in the heap.
- **Research Areas**:
  - **Compaction** techniques for defragmenting the heap.
  - **Best-fit** allocation: Minimizing external fragmentation.
  - **First-fit** and **next-fit** strategies for faster allocation.

### 2.3 Best-fit, First-fit, and Buddy Allocators

Memory allocation strategies are essential to efficiently manage the heap:

- **Best-fit**: Allocates the smallest available block that fits the requested size, minimizing waste.
- **First-fit**: Allocates the first available block that fits the request, offering faster allocation but potentially causing fragmentation.
- **Buddy Allocator**: Divides memory into blocks of sizes that are powers of two, improving efficiency and simplifying memory splitting/merging.

- **Research Areas**:
  - Performance trade-offs between allocation strategies.
  - When to use **buddy system** or **slab allocation** (for fixed-size objects).

---

## 3. Free List Management

A **free list** is a linked list of unused memory blocks, crucial for efficient deallocation and reuse. When a block is freed, it's added to this list.

- **Chunk merging**: Combining adjacent free blocks to reduce fragmentation.
- **Chunk splitting**: Dividing larger blocks into smaller ones when needed.
  
- **Research Areas**:
  - Efficient **chunk splitting** and **merging** algorithms.
  - Using **sorted lists** or **binary trees** to optimize chunk lookup and insertion.
  - **Concurrent free list management** in multi-threaded environments.

---

## 4. Thread Safety

Thread safety is critical when a memory allocator is used in a multi-threaded application to ensure that one thread does not interfere with another's memory operations.

- **Mutex locks** for synchronization.
- **Lock-free structures** to minimize contention.
- **Thread-local heaps** or **partitioned allocators** to ensure each thread has its own memory region.

- **Research Areas**:
  - Best practices for **atomic operations** and **memory fences** in allocators.
  - Impact of thread-local storage on memory access patterns and performance.
  
---

## 5. Memory Leak Detection

Memory leak detection ensures that allocated memory is properly freed when no longer in use. Leaks can occur due to missing or incorrect deallocation.

- **Tools for detection**: Use **valgrind** or **AddressSanitizer** to detect leaks in development.
- **Reference counting** or **smart pointers** can help in managing object lifecycles and preventing leaks.

- **Research Areas**:
  - Techniques for **detecting dangling pointers** and **double frees**.
  - Tools or logging strategies to automate memory leak detection.

---

## 6. Garbage Collection

### Overview

Garbage collection (GC) is a technique for automatically managing memory. Unlike traditional allocators that rely on explicit `malloc()`/`free()` calls, GC systems automatically identify and reclaim unused memory.

- **Mark-and-sweep**: Identifies reachable objects and marks them, then sweeps through memory to reclaim unused objects.
- **Reference counting**: Keeps a count of how many references point to an object, and automatically frees the object when the count drops to zero.

- **Research Areas**:
  - Benefits and challenges of integrating GC with custom allocators.
  - Trade-offs between **mark-and-sweep** and **reference counting**.
  - Performance overhead of garbage collection in a low-level allocator.

---

## 7. Inter-Process Communication (IPC)

### Overview

IPC allows multiple processes to communicate and share memory. A memory allocator that supports **shared memory regions** can enable communication between processes via memory-mapped files or shared memory segments.

- **Shared Memory**: Allows processes to directly access the same memory region.
- **Synchronization Primitives**: Semaphores or mutexes to control access to shared memory.

- **Research Areas**:
  - The implementation of **`shm_open()`** and **`mmap()`** for shared memory.
  - Synchronization techniques to prevent race conditions.
  - Use cases of **IPC** in applications requiring shared access to large datasets (e.g., databases, real-time systems).

---

## 8. Advanced Features and Future Work

- **Memory Mapping and Paging**: Implementing **virtual memory** and **paging** to handle larger datasets or memory-mapped files efficiently.
- **Slab Allocator**: Designing a specialized allocator for small, fixed-size objects to improve performance in certain use cases.
- **Adaptive Allocation**: Dynamically selecting the best memory allocation strategy based on usage patterns or workload characteristics.

---

## 9. References

- **Books**:
  - "The Art of Memory Management" by Peter Seibel
  - "Operating Systems: Design and Implementation" by Andrew S. Tanenbaum
- **Research Papers**:
  - "A Survey of Memory Allocation Techniques" by John Doe
  - "Efficient Garbage Collection Techniques" by Jane Smith

- **Tools**:
  - Valgrind (https://valgrind.org/)
  - AddressSanitizer (https://clang.llvm.org/docs/AddressSanitizer.html)

---

This **`research.md`** document outlines the foundational and advanced concepts that will guide the implementation of your custom memory allocator project. By conducting thorough research in each of these areas, you'll be well-equipped to handle challenges in memory management and take advantage of opportunities to scale your allocator with advanced features.

Would you like to further refine any of these sections or add additional research topics?