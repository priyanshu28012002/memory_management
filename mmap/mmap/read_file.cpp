#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <chrono>
#include <thread>

class FileReader {
public:
    // Constructor to initialize member variables
    FileReader(const char* filename) : filename(filename), src_ptr(nullptr), file_size(0), src_fd(-1) {}

    // Setup function to open, get size, and map the file to memory
    bool setup() {
        // Open the source file
        src_fd = open(filename, O_RDONLY);
        if (src_fd == -1) {
            perror("Error opening source file");
            return false;
        }

        // Get the size of the source file
        struct stat st;
        if (fstat(src_fd, &st) == -1) {
            perror("Error getting file size");
            close(src_fd);
            return false;
        }
        file_size = st.st_size;

        // Memory map the source file
        src_ptr = mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
        if (src_ptr == MAP_FAILED) {
            perror("Error mapping source file to memory");
            close(src_fd);
            return false;
        }

        return true;
    }

    // Function to print the content of the memory-mapped file
    void printContent() const {
        if (src_ptr != nullptr) {
            std::cout.write(reinterpret_cast<char*>(src_ptr), file_size);
            std::cout << std::endl;
        } else {
            std::cerr << "Error: File not mapped correctly." << std::endl;
        }
    }

    // Destructor to clean up resources
    ~FileReader() {
        if (src_ptr != nullptr) {
            munmap(src_ptr, file_size);  // Unmap the source file
        }
        if (src_fd != -1) {
            close(src_fd);  // Close the file descriptor
        }
    }

private:
    const char* filename;
    void* src_ptr;      // Pointer to the memory-mapped file
    size_t file_size;   // Size of the file
    int src_fd;         // File descriptor for the source file
};

// Timer function that calls a function at set intervals
void timerFunction(FileReader &fileReader, int interval_seconds) {
    while (true) {
        // Set up and print file content
        if (fileReader.setup()) {
            fileReader.printContent();
        } else {
            std::cerr << "File setup failed." << std::endl;
        }

        // Wait for the specified interval (in seconds)
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_seconds));
    }
}

int main() {
    const char* filename = "file1.txt";
    
    // Create FileReader instance
    FileReader fileReader(filename);
    
    // Set the timer to call setup and printContent every 5 seconds
    int interval_seconds = 50;  // Set the interval to 5 seconds
    timerFunction(fileReader, interval_seconds);

    return 0;
}
