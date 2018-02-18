
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
