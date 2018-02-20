
#include "heap.h"
#include "gtest/gtest.h"

const size_t kTestAllocationBufferSize = 1024;
const size_t kInvalidAllocationBufferSize = 0;

TEST(Heap, Construction) {
    ngen::memory::Heap heap;

    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());
}

TEST(Heap, FailedInitialization) {
    auto *allocationBuffer = new char[kTestAllocationBufferSize];

    ngen::memory::Heap heap;

    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());
    EXPECT_EQ(ngen::memory::kAllocationStrategy_Invalid, heap.getAllocationStrategy());

    EXPECT_FALSE(heap.initialize(nullptr, kInvalidAllocationBufferSize, ngen::memory::kAllocationStrategy_First));
    EXPECT_FALSE(heap.initialize(nullptr, kTestAllocationBufferSize, ngen::memory::kAllocationStrategy_First));
    EXPECT_FALSE(heap.initialize(allocationBuffer, kInvalidAllocationBufferSize, ngen::memory::kAllocationStrategy_First));

    EXPECT_FALSE(heap.initialize(nullptr, kInvalidAllocationBufferSize, ngen::memory::kAllocationStrategy_Smallest));
    EXPECT_FALSE(heap.initialize(nullptr, kTestAllocationBufferSize, ngen::memory::kAllocationStrategy_Smallest));
    EXPECT_FALSE(heap.initialize(allocationBuffer, kInvalidAllocationBufferSize, ngen::memory::kAllocationStrategy_Smallest));

    // Make sure these variables haven't changed during the above calls.
    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());
    EXPECT_EQ(ngen::memory::kAllocationStrategy_Invalid, heap.getAllocationStrategy());

    delete [] allocationBuffer;
}

TEST(Heap, Initialize_Invalid) {
    auto *allocationBuffer = new char[kTestAllocationBufferSize];

    ngen::memory::Heap heap;

    EXPECT_FALSE(heap.initialize(allocationBuffer, kTestAllocationBufferSize, ngen::memory::kAllocationStrategy_Invalid));

    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());
    EXPECT_EQ(ngen::memory::kAllocationStrategy_Invalid, heap.getAllocationStrategy());

    delete [] allocationBuffer;
}

TEST(Heap, Initialize_First) {
    auto *allocationBuffer = new char[kTestAllocationBufferSize];

    ngen::memory::Heap heap;

    EXPECT_TRUE(heap.initialize(allocationBuffer, kTestAllocationBufferSize, ngen::memory::kAllocationStrategy_First));

    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());
    EXPECT_EQ(ngen::memory::kAllocationStrategy_First, heap.getAllocationStrategy());

    delete [] allocationBuffer;
}

TEST(Heap, Initialize_Smallest) {
    auto *allocationBuffer = new char[kTestAllocationBufferSize];

    ngen::memory::Heap heap;

    EXPECT_TRUE(heap.initialize(allocationBuffer, kTestAllocationBufferSize, ngen::memory::kAllocationStrategy_Smallest));

    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());
    EXPECT_EQ(ngen::memory::kAllocationStrategy_Smallest, heap.getAllocationStrategy());

    delete [] allocationBuffer;
}

TEST(Heap, SingleAllocation) {
    auto *allocationBuffer = new char[kTestAllocationBufferSize];

    ngen::memory::Heap heap;

    EXPECT_TRUE(heap.initialize(allocationBuffer, kTestAllocationBufferSize));

    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());

    void *testAllocation = heap.alloc(64);

    EXPECT_NE(nullptr, testAllocation);
    EXPECT_EQ(1, heap.getAllocations());
    EXPECT_EQ(1, heap.getTotalAllocations());
    EXPECT_EQ(0, heap.getFailedAllocations());

    delete [] allocationBuffer;
}

TEST(Heap, DoubleAllocation) {
    auto *allocationBuffer = new char[kTestAllocationBufferSize];

    ngen::memory::Heap heap;

    EXPECT_TRUE(heap.initialize(allocationBuffer, kTestAllocationBufferSize));

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

    delete [] allocationBuffer;
}

TEST(Heap, AllocationTooLarge) {
    auto *allocationBuffer = new char[kTestAllocationBufferSize];

    ngen::memory::Heap heap;

    EXPECT_TRUE(heap.initialize(allocationBuffer, kTestAllocationBufferSize));

    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());

    void *testAllocation = heap.alloc(kTestAllocationBufferSize);

    EXPECT_EQ(nullptr, testAllocation);
    EXPECT_EQ(0, heap.getAllocations());
    EXPECT_EQ(0, heap.getTotalAllocations());
    EXPECT_EQ(1, heap.getFailedAllocations());

    delete [] allocationBuffer;
}

TEST(Heap, DoubleAllocationWithFailure) {
    auto *allocationBuffer = new char[kTestAllocationBufferSize];

    ngen::memory::Heap heap;

    EXPECT_TRUE(heap.initialize(allocationBuffer, kTestAllocationBufferSize));

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

    delete [] allocationBuffer;
}

