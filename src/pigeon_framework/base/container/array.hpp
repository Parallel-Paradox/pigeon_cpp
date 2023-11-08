#ifndef PIGEON_FRAMEWORK_BASE_CONTAINER_ARRAY
#define PIGEON_FRAMEWORK_BASE_CONTAINER_ARRAY

#include <algorithm>
#include <concepts>
#include <initializer_list>
#include <iterator>
#include <stdexcept>

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

  explicit ArrayIterator(pointer ptr) : ptr_(ptr) {}

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
concept ArrayValue = std::default_initializable<T> && std::movable<T>;

template <ArrayValue T>
class Array {
 public:
  using Iterator = ArrayIterator<T>;
  using ConstIterator = ArrayIterator<const T>;

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
      ++size_;
    }
  }

  void EmplaceBack(T&& val) {
    EnsureNotFull();
    data_[size_] = std::move(val);
    ++size_;
  }

  template <typename... Args>
  void EmplaceBack(Args... args) {
    EnsureNotFull();
    (&data_[size_])->~T();  // Manual destroy before placement new.
    new (&data_[size_]) T(args...);
    ++size_;
  }

  void Reserve(size_t capacity) {
    if (capacity <= capacity_) {
      return;
    }
    SetCapacity(capacity);
  }

  void Resize(size_t size) {
    if (size == size_) {
      return;
    } else if (size > size_) {
      Reserve(size);
      size_ = size;
      return;
    }
    while (size_ > size) {
      PopBack();
    }
  }

  void ShrinkToFit() {
    if (size_ == capacity_) {
      return;
    }
    SetCapacity(size_);
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
    --size_;
    return std::move(data_[size_]);
  }

  void Swap(size_t index_a, size_t index_b) {
    if (index_a == index_b) {
      return;
    }
    T temp(std::move(data_[index_a]));
    data_[index_a] = std::move(data_[index_b]);
    data_[index_b] = std::move(temp);
  }

  void Insert(size_t index, const T& val) { Insert(index, T(val)); }

  void Insert(size_t index, T&& val) {
    EmplaceBack(std::move(val));
    for (size_t i = size_ - 1; i > index; --i) {
      Swap(i, i - 1);
    }
  }

  T Remove(size_t index) {
    for (size_t i = index; i < size_ - 1; ++i) {
      Swap(i, i + 1);
    }
    return PopBack();
  }

  T SwapRemove(size_t index) {
    Swap(index, size_ - 1);
    return PopBack();
  }

  bool IsEmpty() const { return size_ == 0; }

  size_t Size() const { return size_; }

  size_t Capacity() const { return capacity_; }

  void SetCapacity(size_t capacity) {
    size_t size = std::min(capacity, size_);

    T* new_data = new T[capacity];
    for (size_t i = 0; i < size; ++i) {
      new_data[i] = std::move(data_[i]);
    }
    if (data_ != nullptr) {
      delete[] data_;
    }
    data_ = new_data;
    size_ = size;
    capacity_ = capacity;
  }

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
