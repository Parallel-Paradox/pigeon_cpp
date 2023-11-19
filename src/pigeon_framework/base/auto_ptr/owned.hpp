#ifndef PIGEON_FRAMEWORK_BASE_AUTO_PTR_OWNED
#define PIGEON_FRAMEWORK_BASE_AUTO_PTR_OWNED

#include <functional>
#include <utility>
#include "pigeon_framework/define.hpp"

#define INSTANTIATE_OWNED(ValueType)                         \
  template class PIGEON_API std::function<void(ValueType*)>; \
  template class PIGEON_API pigeon::Owned<ValueType>;

namespace pigeon {

template <typename T>
class Owned {
 public:
  using Destructor = std::function<void(T*)>;

  Owned() = default;

  explicit Owned(T* raw_ptr, Destructor destructor = DefaultDestructor)
      : raw_ptr_(raw_ptr), destructor_(destructor) {}

  template <typename... Args>
  explicit Owned(Args... args, Destructor destructor = DefaultDestructor)
      : raw_ptr_(new T(args...)), destructor_(destructor) {}

  Owned(const Owned& other) = delete;

  Owned(Owned&& other) noexcept
      : raw_ptr_(other.raw_ptr_), destructor_(other.destructor_) {
    other.raw_ptr_ = nullptr;
  }

  Owned& operator=(Owned&& other) noexcept {
    if (this != &other) {
      this->~Owned();
      new (this) Owned(std::move(other));
    }
    return *this;
  }

  ~Owned() { destructor_(raw_ptr_); }

  T* operator->() const { return raw_ptr_; }

  T& operator*() const { return *raw_ptr_; }

  T* Get() const { return raw_ptr_; }

  bool IsNull() const { return raw_ptr_ == nullptr; }

 private:
  static void DefaultDestructor(T* raw_ptr) { delete raw_ptr; }

  T* raw_ptr_{nullptr};
  Destructor destructor_;
};

}  // namespace pigeon

#endif  // PIGEON_FRAMEWORK_BASE_AUTO_PTR_OWNED
