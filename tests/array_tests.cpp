#include <gtest/gtest.h>
#include <stdint.h>
#include <iterator>
#include <stdexcept>
#include "pigeon_framework/base/container/array.hpp"

using namespace pigeon;

class OwnedInt {
 public:
  OwnedInt() = default;

  OwnedInt(int32_t num, int32_t* destruct_cnt)
      : num_(num), destruct_cnt_(destruct_cnt) {}

  OwnedInt(const OwnedInt& other) = delete;

  OwnedInt(OwnedInt&& other) {
    num_ = other.num_;
    destruct_cnt_ = other.destruct_cnt_;
    other.destruct_cnt_ = nullptr;
  }

  OwnedInt& operator=(OwnedInt&& other) {
    if (this != &other) {
      this->~OwnedInt();
      new (this) OwnedInt(std::move(other));
    }
    return *this;
  }

  ~OwnedInt() {
    if (destruct_cnt_ != nullptr) {
      (*destruct_cnt_)++;
    }
  }

  int32_t num_{0};
  int32_t* destruct_cnt_{nullptr};
};

TEST(ArrayTests, CopyConstructed) {
  Array<int32_t> src = {0, 1};
  Array<int32_t> dst = Array<int32_t>(src);
  EXPECT_EQ(dst[0], 0);
  EXPECT_EQ(dst[1], 1);
  EXPECT_EQ(src, dst);
}

TEST(ArrayTests, CopyConstructedFail) {
  Array<OwnedInt> src;
  try {
    Array<OwnedInt> dst(src);
    EXPECT_TRUE(false);
  } catch (std::invalid_argument e) {
    EXPECT_STREQ(e.what(), "The type of array can't be copy.");
  }
}

TEST(ArrayTests, MoveConstructed) {
  int32_t destruct_cnt = 0;
  Array<OwnedInt> src;
  src.EmplaceBack(OwnedInt(0, &destruct_cnt));
  src.EmplaceBack(OwnedInt(1, &destruct_cnt));
  Array<OwnedInt> dst(std::move(src));
  EXPECT_TRUE(src.IsEmpty());
  EXPECT_EQ(dst[0].num_, 0);
  EXPECT_EQ(dst[1].num_, 1);
  EXPECT_EQ(destruct_cnt, 0);
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
  Array<OwnedInt> src;
  try {
    Array<OwnedInt> dst;
    dst = src;
    EXPECT_TRUE(false);
  } catch (std::invalid_argument e) {
    EXPECT_STREQ(e.what(), "The type of array can't be copy.");
  }
}

TEST(ArrayTests, SetByMove) {
  int32_t destruct_cnt = 0;
  Array<OwnedInt> src;
  src.EmplaceBack(OwnedInt(0, &destruct_cnt));
  src.EmplaceBack(OwnedInt(1, &destruct_cnt));
  Array<OwnedInt> dst;
  dst.EmplaceBack(OwnedInt(2, &destruct_cnt));
  dst = std::move(src);
  EXPECT_TRUE(src.IsEmpty());
  EXPECT_EQ(dst[0].num_, 0);
  EXPECT_EQ(dst[1].num_, 1);
  EXPECT_EQ(destruct_cnt, 1);
}

TEST(ArrayTests, CapacityReserve) {
  Array<int32_t> array = {0, 1, 2};
  EXPECT_EQ(array.Size(), 3);
  EXPECT_EQ(array.Capacity(), 3);
  array.PushBack(3);
  EXPECT_EQ(array.Size(), 4);
  EXPECT_EQ(array.Capacity(), 6);
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
