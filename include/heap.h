
#if !defined(MEMORY_HEAP_HEADER_INCLUDED_STRANGE_SECRETS)
#define MEMORY_HEAP_HEADER_INCLUDED_STRANGE_SECRETS

////////////////////////////////////////////////////////////////////////////

#include <cstddef>
#include <cstdint>

#include "allocation_strategy.h"


////////////////////////////////////////////////////////////////////////////

namespace ngen::memory {
    class Heap;

    struct Allocation {
        Heap *heap;             // The heap from which we were allocated
        size_t size;            // Size (in bytes) of memory allocation
        size_t line;            // Line number that made the allocation (debug only)
        size_t blockSize;       // Total size (in bytes) of allocated memory block, including header and footer.
        size_t id;              // Global allocation identifier
        uintptr_t addr;         // Start address of allocation block
        bool isArray;           // True if allocation was made using array operator
        const char *fileName;   // Path to file that made the allocation (debug only)
        char sentinel[4];       // Bytes that are used to detect buffer over-runs of allocated data.
    };

    struct FreeBlock {
        size_t size;            // Total size of memory block (including the FreeBlock structure itself)
        FreeBlock *previous;    // Previous FreeBlock in linked list
        FreeBlock *next;        // Next FreeBlock in linked list
    };

    class Heap {
    public:
        Heap();
        ~Heap() = default;

        Heap(const Heap &other) = delete;
        Heap &operator=(const Heap &other) = delete;

        bool initialize(void *memoryBlock, size_t blockSize);
        bool initialize(void *memoryBlock, size_t blockSize, kAllocationStrategy allocationStrategy);

        [[nodiscard]] void* alloc(size_t dataLength);
        [[nodiscard]] void* alignedAlloc(size_t dataLength, size_t alignment);

        [[nodiscard]] void* alloc(size_t dataLength, const char *fileName, size_t line);
        [[nodiscard]] void* alignedAlloc(size_t dataLength, size_t alignment, const char *fileName, size_t line);

        [[nodiscard]] void* allocArray(size_t dataLength);
        [[nodiscard]] void* alignedAllocArray(size_t dataLength, size_t alignment);

        [[nodiscard]] void* allocArray(size_t dataLength, const char *fileName, size_t line);
        [[nodiscard]] void* alignedAllocArray(size_t dataLength, size_t alignment, const char *fileName, size_t line);

        bool deallocate(void *ptr, bool isArray, const char *fileName, size_t line);

        [[nodiscard]] size_t getSize() const;
        [[nodiscard]] size_t getAllocations() const;
        [[nodiscard]] size_t getTotalAllocations() const;
        [[nodiscard]] size_t getFailedAllocations() const;

        [[nodiscard]] kAllocationStrategy getAllocationStrategy() const;

    private:
        [[nodiscard]] FreeBlock* gatherMemory(FreeBlock *block);
        [[nodiscard]] Allocation* consumeMemory(FreeBlock *freeBlock, size_t dataLength, size_t alignment);

        void insertFreeBlock(FreeBlock *block);

        [[nodiscard]] FreeBlock* findFreeBlock(size_t dataLength, size_t alignment) const;
        [[nodiscard]] FreeBlock* findFreeBlock_first(size_t dataLength, size_t alignment) const;
        [[nodiscard]] FreeBlock* findFreeBlock_smallest(size_t dataLength, size_t alignment) const;

        [[nodiscard]] Allocation* allocate(size_t dataLength, size_t alignment, bool isArray, const char *fileName, size_t line);

    private:
        FreeBlock *m_rootBlock;
        void *m_memoryBlock;

        kAllocationStrategy m_allocationStrategy;

        size_t m_heapLength;
        size_t m_allocations;
        size_t m_totalAllocations;
        size_t m_failedAllocations;
    };

    //! \brief Retrieves the total size of the memory heap.
    //! \returns The size (in bytes) of the total memory pool managed by this Heap object.
    inline size_t Heap::getSize() const {
        return m_heapLength;
    }

    //! \brief Retrieves the number of allocations that are currently live within the heap.
    //! \returns The number of allocations currently still live within the heap.
    inline size_t Heap::getAllocations() const {
        return m_allocations;
    }

    //! \brief Retrieves the number of allocations made with this heap during the course of its lifetime.
    //! \returns The number of allocations made by using the heap during the course of its lifetime.
    inline size_t Heap::getTotalAllocations() const {
        return m_totalAllocations;
    }

    //! \brief Retrieves the number of allocation requests that have been requested but failed.
    //! \returns The number of allocation requests that have been failed by this heap.
    inline size_t Heap::getFailedAllocations() const {
        return m_failedAllocations;
    }

    //! \brief Retrieves the allocation strategy being used by this memory heap.
    //! \reutrns The allocation strategy being used by the mrmoty heap.
    inline kAllocationStrategy Heap::getAllocationStrategy() const {
        return m_allocationStrategy;
    }
}

////////////////////////////////////////////////////////////////////////////

#endif //!defined(MEMORY_HEAP_HEADER_INCLUDED_STRANGE_SECRETS)
