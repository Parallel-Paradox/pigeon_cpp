#ifndef PIGEON_FRAMEWORK_BASE_CONTAINER_ARRAY
#define PIGEON_FRAMEWORK_BASE_CONTAINER_ARRAY

#include <concepts>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include "pigeon_framework/define.hpp"

namespace pigeon {

template <typename T>
class ArrayIterator {
 public:
  using iterator_concept = std::contiguous_iterator_tag;
  using iterator_category = std::random_access_iterator_tag;
  using iterator_type = ArrayIterator;
  using value_type = T;
  using difference_type = ptrdiff_t;
  using pointer = T*;
  using reference = T&;

  ArrayIterator() = default;

  ArrayIterator(const ArrayIterator& other) : ptr_(other.ptr_) {}

  ArrayIterator(pointer ptr) : ptr_(ptr) {}

  reference operator*() const { return *ptr_; }

  pointer operator->() const { return ptr_; }

  reference operator[](difference_type diff) const { return ptr_[diff]; }

  iterator_type& operator++() {
    ++ptr_;
    return *this;
  }

  iterator_type operator++(int) {
    iterator_type temp(*this);
    ++(*this);
    return temp;
  }

  iterator_type& operator--() {
    --ptr_;
    return *this;
  }

  iterator_type operator--(int) {
    iterator_type temp(*this);
    --(*this);
    return temp;
  }

  iterator_type& operator+=(difference_type diff) {
    ptr_ += diff;
    return *this;
  }

  iterator_type operator+(difference_type diff) const {
    iterator_type temp(*this);
    temp += diff;
    return temp;
  }

  iterator_type& operator-=(difference_type diff) {
    ptr_ -= diff;
    return *this;
  }

  iterator_type operator-(difference_type diff) const {
    iterator_type temp(*this);
    temp -= diff;
    return temp;
  }

  difference_type operator-(const iterator_type& other) const {
    return ptr_ - other.ptr_;
  }

  bool operator==(const iterator_type& other) const {
    return ptr_ == other.ptr_;
  }

  bool operator!=(const iterator_type& other) const {
    return !(*this == other);
  }

  bool operator<(const iterator_type& other) const { return ptr_ < other.ptr_; }

  bool operator>(const iterator_type& other) const { return other < *this; }

  bool operator>=(const iterator_type& other) const { return !(*this < other); }

  bool operator<=(const iterator_type& other) const { return !(other < *this); }

 private:
  pointer ptr_;
};

template <typename T>
ArrayIterator<T> operator+(typename ArrayIterator<T>::difference_type diff,
                           ArrayIterator<T> iter) {
  return iter + diff;
}

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
      for (size_t i = 0; i < size_; ++i) {
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
    for (size_t i = 0; i < size_; ++i) {
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

  ~Array() noexcept { Clear(); }

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
      for (size_t i = 0; i < size_; ++i) {
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
    for (size_t i = 0; i < size_; ++i) {
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

  size_t Size() const { return size_; }

  size_t Capacity() const { return capacity_; }

  using Iterator = ArrayIterator<T>;
  using ConstIterator = ArrayIterator<const T>;

  Iterator begin() { return Iterator(data_); }

  Iterator end() { return Iterator(data_ + size_); }

  ConstIterator begin() const { return ConstIterator(data_); }

  ConstIterator end() const { return ConstIterator(data_ + size_); }

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
