#include <gtest/gtest.h>
#include "pigeon_framework/base/auto_ptr/owned.hpp"
#include "pigeon_framework/base/auto_ptr/shared.hpp"
#include "pigeon_framework/base/auto_ptr/unretained.hpp"

using namespace pigeon;

TEST(AutoPtrTests, CustomDestructor) {
  int32_t destruct_cnt = 0;
  {
    auto custom_destructor = [](int32_t* cnt) {
      (*cnt)++;
    };
    auto owned_ptr = Owned<int32_t>(&destruct_cnt, custom_destructor);
    auto shared_ptr = SharedLocal<int32_t>(&destruct_cnt, custom_destructor);
    auto shared_ptr_clone = shared_ptr.Clone();
    EXPECT_EQ(shared_ptr.RefCnt(), 2);
  }
  EXPECT_EQ(destruct_cnt, 2);
}

TEST(AutoPtrTests, UpgradeUnretained) {
  UnretainedLocal<int32_t> unretained;
  auto retained = unretained.TryUpgrade();
  EXPECT_TRUE(retained.IsNull());
  {
    auto shared = SharedLocal<int32_t>::New(0);
    EXPECT_EQ(shared.UnretainedRefCnt(), 0);
    auto unretained_local = UnretainedLocal<int32_t>(shared);
    EXPECT_EQ(shared.RefCnt(), 1);
    EXPECT_EQ(shared.UnretainedRefCnt(), 1);
    auto retained = unretained_local.TryUpgrade();
    EXPECT_EQ(retained.RefCnt(), 2);
    unretained = unretained_local.Clone();
    EXPECT_EQ(shared.UnretainedRefCnt(), 2);
  }
  retained = unretained.TryUpgrade();
  EXPECT_TRUE(retained.IsNull());
}
