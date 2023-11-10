#ifndef PIGEON_FRAMEWORK_BASE_AUTO_PTR_SHARED
#define PIGEON_FRAMEWORK_BASE_AUTO_PTR_SHARED

#include <functional>
#include <utility>

namespace pigeon {

template <typename T>
class Unretained;

template <typename T>
class Shared {
 public:
  using Destructor = std::function<void(T*)>;

  struct RefCount {
    virtual size_t Get() = 0;
    virtual void Increase() = 0;
    virtual bool TryDecrease() = 0;
    virtual ~RefCount() = default;
  };

  struct ThreadLocalRefCount : public RefCount {
    size_t cnt_{1};

    size_t Get() override { return cnt_; }

    void Increase() override { cnt_++; }

    bool TryDecrease() override {
      cnt_--;
      return cnt_ == 0 ? false : true;
    }

    ~ThreadLocalRefCount() override = default;
  };

  struct ThreadSafeRefCount : public RefCount {
    std::atomic_size_t cnt_{1};

    size_t Get() override { return cnt_.load(); }

    void Increase() override { cnt_.fetch_add(1); }

    bool TryDecrease() override {
      return cnt_.fetch_sub(1) == 1 ? false : true;
    }

    ~ThreadSafeRefCount() = default;
  };

  Shared() = default;

  Shared(const Shared& other)
      : raw_ptr_(other.raw_ptr_),
        ref_cnt_(other.ref_cnt_),
        destructor_(other.destructor_) {
    if (ref_cnt_ != nullptr) {
      ref_cnt_->Increase();
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
    if (!ref_cnt_->TryDecrease()) {
      destructor_(raw_ptr_);
      delete ref_cnt_;
    }
  }

  static Shared ByThreadLocal(T* raw_ptr,
                              Destructor destructor = DefaultDestructor) {
    return std::move(Shared(raw_ptr, new ThreadLocalRefCount(), destructor));
  }

  template <typename... Args>
  static Shared ByThreadLocal(Args... args,
                              Destructor destructor = DefaultDestructor) {
    return std::move(
        Shared(new T(args...), new ThreadLocalRefCount(), destructor));
  }

  static Shared ByThreadSafe(T* raw_ptr,
                             Destructor destructor = DefaultDestructor) {
    return std::move(Shared(raw_ptr, new ThreadSafeRefCount(), destructor));
  }

  template <typename... Args>
  static Shared ByThreadSafe(Args... args,
                             Destructor destructor = DefaultDestructor) {
    return std::move(
        Shared(new T(args...), new ThreadSafeRefCount(), destructor));
  }

  T* operator->() const { return raw_ptr_; }

  T& operator*() const { return *raw_ptr_; }

  T* Get() const { return raw_ptr_; }

  bool IsNull() const { return raw_ptr_ == nullptr; }

  size_t RefCnt() const { return ref_cnt_->Get(); }

 private:
  friend class Unretained<T>;

  Shared(T* raw_ptr, RefCount* ref_cnt, Destructor destructor)
      : raw_ptr_(raw_ptr), ref_cnt_(ref_cnt), destructor_(destructor) {}

  static void DefaultDestructor(T* raw_ptr) { delete raw_ptr; }

  T* raw_ptr_{nullptr};
  RefCount* ref_cnt_{nullptr};
  Destructor destructor_;
};

}  // namespace pigeon

#endif  // PIGEON_FRAMEWORK_BASE_AUTO_PTR_SHARED
