#ifndef PIGEON_FRAMEWORK_BASE_AUTO_PTR_SHARED
#define PIGEON_FRAMEWORK_BASE_AUTO_PTR_SHARED

#include <atomic>
#include <concepts>
#include <functional>
#include <utility>
#include "pigeon_framework/define.hpp"

#define INSTANTIATE_SHARED_ASYNC(ValueType)                  \
  template class PIGEON_API std::function<void(ValueType*)>; \
  template class PIGEON_API                                  \
      pigeon::Shared<ValueType, pigeon::ThreadSafeRefCount>;

#define INSTANTIATE_SHARED_LOCAL(ValueType)                  \
  template class PIGEON_API std::function<void(ValueType*)>; \
  template class PIGEON_API                                  \
      pigeon::Shared<ValueType, pigeon::ThreadLocalRefCount>;

namespace pigeon {

struct RefCount {
  virtual size_t Get() = 0;
  virtual void Increase() = 0;
  virtual bool TryDecrease() = 0;
  virtual ~RefCount() = default;
};

struct ThreadLocalRefCount : public RefCount {
  size_t cnt_{0};

  size_t Get() override { return cnt_; }

  void Increase() override { cnt_++; }

  bool TryDecrease() override {
    cnt_--;
    return cnt_ == 0 ? false : true;
  }

  ~ThreadLocalRefCount() override = default;
};

struct ThreadSafeRefCount : public RefCount {
  std::atomic_size_t cnt_{0};

  size_t Get() override { return cnt_.load(); }

  void Increase() override { cnt_.fetch_add(1); }

  bool TryDecrease() override { return cnt_.fetch_sub(1) == 1 ? false : true; }

  ~ThreadSafeRefCount() override = default;
};

template <typename R>
concept AsRefCount =
    std::default_initializable<R> && std::derived_from<R, RefCount>;

template <typename T, AsRefCount R>
class Unretained;

template <typename T, AsRefCount R>
class Shared {
 public:
  using Destructor = std::function<void(T*)>;

  Shared() = default;
  Shared(const Shared& other) = delete;

  Shared Clone() const {
    auto cloned = Shared(raw_ptr_, ref_cnt_, unretained_ref_cnt_, destructor_);
    if (ref_cnt_) {
      ref_cnt_->Increase();
    }
    return cloned;
  }

  Shared(Shared&& other) noexcept
      : raw_ptr_(other.raw_ptr_),
        ref_cnt_(other.ref_cnt_),
        unretained_ref_cnt_(other.unretained_ref_cnt_),
        destructor_(other.destructor_) {
    other.raw_ptr_ = nullptr;
    other.ref_cnt_ = nullptr;
    other.unretained_ref_cnt_ = nullptr;
    other.destructor_ = DefaultDestructor;
  }

  Shared(T* raw_ptr, Destructor destructor = DefaultDestructor)
      : raw_ptr_(raw_ptr),
        ref_cnt_(new R()),
        unretained_ref_cnt_(new R()),
        destructor_(destructor) {
    ref_cnt_->Increase();
  }

  template <typename... Args>
  static Shared New(Args&&... args) {
    return Shared(new T(args...));
  }

  Shared& operator=(const Shared& other) {
    if (this != &other) {
      this->~Shared();
      new (this) Shared(other.Clone());
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
    if (ref_cnt_ != nullptr && !ref_cnt_->TryDecrease()) {
      destructor_(raw_ptr_);
      if (unretained_ref_cnt_ == nullptr) {
        delete ref_cnt_;
      }
    }
  }

  T* operator->() const { return raw_ptr_; }

  T& operator*() const { return *raw_ptr_; }

  T* Get() const { return raw_ptr_; }

  bool IsNull() const { return raw_ptr_ == nullptr; }

  size_t RefCnt() const { return ref_cnt_->Get(); }

  size_t UnretainedRefCnt() const { return unretained_ref_cnt_->Get(); }

 private:
  friend class Unretained<T, R>;

  Shared(T* raw_ptr, RefCount* ref_cnt, RefCount* unretained_ref_cnt,
         Destructor destructor)
      : raw_ptr_(raw_ptr),
        ref_cnt_(ref_cnt),
        unretained_ref_cnt_(unretained_ref_cnt),
        destructor_(destructor) {}

  static void DefaultDestructor(T* raw_ptr) { delete raw_ptr; }

  T* raw_ptr_{nullptr};
  RefCount* ref_cnt_{nullptr};
  RefCount* unretained_ref_cnt_{nullptr};
  Destructor destructor_{DefaultDestructor};
};

template <typename T>
using SharedLocal = Shared<T, ThreadLocalRefCount>;

template <typename T>
using SharedAsync = Shared<T, ThreadSafeRefCount>;

}  // namespace pigeon

#endif  // PIGEON_FRAMEWORK_BASE_AUTO_PTR_SHARED
