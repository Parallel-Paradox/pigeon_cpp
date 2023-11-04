#ifndef PIGEON_FRAMEWORK_BASE_AUTO_PTR_OWNED
#define PIGEON_FRAMEWORK_BASE_AUTO_PTR_OWNED

#include "pigeon_framework/define.hpp"

namespace pigeon {

template <typename T>
class Owned {
 public:
  Owned() = default;

  explicit Owned(T* raw_ptr) : raw_ptr_(raw_ptr) {}

  template <typename... Args>
  Owned(Args... args) : raw_ptr_(new T(args...)) {}

  Owned(const Owned& other) = delete;

  Owned(Owned&& other) noexcept {
    raw_ptr_ = other.raw_ptr_;
    other.raw_ptr_ = nullptr;
  }

  Owned& operator=(Owned&& other) noexcept {
    if (this == &other) {
      return *this;
    }
    delete raw_ptr_;
    raw_ptr_ = other.raw_ptr_;
    other.raw_ptr_ = nullptr;
    return *this;
  }

  ~Owned() { delete raw_ptr_; }

  T* Get() const { return raw_ptr_; }

  bool IsNull() const { return raw_ptr_ == nullptr; }

 private:
  T* raw_ptr_{nullptr};
};

}  // namespace pigeon

#endif  // PIGEON_FRAMEWORK_BASE_AUTO_PTR_OWNED
