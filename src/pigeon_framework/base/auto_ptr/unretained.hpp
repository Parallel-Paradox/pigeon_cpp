#ifndef PIGEON_FRAMEWORK_BASE_AUTO_PTR_UNRETAINED
#define PIGEON_FRAMEWORK_BASE_AUTO_PTR_UNRETAINED

#include "pigeon_framework/base/auto_ptr/shared.hpp"
#include "pigeon_framework/define.hpp"

#define INSTANTIATE_UNRETAINED_ASYNC(ValueType)              \
  template class PIGEON_API std::function<void(ValueType*)>; \
  template class PIGEON_API                                  \
      pigeon::Unretained<ValueType, pigeon::ThreadSafeRefCount>;

#define INSTANTIATE_UNRETAINED_LOCAL(ValueType)              \
  template class PIGEON_API std::function<void(ValueType*)>; \
  template class PIGEON_API                                  \
      pigeon::Unretained<ValueType, pigeon::ThreadLocalRefCount>;

namespace pigeon {

template <typename T, AsRefCount R>
class Unretained {
 public:
  using Destructor = typename Shared<T, R>::Destructor;

  Unretained() = default;

  explicit Unretained(Shared<T, R>& ptr)
      : raw_ptr_(ptr.raw_ptr_),
        ref_cnt_(ptr.ref_cnt_),
        destructor_(ptr.destructor_) {
    if (ptr.unretained_ref_cnt_ == nullptr) {  // Lazy initialize
      ptr.unretained_ref_cnt_ = new R();
    } else {
      ptr.unretained_ref_cnt_->Increase();
    }
    unretained_ref_cnt_ = ptr.unretained_ref_cnt_;
  }

  Unretained(const Unretained& other)
      : raw_ptr_(other.raw_ptr_),
        ref_cnt_(other.ref_cnt_),
        unretained_ref_cnt_(other.unretained_ref_cnt_),
        destructor_(other.destructor_) {
    if (unretained_ref_cnt_ != nullptr) {
      unretained_ref_cnt_->Increase();
    }
  }

  Unretained(Unretained&& other) noexcept
      : raw_ptr_(other.raw_ptr_),
        ref_cnt_(other.ref_cnt_),
        unretained_ref_cnt_(other.unretained_ref_cnt_),
        destructor_(other.destructor_) {
    other.raw_ptr_ = nullptr;
    other.ref_cnt_ = nullptr;
    other.unretained_ref_cnt_ = nullptr;
  }

  ~Unretained() {
    if (unretained_ref_cnt_ != nullptr && !unretained_ref_cnt_->TryDecrease()) {
      delete ref_cnt_;
      delete unretained_ref_cnt_;
    }
  }

  Unretained& operator=(const Unretained& other) {
    if (this != &other) {
      this->~Unretained();
      new (this) Unretained(other);
    }
    return *this;
  }

  Unretained& operator=(Unretained&& other) noexcept {
    if (this != &other) {
      this->~Unretained();
      new (this) Unretained(std::move(other));
    }
    return *this;
  }

  Shared<T, R> TryUpgrade() const {
    if (ref_cnt_ != nullptr && ref_cnt_->Get() != 0) {
      ref_cnt_->Increase();
      return Shared<T, R>(raw_ptr_, ref_cnt_, unretained_ref_cnt_, destructor_);
    } else {
      return Shared<T, R>();
    }
  }

 private:
  T* raw_ptr_{nullptr};
  RefCount* ref_cnt_{nullptr};
  RefCount* unretained_ref_cnt_{nullptr};
  Destructor destructor_;
};

template <typename T>
using UnretainedLocal = Unretained<T, ThreadLocalRefCount>;

template <typename T>
using UnretainedAsync = Unretained<T, ThreadSafeRefCount>;

};  // namespace pigeon

#endif  // PIGEON_FRAMEWORK_BASE_AUTO_PTR_UNRETAINED
