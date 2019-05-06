#include <iostream>
#include <vector>
#include <cstdlib>
#include <new>

class MemoryPool {
    public:
        ~MemoryPool() {
            free(mem);
        }
        
        void *get_memory(size_t n) {
            auto new_size = size + n;
            std::cout << "Current size = " << size << ", "
                      << "new size = " << new_size << std::endl;
            this->mem = static_cast<char *>(std::realloc(mem, new_size));
            if (this->mem == nullptr) {
                std::cout << "Allocation failed!" << std::endl;
                exit(EXIT_FAILURE);
            }
            void *new_ptr = static_cast<char *>(mem) + size;
            size += n;
            return new_ptr;
        }
    
    private:
        void *mem = nullptr;
        size_t size = 0;
};

template <class T>
class CustomAllocator {
    public:    
        typedef T value_type;
        
        CustomAllocator(MemoryPool *memory_pool) {
            this->memory_pool = memory_pool;
        }
        
        T *allocate(std::size_t n) {
            std::cout << "Requesting " << n << "*" << sizeof(T) << "="
                 << (n*sizeof(T)) << " bytes." << std::endl;
            if (n > std::size_t(-1) / sizeof(T)) {
                throw std::bad_alloc();
            }
            
            auto memory = memory_pool->get_memory(n * sizeof(T));
            return static_cast<T *>(memory);
    
            throw std::bad_alloc();
        }
        
        void deallocate(T*, std::size_t) noexcept {
            // Not implemented
        }
        
    private:
        MemoryPool *memory_pool;    
};

typedef CustomAllocator<std::string> StringAllocator;

typedef std::vector<std::string, StringAllocator> StringVector;

int main() {
    
    MemoryPool memory_pool;
    
    std::cout << "Requesting memory for Allocator." << std::endl;
    auto allocator_memory = memory_pool.get_memory(sizeof(StringAllocator));
    
    auto string_allocator = new(allocator_memory) StringAllocator(&memory_pool);
    
    std::cout << "Requesting memory for std::vector." << std::endl;
    auto vector_memory = memory_pool.get_memory(sizeof(StringVector));
    
    auto vec = new(vector_memory) StringVector(*string_allocator);
    
    for (auto str : {"This", "is", "a", "test", "for", "a", "std::vector",
                     "with", "a", "custom", "allocator"})
    {
        std::cout << "Adding string \"" << str << "\"" << std::endl;
        vec->push_back(str);
    }
    
    for (auto str : *vec) {
        std::cout << "Reading back string \"" << str << "\"" << std::endl;
    }
    
    vec->~StringVector();
    
    string_allocator->~StringAllocator();
    
    return 0;
}

