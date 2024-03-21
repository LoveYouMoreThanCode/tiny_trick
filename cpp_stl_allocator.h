#include <memory>

template <typename T> class MyAllocator : public std::allocator<T> {
  public:
    template <typename U> struct rebind {
        typedef MyAllocator<U> other;
    };

    template <typename U> MyAllocator(const MyAllocator<U>& alloc) {}

    MyAllocator() throw(){};

    // \brief copy ctor
    MyAllocator(const MyAllocator&) throw(){};

  public:
    T* allocate(size_t n, const void* hint = 0) {
        T* res = (T*)malloc(n * sizeof(T));
        printf("allocate(%lu, %p) T_size:%lu\n", n * sizeof(T), res, sizeof(T));
        return res;
    }

    void deallocate(T* ptr, size_t n) {
        printf("deallocate(%p, %lu) T_size:%lu\n", ptr, n * sizeof(T), sizeof(T));
        free(ptr);
    }
};
