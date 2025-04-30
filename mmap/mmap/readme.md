# File Copy and Append Utility

This program copies the contents of one file to another, with optional flags for appending or adding a newline before appending the contents.

## Workflow

The program performs the following steps:

1. **Open Source File**: The source file (`file1.txt`) is opened in read-only mode.
2. **Get File Size**: The size of the source file is determined to ensure the entire file is read.
3. **Memory Mapping**: The source file is memory-mapped to allow direct access to its contents.
4. **Open Destination File**: The destination file (`file2.txt`) is opened with the specified flags:
   - If the `append` flag is set, the file is opened for appending.
   - If the `append` flag is not set, the file is opened in truncate mode (i.e., overwriting the existing contents).
5. **Newline Insertion**: If the `newLine` flag is set and the destination file already has content, a newline is written before appending the new content.
6. **Write Content**: The contents of the source file are written to the destination file.
7. **Cleanup**: The memory-mapped source file is unmapped and both the source and destination file descriptors are closed.

## Flags

The program accepts the following flags via command-line arguments:

1. `append`: If this flag is set to `true`, the contents of the source file will be appended to the destination file.
   - Default: `false` (the destination file will be overwritten).

2. `newLine`: If this flag is set to `true` and the `append` flag is enabled, a newline character will be inserted before appending the new content if the destination file is not empty.
   - Default: `false`.

## How to Run

### Compile the Program

1. **Ensure you have a C++ compiler** (e.g., `g++`).

2. **Compile the program** using the following command:

   ```bash
   g++ -o file_copy_append file_copy_append.cpp


Example Usage
Default behavior (overwrite destination file):

bash
Copy
./file_copy_append
This will copy the content from file1.txt to file2.txt and overwrite file2.txt.

Append content to the destination file:

bash
Copy
./file_copy_append true
This will append the content of file1.txt to file2.txt.

Append content with a newline before it:

bash
Copy
./file_copy_append true true
This will append the content of file1.txt to file2.txt and insert a newline before appending if file2.txt has existing content.


Here is an explanation of the flags used with the system calls `fstat`, `mmap`, `munmap`, and `lseek`, which are used in your code.

### 1. **`fstat` Flags**

`fstat` is used to retrieve information about a file, such as its size, permissions, and file type. The `fstat` function itself does not use flags directly, but it populates a `struct stat` with file information. Here's how it works:

```cpp
struct stat st;
fstat(src_fd, &st);
```

- **`src_fd`**: This is the file descriptor obtained from opening the file.
- **`&st`**: This is a pointer to a `struct stat` where the file information is stored.

The `struct stat` has the following relevant fields:
- **`st_size`**: The size of the file in bytes.
- **`st_mode`**: The file mode (file type and permissions).
- **`st_uid`, `st_gid`**: The user ID and group ID of the file's owner.
- **`st_atime`, `st_mtime`, `st_ctime`**: The access, modification, and status change times.

### 2. **`mmap` Flags**

`mmap` is used to map a file or device into memory. The flags control the access and behavior of the memory mapping.

```cpp
void *src_ptr = mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
```

Here are the flags used:

- **`PROT_READ`**: Specifies that the mapped memory region should be readable.
  - Other possible protections include:
    - `PROT_WRITE`: Allows writing to the mapped memory.
    - `PROT_EXEC`: Allows execution of instructions in the mapped memory.
    - `PROT_NONE`: Disallows access to the mapped region.

- **`MAP_PRIVATE`**: Creates a private copy of the memory-mapped file. Changes made to the mapped region will not be written back to the file, and the original file remains unchanged. This is useful for reading the file without modifying it.
  - Other options:
    - `MAP_SHARED`: Changes made to the mapped memory are reflected in the file.

- **`src_fd`**: The file descriptor of the source file, which is mapped into memory.
- **`0`**: The offset within the file to start mapping from (usually 0 if you're mapping from the beginning of the file).

### 3. **`munmap` Flags**

`munmap` is used to unmap a previously mapped memory region. It does not take flags, but its parameters control the behavior:

```cpp
munmap(src_ptr, file_size);
```

- **`src_ptr`**: The pointer to the start of the mapped memory region, which is returned by `mmap`.
- **`file_size`**: The size of the mapped memory region that is being unmapped.

There are no flags in `munmap`, but it ensures that the memory region is released.

### 4. **`lseek` Flags**

`lseek` is used to reposition the offset of the file descriptor. It allows you to set the current position in the file (i.e., where the next read or write will occur). The flags control how the offset is calculated:

```cpp
off_t current_offset = lseek(dest_fd, 0, SEEK_END);
```

Here are the flags used:

- **`SEEK_END`**: Sets the offset relative to the end of the file. The new offset will be the current size of the file, which is useful to determine the current end of the file for appending content.
  - Other options for `whence`:
    - `SEEK_SET`: Sets the offset relative to the beginning of the file (i.e., the offset is directly the `offset` parameter).
    - `SEEK_CUR`: Sets the offset relative to the current offset (i.e., adds `offset` to the current file offset).

- **`dest_fd`**: The file descriptor of the destination file.
- **`0`**: The offset argument, which is `0` when we want to set the position to the end of the file (since we're using `SEEK_END`).

### Summary of Flags

1. **`fstat`**: No specific flags for `fstat`, but it populates a `struct stat` with file metadata (e.g., size, permissions).
   
2. **`mmap`**:
   - **`PROT_READ`**: Read access to the mapped memory.
   - **`MAP_PRIVATE`**: Private memory mapping, changes are not reflected back to the file.

3. **`munmap`**: No flags, just unmaps a memory region.

4. **`lseek`**:
   - **`SEEK_END`**: Sets the file offset to the end of the file.
   - **`SEEK_SET`**, **`SEEK_CUR`**: Other options to reposition the file offset.

These system calls and their respective flags are crucial for efficiently handling files at a low level, particularly when working with large files or performing complex file manipulations like memory-mapping and file offset manipulation.



1 first we need the file discriptor to get the size of the file 

int fd = open(filePath, mode);
int fd = open(filePath, O_RDONLY) ; 
int fd = open(filePath, O_WRONLY) ; 
int fd = open(filePath, O_RDWR) ; 

struct stat sb;

A file table entry tracks the current offset referenced by all read or write operations to the file and the open mode (O_RDONLY, O_WRONLY, or O_RDWR) of the file.


2 use mmap to read the data 
char* file_in_memory = mmap(NULL , sb.size(), PROT_READ, MAP_PRIVATE,fd,0)

PROT_READ for
PROT_WRITE for


MAP_PRIVETE for 
MAP_SHARE for
0 from bigining of the file


printf(Print the file )
for(int i=0; i<sb.size; i++){
    file_in_memory = "a";
    printf("%c" , file_in_memory[i]);
}


munmap(file_in_memory, sb,st_size);
close(fd)

Yes, `mmap` (memory mapping) is a powerful system call in Unix-like operating systems (including Linux) that allows you to map files or devices directly into memory. This enables your program to access the file's contents as though they were part of the process's memory address space, rather than dealing with read and write operations.

### Memory Mapping with `mmap`

Here’s how `mmap` can be used to store data in memory (RAM) and read it directly:

1. **Store Data in RAM:**
   `mmap` can be used to map a portion of memory to a file, but it can also be used to allocate anonymous memory regions in RAM that are not backed by any file. This can be useful for creating shared memory between processes or dynamically allocating large blocks of memory.

2. **Access the Memory:**
   After mapping the memory, the data stored at the mapped memory addresses can be directly accessed, modified, or read without having to explicitly deal with I/O operations.

### Example of `mmap` to Store and Read Data in RAM:

Here’s an example of using `mmap` to allocate memory in RAM and store some data there:

```cpp
#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

int main() {
    // Size of the memory region to map
    size_t size = 4096;  // 4 KB

    // Create a memory-mapped region in RAM (anonymous memory)
    void* addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    if (addr == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }

    // Write some data to the mapped memory
    const char* message = "Hello, mmap!";
    strncpy(static_cast<char*>(addr), message, size);

    // Read data from the mapped memory
    std::cout << "Data from mmap: " << static_cast<char*>(addr) << std::endl;

    // Unmap the memory when done
    if (munmap(addr, size) == -1) {
        perror("munmap failed");
        return 1;
    }

    return 0;
}
```

### Explanation:

1. **`mmap()`**: This system call creates a mapping in the virtual address space of the calling process. In this case:
   - `NULL` means the kernel can choose where to map the memory.
   - `size` is the number of bytes to map.
   - `PROT_READ | PROT_WRITE` allows the mapped memory to be read from and written to.
   - `MAP_PRIVATE | MAP_ANONYMOUS` indicates that the memory is not backed by any file and is private to the process.
   - `-1` and `0` are placeholders because we are not mapping a file.

2. **`strncpy()`**: This function copies a string into the mapped memory region.

3. **Accessing Memory**: The data is now stored in RAM, and you can access it directly from the memory region.

4. **`munmap()`**: This function unmaps the memory when you are done, releasing the memory back to the system.

### Key Flags for `mmap`:

- **`MAP_PRIVATE`**: This flag indicates that changes to the mapped memory will not be written back to the underlying file. It's used for private memory mapping (in this case, anonymous memory).
- **`MAP_ANONYMOUS`**: This flag indicates that no file is associated with the memory region. The memory is initialized to zero, and it is not backed by any file.
- **`PROT_READ | PROT_WRITE`**: These flags specify that the memory region will be readable and writable.
  
### Use Cases:

1. **Efficient File I/O**: By mapping files into memory, you can manipulate them as if they are arrays in memory, allowing efficient access to file data without using traditional read/write calls.
   
2. **Shared Memory**: You can use `mmap` to create shared memory regions between processes. This is typically used for inter-process communication (IPC).

3. **Memory Allocation**: You can use `mmap` to allocate large blocks of memory directly, which may help you manage memory usage in performance-critical applications.

### Reading from Mapped Memory:

Once memory is mapped, you can read from it just like a normal pointer. For instance, after using `mmap` in the example above, you can treat `addr` as a pointer to memory, accessing it directly.

In summary, `mmap` allows you to allocate memory that resides in RAM, and you can use it to store and read data directly without traditional I/O operations. It is efficient for both large data handling and inter-process communication scenarios.

When implementing Inter-Process Communication (IPC) using shared memory, the location of the shared memory region is in **RAM**, not on the SSD. Shared memory is a mechanism that allows multiple processes to access the same region of memory (RAM), enabling fast communication between processes.

### How Shared Memory Works:

1. **In RAM**: 
   - Shared memory regions are allocated directly in the system's physical memory (RAM). This allows different processes to access and manipulate the data directly in memory, which is much faster than traditional file-based communication mechanisms (such as writing to and reading from a file stored on disk).
   - Since shared memory resides in RAM, it's typically **volatile**. That means once the system is shut down or restarted, the data in shared memory will be lost.

2. **Memory-mapped Files vs. Shared Memory**:
   - In the context of IPC, shared memory is typically created using `shmget`/`shmat` (on Linux/Unix systems) or `CreateFileMapping`/`MapViewOfFile` (on Windows).
   - However, you can also use **memory-mapped files** (with `mmap`) to map files into memory. While this can involve RAM, the backing storage is a **file on disk**. The memory-mapped file itself can either be backed by physical storage (SSD or HDD) or anonymous (no file, only RAM).
     - **Anonymous mapping** (e.g., `MAP_ANONYMOUS` in `mmap`) does not involve any disk storage and resides solely in RAM.
     - **File-backed mapping** (e.g., `MAP_SHARED` with a file descriptor) involves mapping a file on disk (SSD or HDD) into memory. Data written to this shared memory region can be persisted to disk.

### Location of Shared Memory:

- **Shared Memory (without backing file)**: 
  - The memory region used for IPC will be mapped directly into the process's address space in **RAM**. This memory region can be accessed by any process that has been granted access to it.
  
- **File-backed Memory** (e.g., memory-mapped files):
  - If the shared memory region is backed by a file (i.e., the file exists on disk, like on an SSD or HDD), the file is mapped into memory using the `mmap` system call. The **data is stored in RAM** but is also written to the backing file on disk. For example, if a memory-mapped file is used for shared memory, the data can be persisted on disk (SSD/HDD) even if the processes using it are no longer running.

### Example:

1. **Anonymous Shared Memory (RAM-only)**:

   - If you're using `shmget`/`shmat` or `mmap` with `MAP_ANONYMOUS`, no file is associated with the shared memory region, so it resides in **RAM**. It is temporary and will be lost when the system shuts down.

   Example (Linux):
   ```cpp
   #include <sys/mman.h>
   #include <sys/types.h>
   #include <sys/stat.h>
   #include <fcntl.h>
   #include <unistd.h>
   #include <stdio.h>

   int main() {
       size_t size = 4096;  // Size of shared memory region

       // Create anonymous shared memory
       void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
       if (addr == MAP_FAILED) {
           perror("mmap failed");
           return 1;
       }

       // Use the shared memory
       snprintf((char*)addr, size, "Hello, shared memory!");

       printf("Shared Memory: %s\n", (char*)addr);

       // Clean up
       if (munmap(addr, size) == -1) {
           perror("munmap failed");
           return 1;
       }

       return 0;
   }
   ```

   In this example, the shared memory is created in **RAM**, and the data does not persist after the program ends or the system is rebooted.

2. **File-backed Shared Memory (SSD or HDD)**:
   
   If a file is involved, the memory region is backed by that file (typically on SSD or HDD), and the data is stored in **RAM** but can be persisted on the disk.
   
   Example (Linux):
   ```cpp
   #include <sys/mman.h>
   #include <fcntl.h>
   #include <unistd.h>
   #include <stdio.h>

   int main() {
       const char* filename = "/tmp/shared_mem_file";  // File on disk (SSD/HDD)

       // Open the file
       int fd = open(filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
       if (fd == -1) {
           perror("Failed to open file");
           return 1;
       }

       // Set the file size (size of the shared memory region)
       size_t size = 4096;  // 4 KB
       if (ftruncate(fd, size) == -1) {
           perror("Failed to set file size");
           return 1;
       }

       // Create file-backed memory mapping
       void* addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
       if (addr == MAP_FAILED) {
           perror("mmap failed");
           return 1;
       }

       // Use the shared memory
       snprintf((char*)addr, size, "Hello, file-backed shared memory!");

       printf("Shared Memory: %s\n", (char*)addr);

       // Clean up
       if (munmap(addr, size) == -1) {
           perror("munmap failed");
           return 1;
       }

       close(fd);
       return 0;
   }
   ```

   Here, the shared memory is mapped to a file on disk, and while the actual data resides in **RAM**, the file on the disk is used for persistence. The data can be retained on disk even after the program ends.

### Summary:

- **Shared memory for IPC** is typically stored in **RAM**.
- If it’s **anonymous shared memory**, no file is involved, and the data is stored in RAM only.
- If it’s **file-backed memory** (using `mmap` with a file), the data resides in **RAM**, but it can also be persisted to a file on **disk** (SSD or HDD).

In both cases, shared memory provides very fast access compared to other IPC mechanisms like pipes or message queues, because it avoids copying data between user space and kernel space. However, shared memory regions backed by files (file-backed) provide persistence (on disk), while anonymous shared memory does not.
