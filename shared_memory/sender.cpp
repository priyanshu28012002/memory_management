#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/shm.h>
#include <cstring>

int main()
{
    int i;
    void *shared_memory;  // Pointer to hold the address of the shared memory
    char buff[100];       // Buffer to hold user input

    int shmid;            // Shared memory ID (returned by shmget)
    
    // Create a shared memory segment with a unique key (2345), size (1024 bytes), and permissions (0666: read/write for everyone)
    // IPC_CREAT is used to create the shared memory segment if it doesn't already exist
    shmid = shmget((key_t)2345, 1024, 0666 | IPC_CREAT); 
    
    // Check if shared memory was created successfully
    if (shmid == -1) {
        std::cerr << "Failed to create shared memory segment." << std::endl;
        return 1; // Exit if shmget fails
    }
    
    // Print the shared memory ID to confirm creation
    std::cout << "Key of shared memory is " << shmid << std::endl;
    
    // Attach the shared memory to the process's address space, making it accessible
    shared_memory = shmat(shmid, nullptr, 0);  // shmat returns the address of the shared memory segment
    
    // Check if shared memory attachment is successful
    if (shared_memory == (void*)-1) {
        std::cerr << "Failed to attach shared memory." << std::endl;
        return 1; // Exit if shmat fails
    }
    
    std::cout << "Process attached at " << shared_memory << std::endl; // Print the address where the shared memory is attached
    
    // Ask the user to input data
    std::cout << "Enter some data to write to shared memory: ";
    
    // Read up to 100 characters from standard input (keyboard) into the buffer 'buff'
    std::cin.getline(buff, 100);  // Use C++ std::cin to read input
    
    // Copy the input data from 'buff' to the shared memory segment
    std::strcpy((char*)shared_memory, buff); // Data from buff is written to the shared memory
    
    // Print the content written to shared memory
    std::cout << "You wrote: " << (char*)shared_memory << std::endl;  // Print the data stored in shared memory
    
    return 0;
}
