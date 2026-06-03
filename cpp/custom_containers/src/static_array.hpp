#pragma once

#include "utils/generic_array_container.hpp"

namespace custom_containers {

template <typename InternalType, std::size_t _Size, typename PublicType = InternalType>
class static_array : public generic_array_container<InternalType, PublicType> {

  using value_type = PublicType;

  using base_class = generic_array_container<InternalType, PublicType>;

private:
  InternalType _static_data[_Size];

public:
  // default ctor
  static_array() {
    this->_size = _Size;
    this->_data = this->_static_data;
  }

  virtual ~static_array() = default;

  // block copy
  static_array(const static_array& other) = delete;
  static_array& operator=(const static_array& other) = delete;

  // block move
  static_array(static_array&& other) = delete;
  static_array& operator=(static_array&& other) = delete;

  // // copy ctor
  // static_array(const static_array& other) {
  //   this->_data = other._data;
  // }

  // // copy operator=
  // static_array& operator=(const static_array& other) {
  //   if (&other == this) {
  //     return *this;
  //   }
  //   this->_data = other._data;
  //   return *this;
  // }

  // // move ctor
  // static_array(static_array&& other) : base_class(other) {
  //   std::swap(this->_static_data, other._static_data); // copy (-_-)
  //   this->_data = this->_static_data;
  //   other._data = other._static_data;
  // }

  // // move operator=
  // static_array& operator=(static_array&& other) {
  //   base_class::operator=(std::move(other));
  //   std::swap(_static_data, other._static_data); // copy (-_-)
  //   return *this;
  // }

};

} // namespace custom_containers
