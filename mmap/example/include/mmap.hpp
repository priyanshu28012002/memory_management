class BufferedFile {
    private:
        std::fstream file;
        std::vector<char> read_buffer;
        std::vector<char> write_buffer;
        size_t buffer_size;
        size_t read_pos;
        size_t write_pos;
        std::string mode;
    
        void flush_write_buffer() {
            if (write_pos > 0) {
                file.write(write_buffer.data(), write_pos);
                write_pos = 0;
            }
        }
    
        void fill_read_buffer() {
            file.read(read_buffer.data(), buffer_size);
            read_pos = 0;
        }
    
    public:
        BufferedFile(const std::string& filename, const std::string& mode = "r+", size_t buf_size = 4096)
            : buffer_size(buf_size), read_pos(0), write_pos(0), mode(mode) {
            
            std::ios_base::openmode fmode = std::ios_base::binary;
            if (mode.find('r') != std::string::npos) fmode |= std::ios_base::in;
            if (mode.find('w') != std::string::npos) fmode |= std::ios_base::out;
            if (mode.find('a') != std::string::npos) fmode |= std::ios_base::app;
            if (mode.find('t') != std::string::npos) fmode &= ~std::ios_base::binary;
    
            file.open(filename, fmode);
            if (!file.is_open()) {
                throw std::runtime_error("Failed to open file: " + filename);
            }
    
            read_buffer.resize(buffer_size);
            write_buffer.resize(buffer_size);
            if (mode.find('r') != std::string::npos) {
                fill_read_buffer();
            }
        }
    
        ~BufferedFile() {
            if (mode.find('w') != std::string::npos || mode.find('a') != std::string::npos) {
                flush_write_buffer();
            }
            file.close();
        }
    
        size_t read(char* dest, size_t size) {
            if (mode.find('r') == std::string::npos) {
                throw std::runtime_error("File not opened in read mode");
            }
    
            size_t total_read = 0;
            while (size > 0) {
                size_t available = file.gcount() - read_pos;
                if (available == 0) {
                    fill_read_buffer();
                    available = file.gcount();
                    if (available == 0) break; // EOF
                }
    
                size_t to_copy = std::min(size, available);
                std::memcpy(dest + total_read, read_buffer.data() + read_pos, to_copy);
                read_pos += to_copy;
                total_read += to_copy;
                size -= to_copy;
            }
    
            return total_read;
        }
    
        size_t write(const char* src, size_t size) {
            if (mode.find('w') == std::string::npos && mode.find('a') == std::string::npos) {
                throw std::runtime_error("File not opened in write/append mode");
            }
    
            size_t total_written = 0;
            while (size > 0) {
                size_t available = buffer_size - write_pos;
                if (available == 0) {
                    flush_write_buffer();
                    available = buffer_size;
                }
    
                size_t to_copy = std::min(size, available);
                std::memcpy(write_buffer.data() + write_pos, src + total_written, to_copy);
                write_pos += to_copy;
                total_written += to_copy;
                size -= to_copy;
            }
    
            return total_written;
        }
    
        void seek(size_t pos) {
            flush_write_buffer();
            file.seekg(pos);
            file.seekp(pos);
            read_pos = 0;
            file.gcount(); // Reset gcount
        }
    
        void sync() {
            flush_write_buffer();
            file.flush();
        }
    };
    
    // Shared Memory IPC Class
    class SharedMemory {
    private:
        std::string name;
        int fd;
        void* addr;
        size_t size;
        bool is_owner;
        static std::mutex creation_mutex;
    
        void cleanup() {
            if (addr != MAP_FAILED) {
                munmap(addr, size);
            }
            if (fd != -1) {
                close(fd);
            }
            if (is_owner) {
                shm_unlink(name.c_str());
            }
        }
    
    public:
        SharedMemory(const std::string& name, size_t size, bool create = false)
            : name("/" + name), fd(-1), addr(MAP_FAILED), size(size), is_owner(create) {
            
            std::lock_guard<std::mutex> lock(creation_mutex);
            
            if (create) {
                fd = shm_open(this->name.c_str(), O_CREAT | O_RDWR | O_EXCL, 0666);
                if (fd == -1 && errno == EEXIST) {
                    shm_unlink(this->name.c_str());
                    fd = shm_open(this->name.c_str(), O_CREAT | O_RDWR, 0666);
                }
            } else {
                fd = shm_open(this->name.c_str(), O_RDWR, 0666);
            }
    
            if (fd == -1) {
                throw std::runtime_error("shm_open failed: " + std::string(strerror(errno)));
            }
    
            if (create && ftruncate(fd, size) == -1) {
                cleanup();
                throw std::runtime_error("ftruncate failed: " + std::string(strerror(errno)));
            }
    
            addr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if (addr == MAP_FAILED) {
                cleanup();
                throw std::runtime_error("mmap failed: " + std::string(strerror(errno)));
            }
        }
    
        ~SharedMemory() {
            cleanup();
        }
    
        void* get_address() const {
            return addr;
        }
    
        size_t get_size() const {
            return size;
        }
    
        template<typename T>
        T* get_as() {
            return reinterpret_cast<T*>(addr);
        }
    
        // Atomic operations for inter-process synchronization
        template<typename T>
        T atomic_load(size_t offset) {
            static_assert(std::is_integral<T>::value || std::is_pointer<T>::value, 
                         "Atomic operations only support integral and pointer types");
            return __atomic_load_n(reinterpret_cast<T*>(reinterpret_cast<char*>(addr) + offset), __ATOMIC_SEQ_CST);
        }
    
        template<typename T>
        void atomic_store(size_t offset, T value) {
            static_assert(std::is_integral<T>::value || std::is_pointer<T>::value, 
                         "Atomic operations only support integral and pointer types");
            __atomic_store_n(reinterpret_cast<T*>(reinterpret_cast<char*>(addr) + offset), value, __ATOMIC_SEQ_CST);
        }
    
        template<typename T>
        bool atomic_compare_exchange(size_t offset, T& expected, T desired) {
            static_assert(std::is_integral<T>::value || std::is_pointer<T>::value, 
                         "Atomic operations only support integral and pointer types");
            return __atomic_compare_exchange_n(
                reinterpret_cast<T*>(reinterpret_cast<char*>(addr) + offset),
                &expected,
                desired,
                false,
                __ATOMIC_SEQ_CST,
                __ATOMIC_SEQ_CST
            );
        }
    };
    
    