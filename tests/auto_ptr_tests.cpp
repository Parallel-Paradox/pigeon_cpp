#include <gtest/gtest.h>
#include "pigeon_framework\base\auto_ptr\owned.hpp"
#include "pigeon_framework\base\auto_ptr\shared.hpp"
#include "pigeon_framework\base\auto_ptr\unretained.hpp"

using namespace pigeon;

TEST(AutoPtrTests, CustomDestructor) {
  int32_t destruct_cnt = 0;
  {
    auto custom_destructor = [](int32_t* cnt) {
      (*cnt)++;
    };
    auto owned_ptr = Owned<int32_t>(&destruct_cnt, custom_destructor);
    auto shared_ptr =
        Shared<int32_t>::ByThreadLocal(&destruct_cnt, custom_destructor);
    auto shared_ptr_clone = shared_ptr;
    EXPECT_EQ(shared_ptr.RefCnt(), 2);
  }
  EXPECT_EQ(destruct_cnt, 2);
}
