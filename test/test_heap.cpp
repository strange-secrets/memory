
#include <memory>
#include "heap.h"
#include "gtest/gtest.h"

const size_t kTestAllocationBufferSize = 1024;
const size_t kInvalidAllocationBufferSize = 0;

namespace {
    //! \brief  Helper method that determines whether or not the specified pointer has the specified alignment.
    //! \param ptr [in] - The pointer whose alignment is to be verified.
    //! \param alignment [in] - The alignment the pointer is expected to have *must* be a power of two (this is not verified).
    //! \returns True if the pointer has the specified alignment otherwise false.
    bool validateAlignment(void *ptr, size_t alignment) {
        const auto raw = reinterpret_cast<uintptr_t>(ptr);
        return (0 == (raw & (alignment - 1)));
    }
}

TEST(Heap, Construction) {
    ngen::memory::Heap heap;

    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());
}

TEST(Heap, FailedInitialization) {
    std::unique_ptr<char[]> allocationBuffer(new char[kTestAllocationBufferSize]);

    ngen::memory::Heap heap;

    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());
    EXPECT_EQ(ngen::memory::kAllocationStrategy::Invalid, heap.getAllocationStrategy());

    EXPECT_FALSE(heap.initialize(nullptr, kInvalidAllocationBufferSize, ngen::memory::kAllocationStrategy::First));
    EXPECT_FALSE(heap.initialize(nullptr, kTestAllocationBufferSize, ngen::memory::kAllocationStrategy::First));
    EXPECT_FALSE(heap.initialize(allocationBuffer.get(), kInvalidAllocationBufferSize, ngen::memory::kAllocationStrategy::First));

    EXPECT_FALSE(heap.initialize(nullptr, kInvalidAllocationBufferSize, ngen::memory::kAllocationStrategy::Smallest));
    EXPECT_FALSE(heap.initialize(nullptr, kTestAllocationBufferSize, ngen::memory::kAllocationStrategy::Smallest));
    EXPECT_FALSE(heap.initialize(allocationBuffer.get(), kInvalidAllocationBufferSize, ngen::memory::kAllocationStrategy::Smallest));

    // Make sure these variables haven't changed during the above calls.
    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());
    EXPECT_EQ(ngen::memory::kAllocationStrategy::Invalid, heap.getAllocationStrategy());
}

TEST(Heap, Initialize_Invalid) {
    std::unique_ptr<char[]> allocationBuffer(new char[kTestAllocationBufferSize]);

    ngen::memory::Heap heap;

    EXPECT_FALSE(heap.initialize(allocationBuffer.get(), kTestAllocationBufferSize, ngen::memory::kAllocationStrategy::Invalid));

    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());
    EXPECT_EQ(ngen::memory::kAllocationStrategy::Invalid, heap.getAllocationStrategy());
}

TEST(Heap, Initialize_First) {
    std::unique_ptr<char[]> allocationBuffer(new char[kTestAllocationBufferSize]);

    ngen::memory::Heap heap;

    EXPECT_TRUE(heap.initialize(allocationBuffer.get(), kTestAllocationBufferSize, ngen::memory::kAllocationStrategy::First));
    EXPECT_EQ(kTestAllocationBufferSize, heap.getSize());

    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());
    EXPECT_EQ(ngen::memory::kAllocationStrategy::First, heap.getAllocationStrategy());
}

TEST(Heap, Initialize_Smallest) {
    std::unique_ptr<char[]> allocationBuffer(new char[kTestAllocationBufferSize]);

    ngen::memory::Heap heap;

    EXPECT_TRUE(heap.initialize(allocationBuffer.get(), kTestAllocationBufferSize, ngen::memory::kAllocationStrategy::Smallest));
    EXPECT_EQ(kTestAllocationBufferSize, heap.getSize());

    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());
    EXPECT_EQ(ngen::memory::kAllocationStrategy::Smallest, heap.getAllocationStrategy());
}

TEST(Heap, SingleAllocation) {
    std::unique_ptr<char[]> allocationBuffer(new char[kTestAllocationBufferSize]);

    ngen::memory::Heap heap;

    EXPECT_TRUE(heap.initialize(allocationBuffer.get(), kTestAllocationBufferSize));
    EXPECT_EQ(kTestAllocationBufferSize, heap.getSize());

    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());

    void *testAllocation = heap.alloc(64);

    EXPECT_NE(nullptr, testAllocation);
    EXPECT_EQ(1, heap.getAllocations());
    EXPECT_EQ(1, heap.getTotalAllocations());
    EXPECT_EQ(0, heap.getFailedAllocations());
}

TEST(Heap, DoubleAllocation) {
    std::unique_ptr<char[]> allocationBuffer(new char[kTestAllocationBufferSize]);

    ngen::memory::Heap heap;

    EXPECT_TRUE(heap.initialize(allocationBuffer.get(), kTestAllocationBufferSize));
    EXPECT_EQ(kTestAllocationBufferSize, heap.getSize());

    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());

    void *testAllocationA = heap.alloc(64);
    void *testAllocationB = heap.alloc(64);

    EXPECT_NE(nullptr, testAllocationA);
    EXPECT_NE(nullptr, testAllocationB);

    EXPECT_NE(testAllocationA, testAllocationB);

    EXPECT_EQ(2, heap.getAllocations());
    EXPECT_EQ(2, heap.getTotalAllocations());
    EXPECT_EQ(0, heap.getFailedAllocations());
}

TEST(Heap, AllocationTooLarge) {
    std::unique_ptr<char[]> allocationBuffer(new char[kTestAllocationBufferSize]);

    ngen::memory::Heap heap;

    EXPECT_TRUE(heap.initialize(allocationBuffer.get(), kTestAllocationBufferSize));
    EXPECT_EQ(kTestAllocationBufferSize, heap.getSize());

    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());

    void *testAllocation = heap.alloc(kTestAllocationBufferSize);

    EXPECT_EQ(nullptr, testAllocation);
    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());
    EXPECT_EQ(1, heap.getFailedAllocations());
}

TEST(Heap, DoubleAllocationWithFailure) {
    std::unique_ptr<char[]> allocationBuffer(new char[kTestAllocationBufferSize]);

    ngen::memory::Heap heap;

    EXPECT_TRUE(heap.initialize(allocationBuffer.get(), kTestAllocationBufferSize));
    EXPECT_EQ(kTestAllocationBufferSize, heap.getSize());

    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());

    void *testAllocationA = heap.alloc(64);
    void *testAllocationB = heap.alloc(kTestAllocationBufferSize);
    void *testAllocationC = heap.alloc(64);
    void *testAllocationD = heap.alloc(kTestAllocationBufferSize);

    EXPECT_NE(nullptr, testAllocationA);
    EXPECT_EQ(nullptr, testAllocationB);
    EXPECT_NE(nullptr, testAllocationC);
    EXPECT_EQ(nullptr, testAllocationD);

    EXPECT_NE(testAllocationA, testAllocationC);

    EXPECT_EQ(2, heap.getAllocations());
    EXPECT_EQ(2, heap.getTotalAllocations());
    EXPECT_EQ(2, heap.getFailedAllocations());
}

TEST(Heap, DeallocateSingle) {
    std::unique_ptr<char[]> allocationBuffer(new char[kTestAllocationBufferSize]);

    ngen::memory::Heap heap;

    EXPECT_TRUE(heap.initialize(allocationBuffer.get(), kTestAllocationBufferSize));
    EXPECT_EQ(kTestAllocationBufferSize, heap.getSize());

    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());

    void *testAllocation = heap.alloc(64);

    EXPECT_NE(nullptr, testAllocation);
    EXPECT_EQ(1, heap.getAllocations());
    EXPECT_EQ(1, heap.getTotalAllocations());
    EXPECT_EQ(0, heap.getFailedAllocations());

    EXPECT_TRUE(heap.deallocate(nullptr, true, nullptr, 0));
    EXPECT_TRUE(heap.deallocate(nullptr, false, nullptr, 0));

    EXPECT_TRUE(heap.deallocate(testAllocation, false, nullptr, 0));
    EXPECT_FALSE(heap.deallocate(testAllocation, false, nullptr, 0));

    EXPECT_TRUE(heap.deallocate(nullptr, true, nullptr, 0));
    EXPECT_TRUE(heap.deallocate(nullptr, false, nullptr, 0));

    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(1, heap.getTotalAllocations());
    EXPECT_EQ(0, heap.getFailedAllocations());
}

//! \brief This test performs a single allocation/release and repeats it multiple times to ensure the heap does not
//! get irreversibly fragmented.
TEST(Heap, DeallocateSingleFlood) {
    std::unique_ptr<char[]> allocationBuffer(new char[kTestAllocationBufferSize]);

    ngen::memory::Heap heap;

    EXPECT_TRUE(heap.initialize(allocationBuffer.get(), kTestAllocationBufferSize));
    EXPECT_EQ(kTestAllocationBufferSize, heap.getSize());

    EXPECT_EQ(0, heap.getTotalAllocations());

    const size_t repeatCount = 1024;

    // Perform the allocation/free multiple times
    for (size_t loop = 0; loop < repeatCount; ++loop) {
        EXPECT_EQ(0, heap.getAllocations());

        void *testAllocation = heap.alloc(64);

        EXPECT_NE(nullptr, testAllocation);
        EXPECT_EQ(1, heap.getAllocations());
        EXPECT_EQ(0, heap.getFailedAllocations());

        EXPECT_TRUE(heap.deallocate(nullptr, true, nullptr, 0));
        EXPECT_TRUE(heap.deallocate(nullptr, false, nullptr, 0));

        EXPECT_TRUE(heap.deallocate(testAllocation, false, nullptr, 0));
        EXPECT_FALSE(heap.deallocate(testAllocation, false, nullptr, 0));

        EXPECT_TRUE(heap.deallocate(nullptr, true, nullptr, 0));
        EXPECT_TRUE(heap.deallocate(nullptr, false, nullptr, 0));

        EXPECT_EQ(0, heap.getAllocations());
        EXPECT_EQ(0, heap.getFailedAllocations());
    }

    EXPECT_EQ(repeatCount, heap.getTotalAllocations());
}

TEST(Heap, DeallocateArrayMismatch) {
    std::unique_ptr<char[]> allocationBuffer(new char[kTestAllocationBufferSize]);

    ngen::memory::Heap heap;

    EXPECT_TRUE(heap.initialize(allocationBuffer.get(), kTestAllocationBufferSize));
    EXPECT_EQ(kTestAllocationBufferSize, heap.getSize());

    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());

    void *testAllocation = heap.alloc(64);

    EXPECT_NE(nullptr, testAllocation);
    EXPECT_EQ(1, heap.getAllocations());
    EXPECT_EQ(1, heap.getTotalAllocations());
    EXPECT_EQ(0, heap.getFailedAllocations());

    EXPECT_FALSE(heap.deallocate(testAllocation, true, nullptr, 0));
    EXPECT_EQ(1, heap.getAllocations());

    EXPECT_TRUE(heap.deallocate(testAllocation, false, nullptr, 0));
    EXPECT_EQ(0, heap.getAllocations());

    // Also test when the data is allocated as an array
    testAllocation = heap.allocArray(64);

    EXPECT_NE(nullptr, testAllocation);
    EXPECT_EQ(1, heap.getAllocations());
    EXPECT_EQ(2, heap.getTotalAllocations());
    EXPECT_EQ(0, heap.getFailedAllocations());

    EXPECT_FALSE(heap.deallocate(testAllocation, false, nullptr, 0));
    EXPECT_EQ(1, heap.getAllocations());

    EXPECT_TRUE(heap.deallocate(testAllocation, true, nullptr, 0));
    EXPECT_EQ(0, heap.getAllocations());
}

TEST(Heap, DeallocateHeapMismatch) {
    std::unique_ptr<char[]> allocationBufferA(new char[kTestAllocationBufferSize]);
    std::unique_ptr<char[]> allocationBufferB(new char[kTestAllocationBufferSize]);

    ngen::memory::Heap heapA;
    ngen::memory::Heap heapB;

    EXPECT_TRUE(heapA.initialize(allocationBufferA.get(), kTestAllocationBufferSize));
    EXPECT_TRUE(heapB.initialize(allocationBufferB.get(), kTestAllocationBufferSize));
    EXPECT_EQ(kTestAllocationBufferSize, heapA.getSize());
    EXPECT_EQ(kTestAllocationBufferSize, heapB.getSize());

    EXPECT_EQ(0, heapA.getAllocations());
    EXPECT_EQ(0, heapA.getTotalAllocations());

    EXPECT_EQ(0, heapB.getAllocations());
    EXPECT_EQ(0, heapB.getTotalAllocations());

    void *testAllocationA = heapA.alloc(64);
    void *testAllocationB = heapB.alloc(64);

    EXPECT_NE(nullptr, testAllocationA);
    EXPECT_NE(nullptr, testAllocationB);

    EXPECT_EQ(1, heapA.getAllocations());
    EXPECT_EQ(1, heapA.getTotalAllocations());
    EXPECT_EQ(0, heapA.getFailedAllocations());

    EXPECT_EQ(1, heapB.getAllocations());
    EXPECT_EQ(1, heapB.getTotalAllocations());
    EXPECT_EQ(0, heapB.getFailedAllocations());

    EXPECT_FALSE(heapA.deallocate(testAllocationB, false, nullptr, 0));
    EXPECT_FALSE(heapB.deallocate(testAllocationA, false, nullptr, 0));

    EXPECT_FALSE(heapA.deallocate(testAllocationB, true, nullptr, 0));
    EXPECT_FALSE(heapB.deallocate(testAllocationA, true, nullptr, 0));

    EXPECT_TRUE(heapA.deallocate(testAllocationA, false, nullptr, 0));
    EXPECT_TRUE(heapB.deallocate(testAllocationB, false, nullptr, 0));

    EXPECT_EQ(0, heapA.getAllocations());
    EXPECT_EQ(1, heapA.getTotalAllocations());
    EXPECT_EQ(0, heapA.getFailedAllocations());

    EXPECT_EQ(0, heapB.getAllocations());
    EXPECT_EQ(1, heapB.getTotalAllocations());
    EXPECT_EQ(0, heapB.getFailedAllocations());
}

//! \brief Test behaviour when entire buffer has been allocated.
TEST(Heap, FullAllocation) {
    std::unique_ptr<char[]> allocationBuffer(new char[kTestAllocationBufferSize]);

    ngen::memory::Heap heap;

    EXPECT_TRUE(heap.initialize(allocationBuffer.get(), kTestAllocationBufferSize));
    EXPECT_EQ(kTestAllocationBufferSize, heap.getSize());

    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());

    void *testAllocation = heap.alloc(kTestAllocationBufferSize - sizeof(ngen::memory::Allocation));

    EXPECT_NE(nullptr, testAllocation);
    EXPECT_EQ(1, heap.getAllocations());
    EXPECT_EQ(1, heap.getTotalAllocations());
    EXPECT_EQ(0, heap.getFailedAllocations());

    void *testAllocationB = heap.alloc(64);

    EXPECT_EQ(nullptr, testAllocationB);
    EXPECT_EQ(1, heap.getAllocations());
    EXPECT_EQ(1, heap.getTotalAllocations());
    EXPECT_EQ(1, heap.getFailedAllocations());

    EXPECT_TRUE(heap.deallocate(testAllocation, false, nullptr, 0));

    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(1, heap.getTotalAllocations());
    EXPECT_EQ(1, heap.getFailedAllocations());

    testAllocation = heap.alloc(64);
    EXPECT_NE(nullptr, testAllocation);
    EXPECT_EQ(1, heap.getAllocations());
    EXPECT_EQ(2, heap.getTotalAllocations());
    EXPECT_EQ(1, heap.getFailedAllocations());

    EXPECT_TRUE(heap.deallocate(testAllocation, false, nullptr, 0));
}
