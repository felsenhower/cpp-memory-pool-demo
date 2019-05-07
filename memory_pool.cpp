#include <iostream>
#include <vector>
#include <cstdlib>
#include <new>

class MemoryPool {
    public:
        MemoryPool(size_t size) {
            this->size = size;
            this->mem = new char[size];
        }
    
        ~MemoryPool() {
            delete[] mem;
        }
        
        void *get_memory(size_t n) {
            size_t new_offset = offset + n;
            std::cout << "Current offset = " << offset << ", "
                      << "new offset = " << new_offset << std::endl;
            if (new_offset > size) {
                return nullptr;
            }
            char *new_ptr = mem + offset;
            offset += n;
            return new_ptr;
        }
    
    private:
        char *mem = nullptr;
        size_t size = 0;
        size_t offset = 0;
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
            void *memory = memory_pool->get_memory(n * sizeof(T));
            if (memory == nullptr) {
                throw std::bad_alloc();
            }
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
    
    MemoryPool memory_pool(4096);
    
    std::cout << "Requesting memory for Allocator." << std::endl;
    void *allocator_memory = memory_pool.get_memory(sizeof(StringAllocator));
    
    auto string_allocator = new(allocator_memory) StringAllocator(&memory_pool);
    
    std::cout << "Requesting memory for std::vector." << std::endl;
    void *vector_memory = memory_pool.get_memory(sizeof(StringVector));
    
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


