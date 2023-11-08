#ifndef PIGEON_FRAMEWORK_BASE_AUTO_PTR_SHARED
#define PIGEON_FRAMEWORK_BASE_AUTO_PTR_SHARED

#include <utility>

namespace pigeon {

template <typename T>
class Shared {
 public:
  Shared() = default;

  explicit Shared(T* raw_ptr) : raw_ptr_(raw_ptr), ref_cnt_(new size_t(1)) {}

  template <typename... Args>
  Shared(Args... args) : raw_ptr_(new T(args...)), ref_cnt_(new size_t(1)) {}

  Shared(const Shared& other)
      : raw_ptr_(other.raw_ptr_), ref_cnt_(other.ref_cnt_) {
    if (ref_cnt_ != nullptr) {
      *ref_cnt_ += 1;
    }
  }

  Shared(Shared&& other) noexcept
      : raw_ptr_(other.raw_ptr_), ref_cnt_(other.ref_cnt_) {
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
      delete raw_ptr_;
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
  T* raw_ptr_{nullptr};
  size_t* ref_cnt_{nullptr};
};

}  // namespace pigeon

#endif  // PIGEON_FRAMEWORK_BASE_AUTO_PTR_SHARED
