#include <iostream>
#include <vector>
#include <cstdlib>
#include <new>

using namespace std;

static struct {
    char *mem;
    size_t offset = 0;
    size_t size;
} memory_pool;

template <class T>
struct Mallocator {
    
    typedef T value_type;
    
    Mallocator() = default;
    
    template <class U> constexpr Mallocator(const Mallocator<U>&) noexcept {}
    
    T* allocate(std::size_t n) {
        cout << "Allocating " << n << "*" << sizeof(T) << "="
             << (n*sizeof(T)) << " bytes" << std::endl;
        if (n > std::size_t(-1) / sizeof(T)) {
            throw std::bad_alloc();
        }
        memory_pool.offset += size_t(n * sizeof(T));
        if (memory_pool.offset > memory_pool.size) {
            throw std::bad_alloc();
        }
        void *foo = memory_pool.mem + memory_pool.offset;
        T *p = static_cast<T*>(foo);
        return p;
        throw std::bad_alloc();
    }
    
  void deallocate(T*, std::size_t) noexcept {
      // Not implemented
  }
  
};

template <class T, class U>
bool operator==(const Mallocator<T>&, const Mallocator<U>&) { return true; }

template <class T, class U>
bool operator!=(const Mallocator<T>&, const Mallocator<U>&) { return false; }


typedef std::vector<std::string,Mallocator<string>> stringvec;


int main() {
    
    memory_pool.mem = new char[4096];
    
    memory_pool.size = 4096;
    
    memory_pool.offset += sizeof(stringvec);
    
    stringvec *vec = new(memory_pool.mem) stringvec();
    
    for (auto str : {"This", "is", "a", "test", "for", "a", "std::vector",
                     "with", "a", "custom", "allocator"})
    {
        cout << "Adding string \"" << str << "\"" << endl;
        vec->push_back(str);
    }
    
    for (auto str : *vec) {
        cout << "Reading back string \"" << str << "\"" << endl;
    }
    
    vec->~stringvec();
    
    delete[] memory_pool.mem;
    
    return 0;
}
