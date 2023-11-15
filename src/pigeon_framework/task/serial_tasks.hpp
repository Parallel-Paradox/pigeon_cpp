#ifndef PIGEON_FRAMEWORK_TASK_SERIAL_TASKS
#define PIGEON_FRAMEWORK_TASK_SERIAL_TASKS

#include "pigeon_framework/base/auto_ptr/owned.hpp"
#include "pigeon_framework/base/container/array.hpp"
#include "pigeon_framework/define.hpp"
#include "pigeon_framework/task/task.hpp"

AUTO_PTR_INSTANTIATE(Owned, pigeon::Task);

namespace pigeon {

template class PIGEON_API Array<Owned<Task>>;

class PIGEON_API SerialTasks : public Task {
 public:
  SerialTasks() = default;
  ~SerialTasks() override = default;

  Status Execute() override;

 private:
  Array<Owned<Task>> tasks_;
};

}  // namespace pigeon

#endif  // PIGEON_FRAMEWORK_TASK_SERIAL_TASKS
