
#if !defined(MEMORY_HEADER_INCLUDED_STRANGE_SECRETS)
#define MEMORY_HEADER_INCLUDED_STRANGE_SECRETS

////////////////////////////////////////////////////////////////////////////

#include "heap.h"


////////////////////////////////////////////////////////////////////////////

#if defined(_DEBUG)
    #define NGEN_NEW(heap)                      new(heap, __FILE__, __LINE__)
    #define NGEN_ALIGNED_NEW(heap, alignment)   new(heap, alignment, __FILE__, __LINE__)
    #define NGEN_DELETE(heap)                   delete(heap, __FILE__, __LINE__)
#else
    #define NGEN_NEW(heap)                      new(heap)
    #define NGEN_ALIGNED_NEW(heap, alignment)   new(heap, alignment)
    #define NGEN_DELETE(heap)                   delete(heap)
#endif //defined(_DEBUG)


////////////////////////////////////////////////////////////////////////////

inline void* operator new(size_t count, ngen::memory::Heap *heap) {
    return heap->alloc(count);
}

inline void* operator new(size_t count, ngen::memory::Heap *heap, size_t alignment) {
    return heap->alignedAlloc(count, alignment);
}

inline void* operator new(size_t count, ngen::memory::Heap *heap, const char *fileName, size_t line) {
    return heap->alloc(count, fileName, line);
}

inline void* operator new(size_t count, ngen::memory::Heap *heap, size_t alignment, const char *fileName, size_t line) {
    return heap->alignedAlloc(count, alignment, fileName, line);
}

inline void* operator new[](size_t count, ngen::memory::Heap *heap) {
    return heap->allocArray(count);
}

inline void* operator new[](size_t count, ngen::memory::Heap *heap, size_t alignment) {
    return heap->alignedAllocArray(count, alignment);
}

inline void* operator new[](size_t count, ngen::memory::Heap *heap, const char *fileName, size_t line) {
    return heap->allocArray(count, fileName, line);
}

inline void* operator new[](size_t count, ngen::memory::Heap *heap, size_t alignment, const char *fileName, size_t line) {
    return heap->alignedAllocArray(count, alignment, fileName, line);
}

inline void operator delete(void *ptr, ngen::memory::Heap *heap) {
    heap->deallocate(ptr, false, nullptr, 0);
}

inline void operator delete(void *ptr, ngen::memory::Heap *heap, const char *fileName, size_t line) {
    heap->deallocate(ptr, false, fileName, line);
}

inline void operator delete[](void *ptr, ngen::memory::Heap *heap) {
    heap->deallocate(ptr, true, nullptr, 0);
}

inline void operator delete[](void *ptr, ngen::memory::Heap *heap, const char *fileName, size_t line) {
    heap->deallocate(ptr, true, fileName, line);
}


////////////////////////////////////////////////////////////////////////////

#endif //!defined(MEMORY_HEADER_INCLUDED_STRANGE_SECRETS)
