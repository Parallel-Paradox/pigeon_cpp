#ifndef PIGEON_FRAMEWORK_BASE_CONTAINER_ARRAY
#define PIGEON_FRAMEWORK_BASE_CONTAINER_ARRAY

#include <concepts>
#include <initializer_list>
#include <stdexcept>
#include "pigeon_framework/define.hpp"

namespace pigeon {

template <typename T>
requires std::movable<T> class Array {
 public:
  Array() = default;

  Array(std::initializer_list<T> list) {
    Reserve(list.size());
    for (const T& item : list) {
      PushBack(std::move(item));
    }
  }

  Array(const Array& other) {
    if constexpr (!std::copyable<T>) {
      throw std::invalid_argument("The type of array can't be copy.");
    } else {
      size_ = other.size_;
      capacity_ = other.capacity_;
      data_ = new T[capacity_];
      for (size_t i = 0; i < size_; i++) {
        data_[i] = other.data_[i];
      }
    }
  }

  Array& operator=(const Array& other) {
    if (this != &other) {
      this->~Array();
      new (this) Array(other);
    }
    return *this;
  }

  Array(Array&& other) noexcept {
    size_ = other.size_;
    capacity_ = other.capacity_;
    data_ = new T[capacity_];
    for (size_t i = 0; i < size_; i++) {
      data_[i] = std::move(other.data_[i]);
    }
    other.Clear();
  }

  Array& operator=(Array&& other) noexcept {
    if (this != &other) {
      this->~Array();
      new (this) Array(std::move(other));
    }
    return *this;
  }

  ~Array() { Clear(); }

  T& operator[](size_t index) { return data_[index]; }

  bool operator==(const Array& other) const {
    if (size_ != other.size_) {
      return false;
    }
    if (data_ == other.data_) {
      return true;
    }
    if constexpr (!std::equality_comparable<T>) {
      return false;
    } else {
      for (size_t i = 0; i < size_; i++) {
        if (data_[i] != other.data_[i]) {
          return false;
        }
      }
      return true;
    }
  }

  void PushBack(const T& val) {
    if constexpr (!std::copyable<T>) {
      throw std::invalid_argument("The type of array can't be copy.");
    } else {
      EnsureNotFull();
      data_[size_] = val;
      size_++;
    }
  }

  void EmplaceBack(T&& val) {
    EnsureNotFull();
    data_[size_] = std::move(val);
    size_++;
  }

  template <typename... Args>
  void EmplaceBack(Args... args) {
    EnsureNotFull();
    (&data_[size_])->~T();  // Manual destroy before placement new.
    new (&data_[size_]) T(args...);
    size_++;
  }

  void Reserve(size_t size) {
    if (size <= capacity_) {
      return;
    }
    T* new_data = new T[size];
    for (size_t i = 0; i < size_; i++) {
      new_data[i] = std::move(data_[i]);
    }
    if (data_ != nullptr) {
      delete[] data_;
    }
    data_ = new_data;
    capacity_ = size;
  }

  void Clear() {
    if (data_ != nullptr) {
      delete[] data_;
      data_ = nullptr;
    }
    size_ = 0;
    capacity_ = 0;
  }

  T PopBack() {
    if (IsEmpty()) {
      throw std::out_of_range("Try to pop from an empty array.");
    }
    size_--;
    return std::move(data_[size_]);
  }

  bool IsEmpty() const { return size_ == 0; }

  // TODO: iterator

 private:
  void EnsureNotFull() {
    if (capacity_ == 0) {
      capacity_ = 1;
      data_ = new T[1];
    } else if (size_ == capacity_) {
      Reserve(2 * capacity_);
    }
  }

  T* data_{nullptr};
  size_t size_{0};
  size_t capacity_{0};
};

}  // namespace pigeon

#endif  // PIGEON_FRAMEWORK_BASE_CONTAINER_ARRAY
