#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <chrono>
#include <thread>
#include <atomic>
#include <fstream>
#include <sstream>
#include <cstring>  // Include this header for memcpy

std::atomic<uint8_t> timeBuffer(0);  // Lock-free buffer to store the current time (in 8-bit)

class TimeStorer {
public:
    TimeStorer(const char* filename) : filename(filename) {
        // Open file for memory mapping with O_TRUNC to clear file contents
        fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (fd == -1) {
            perror("Error opening file");
            exit(1);
        }

        // Initially, we will write some content to the file so that mmap can map it
        writeToFile("Initial Time Data\n");

        // Get the size of the file for memory mapping
        struct stat st;
        if (fstat(fd, &st) == -1) {
            perror("Error getting file size");
            close(fd);
            exit(1);
        }
        file_size = st.st_size;

        // Memory map the file
        mapBuffer();
    }

    ~TimeStorer() {
        if (buffer != MAP_FAILED) {
            // Unmap the buffer
            munmap(buffer, file_size);
        }
        if (fd != -1) {
            // Close the file descriptor
            close(fd);
        }
    }

    void storeTime() {
        while (true) {
            // Get current time in seconds since epoch
            auto now = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
            uint8_t timeInSeconds = static_cast<uint8_t>(duration.count() % 256); // Keep it within 8 bits

            // Store the time in the atomic buffer
            timeBuffer.store(timeInSeconds, std::memory_order_release);

            // Convert the time to a string and store in the memory buffer
            std::ostringstream oss;
            oss << "Current Time: " << static_cast<int>(timeInSeconds) << "\n";
            std::string timeStr = oss.str();

            // Copy the string into the memory-mapped buffer
            memcpy(buffer, timeStr.c_str(), timeStr.size() + 1);  // +1 for null terminator

            // Write the updated data from the memory buffer to the file (overwrite file)
            writeToFile(timeStr);

            // Sleep for a second before updating again
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

private:
    const char* filename;
    int fd = -1;
    void* buffer = MAP_FAILED;
    size_t file_size = 0;

    void mapBuffer() {
        // Memory map the file for reading and writing (MAP_SHARED for sync with file)
        buffer = mmap(nullptr, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (buffer == MAP_FAILED) {
            perror("Error mapping file to memory");
            close(fd);
            exit(1);
        }
    }

    void writeToFile(const std::string& data) {
        // Write data to the file (overwrite mode)
        std::ofstream outFile(filename, std::ios::out);  // No append flag, will overwrite the file
        if (outFile.is_open()) {
            outFile << data;  // Write current time data to file
        } else {
            std::cerr << "Error opening file for writing." << std::endl;
        }
    }
};

int main() {
    const char* filename = "file1.txt";

    // Create TimeStorer instance and run in a separate thread
    TimeStorer timeStorer(filename);

    // Run the storeTime function in a separate thread
    std::thread timeThread(&TimeStorer::storeTime, &timeStorer);

    // Join the time thread (this will run indefinitely, so it won't really return)
    timeThread.join();

    return 0;
}
