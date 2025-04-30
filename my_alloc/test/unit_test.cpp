#include "my_alloc.hpp"
#include <gtest/gtest.h>
#include <unistd.h>
#include "ds.hpp"


// ====== MYALLOC TEST ======

TEST(MyAllocTest, BasicAllocationAndFree) {
    void* start_brk = sbrk(0);

    void* ptr1 = my_alloc::heap_alloc(32);
    void* ptr2 = my_alloc::heap_alloc(64);
    void* ptr3 = my_alloc::heap_alloc(128);

    ASSERT_NE(ptr1, nullptr);
    ASSERT_NE(ptr2, nullptr);
    ASSERT_NE(ptr3, nullptr);

    EXPECT_NE(ptr1, ptr2);
    EXPECT_NE(ptr2, ptr3);
    EXPECT_NE(ptr1, ptr3);

    my_alloc::heap_free(ptr2);

    void* ptr4 = my_alloc::heap_alloc(64);
    ASSERT_NE(ptr4, nullptr);
    EXPECT_EQ(ptr4, ptr2);

    my_alloc::heap_free(ptr1);
    my_alloc::heap_free(ptr3);
    my_alloc::heap_free(ptr4);

    void* end_brk = sbrk(0);
    EXPECT_GE(end_brk, start_brk);
}

// ====== MYALLOC ZERO ALLOCATION TEST ======

TEST(MyAllocTest, ZeroAndNullAllocation) {
    void* null_alloc = my_alloc::heap_alloc(0);
    EXPECT_EQ(null_alloc, nullptr);

    EXPECT_NO_FATAL_FAILURE(my_alloc::heap_free(nullptr));
}



// ====== Vector TEST ======
TEST(DataStructureTest, VectorBasicOperations) {
    Vector<int> vec;
    EXPECT_EQ(vec.size(), 0);

    for (int i = 0; i < 5; ++i) {
        vec.push_back(i * 10);
    }

    EXPECT_EQ(vec.size(), 5);
    EXPECT_EQ(vec[0], 0);
    EXPECT_EQ(vec[1], 10);
    EXPECT_EQ(vec[4], 40);

    vec[2] = 999;
    EXPECT_EQ(vec[2], 999);
}

// ====== HashTable TEST ======
TEST(DataStructureTest, HashTableInsertAndRetrieve) {
    HashTable ht;

    ht.insert("key1", 100);
    ht.insert("key2", 200);

    int* val = ht.get("key1");
    ASSERT_NE(val, nullptr);
    EXPECT_EQ(*val, 100);

    val = ht.get("key2");
    ASSERT_NE(val, nullptr);
    EXPECT_EQ(*val, 200);

    val = ht.get("missing");
    EXPECT_EQ(val, nullptr);
}

// ====== Stack TEST ======
TEST(DataStructureTest, StackPushPop) {
    Stack<int> stack;

    stack.push(5);
    stack.push(10);
    stack.push(15);

    EXPECT_FALSE(stack.empty());

    EXPECT_EQ(stack.pop(), 15);
    EXPECT_EQ(stack.pop(), 10);
    EXPECT_EQ(stack.pop(), 5);
    EXPECT_TRUE(stack.empty());
}

// ====== Queue TEST ======
TEST(DataStructureTest, QueueEnqueueDequeue) {
    Queue<int> queue;

    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);

    EXPECT_FALSE(queue.empty());

    EXPECT_EQ(queue.dequeue(), 1);
    EXPECT_EQ(queue.dequeue(), 2);
    EXPECT_EQ(queue.dequeue(), 3);

    EXPECT_TRUE(queue.empty());
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
