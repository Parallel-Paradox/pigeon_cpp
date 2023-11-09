#ifndef PIGEON_FRAMEWORK_BASE_AUTO_PTR_SHARED
#define PIGEON_FRAMEWORK_BASE_AUTO_PTR_SHARED

#include <functional>
#include <utility>

namespace pigeon {

template <typename T>
class Shared {
 public:
  using Destructor = std::function<void(T*)>;

  Shared() = default;

  explicit Shared(T* raw_ptr, Destructor destructor = DefaultDestructor)
      : raw_ptr_(raw_ptr), ref_cnt_(new size_t(1)), destructor_(destructor) {}

  template <typename... Args>
  Shared(Args... args, Destructor destructor = DefaultDestructor)
      : raw_ptr_(new T(args...)),
        ref_cnt_(new size_t(1)),
        destructor_(destructor) {}

  Shared(const Shared& other)
      : raw_ptr_(other.raw_ptr_),
        ref_cnt_(other.ref_cnt_),
        destructor_(other.destructor_) {
    if (ref_cnt_ != nullptr) {
      *ref_cnt_ += 1;
    }
  }

  Shared(Shared&& other) noexcept
      : raw_ptr_(other.raw_ptr_),
        ref_cnt_(other.ref_cnt_),
        destructor_(other.destructor_) {
    other.raw_ptr_ = nullptr;
    other.ref_cnt_ = nullptr;
  }

  Shared& operator=(const Shared& other) {
    if (this != &other) {
      this->~Shared();
      new (this) Shared(other);
    }
    return *this;
  }

  Shared& operator=(Shared&& other) noexcept {
    if (this != &other) {
      this->~Shared();
      new (this) Shared(std::move(other));
    }
    return *this;
  }

  ~Shared() {
    if (ref_cnt_ == nullptr) {
      return;
    }
    if (*ref_cnt_ == 1) {
      destructor_(raw_ptr_);
      delete ref_cnt_;
    } else {
      *ref_cnt_--;
    }
  }

  T* operator->() const { return raw_ptr_; }

  T& operator*() const { return *raw_ptr_; }

  T* Get() const { return raw_ptr_; }

  bool IsNull() const { return raw_ptr_ == nullptr; }

  size_t RefCnt() const { return *ref_cnt_; }

 private:
  static void DefaultDestructor(T* raw_ptr) { delete raw_ptr; }

  T* raw_ptr_{nullptr};
  size_t* ref_cnt_{nullptr};
  Destructor destructor_;
};

}  // namespace pigeon

#endif  // PIGEON_FRAMEWORK_BASE_AUTO_PTR_SHARED
