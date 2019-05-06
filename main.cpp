#include <iostream>
#include <vector>
#include <cstdlib>
#include <new>

struct MemoryPool {
    char *mem;
    size_t offset = 0;
    size_t size;
};

template <class T>
struct CustomAllocator {
    
    typedef T value_type;
    
    MemoryPool *memory_pool;    
    
    CustomAllocator(MemoryPool *memory_pool) {
        this->memory_pool = memory_pool;
    }
    
    T* allocate(std::size_t n) {
        std::cout << "Allocating " << n << "*" << sizeof(T) << "="
             << (n*sizeof(T)) << " bytes" << std::endl;
        if (n > std::size_t(-1) / sizeof(T)) {
            throw std::bad_alloc();
        }
        memory_pool->offset += size_t(n * sizeof(T));
        if (memory_pool->offset > memory_pool->size) {
            throw std::bad_alloc();
        }
        void *foo = memory_pool->mem + memory_pool->offset;
        T *p = static_cast<T*>(foo);
        return p;
        throw std::bad_alloc();
    }
  void deallocate(T*, std::size_t) noexcept {
      // Not implemented
  }
};

typedef CustomAllocator<std::string> StringAllocator;

typedef std::vector<std::string, StringAllocator> StringVector;

int main() {
    
    MemoryPool memory_pool;
    
    memory_pool.mem = new char[4096];
    
    memory_pool.size = 4096;
    
    memory_pool.offset += sizeof(StringVector);
    
    StringVector *vec = new(memory_pool.mem) StringVector((StringAllocator(&memory_pool)));
    
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
    
    delete[] memory_pool.mem;
    
    return 0;
}
