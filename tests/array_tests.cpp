#include <gtest/gtest.h>
#include <stdint.h>
#include <exception>
#include <iterator>
#include <stdexcept>
#include "pigeon_framework/base/auto_ptr/owned.hpp"
#include "pigeon_framework/base/container/array.hpp"

using namespace pigeon;

TEST(ArrayTests, CopyConstructed) {
  Array<int32_t> src = {0, 1};
  Array<int32_t> dst = Array<int32_t>(src);
  EXPECT_EQ(dst[0], 0);
  EXPECT_EQ(dst[1], 1);
  EXPECT_EQ(src, dst);
}

TEST(ArrayTests, CopyConstructedFail) {
  Array<Owned<int32_t>> src;
  EXPECT_THROW(Array<Owned<int32_t>> dst(src), std::invalid_argument);
}

TEST(ArrayTests, MoveConstructed) {
  int32_t destruct_cnt = 0;
  auto destructor = [&destruct_cnt](int32_t* ptr) {
    delete ptr;
    destruct_cnt += 1;
  };

  {
    Array<Owned<int32_t>> src;
    src.EmplaceBack(Owned<int32_t>(new int32_t(0), destructor));
    src.EmplaceBack(Owned<int32_t>(new int32_t(1), destructor));
    auto* raw_src = src.Get();
    Array<Owned<int32_t>> dst(std::move(src));
    EXPECT_TRUE(src.IsEmpty());
    EXPECT_EQ(*dst[0], 0);
    EXPECT_EQ(*dst[1], 1);
    EXPECT_EQ(raw_src, dst.Get());
    EXPECT_EQ(destruct_cnt, 0);
  }
  EXPECT_EQ(destruct_cnt, 2);
}

TEST(ArrayTests, SetByCopy) {
  Array<int32_t> src = {0, 1};
  Array<int32_t> dst;
  dst = Array<int32_t>(src);
  EXPECT_EQ(dst[0], 0);
  EXPECT_EQ(dst[1], 1);
  EXPECT_EQ(src, dst);
}

TEST(ArrayTests, SetByCopyFail) {
  Array<Owned<int32_t>> src;
  Array<Owned<int32_t>> dst;
  EXPECT_THROW(dst = src, std::invalid_argument);
}

TEST(ArrayTests, SetByMove) {
  int32_t destruct_cnt = 0;
  auto destructor = [&destruct_cnt](int32_t* ptr) {
    delete ptr;
    destruct_cnt += 1;
  };

  Array<Owned<int32_t>> src;
  src.EmplaceBack(Owned<int32_t>(new int32_t(0), destructor));
  src.EmplaceBack(Owned<int32_t>(new int32_t(1), destructor));
  Array<Owned<int32_t>> dst;
  dst.EmplaceBack(Owned<int32_t>(new int32_t(2), destructor));
  dst = std::move(src);
  EXPECT_TRUE(src.IsEmpty());
  EXPECT_EQ(*dst[0], 0);
  EXPECT_EQ(*dst[1], 1);
  EXPECT_EQ(destruct_cnt, 1);
}

TEST(ArrayTests, CapacityReserve) {
  Array<int32_t> array = {0, 1, 2};
  EXPECT_EQ(array.Size(), 3);
  EXPECT_EQ(array.Capacity(), 3);

  array.PushBack(3);
  EXPECT_EQ(array.Size(), 4);
  EXPECT_EQ(array.Capacity(), 6);

  array.Reserve(5);
  EXPECT_EQ(array.Size(), 4);
  EXPECT_EQ(array.Capacity(), 6);

  array.SetCapacity(5);
  EXPECT_EQ(array.Size(), 4);
  EXPECT_EQ(array.Capacity(), 5);

  array.ShrinkToFit();
  EXPECT_EQ(array.Size(), 4);
  EXPECT_EQ(array.Size(), array.Capacity());
}

TEST(ArrayTests, ResizeArray) {
  Array<int32_t> array = {1, 2};
  EXPECT_EQ(array.Size(), 2);
  EXPECT_EQ(array.Capacity(), 2);

  array.Resize(3);
  EXPECT_EQ(array.Size(), 3);
  EXPECT_EQ(array.Capacity(), 3);
  EXPECT_EQ(array[2], 0);

  array.Resize(1);
  EXPECT_EQ(array.Size(), 1);
  EXPECT_EQ(array.Capacity(), 3);
}

TEST(ArrayTests, CopyableCommonOps) {
  Array<int32_t> array;
  array.PushBack(0);     // 0
  array.EmplaceBack(2);  // 0, 2
  array.Insert(1, 1);    // 0, 1, 2
  array.Insert(3, 3);    // 0, 1, 2, 3
  array.Remove(3);       // 0, 1, 2
  array.SwapRemove(0);   // 2, 1
  array.Swap(0, 1);      // 1, 2

  EXPECT_EQ(array[0], 1);
  EXPECT_EQ(array[1], 2);
  EXPECT_EQ(array.Size(), 2);
}

TEST(ArrayTests, MovableFailOps) {
  Array<Owned<int32_t>> array;
  EXPECT_THROW(array.PushBack(Owned<int32_t>::New(0)), std::invalid_argument);
  auto num = Owned<int32_t>::New(1);
  EXPECT_THROW(array.Insert(1, num), std::invalid_argument);
}

TEST(ArrayTests, OutOfRangeFailOps) {
  Array<int32_t> array;
  EXPECT_THROW(array.Insert(1, 1), std::out_of_range);
  EXPECT_THROW(array.Remove(1), std::out_of_range);
  EXPECT_THROW(array.SwapRemove(1), std::out_of_range);
}

TEST(ArrayTests, IterateArray) {
  EXPECT_TRUE(std::contiguous_iterator<Array<int32_t>::Iterator>);
  EXPECT_TRUE(std::contiguous_iterator<Array<int32_t>::ConstIterator>);
  Array<int32_t> array = {0, 1, 2};
  for (auto iter = array.begin(); iter != array.end(); ++iter) {
    *iter += 1;
  }
  for (auto& num : array) {
    num += 1;
  }
  const Array<int32_t> compare = {2, 3, 4};
  auto arr_iter = array.begin();
  auto cmp_iter = compare.begin();
  for (int32_t i = 0; i < 3; ++i) {
    EXPECT_EQ(arr_iter[i], cmp_iter[i]);
  }
}
