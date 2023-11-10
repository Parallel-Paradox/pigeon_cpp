#ifndef PIGEON_FRAMEWORK_BASE_AUTO_PTR_UNRETAINED
#define PIGEON_FRAMEWORK_BASE_AUTO_PTR_UNRETAINED

#include "pigeon_framework/base/auto_ptr/shared.hpp"

namespace pigeon {

template <typename T>
class Unretained {
 public:
  using Destructor = typename Shared<T>::Destructor;
  using RefCount = typename Shared<T>::RefCount;

  Unretained() = default;

  explicit Unretained(const Shared<T>& ptr)
      : raw_ptr_(ptr.raw_ptr_),
        ref_cnt_(ptr.ref_cnt_),
        destructor_(ptr.destructor_) {}

  ~Unretained() = default;

  Unretained& operator=(const Shared<T>& ptr) {
    new (this) Unretained(ptr);
    return *this;
  }

  Shared<T> TryUpgrade() const {
    if (ref_cnt_ != nullptr) {
      ref_cnt_->Increase();
    }
    return std::move(Shared(raw_ptr_, ref_cnt_, destructor_));
  }

 private:
  T* raw_ptr_{nullptr};
  RefCount* ref_cnt_{nullptr};
  Destructor destructor_;
};

};  // namespace pigeon

#endif  // PIGEON_FRAMEWORK_BASE_AUTO_PTR_UNRETAINED
