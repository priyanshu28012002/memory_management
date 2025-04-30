# Custom Memory Allocation Library
## **Overview**
The `my_alloc` project aims to implement a custom memory allocation library that offers fine-grained control over memory management by memory allocator, file I/O, inter-process communication (IPC). The goal is to create a highly flexible, efficient, and scalable allocator with custom memory management techniques.

---

## **Implementation**

### 1. **Memory Allocation Using `sbrk` and `brk` System Call**
   - **Purpose**: Implement a memory allocator `heep_alloc()` using `sbrk()` and `brk()` system calls to manage the program's heap.
   - **Challenges**: 
     - `sbrk` is deprecated on some platforms.
     - Fragmentation management.
     - Efficient block management.
     - Implement memory allocation using `sbrk` (or `mmap` as an alternative).
     - Consider using **buddy allocation**, **first-fit**, or **best-fit** strategies to reduce fragmentation.
     - Track allocated and free blocks using **free lists**.
---

### 2. **Free Memory After Use (Delete Memory)**
   - **Purpose**: Implement a `heep_free()` function to release memory back to reuse.
   - **Challenges**:
     - Handling fragmentation and reusing freed blocks.
     - Safely managing errors like double-free or invalid free.
     - Use a **linked list** or **free list** to manage free memory blocks.
     - Implement **safety checks** (e.g., double-free detection).
     - Merge adjacent free blocks to reduce fragmentation.

---

### 3. **Track Memory Usage in the Heap**
   - **Purpose**: Track allocated memory and monitor heap usage.
   - **Challenges**:
     - Efficient memory tracking without introducing overhead.
     - Managing fragmented memory.
     - Maintain **metadata** for each allocated block (e.g., size, status).
     - Use a **linked list** or **bitmap** to track the status of each block in the heap.
     - Implement debugging tools for memory fragmentation statistics.

---


## **Features**

#### 1. **Thread Safety**
   - Ensure that the allocator is thread-safe when used in multi-threaded environments.
  
- Use **mutexes** or **locks** to synchronize memory access in multi-threaded applications.
- Consider using **lock-free data structures** or **atomic operations** to reduce contention.

#### 2. **Performance Optimization**
   - Optimize the memory allocator for performance, reducing overhead and fragmentation.
- Implement **benchmarking tools** to test performance under various scenarios.
- Profile memory usage and fragmentation, and experiment with **block size** tuning.
- Consider using **slab allocation** for objects of fixed sizes to speed up allocation.

#### 3. **Memory Leak Detection**
   - Implement a tool to detect memory leaks in programs using your allocator.
- Keep track of all allocated blocks and check for any unfreed memory at program exit.
- Provide **error messages** or warnings if memory is not freed properly.
- Use **debugging tools** to inspect memory allocation patterns and detect leaks.
  



---


This documentation provides an overview of the key features and design decisions for the `my_alloc` project. As you implement each feature, refer to this documentation to ensure you stay on track and address potential challenges. It's important to incrementally build and test each component, starting with basic memory allocation and progressing. As the project grows, keep iterating, benchmarking, and optimizing for performance.

---

Looking ahead, the `my_alloc` framework is well-positioned to support **advanced memory management features**, including:

- **Garbage Collection**: **Integration of automatic or semi-automatic garbage collection strategies** (e.g., **mark-and-sweep**, **reference counting**) to manage object lifecycles. This will automate the process of reclaiming unused memory, reducing the need for explicit memory management and improving application stability by preventing memory leaks.

- **Inter-Process Communication (IPC)**: **Support for shared memory regions** (e.g., using `shm_open`, `mmap`) to enable memory sharing between processes. This could include optional **synchronization primitives** like semaphores or mutexes to coordinate access and ensure thread safety across processes.

---