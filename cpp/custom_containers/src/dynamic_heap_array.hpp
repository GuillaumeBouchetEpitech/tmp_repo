
#pragma once

#include "utils/generic_array_container.hpp"

namespace custom_containers {

template <typename InternalType,
          typename PublicType = InternalType,
          std::size_t initial_size = 0,
          typename Allocator = std::allocator<InternalType>>
class dynamic_heap_array : public generic_array_container<InternalType, PublicType> {

  using value_type = PublicType;
  using internal_type = InternalType;
  using base_class = generic_array_container<PublicType, InternalType>;

protected:
  using traits_t = std::allocator_traits<Allocator>; // The matching trait

protected:
  std::size_t _capacity = 0;

protected:
  // allocate memory only, will not call any constructor
  internal_type* allocate_memory(std::size_t size) {
    Allocator alloc;
    internal_type* newData = alloc.allocate(size);
    return newData;
  }

  // deallocate memory only, will not call any destructor
  void deallocate_memory(internal_type* data, std::size_t size) {
    Allocator alloc;
    alloc.deallocate(data, size);
  }

  // call the move constructor only, do not allocate memory
  void call_constructor(internal_type* dataPtr) {
    Allocator alloc;
    traits_t::construct(alloc, dataPtr);
  }

  // call the move constructor only, do not allocate memory
  void call_copy_constructor(internal_type* dataPtr, const internal_type& value) {
    Allocator alloc;
    traits_t::construct(alloc, dataPtr, value);
  }

  // call the move constructor only, do not allocate memory
  void call_move_constructor(internal_type* dataPtr, internal_type&& value) {
    Allocator alloc;
    traits_t::construct(alloc, dataPtr, std::move(value));
  }

  // call the move constructor only, do not allocate memory
  template <typename... Args> internal_type& emplace_move_constructor(internal_type* dataPtr, Args&&... args) {
    Allocator alloc;
    traits_t::construct(alloc, dataPtr, std::forward<Args>(args)...);
    return *dataPtr;
  }

  // call the destructor only, do not deallocate memory
  void call_destructor(internal_type* dataPtr) {
    Allocator alloc;
    traits_t::destroy(alloc, dataPtr);
  }

public:
  dynamic_heap_array() {
    if (initial_size > 0) {
      pre_allocate(initial_size);
    }
  }

  // disable copy
  dynamic_heap_array(const dynamic_heap_array& other) = delete;
  dynamic_heap_array& operator=(const dynamic_heap_array& other) = delete;
  // disable copy

  dynamic_heap_array(dynamic_heap_array&& other) : base_class(std::move(other)) {
    std::swap(_capacity, other._capacity);
  }

  dynamic_heap_array& operator=(dynamic_heap_array&& other) {
    base_class::operator=(std::move(other));
    std::swap(_capacity, other._capacity);
    return *this;
  }

  virtual ~dynamic_heap_array() {
    clear();
    deallocate_memory(this->_data, _capacity);
  }

public:
  // may reallocate
  void push_back(const value_type& value) {
    if (this->_size == this->_capacity) {
      _realloc(this->_capacity * 2);
    }

    call_copy_constructor(this->_data + this->_size, reinterpret_cast<const internal_type&>(value));
    ++this->_size;
  }

  // may reallocate
  void push_back(value_type&& value) {
    if (this->_size == this->_capacity) {
      _realloc(this->_capacity * 2);
    }

    call_move_constructor(this->_data + this->_size, std::move(reinterpret_cast<internal_type&&>(value)));
    ++this->_size;
  }

  // may reallocate
  template <typename... Args> value_type& emplace_back(Args&&... args) {
    if (this->_size == this->_capacity) {
      _realloc(this->_capacity * 2);
    }

    value_type& result = emplace_move_constructor(this->_data + this->_size, std::forward<Args>(args)...);
    ++this->_size;

    return result;
  }

public:
  void pop_back() {
    if (this->_size == 0) {
      return;
    }

    --this->_size;

    call_destructor(this->_data + this->_size);
  }

  // no reallocation
  uint32_t unsorted_erase(std::size_t inIndex) {
    if (this->is_out_of_range(inIndex)) {
      return 0;
    }

    uint32_t totalSwapped = 0;

    // swap data at the end
    if (this->_size > 1 && inIndex < this->_size - 1) {
      std::swap(this->_data[inIndex], this->_data[this->_size - 1]);
      ++totalSwapped;
    }

    // remove the back
    pop_back();
    return totalSwapped;
  }

  // no reallocation
  uint32_t sorted_erase(std::size_t inIndex) {
    if (this->is_out_of_range(inIndex)) {
      return 0;
    }

    uint32_t totalSwapped = 0;

    if (this->_size > 1 && inIndex + 1 < this->_size) {

      // swap data at the end
      for (std::size_t ii = inIndex; ii + 1 < this->_size; ++ii) {
        std::swap(this->_data[ii], this->_data[ii + 1]);
        ++totalSwapped;
      }
    }

    // remove the back
    pop_back();
    return totalSwapped;
  }

public:
  void clear() {
    for (std::size_t ii = 0; ii < this->_size; ++ii) {
      call_destructor(this->_data + ii);
    }

    this->_size = 0;
  }

  void ensure_size(std::size_t target_size) {
    if (target_size < this->_size) {
      return;
    }

    _realloc(target_size);

    for (std::size_t ii = this->_size; ii < target_size; ++ii) {
      call_constructor(this->_data + ii);
    }

    this->_size = target_size;
  }

  void pre_allocate(std::size_t capacity) { _realloc(capacity); }

public:
  std::size_t capacity() const { return this->_capacity; }

protected:
  void _realloc(std::size_t newCapacity) {

    // true the first time (when not pre-allocated)
    if (newCapacity == 0) {
      newCapacity = 1;
    }

    if (newCapacity <= this->_capacity) {
      return;
    }

    internal_type* newData = allocate_memory(newCapacity);

    // move the existing data to the new memory
    for (std::size_t ii = 0; ii < this->_size; ++ii) {
      call_move_constructor(newData + ii, std::move(this->_data[ii]));
    }

    // call the dtor on the old memory (which just got moved)
    for (std::size_t ii = 0; ii < this->_size; ++ii) {
      call_destructor(this->_data + ii);
    }

    // deallocate the old memory
    if (this->_capacity > 0) {
      deallocate_memory(this->_data, this->_capacity);
    }

    // replace the old memory/capacity with the new one
    this->_data = newData;
    this->_capacity = newCapacity;
  }
};

} // namespace gero
