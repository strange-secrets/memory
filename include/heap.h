
#if !defined(NGEN_HEAP_HEADER_INCLUDED_NFACTORIAL)
#define NGEN_HEAP_HEADER_INCLUDED_NFACTORIAL

////////////////////////////////////////////////////////////////////////////

#include <cstddef>

#include "allocation_strategy.h"


////////////////////////////////////////////////////////////////////////////

namespace ngen {
    namespace memory {
        struct Allocation {
            size_t      size;
            size_t      line;
            size_t      blockSize;
            uintptr_t   allocation;
            bool        isArray;
            const char  *fileName;
            char        sentinel[4];
        };

        struct FreeBlock {
            size_t      size;
            FreeBlock   *nextBlock;
        };

        class Heap {
        public:
            Heap();
            ~Heap();

            bool initialize(void *memoryBlock, size_t blockSize);
            bool initialize(void *memoryBlock, size_t blockSize, kAllocationStrategy allocationStrategy);

            void* alloc(size_t dataLength);
            void* alignedAlloc(size_t dataLength, size_t alignment);

            void* alloc(size_t dataLength, const char *fileName, size_t line);
            void* alignedAlloc(size_t dataLength, size_t alignment, const char *fileName, size_t line);

            void* allocArray(size_t dataLength);
            void* alignedAllocArray(size_t dataLength, size_t alignment);

            void* allocArray(size_t dataLength, const char *fileName, size_t line);
            void* alignedAllocArray(size_t dataLength, size_t alignment, const char *fileName, size_t line);

            size_t getAllocations() const;
            size_t getTotalAllocations() const;
            kAllocationStrategy getAllocationStrategy() const;

        private:
            Allocation* consumeMemory(FreeBlock *freeBlock, size_t dataLength, size_t alignment);

            FreeBlock* findFreeBlock(size_t dataLength, size_t alignment) const;
            FreeBlock* findFreeBlock_first(size_t dataLength, size_t alignment) const;
            FreeBlock* findFreeBlock_smallest(size_t dataLength, size_t alignment) const;

            Allocation* allocate(size_t dataLength, size_t alignment, bool isArray, const char *fileName, size_t line);

        private:
            FreeBlock*  m_rootBlock;
            void*       m_memoryBlock;

            kAllocationStrategy m_allocationStrategy;

            size_t      m_heapLength;
            size_t      m_allocations;
            size_t      m_totalAllocations;
            size_t      m_failedAllocations;
        };

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

        //! \brief Retrieves the allocation strategy being used by this memory heap.
        //! \reutrns The allocation strategy being used by the mrmoty heap.
        inline kAllocationStrategy Heap::getAllocationStrategy() const {
            return m_allocationStrategy;
        }
    }
}

////////////////////////////////////////////////////////////////////////////

#endif //!defined(NGEN_HEAP_HEADER_INCLUDED_NFACTORIAL)
