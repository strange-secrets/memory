
#include <cstdint>
#include "heap.h"

namespace {
    const size_t DEFAULT_ALIGNMENT = 4;
    const size_t MAXIMUM_ALIGNMENT = 32;

    const ngen::memory::kAllocationStrategy DEFAULT_ALLOCATION_STRATEGY = ngen::memory::kAllocationStrategy_First;

    const char* kHeaderSentinelData = "ALOC";
    const char* kFooterSentinelData = "COLA";

    //! \brief Simple helper function to determine whether or not a value is a power of 2.
    //! \param value [in] - The number to check if it is a valid power of 2.
    //! \returns True if the supplied value is a power of 2 otherwise returns false.
    bool isPow2(size_t value) {
        return (0 != value && (value & (value -1)) == 0);
    }
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
            Allocation *memory = allocate(dataLength, DEFAULT_ALIGNMENT, false, nullptr, 0);
            return memory ? &memory[1] : nullptr;
        }

        //! \brief  Allocates a block of memory of a specified length.
        //! \param  dataLength [in] -
        //!         Length (in bytes) of the memory block to be allocated.
        //! \param  alignment [in] -
        //!         Specifies the alignment of the allocated memory block. Must be a power of two.
        //! \return Pointer to a memory block at least of the specified length or null if it could not be allocated.
        void* Heap::alignedAlloc(size_t dataLength, size_t alignment) {
            Allocation *memory = allocate(dataLength, alignment, false, nullptr, 0);
            return memory ? &memory[1] : nullptr;
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
            Allocation *memory = allocate(dataLength, DEFAULT_ALIGNMENT, false, fileName, line);
            return memory ? &memory[1] : nullptr;
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
            Allocation *memory = allocate(dataLength, alignment, false, fileName, line);
            return memory ? &memory[1] : nullptr;
        }

        //! \brief  Allocates a block of memory of a specified length.
        //! \param  dataLength [in] -
        //!         Length (in bytes) of the memory block to be allocated.
        //! \return Pointer to a memory block at least of the specified length or null if it could not be allocated.
        void* Heap::allocArray(size_t dataLength) {
            Allocation *memory = allocate(dataLength, DEFAULT_ALIGNMENT, true, nullptr, 0);
            return memory ? &memory[1] : nullptr;
        }

        //! \brief  Allocates a block of memory of a specified length.
        //! \param  dataLength [in] -
        //!         Length (in bytes) of the memory block to be allocated.
        //! \param  alignment [in] -
        //!         Specifies the alignment of the allocated memory block. Must be a power of two.
        //! \return Pointer to a memory block at least of the specified length or null if it could not be allocated.
        void* Heap::alignedAllocArray(size_t dataLength, size_t alignment) {
            Allocation *memory = allocate(dataLength, alignment, true, nullptr, 0);
            return memory ? &memory[1] : nullptr;
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
            Allocation *memory = allocate(dataLength, DEFAULT_ALIGNMENT, true, fileName, line);
            return memory ? &memory[1] : nullptr;
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
            Allocation *memory = allocate(dataLength, alignment, true, fileName, line);
            return memory ? &memory[1] : nullptr;
        }

        //! \brief Attempts to allocate a block of memory with a specified size and alignment.
        //! \param dataLength [in] - The length (in bytes) of the memory block to be allocated.
        //! \param alignment [in] - The alignment (in bytes) of the memory block to be allocated.
        //! \param isArray [in] - True if the allocation is an array otherwise false.
        //! \param fileName [in] - The path of the source file that made the allocation, this may be null.
        //! \param line [in] - The line number within the source file where the allocation was requested.
        //! \returns Pointer to the allocated memory block or nullptr if the allocation could not be made.
        Allocation* Heap::allocate(size_t dataLength, size_t alignment, bool isArray, const char *fileName, size_t line) {
            if (alignment < DEFAULT_ALIGNMENT) {
                alignment = DEFAULT_ALIGNMENT;
            }

            // TODO: Validate that alignment is a power of 2.
            if (isPow2(alignment)) {
                if (alignment <= MAXIMUM_ALIGNMENT) {
                    FreeBlock *freeBlock = findFreeBlock(dataLength, alignment);

                    if (freeBlock) {
                        Allocation *alloc = consumeMemory(freeBlock, dataLength, alignment);
                        if (alloc) {
                            alloc->isArray = isArray;
                            alloc->fileName = fileName;
                            alloc->line = line;

                            m_allocations++;
                            m_totalAllocations++;

                            return alloc;
                        }
                    }
                } else {
                    // TODO: Log ERR: Large alignment of {alignment} was requested.
                    // TODO: We use quite a simple strategy to guarantee the alignment, so large alignments can cause a waste of memory
                    // TODO: We can account for large alignments in the future.
                }
            } else {
                // TODO: Log ERR: alignment was not a power of 2
            }

            m_failedAllocations++;
            return nullptr;
        }

        //! \brief Consumes an amount of memory from the specified FreeBlock.
        //! \param freeBlock [in] - The memory block we are to consume.
        //! \param dataLength [in] - The number of bytes to be consumed.
        //! \param alignment [in] - The alignment the allocated memory block must have.
        Allocation* Heap::consumeMemory(FreeBlock *freeBlock, size_t dataLength, size_t alignment) {
            //NGEN_ASSERT(freeBlock, "Heap.comsumeMemory - freeBlock was null.");

            const auto rawPtr = reinterpret_cast<uintptr_t>(freeBlock);
            const auto endPtr = rawPtr + freeBlock->size;

            uintptr_t alignedPtr = (rawPtr + sizeof(Allocation) + alignment - 1) / alignment * alignment;
            uintptr_t headerSize = alignedPtr - rawPtr;

            size_t blockLength = headerSize + dataLength;
            size_t remaining = endPtr - (alignedPtr + dataLength);

            // If there isn't enough memory remaining to warrant creating a new free block, then
            // include it inside the allocation.
            if (remaining <= sizeof(Allocation)) {
                blockLength += remaining;
                remaining = 0;
            }

            auto *alloc = reinterpret_cast<Allocation*>(alignedPtr - sizeof(Allocation));

            alloc->allocation = rawPtr;
            alloc->blockSize = blockLength;
            alloc->sentinel[0] = kHeaderSentinelData[0];
            alloc->sentinel[1] = kHeaderSentinelData[1];
            alloc->sentinel[2] = kHeaderSentinelData[2];
            alloc->sentinel[3] = kHeaderSentinelData[3];

            // TODO: Also Need footer sentinel after memory block
            // TODO: Create a new FreeBlock from the remaining space in the free block

            return alloc;
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

            return nullptr;
        }

        //! \brief Searches the available free memory blocks for an appropriate block to be used for the described allocation, chooses the smallest free block available.
        //! \param dataLength [in] - The length (in bytes) of the memory block that has requested allocation.
        //! \param alignment [in] - The alignment (in bytes) the memory allocation requires.
        //! \returns Pointer to the FreeBlock that can successfully allocate the described memory block.
        FreeBlock* Heap::findFreeBlock_smallest(size_t dataLength, size_t alignment) const {
            FreeBlock *selected = nullptr;

            for (FreeBlock *search = m_rootBlock; search; search = search->nextBlock) {
                if (dataLength <= search->size) {
                    const auto rawPtr = reinterpret_cast<uintptr_t>(search);
                    const auto endPtr = rawPtr + search->size;

                    uintptr_t alignedPtr = (rawPtr + sizeof(Allocation) + alignment - 1) / alignment * alignment;
                    if (alignedPtr >= rawPtr && alignedPtr < endPtr && (endPtr - alignedPtr) >= dataLength) {
                        if (!selected || search->size < selected->size) {
                            selected = search;
                        }
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
                if (dataLength <= search->size) {
                    const auto rawPtr = reinterpret_cast<uintptr_t>(search);
                    const auto endPtr = rawPtr + search->size;

                    uintptr_t alignedPtr = (rawPtr + sizeof(Allocation) + alignment - 1) / alignment * alignment;
                    if (alignedPtr > rawPtr && alignedPtr < endPtr && (endPtr - alignedPtr) >= dataLength) {
                        return search;
                    }
                }
            }

            return nullptr;
        }
    }
}
