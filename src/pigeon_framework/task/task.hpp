#ifndef PIGEON_FRAMEWORK_TASK_TASK
#define PIGEON_FRAMEWORK_TASK_TASK

#include <concepts>
#include "pigeon_framework/define.hpp"

namespace pigeon {

class PIGEON_API Task {
 public:
  enum Status { Keep, Done };

  virtual ~Task() = default;
  virtual Status Execute() = 0;
};

template <typename T>
concept AsTask = std::derived_from<T, Task>;

}  // namespace pigeon

#endif  // PIGEON_FRAMEWORK_TASK_TASK
