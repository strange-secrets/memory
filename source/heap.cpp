
#include "heap.h"

namespace {
    const size_t DEFAULT_ALIGNMENT = 4;
    const size_t MAXIMUM_ALIGNMENT = 32;

    const ngen::memory::kAllocationStrategy DEFAULT_ALLOCATION_STRATEGY = ngen::memory::kAllocationStrategy_First;
}

namespace ngen {
    namespace memory {
        Heap::Heap()
        : m_rootBlock(nullptr)
        , m_memoryBlock(nullptr)
        , m_allocationStrategy(kAllocationStrategy_Invalid)
        , m_heapLength(0)
        , m_allocations(0)
        , m_totalAllocations(0)
        , m_failedAllocations(0)
        {

        }

        Heap::~Heap() {

        }

        //! \brief Prepares the memory heap for use by the application, using the default allocation strategy.
        //! \param memoryBlock [in] - Pointer to the raw memory block to be used by this heap object.
        //! \param blockSize [in] - Length (in bytes) of the memory block pointed to by the memoryBlock parameter.
        //! \returns True if the heap was initialized successfully otherwise false.
        bool Heap::initialize(void *memoryBlock, size_t blockSize) {
            return initialize(memoryBlock, blockSize, DEFAULT_ALLOCATION_STRATEGY);
        }

        //! \brief Prepares the memory heap for use by the application.
        //! \param memoryBlock [in] - Pointer to the raw memory block to be used by this heap object.
        //! \param blockSize [in] - Length (in bytes) of the memory block pointed to by the memoryBlock parameter.
        //! \param allocationStrategy [in] - The allocation strategy to be used by this heap.
        //! \returns True if the heap was initialized successfully otherwise false.
        bool Heap::initialize(void *memoryBlock, size_t blockSize, kAllocationStrategy allocationStrategy) {
            if (m_memoryBlock) {
                return false;
            }

            if (!memoryBlock) {
                return false;
            }

            if (!blockSize) {
                return false;
            }

            if (allocationStrategy == kAllocationStrategy_Invalid) {
                return false;
            }

            m_heapLength = blockSize;
            m_memoryBlock = memoryBlock;
            m_allocationStrategy = allocationStrategy;
            return true;
        }

        //! \brief  Allocates a block of memory of a specified length.
        //! \param  dataLength [in] -
        //!         Length (in bytes) of the memory block to be allocated.
        //! \return Pointer to a memory block at least of the specified length or null if it could not be allocated.
        void* Heap::alloc(size_t dataLength) {
            Allocation *memory = allocate(dataLength, DEFAULT_ALIGNMENT, nullptr, 0);
            if (memory) {

            }

            return nullptr;
        }

        //! \brief  Allocates a block of memory of a specified length.
        //! \param  dataLength [in] -
        //!         Length (in bytes) of the memory block to be allocated.
        //! \param  alignment [in] -
        //!         Specifies the alignment of the allocated memory block. Must be a power of two.
        //! \return Pointer to a memory block at least of the specified length or null if it could not be allocated.
        void* Heap::alignedAlloc(size_t dataLength, size_t alignment) {
            Allocation *memory = allocate(dataLength, alignment, nullptr, 0);
            if (memory) {

            }

            return nullptr;
        }

        //! \brief  Allocates a block of memory of a specified length.
        //! \param  dataLength [in] -
        //!         Length (in bytes) of the memory block to be allocated.
        //! \param  fileName [in] -
        //!         Pointer to string containing the path of the file where the allocation took place.
        //! \param  line [in] -
        //!         The line number within the source file where the allocation took place.
        //! \return Pointer to a memory block at least of the specified length or null if it could not be allocated.
        void* Heap::alloc(size_t dataLength, const char *fileName, size_t line) {
            Allocation *memory = allocate(dataLength, DEFAULT_ALIGNMENT, fileName, line);
            if (memory) {

            }

            return nullptr;
        }

        //! \brief  Allocates a block of memory of a specified length.
        //! \param  dataLength [in] -
        //!         Length (in bytes) of the memory block to be allocated.
        //! \param  alignment [in] -
        //!         Specifies the alignment of the allocated memory block. Must be a power of two.
        //! \param  fileName [in] -
        //!         Pointer to string containing the path of the file where the allocation took place.
        //! \param  line [in] -
        //!         The line number within the source file where the allocation took place.
        //! \return Pointer to a memory block at least of the specified length or null if it could not be allocated.
        void* Heap::alignedAlloc(size_t dataLength, size_t alignment, const char *fileName, size_t line) {
            Allocation *memory = allocate(dataLength, alignment, fileName, line);
            if (memory) {

            }

            return nullptr;
        }

        //! \brief  Allocates a block of memory of a specified length.
        //! \param  dataLength [in] -
        //!         Length (in bytes) of the memory block to be allocated.
        //! \return Pointer to a memory block at least of the specified length or null if it could not be allocated.
        void* Heap::allocArray(size_t dataLength) {
            Allocation *memory = allocate(dataLength, DEFAULT_ALIGNMENT, nullptr, 0);
            if (memory) {
                memory->isArray = true;
            }

            return nullptr;
        }

        //! \brief  Allocates a block of memory of a specified length.
        //! \param  dataLength [in] -
        //!         Length (in bytes) of the memory block to be allocated.
        //! \param  alignment [in] -
        //!         Specifies the alignment of the allocated memory block. Must be a power of two.
        //! \return Pointer to a memory block at least of the specified length or null if it could not be allocated.
        void* Heap::alignedAllocArray(size_t dataLength, size_t alignment) {
            Allocation *memory = allocate(dataLength, alignment, nullptr, 0);
            if (memory) {
                memory->isArray = true;
            }

            return nullptr;
        }

        //! \brief  Allocates a block of memory of a specified length.
        //! \param  dataLength [in] -
        //!         Length (in bytes) of the memory block to be allocated.
        //! \param  fileName [in] -
        //!         Pointer to string containing the path of the file where the allocation took place.
        //! \param  line [in] -
        //!         The line number within the source file where the allocation took place.
        //! \return Pointer to a memory block at least of the specified length or null if it could not be allocated.
        void* Heap::allocArray(size_t dataLength, const char *fileName, size_t line) {
            Allocation *memory = allocate(dataLength, DEFAULT_ALIGNMENT, fileName, line);
            if (memory) {
                memory->isArray = true;
            }

            return nullptr;
        }

        //! \brief  Allocates a block of memory of a specified length.
        //! \param  dataLength [in] -
        //!         Length (in bytes) of the memory block to be allocated.
        //! \param  alignment [in] -
        //!         Specifies the alignment of the allocated memory block. Must be a power of two.
        //! \param  fileName [in] -
        //!         Pointer to string containing the path of the file where the allocation took place.
        //! \param  line [in] -
        //!         The line number within the source file where the allocation took place.
        //! \return Pointer to a memory block at least of the specified length or null if it could not be allocated.
        void* Heap::alignedAllocArray(size_t dataLength, size_t alignment, const char *fileName, size_t line) {
            Allocation *memory = allocate(dataLength, alignment, fileName, line);
            if (memory) {
                memory->isArray = true;
            }

            return nullptr;
        }

        //! \brief Attempts to allocate a block of memory with a specified size and alignment.
        //! \param dataLength [in] - The length (in bytes) of the memory block to be allocated.
        //! \param alignment [in] - The alignment (in bytes) of the memory block to be allocated.
        //! \param fileName [in] - The path of the source file that made the allocation, this may be null.
        //! \param line [in] - The line number within the source file where the allocation was requested.
        //! \returns Pointer to the allocated memory block or nullptr if the allocation could not be made.
        Allocation* Heap::allocate(size_t dataLength, size_t alignment, const char *fileName, size_t line) {
            // TODO: Validate that alignment is a power of 2.

            if (alignment <= MAXIMUM_ALIGNMENT) {
                FreeBlock *block = findFreeBlock(dataLength, alignment);

                if (block) {
                    // TODO: Consume memory from block
                }
            } else {
                // TODO: Log WARN: Large alignment of {alignment} was requested.
            }

            return nullptr;
        }

        //! \brief Searches the available free memory blocks for an appropriate block to be used for the described allocation.
        //! \param dataLength [in] - The length (in bytes) of the memory block that has requested allocation.
        //! \param alignment [in] - The alignment (in bytes) the memory allocation requires.
        //! \returns Pointer to the FreeBlock that can successfully allocate the described memory block.
        FreeBlock* Heap::findFreeBlock(size_t dataLength, size_t alignment) const {
            switch (m_allocationStrategy) {
                case kAllocationStrategy_First:
                    return findFreeBlock_first(dataLength, alignment);

                case kAllocationStrategy_Smallest:
                    return findFreeBlock_smallest(dataLength, alignment);

                default:
                    // TODO: Log error - Unknown allocation strategy
                    break;
            }
        }

        //! \brief Searches the available free memory blocks for an appropriate block to be used for the described allocation, chooses the smallest free block available.
        //! \param dataLength [in] - The length (in bytes) of the memory block that has requested allocation.
        //! \param alignment [in] - The alignment (in bytes) the memory allocation requires.
        //! \returns Pointer to the FreeBlock that can successfully allocate the described memory block.
        FreeBlock* Heap::findFreeBlock_smallest(size_t dataLength, size_t alignment) const {
            FreeBlock *selected = nullptr;

            for (FreeBlock *search = m_rootBlock; search; search = search->nextBlock) {
                const auto rawPtr = reinterpret_cast<uintptr_t>(search);
                const uintptr_t alignedPtr = (rawPtr + alignment - 1) / alignment * alignment;
                const uintptr_t physicalSpace = alignedPtr - rawPtr + dataLength;

                // TODO: Take into account the allocation header also

                if (search->size >= physicalSpace) {
                    if (!selected || search->size < selected->size) {
                        selected = search;
                    }
                }
            }

            return selected;
        }

        //! \brief Searches the available free memory blocks for an appropriate block to be used for the described allocation.
        //! \param dataLength [in] - The length (in bytes) of the memory block that has requested allocation.
        //! \param alignment [in] - The alignment (in bytes) the memory allocation requires.
        //! \returns Pointer to the FreeBlock that can successfully allocate the described memory block.
        FreeBlock* Heap::findFreeBlock_first(size_t dataLength, size_t alignment) const {
            for (FreeBlock *search = m_rootBlock; search; search = search->nextBlock) {
                const auto rawPtr = reinterpret_cast<uintptr_t>(search);
                const uintptr_t alignedPtr = (rawPtr + alignment - 1) / alignment * alignment;
                const uintptr_t physicalSpace = alignedPtr - rawPtr + dataLength;

                // TODO: Take into account the allocation header also

                if (search->size >= physicalSpace) {
                    return search;
                }
            }

            return nullptr;
        }
    }
}
