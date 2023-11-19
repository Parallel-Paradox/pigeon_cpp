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
  using RefCount = typename Shared<T, R>::RefCount;

  Unretained() = default;

  explicit Unretained(const Shared<T, R>& ptr)
      : raw_ptr_(ptr.raw_ptr_),
        ref_cnt_(ptr.ref_cnt_),
        destructor_(ptr.destructor_) {}

  ~Unretained() = default;

  Unretained& operator=(const Shared<T, R>& ptr) {
    new (this) Unretained(ptr);
    return *this;
  }

  Shared<T, R> TryUpgrade() const {
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

template <typename T>
using UnretainedLocal = Unretained<T, ThreadLocalRefCount>;

template <typename T>
using UnretainedAsync = Unretained<T, ThreadSafeRefCount>;

};  // namespace pigeon

#endif  // PIGEON_FRAMEWORK_BASE_AUTO_PTR_UNRETAINED
