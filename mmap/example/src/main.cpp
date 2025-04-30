// #include <myalloc/my_alloc.hpp>
// #include <iostream>
// #include "ds.hpp"


// #include <memory>  

// using namespace std;

// int main(int argc,char ** argv) {
//     // init the MYALLOc
//     // int res = MYALLOC::init(argc,argv);
    
//     // if(res ==0){ 
//     //     std::cout<<"MYALLOC::init success full"<<std::endl;
//     // }
//     // else{
//     //     std::cout<<"MYALLOC::init Fails full"<<std::endl;
//     // }
//     // // object of the memory management
//     // MYALLOC mm;
//     // you ask for the a 4 bit of memory sizeof(int) i will provide the starting of the memory address
//     // make the int float string  Character Boolean Date/Time  Arrays Objectson the heap

//     // int* ptr =  (int *)mm.allocate(sizeof(int)*2);
//     // *ptr = 4;
//     // ptr++;
//     // *ptr = 5;
//     // std::cout<<*ptr<<std::endl;
//     // ptr--;
//     // std::cout<<*ptr<<std::endl;
//     // mm.deallocate(ptr);

    
//     // int id = mm.allocate("int");
//      //int id = mm.allocate("float");
//     //int id = mm.allocate("char");

//     //int val = mm.get(id);
//     // mm.set(id,5);
//     // mm.deallocate(id);






//     // moniter the file  at interval
//     // read the file set the int 1 
//     // const char* filename = "/home/octobotics/Desktop/Working/memory_management/mm/my_alloc/include/my_alloc.hpp";
//     // mm.moniterFile(filename,5);

//     // write the str on the file and control the condition 
//     // const char* filename = "/home/octobotics/Desktop/Working/memory_management/mm/my_alloc/test/test.txt";
//     // std::string str = "✅ Final Goal: writeFile() Method in MMap Class";
//     // mm.writeFile(filename, str);



    
//     return 0;
// }


// #include <iostream>
// #include <memory>  
// #include <vector>
// #include "my_alloc.hpp"
// using namespace std;



// int main(int argc,char ** argv) {
//     // init the MYALLOc
//     int res = MYALLOC::init(argc,argv);
    
//     if(res ==0){ 
//         std::cout<<"MYALLOC::init success full"<<std::endl;
//     }
//     else{
//         std::cout<<"MYALLOC::init Fails full"<<std::endl;
//     }
//     // object of the memory management
//     MYALLOC mm;
//     // you ask for the a 4 bit of memory sizeof(int) i will provide the starting of the memory address
//     // make the int float string  Character Boolean Date/Time  Arrays Objectson the heap

//     // int* ptr =  (int *)mm.allocate(sizeof(int)*2);
//     // *ptr = 4;
//     // ptr++;
//     // *ptr = 5;
//     // std::cout<<*ptr<<std::endl;
//     // ptr--;
//     // std::cout<<*ptr<<std::endl;
//     // mm.deallocate(ptr);

    
//     int id = mm.allocate("int");
//      //int id = mm.allocate("float");
//     //int id = mm.allocate("char");

//     //int val = mm.get(id);
//     // mm.set(id,5);
//     // mm.deallocate(id);






//     // moniter the file  at interval
//     // read the file set the int 1 
//     // const char* filename = "/home/octobotics/Desktop/Working/memory_management/mm/my_alloc/include/my_alloc.hpp";
//     // mm.moniterFile(filename,5);

//     // write the str on the file and control the condition 
//     // const char* filename = "/home/octobotics/Desktop/Working/memory_management/mm/my_alloc/test/test.txt";
//     // std::string str = "✅ Final Goal: writeFile() Method in MMap Class";
//     // mm.writeFile(filename, str);



    
//     return 0;
// }

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



// std::mutex SharedMemory::creation_mutex;

// // Example usage
// int main() {

//     BufferedFile file("test.bin", "w+", 1024);
//     const char* data = "Hello, buffered world!";
//     file.write(data, strlen(data));
//     file.seek(0);
//     // try{
//     //         BufferedFile file("test.bin", "w+", 1024);
//     //         const char* data = "Hello, buffered world!";
//     //         file.write(data, strlen(data));
//     //         file.seek(0);
            
//     //         char buffer[256] = {0};
//     //         file.read(buffer, sizeof(buffer));
//     //         std::cout << "Read from file: " << buffer << std::endl;
//     //     }

//         // Shared memory example
//     //     {
//     //         bool is_parent = true;
//     //         SharedMemory shm("test_shared_mem", 4096, true);
            
//     //         if (fork() == 0) {
//     //             // Child process
//     //             is_parent = false;
//     //             SharedMemory shm_child("test_shared_mem", 4096);
//     //             int* counter = shm_child.get_as<int>();
                
//     //             for (int i = 0; i < 10; ++i) {
//     //                 int expected = *counter;
//     //                 while (!shm_child.atomic_compare_exchange(0, expected, expected + 1)) {
//     //                     usleep(1000);
//     //                 }
//     //                 std::cout << "Child incremented counter to " << expected + 1 << std::endl;
//     //                 usleep(500000);
//     //             }
//     //             exit(0);
//     //         } else {
//     //             // Parent process
//     //             int* counter = shm.get_as<int>();
//     //             *counter = 0;
                
//     //             for (int i = 0; i < 5; ++i) {
//     //                 int val = shm.atomic_load<int>(0);
//     //                 shm.atomic_store<int>(0, val + 1);
//     //                 std::cout << "Parent incremented counter to " << val + 1 << std::endl;
//     //                 usleep(1000000);
//     //             }
                
//     //             wait(nullptr); // Wait for child
//     //             std::cout << "Final counter value: " << *counter << std::endl;
//     //         }
//     //     }
//     // } catch (const std::exception& e) {
//     //     std::cerr << "Error: " << e.what() << std::endl;
//     //     return 1;
//     // }

//     return 0;
// }