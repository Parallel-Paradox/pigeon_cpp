#include "pigeon_framework/task/serial_tasks.hpp"

using namespace pigeon;

Task::Status SerialTasks::Execute() {
  size_t size = tasks_.Size();
  size_t tail(0);
  for (size_t i = 0; i < size; ++i) {
    if (tasks_[i]->Execute() == Status::Keep) {
      tasks_.Swap(i, tail);
      ++tail;
    }
  }
  tasks_.Resize(tail);
  return tail == 0 ? Status::Done : Status::Keep;
}
