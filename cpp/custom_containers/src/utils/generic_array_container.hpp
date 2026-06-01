
#pragma once

// #include "utils/basic_double_linked_list.hpp"

#include <functional>
#include <memory>
#include <string>

//
//
//
//
//

//
//
//
//
//

//
//
//
//
//

namespace custom_containers {

//
//
//
//
//

//
//
//
//
//

//
//
//
//
//

//MARK: base_iterator
template <typename generic_array_container>
class generic_array_container_base_iterator
{

  friend generic_array_container;

public:
  // -> https://www.fluentcpp.com/2018/05/08/std-iterator-deprecated/

  using value_type = typename generic_array_container::value_type;

  using iterator_category = std::random_access_iterator_tag;
  // using value_type = int;
  using difference_type = int;
  using pointer = value_type*;
  using reference = value_type&;

  // // using difference_type = typename std::iterator<std::random_access_iterator_tag, value_type>::difference_type;
  // using difference_type = int64_t;

protected:
  generic_array_container* _container;
  int _index;
  bool _forward;

public:
  generic_array_container_base_iterator(generic_array_container& container, int index, bool forward)
    : _container(&container), _index(index), _forward(forward) {
  }

  // COPY
  generic_array_container_base_iterator(const generic_array_container_base_iterator& other) {
    if (&other == this)
      return;

    _container = other._container;
    _index = other._index;
    _forward = other._forward;
  }

  // MOVE
  generic_array_container_base_iterator(generic_array_container_base_iterator&& other) {
    if (&other == this)
      return;

    std::swap(_container, other._container);
    std::swap(_index, other._index);
    std::swap(_forward, other._forward);
  }

  // COPY
  generic_array_container_base_iterator& operator=(const generic_array_container_base_iterator& other) {
    if (&other == this)
      return *this;

    _container = other._container;
    _index = other._index;
    _forward = other._forward;

    return *this;
  }

  // MOVE
  generic_array_container_base_iterator& operator=(generic_array_container_base_iterator&& other) {
    if (&other == this)
      return *this;

    std::swap(_container, other._container);
    std::swap(_index, other._index);
    std::swap(_forward, other._forward);

    return *this;
  }

  ~generic_array_container_base_iterator() {}

public:
  bool is_valid() const { return _container != nullptr; }

protected:
  void _ensure_is_valid() const {
    if (!is_valid()) {
      throw std::runtime_error("invalid iterator");
    }
  }

public:
  bool operator==(const generic_array_container_base_iterator& rhs) const {
    return (_container == rhs._container && _index == rhs._index);
  }
  bool operator<(const generic_array_container_base_iterator& rhs) const { return _index < rhs._index; }

#if 0
  bool operator!=(const generic_array_container_base_iterator& rhs) const { return !(*this == rhs); }
  bool operator>(const generic_array_container_base_iterator& rhs) const { return _index > rhs._index; }
  bool operator<=(const generic_array_container_base_iterator& rhs) const { return _index <= rhs._index; }
  bool operator>=(const generic_array_container_base_iterator& rhs) const { return _index >= rhs._index; }
#else
  bool operator!=(const generic_array_container_base_iterator& rhs) const {
    return !generic_array_container_base_iterator::operator==(rhs);
  }
  bool operator>(const generic_array_container_base_iterator& rhs) const {
    return (!generic_array_container_base_iterator::operator<(rhs) &&
            !generic_array_container_base_iterator::operator==(rhs));
  }
  bool operator<=(const generic_array_container_base_iterator& rhs) const {
    return !generic_array_container_base_iterator::operator>(rhs);
  }
  bool operator>=(const generic_array_container_base_iterator& rhs) const {
    return !generic_array_container_base_iterator::operator<(rhs);
  }
#endif

public:
  difference_type operator-(const generic_array_container_base_iterator& rhs) const { return _index - rhs._index; }
  difference_type operator+(const generic_array_container_base_iterator& rhs) const { return _index + rhs._index; }

  generic_array_container_base_iterator& operator+=(difference_type rhs) {
    _index += rhs;
    return *this;
  }
  generic_array_container_base_iterator& operator-=(difference_type rhs) {
    _index -= rhs;
    return *this;
  }
};

//
//
//
//
//

//
//
//
//
//

//
//
//
//
//

//MARK: iterator
template <typename generic_array_container>
class generic_array_container_iterator : public generic_array_container_base_iterator<generic_array_container> {

  friend generic_array_container;

public:
  using base_type = generic_array_container_base_iterator<generic_array_container>;
  using value_type = typename base_type::value_type;

public:
  generic_array_container_iterator(generic_array_container& container, int index, bool forward) : base_type(container, index, forward) {}

public:
  value_type& operator[](int index) {
    base_type::_ensure_is_valid();
    return (*base_type::_container)[base_type::_index + index];
  }
  const value_type& operator[](int index) const {
    base_type::_ensure_is_valid();
    return (*base_type::_container)[base_type::_index + index];
  }
  value_type* operator->() {
    base_type::_ensure_is_valid();
    return &((*base_type::_container)[base_type::_index]);
  }
  const value_type* operator->() const {
    base_type::_ensure_is_valid();
    return &((*base_type::_container)[base_type::_index]);
  }
  value_type& operator*() {
    base_type::_ensure_is_valid();
    return (*base_type::_container)[base_type::_index];
  }
  const value_type& operator*() const {
    base_type::_ensure_is_valid();
    return (*base_type::_container)[base_type::_index];
  }

public:
  generic_array_container_iterator operator+(typename base_type::difference_type rhs) const {
    base_type::_ensure_is_valid();
    generic_array_container_iterator copy = *this;
    copy._index += int(rhs);
    return copy;
  }
  generic_array_container_iterator operator-(typename base_type::difference_type rhs) const {
    base_type::_ensure_is_valid();
    generic_array_container_iterator copy = *this;
    copy._index -= int(rhs);
    return copy;
  }

  //
  //

  friend inline generic_array_container_iterator operator+(typename base_type::difference_type lhs,
                                                           const generic_array_container_iterator& rhs) {
    base_type::_ensure_is_valid();
    generic_array_container_iterator copy = rhs;
    copy._index = lhs + rhs._index;
    return copy;
  }

  friend inline generic_array_container_iterator operator-(typename base_type::difference_type lhs,
                                                           const generic_array_container_iterator& rhs) {
    base_type::_ensure_is_valid();
    generic_array_container_iterator copy = rhs;
    copy._index = lhs - rhs._index;
    return copy;
  }

  //
  //

  generic_array_container_iterator operator++() // ++pre
  {
    base_type::_ensure_is_valid();
    // if (base_type::_index < int(base_type::_container->size()))
    if (base_type::_forward) {
      ++base_type::_index;
    } else {
      --base_type::_index;
    }
    return *this;
  }

  generic_array_container_iterator operator--() // --pre
  {
    base_type::_ensure_is_valid();
    // if (base_type::_index > 0)
    if (base_type::_forward) {
      --base_type::_index;
    } else {
      ++base_type::_index;
    }
    return *this;
  }

  //
  //

  generic_array_container_iterator operator++(int) // post++
  {
    base_type::_ensure_is_valid();
    generic_array_container_iterator copy = *this;
    ++(*this); // reuse ++pre
    return copy;
  }

  generic_array_container_iterator operator--(int) // post--
  {
    base_type::_ensure_is_valid();
    generic_array_container_iterator copy = *this;
    --(*this); // reuse --pre
    return copy;
  }

  //
  //

  typename base_type::difference_type operator-(const generic_array_container_iterator& rhs) const {
    base_type::_ensure_is_valid();
    return base_type::_index - rhs._index;
  }
};

//
//
//
//
//

//
//
//
//
//

//
//
//
//
//

//MARK: const_iterator
template <typename generic_array_container>
class generic_array_container_const_iterator : public generic_array_container_base_iterator<generic_array_container> {

  friend generic_array_container;

public:
  using base_type = generic_array_container_base_iterator<generic_array_container>;
  using value_type = typename base_type::value_type;

public:
  generic_array_container_const_iterator(generic_array_container& container, int index, bool forward) : base_type(container, index, forward) {}

public:
  const value_type& operator[](int index) const {
    base_type::_ensure_is_valid();
    return (*base_type::_container)[base_type::_index + index];
  }
  const value_type* operator->() const {
    base_type::_ensure_is_valid();
    return &((*base_type::_container)[base_type::_index]);
  }
  const value_type& operator*() const {
    base_type::_ensure_is_valid();
    return (*base_type::_container)[base_type::_index];
  }

public:
  generic_array_container_const_iterator operator+(int value) const {
    generic_array_container_const_iterator copy = *this;
    copy._index += value;
    return copy;
  }
  generic_array_container_const_iterator operator-(int value) const {
    generic_array_container_const_iterator copy = *this;
    copy._index -= value;
    return copy;
  }

  friend inline generic_array_container_const_iterator operator+(typename base_type::difference_type lhs,
                                                                 const generic_array_container_const_iterator& rhs) {
    // generic_array_container_const_iterator copy = *this;
    generic_array_container_const_iterator copy = rhs;
    copy._index = lhs + rhs._index;
    return copy;
    // return generic_array_container_const_iterator(lhs+rhs._ptr);
  }

  friend inline generic_array_container_const_iterator operator-(typename base_type::difference_type lhs,
                                                                 const generic_array_container_const_iterator& rhs) {
    // generic_array_container_const_iterator copy = *this;
    generic_array_container_const_iterator copy = rhs;
    copy._index = lhs - rhs._index;
    return copy;
    // return generic_array_container_const_iterator(lhs-rhs._ptr);
  }

  generic_array_container_const_iterator operator++() const // ++pre
  {
    // _ensure_is_valid();
    // if (base_type::_index < int(base_type::_container->size())) {

    // ++const_cast<generic_array_container_const_iterator*>(this)->base_type::_index;
    if (base_type::_forward) {
      ++const_cast<generic_array_container_const_iterator*>(this)->base_type::_index;
    } else {
      --const_cast<generic_array_container_const_iterator*>(this)->base_type::_index;
    }

    // }
    return *this;
  }
  generic_array_container_const_iterator operator--() const // --pre
  {
    // _ensure_is_valid();
    // if (base_type::_index > 0) {

    // --const_cast<generic_array_container_const_iterator*>(this)->base_type::_index;
    if (base_type::_forward) {
      --const_cast<generic_array_container_const_iterator*>(this)->base_type::_index;
    } else {
      ++const_cast<generic_array_container_const_iterator*>(this)->base_type::_index;
    }

    // }
    return *this;
  }

  generic_array_container_const_iterator operator++(int) // post++
  {
    generic_array_container_const_iterator copy = *this;
    ++(*this);
    return copy;
  }

  generic_array_container_const_iterator operator--(int) // post--
  {
    generic_array_container_iterator copy = *this;
    --(*this);
    return copy;
  }
};

//
//
//
//
//

//
//
//
//
//

//
//
//
//
//

//MARK: interface
template <typename PublicType>
class interface_generic_array_container {

public:
  using value_type = PublicType;
  using base_iterator = generic_array_container_base_iterator<interface_generic_array_container<value_type>>;
  using iterator = generic_array_container_iterator<interface_generic_array_container<value_type>>;
  using const_iterator = generic_array_container_const_iterator<interface_generic_array_container<value_type>>;

public:
  virtual ~interface_generic_array_container() = default;

public:
  virtual bool is_empty() const = 0;
  virtual std::size_t size() const = 0;
  virtual bool is_out_of_range(std::size_t index) const = 0;

public:
  // support out of range index (negative values included)
  virtual const value_type& operator[](int index) const = 0;
  virtual value_type& operator[](int index) = 0;

  virtual const value_type& at(std::size_t index) const = 0;
  virtual value_type& at(std::size_t index) = 0;

public:
  virtual const value_type& front() const = 0;
  virtual value_type& front() = 0;

  virtual const value_type& back() const = 0;
  virtual value_type& back() = 0;

public:
  virtual iterator begin() = 0;
  virtual iterator end() = 0;

  virtual const_iterator begin() const = 0;
  virtual const_iterator end() const = 0;

public:
  virtual iterator rbegin() = 0;
  virtual iterator rend() = 0;

  virtual const_iterator rbegin() const = 0;
  virtual const_iterator rend() const = 0;

  // public:
  //   virtual bool operator==(const generic_array_container& other) const = 0;
  //   virtual bool operator!=(const generic_array_container& other) const = 0;
};

//
//
//
//
//

//MARK: implementation
template <typename InternalType, typename PublicType = InternalType>
class generic_array_container : public interface_generic_array_container<PublicType> {

public:
  using value_type = PublicType;
  using internal_type = InternalType;
  using base_iterator = generic_array_container_base_iterator<interface_generic_array_container<value_type>>;
  using iterator = generic_array_container_iterator<interface_generic_array_container<value_type>>;
  using const_iterator = generic_array_container_const_iterator<interface_generic_array_container<value_type>>;

protected:
  friend base_iterator;
  friend iterator;
  friend const_iterator;

protected:
  std::size_t _size = 0;
  internal_type* _data = nullptr;

public:
  generic_array_container() = default;

  // disable copy
  generic_array_container(const generic_array_container& other) = delete;
  generic_array_container& operator=(const generic_array_container& other) = delete;
  // disable copy

  generic_array_container(generic_array_container&& other) {
    std::swap(_size, other._size);
    std::swap(_data, other._data);
  }

  generic_array_container& operator=(generic_array_container&& other) {
    std::swap(_size, other._size);
    std::swap(_data, other._data);
    return *this;
  }

  virtual ~generic_array_container() = default;

protected:
  void _ensure_not_empty() const {
    if (_size == 0) {
      throw std::runtime_error("empty array");
    }
  }

  std::size_t _get_index(int index) const {
    _ensure_not_empty();
    if (index < 0) {
      index = int(_size) - (-index) % int(_size);
    }
    if (index >= int(_size)) {
      index = index % int(_size);
    }
    return std::size_t(index);
  }

  // std::size_t _get_index(std::size_t index) const
  // {
  //   return std::size_t(index) % _size;
  // }

public:
  iterator begin() override { return iterator(*this, 0, true); }
  iterator end() override { return iterator(*this, int(_size), true); }

  const_iterator begin() const override { return const_iterator(*const_cast<generic_array_container*>(this), 0, true); }
  const_iterator end() const override { return const_iterator(*const_cast<generic_array_container*>(this), int(_size), true); }

public:
  iterator rbegin() override { return iterator(*this, int(_size) - 1, false); }
  iterator rend() override { return iterator(*this, -1, false); }

  const_iterator rbegin() const override { return const_iterator(*const_cast<generic_array_container*>(this), int(_size) - 1, false); }
  const_iterator rend() const override { return const_iterator(*const_cast<generic_array_container*>(this), -1, false); }

public:
  bool is_empty() const override { return _size == 0; }
  std::size_t size() const override { return _size; }
  bool is_out_of_range(std::size_t index) const override { return (index >= _size); }

public:
  // support out of range index (negative values included)
  const value_type& operator[](int index) const override { return _data[_get_index(index)]; }
  value_type& operator[](int index) override { return _data[_get_index(index)]; }

  const value_type& at(std::size_t index) const override {
    if (is_out_of_range(index)) {
      throw std::runtime_error("out of range");
    }
    return _data[index];
  }
  value_type& at(std::size_t index) override {
    if (is_out_of_range(index)) {
      throw std::runtime_error("out of range");
    }
    return _data[index];
  }

  const value_type& front() const override {
    _ensure_not_empty();
    return _data[0];
  }
  value_type& front() override {
    _ensure_not_empty();
    return _data[0];
  }

  const value_type& back() const override {
    _ensure_not_empty();
    return _data[_size - 1];
  }
  value_type& back() override {
    _ensure_not_empty();
    return _data[_size - 1];
  }

public:
  bool operator==(const generic_array_container& other) const { return this == &other; }
  bool operator!=(const generic_array_container& other) const { return !(*this == other); }
};

//
//
//
//
//

//
//
//
//
//

//
//
//
//
//

} // namespace custom_containers
