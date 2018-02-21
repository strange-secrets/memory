
#include <cstdint>
#include <cassert>
#include <cstring>
#include <atomic>

#include "heap.h"

namespace {
    std::atomic<size_t> allocationId;

    const size_t DEFAULT_ALIGNMENT = 4;
    const size_t MAXIMUM_ALIGNMENT = 128;

    const ngen::memory::kAllocationStrategy DEFAULT_ALLOCATION_STRATEGY = ngen::memory::kAllocationStrategy_First;

    const char* kHeaderSentinelData = "ALOC";
    const char* kFooterSentinelData = "COLA";

    //! \brief Simple helper function to determine whether or not a value is a power of 2.
    //! \param value [in] - The number to check if it is a valid power of 2.
    //! \returns True if the supplied value is a power of 2 otherwise returns false.
    inline bool isPow2(size_t value) {
        return (0 != value && (value & (value -1)) == 0);
    }

    //! \brief Given a pointer address, this method returns the next valid address that is aligned with the specified size.
    //! If the pointer is already aligned, it is returned unchanged.
    //! \param ptr [in] The pointer address to be aligned.
    //! \param alignment [in] The desired byte alignment of the pointer.
    //! \return The pointer address aligned to the specified alignment.
    template <typename TType> inline TType alignValue(TType value, size_t alignment) {
        return (value + alignment - 1) / alignment * alignment;
    }

    bool validateSentinel(ngen::memory::Allocation *allocation) {
        return (   allocation->sentinel[0] == kHeaderSentinelData[0]
                && allocation->sentinel[1] == kHeaderSentinelData[1]
                && allocation->sentinel[2] == kHeaderSentinelData[2]
                && allocation->sentinel[3] == kHeaderSentinelData[3]);
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

            // TODO: Validate 'memoryBlock' is of a suitable alignment.
            m_rootBlock = static_cast<FreeBlock*>(memoryBlock);
            m_rootBlock->size = blockSize;
            m_rootBlock->next = nullptr;
            m_rootBlock->previous = nullptr;

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

            if (isPow2(alignment)) {
                if (alignment <= MAXIMUM_ALIGNMENT) {
                    // NOTE: We align the dataLength value when obtaining a memory block to ensure the
                    // end of the memory block is at a suitable location for a new FreeBlock instance to exist.
                    const auto allocationLength = alignValue(dataLength, sizeof(FreeBlock));

                    FreeBlock *freeBlock = findFreeBlock(allocationLength, alignment);

                    if (freeBlock) {
                        Allocation *alloc = consumeMemory(freeBlock, allocationLength, alignment);
                        if (alloc) {
                            alloc->id = allocationId++;
                            alloc->size = dataLength;
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

        //! \brief Releases a memory block previously allocated by this object.
        //! \param ptr [in] - Pointer to the memory block to be released
        //! \param isArray [in] - True if the deallocation came from an array delete operator otherwise false.
        //! \param fileName [in] - The path of the source file that made the deallocation, this may be null.
        //! \param line [in] - The line number within the source file where the deallocation was requested.
        bool Heap::deallocate(void *ptr, bool isArray, const char *fileName, size_t line) {
            // We treat an attempt to free a nullptr as always successful.
            if (ptr) {
                const auto lowerMemoryBoundary = reinterpret_cast<uintptr_t>(m_memoryBlock);
                const auto upperMemoryBoundary = lowerMemoryBoundary + m_heapLength;

                const auto start = reinterpret_cast<uintptr_t>(ptr);
                auto allocation = reinterpret_cast<Allocation*>(start - sizeof(Allocation));

                const auto blockStart = allocation->addr;
                const auto blockSize = allocation->blockSize;
                const auto blockEnd = blockStart + blockSize;

                if (blockStart < lowerMemoryBoundary || blockStart > upperMemoryBoundary) {
                    // TODO: Log ERR allocation outside valid bounds
                    return false;
                }

                if (blockEnd < lowerMemoryBoundary || blockEnd > upperMemoryBoundary) {
                    // TODO: Log ERR allocation span outside valid bounds
                    return false;
                }

                if (!validateSentinel(allocation)) {
                    // TODO: LOG ERR, corrupt memory allocation
                }

                if (allocation->isArray != isArray) {
                    // TODO: Log ERR - array mismatch
                    return false;
                }

                memset(allocation, 0, sizeof(Allocation));

                auto freeBlock = reinterpret_cast<FreeBlock*>(blockStart);
                freeBlock->size = blockSize;
                freeBlock->previous = nullptr;
                freeBlock->next = nullptr;

                insertFreeBlock(freeBlock);

                auto gatheredBlock = gatherMemory(freeBlock);
                // TODO: In debug builds clear memory block 'freeBlock' with some suitable value

                m_allocations--;
            }

            return true;
        }

        //! \brief Given a FreeBlock instance, this method attempts to insert it into our linked list at the appropriate position.
        //! \param block [in] - The FreeBlock instance to be inserted into our linked list.
        void Heap::insertFreeBlock(FreeBlock *block) {
            assert(nullptr != block);
            assert(nullptr == block->previous);
            assert(nullptr == block->next);

            for (auto search = m_rootBlock; search; search = search->next) {
                if (block < search) {
                    block->next = search;
                    block->previous = search->previous;

                    if (search->previous) {
                        search->previous->next = block;
                    } else {
                        m_rootBlock = block;
                    }

                    search->previous = block;
                    return;
                }

                if (!search->next) {
                    // Special case for the tail of our linked list
                    block->previous = search;
                    search->next = block;
                    return;
                }
            }

            m_rootBlock = block;
        }

        //! \brief Given a FreeBlock within our allocator, this method attempts to join it with other consecutive blocks.
        //! \param block [in] - The FreeBlock we should attempt to join.
        //! \returns The FreeBlock instance that contains the gathered memory.
        FreeBlock* Heap::gatherMemory(FreeBlock *block) {
            assert(nullptr != block);

            const auto blockStart = reinterpret_cast<uintptr_t>(block);
            const auto blockEnd = blockStart + block->size;

            if (block->next) {
                const auto nextStart = reinterpret_cast<uintptr_t>(block->next);

                if (blockEnd == nextStart) {
                    block->size += block->next->size;
                    block->next = block->next->next;

                    if (block->next) {
                        block->next->previous = block;
                    }
                }
            }

            if (block->previous) {
                const auto previousStart = reinterpret_cast<uintptr_t>(block->previous);
                const auto previousEnd = previousStart + block->previous->size;

                if (previousEnd == blockStart) {
                    block->previous->size += block->size;
                    block->previous->next = block->next;

                    if (block->next) {
                        block->next->previous = block->previous;
                    }

                    block = block->previous;
                }
            }

            return block;
        }

        //! \brief Consumes an amount of memory from the specified FreeBlock.
        //! \param freeBlock [in] - The memory block we are to consume.
        //! \param dataLength [in] - The number of bytes to be consumed.
        //! \param alignment [in] - The alignment the allocated memory block must have.
        Allocation* Heap::consumeMemory(FreeBlock *freeBlock, size_t dataLength, size_t alignment) {
            //NGEN_ASSERT(freeBlock, "Heap.comsumeMemory - freeBlock was null.");

            const auto rawPtr = reinterpret_cast<uintptr_t>(freeBlock);
            const auto endPtr = rawPtr + freeBlock->size;

            uintptr_t alignedPtr = alignValue(rawPtr + sizeof(Allocation), alignment);
            uintptr_t headerSize = alignedPtr - rawPtr;

            size_t blockLength = headerSize + dataLength;
            size_t remaining = endPtr - (alignedPtr + dataLength);

            // If there isn't enough memory remaining to warrant creating a new free block, then
            // include it inside the allocation.
            if (remaining <= sizeof(Allocation)) {
                blockLength += remaining;
                remaining = 0;
            }

            auto alloc = reinterpret_cast<Allocation*>(alignedPtr - sizeof(Allocation));

            // Insert a new FreeBlock into the memory pool, if there is space remaining.
            if (remaining) {
                assert(remaining >= sizeof(FreeBlock));

                // Create a new FreeBlock from the remaining space
                auto remainingBlock = reinterpret_cast<FreeBlock*>(alignedPtr + dataLength);

                remainingBlock->size = remaining;
                remainingBlock->previous = freeBlock->previous;
                remainingBlock->next = freeBlock->next;

                // TODO: The below code feels a little clunky, maybe refactor in the future
                if (freeBlock->previous) {
                    freeBlock->previous->next = remainingBlock;
                } else {
                    m_rootBlock = remainingBlock;
                }

                if (freeBlock->next) {
                    freeBlock->next->previous = remainingBlock;
                }
            } else {
                if (freeBlock->previous) {
                    freeBlock->previous->next = nullptr;
                } else {
                    m_rootBlock = nullptr;
                }
            }

            alloc->addr = rawPtr;
            alloc->blockSize = blockLength;
            alloc->sentinel[0] = kHeaderSentinelData[0];
            alloc->sentinel[1] = kHeaderSentinelData[1];
            alloc->sentinel[2] = kHeaderSentinelData[2];
            alloc->sentinel[3] = kHeaderSentinelData[3];

            // TODO: Also need footer sentinel after memory block

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

            for (FreeBlock *search = m_rootBlock; search; search = search->next) {
                if (dataLength <= search->size) {
                    const auto rawPtr = reinterpret_cast<uintptr_t>(search);
                    const auto endPtr = rawPtr + search->size;

                    uintptr_t alignedPtr = alignValue(rawPtr + sizeof(Allocation), alignment);
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
            for (FreeBlock *search = m_rootBlock; search; search = search->next) {
                if (dataLength <= search->size) {
                    const auto rawPtr = reinterpret_cast<uintptr_t>(search);
                    const auto endPtr = rawPtr + search->size;

                    uintptr_t alignedPtr = alignValue(rawPtr + sizeof(Allocation), alignment);
                    if (alignedPtr > rawPtr && alignedPtr < endPtr && (endPtr - alignedPtr) >= dataLength) {
                        return search;
                    }
                }
            }

            return nullptr;
        }
    }
}
