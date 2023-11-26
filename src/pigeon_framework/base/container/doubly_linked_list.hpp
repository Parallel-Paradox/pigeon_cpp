#ifndef PIGEON_FRAMEWORK_BASE_CONTAINER_DOUBLY_LINKED_LIST
#define PIGEON_FRAMEWORK_BASE_CONTAINER_DOUBLY_LINKED_LIST

#include <concepts>
#include <iterator>
#include <stdexcept>
#include "pigeon_framework/base/auto_ptr/shared.hpp"
#include "pigeon_framework/base/auto_ptr/unretained.hpp"

namespace pigeon {

template <typename T>
concept ListValue = std::default_initializable<T> && std::movable<T>;

template <ListValue T>
struct DoublyLinkedListNode {
  T item_;
  DoublyLinkedListNode* prev_{nullptr};
  DoublyLinkedListNode* next_{nullptr};

  DoublyLinkedListNode() = default;

  DoublyLinkedListNode(T&& item) : item_(std::move(item)) {}

  DoublyLinkedListNode(const T& item) {
    if constexpr (!std::copyable<T>) {
      throw std::invalid_argument("This type is supposed to be copyable.");
    } else {
      item_ = item;
    }
  }
};

template <ListValue T>
class DoublyLinkedList;

template <typename T>
class DoublyLinkedListConstIterator {
 public:
  using iterator_concept = std::bidirectional_iterator_tag;
  using iterator_category = std::bidirectional_iterator_tag;
  using iterator_type = DoublyLinkedListConstIterator;
  using value_type = const T;
  using difference_type = size_t;
  using pointer = const T*;
  using reference = const T&;

  friend class DoublyLinkedList<T>;

  DoublyLinkedListConstIterator() = default;

 protected:
  DoublyLinkedListNode<T>* here{nullptr};
};

template <typename T>
class DoublyLinkedListIterator : public DoublyLinkedListConstIterator<T> {
 public:
  using value_type = T;
  using pointer = T*;
  using reference = T&;

  friend class DoublyLinkedList<T>;
};

template <ListValue T>
class DoublyLinkedList {
 public:
  using Node = DoublyLinkedListNode<T>;
  using ConstIterator = DoublyLinkedListConstIterator<T>;
  using Iterator = DoublyLinkedListIterator<T>;

 private:
  Node* head_{nullptr};
  Node* tail_{nullptr};
  size_t size{0};
};

}  // namespace pigeon

#endif  // PIGEON_FRAMEWORK_BASE_CONTAINER_DOUBLY_LINKED_LIST
