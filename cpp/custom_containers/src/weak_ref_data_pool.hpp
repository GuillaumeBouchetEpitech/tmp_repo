
#pragma once

#include "utils/basic_double_linked_list.hpp"
#include "dynamic_heap_array.hpp"

#include <functional>

//
//
//

namespace custom_containers {
namespace weak_ref_data_pool {

//
//
//

//forward declaration
template <typename InternalBaseType,
          typename PublicBaseType = InternalBaseType,
          std::size_t initial_size = 256,
          bool no_realloc = true,
          template <typename...> class Allocator = std::allocator
>
class pool_container;

//
//
//

namespace internals {

//
//
//

namespace base_class {

//
//
//

//MARK:non_movable
template<typename provided_base_class>
class non_movable: public provided_base_class {
public:
  // block copy
  non_movable(const non_movable& other) = delete;
  non_movable& operator=(const non_movable& other) = delete;

  // block move semantic
  non_movable(non_movable&& other) = delete;
  non_movable& operator=(non_movable&& other) = delete;

  virtual ~non_movable() = default;
};

//
//
//

}; // namespace base_class

//
//
//

//MARK: internal_data
template <typename InternalBaseType,
          typename PublicBaseType /*= InternalBaseType*/,
          std::size_t initial_size /*= 256*/,
          bool no_realloc /*= true*/,
          template <typename...> class Allocator /*= std::allocator*/
>
struct pool_internal_element
  : public InternalBaseType
  // : public base_class::non_copyable<InternalBaseType>
{
public:

  using pool_type = pool_container<InternalBaseType, PublicBaseType, initial_size, no_realloc, Allocator>;
  using weak_ref = pool_type::weak_ref;

  friend weak_ref;

public:
  int _index = -1;
  bool _is_valid = false;
  basic_double_linked_list _weak_ref_list;

public:
  using internal_base_type = InternalBaseType;
  using internal_base_type::internal_base_type; // reuse parent internal_base_type  class ctor(s)

public:
  pool_internal_element(const pool_internal_element& other) = delete; // block copy
  pool_internal_element(pool_internal_element&& other) : internal_base_type(std::move(other)) {
    if (&other == this) {
      return;
    }
    std::swap(_index, other._index);
    std::swap(_is_valid, other._is_valid);
    std::swap(_weak_ref_list.head_link, other._weak_ref_list.head_link);
    std::swap(_weak_ref_list.size, other._weak_ref_list.size);
    sync_all_ref_index();
  }

  virtual ~pool_internal_element() { invalidate_all_ref(); }

public:
  pool_internal_element& operator=(const pool_internal_element& other) = delete; // block copy
  pool_internal_element& operator=(pool_internal_element&& other) {
    if (&other == this) {
      return *this;
    }

    internal_base_type::operator=(std::move(other));

    std::swap(_index, other._index);
    std::swap(_is_valid, other._is_valid);
    std::swap(_weak_ref_list.head_link, other._weak_ref_list.head_link);
    std::swap(_weak_ref_list.size, other._weak_ref_list.size);
    sync_all_ref_index();
    return *this;
  }

public:
  void invalidate_all_ref() {
    _weak_ref_list.loop_and_reset<weak_ref>([](weak_ref* currRef) {
      if (currRef) {
        currRef->_pool = nullptr;
        currRef->_index = -1;
      }
    });
  }

  void sync_all_ref_index() {
    _weak_ref_list.loop<weak_ref>([this](weak_ref* currRef) {
      if (currRef) {
        currRef->_index = _index;
      }
    });
  }

  void sync_all_ref_pool(pool_type* pool) {
    _weak_ref_list.loop<weak_ref>([pool](weak_ref* currRef) {
      if (currRef) {
        currRef->_pool = pool;
      }
    });
  }

public:
  basic_double_linked_list& get_weak_ref_list() { return _weak_ref_list; }
  bool is_valid() { return _is_valid; }
};

//
//
//

};

//
//
//

//MARK: pool_weak_ref
template <typename InternalBaseType,
          typename PublicBaseType /*= InternalBaseType*/,
          std::size_t initial_size /*= 256*/,
          bool no_realloc /*= true*/,
          template <typename...> class Allocator /*= std::allocator*/
>
struct pool_weak_ref {
  using value_type = internals::base_class::non_movable<PublicBaseType>;
  using pool_type = pool_container<InternalBaseType, PublicBaseType, initial_size, no_realloc, Allocator>;
  using internal_data = internals::pool_internal_element<InternalBaseType, PublicBaseType, initial_size, no_realloc, Allocator>;

  friend pool_type;
  friend internal_data;

private:
  pool_type* _pool = nullptr;
  int32_t _index = -1;

  basic_double_linked_list::link _link;

public:
  explicit pool_weak_ref() = default;

private:
  explicit pool_weak_ref(pool_type* inPool, int32_t inIndex) {
    // CTOR

    if (inPool == nullptr || inPool->_is_out_of_range(std::size_t(inIndex))) {
      return;
    }

    _pool = inPool;
    _index = inIndex;
    _link.user_data = static_cast<void*>(this);

    basic_double_linked_list& list = _pool->_get_itemsPool_data_by_index(std::size_t(_index)).get_weak_ref_list();
    list.add_link(_link);
  }

public:
  // COPY
  pool_weak_ref(const pool_weak_ref& other) {
    if (&other == this) {
      return;
    }
    _doCopy(other);
  }

  // MOVE
  pool_weak_ref(pool_weak_ref&& other) {
    if (&other == this) {
      return;
    }
    _doMove(std::move(other));
  }

  virtual ~pool_weak_ref() {
    invalidate();
  }

public:
  // COPY
  pool_weak_ref& operator=(const pool_weak_ref& other) {
    if (&other == this) {
      return *this;
    }
    _doCopy(other);
    return *this;
  }

  // MOVE
  pool_weak_ref& operator=(pool_weak_ref&& other) {
    if (&other == this) {
      return *this;
    }
    _doMove(std::move(other));
    return *this;
  }

public:
  static pool_weak_ref make_invalid() { return pool_weak_ref(); }

  void invalidate() {
    if (_pool != nullptr && _index >= 0 && !_pool->_is_out_of_range(std::size_t(_index))) {
      basic_double_linked_list& list = _pool->_get_itemsPool_data_by_index(std::size_t(_index)).get_weak_ref_list();
      list.remove_link(_link);
    }

    _link.reset();
    _pool = nullptr;
    _index = -1;
  }

  void invalidate_all() {
    if (_pool != nullptr && _index >= 0 && !_pool->_is_out_of_range(std::size_t(_index))) {
      _pool->_get_itemsPool_data_by_index(std::size_t(_index)).invalidate_all_ref();
    }
  }

private:
  void _doCopy(const pool_weak_ref& other) {
    if (&other == this) {
      return;
    }

    if (is_valid()) {
      basic_double_linked_list& list = _pool->_get_itemsPool_data_by_index(std::size_t(_index)).get_weak_ref_list();
      list.remove_link(_link);
    }

    _pool = other._pool;
    _index = other._index;
    _link.reset();
    _link.user_data = static_cast<void*>(this);

    if (is_valid()) {
      basic_double_linked_list& list = _pool->_get_itemsPool_data_by_index(std::size_t(_index)).get_weak_ref_list();
      list.add_link(_link);
    }
  }

  void _doMove(pool_weak_ref&& other) {
    if (&other == this) {
      return;
    }

    const bool currWasActive = is_valid();
    const bool otherWasActive = other.is_valid();

    // if (!currWasActive && !otherWasActive) {
    //   // do nothing
    // }
    // else
    if (currWasActive && otherWasActive) {

      std::swap(_index, other._index);

      {
        basic_double_linked_list& list = _pool->_get_itemsPool_data_by_index(std::size_t(_index)).get_weak_ref_list();
        list.swap_two_links_from_same_list(other._link, _link);
      }

    } else if (currWasActive && !otherWasActive) {
      // remove current
      // add other

      {
        basic_double_linked_list& list = _pool->_get_itemsPool_data_by_index(std::size_t(_index)).get_weak_ref_list();
        list.remove_link(_link);
      }

      std::swap(_pool, other._pool);
      std::swap(_index, other._index);

      {
        other._link.user_data = static_cast<void*>(&other);
        basic_double_linked_list& list = other._pool->_get_itemsPool_data_by_index(std::size_t(other._index)).get_weak_ref_list();
        list.add_link(other._link);
      }
    } else if (!currWasActive && otherWasActive) {
      // remove other
      // add current

      {
        basic_double_linked_list& list = other._pool->_get_itemsPool_data_by_index(std::size_t(other._index)).get_weak_ref_list();
        list.remove_link(other._link);
      }

      std::swap(_pool, other._pool);
      std::swap(_index, other._index);

      {
        _link.user_data = static_cast<void*>(this);
        basic_double_linked_list& list = _pool->_get_itemsPool_data_by_index(std::size_t(_index)).get_weak_ref_list();
        list.add_link(_link);
      }
    }
  }

public:
  operator bool() const { return is_valid(); }

  bool operator==(const pool_weak_ref& other) const {
    return (is_valid() == other.is_valid() && _pool == other._pool && _index == other._index);
  }

  bool operator!=(const pool_weak_ref& other) const { return !pool_weak_ref::operator==(other); }

public:
  bool is_valid() {
    return (_index >= 0 && _pool && _pool->_get_itemsPool_data_by_index(std::size_t(_index)).is_valid());
  }
  bool is_valid() const {
    return (_index >= 0 && _pool && _pool->_get_itemsPool_data_by_index(std::size_t(_index)).is_valid());
  }

  int32_t index() { return _index; }
  int32_t index() const { return _index; }

  value_type* get() {
    return reinterpret_cast<value_type*>(_index < 0 ? nullptr : &_pool->_get_itemsPool_public_data_by_index(std::size_t(_index)));
  }
  const value_type* get() const {
    return reinterpret_cast<const value_type*>(_index < 0 ? nullptr : &_pool->_get_itemsPool_public_data_by_index(std::size_t(_index)));
  }

  value_type* operator->() { return get(); }
  const value_type* operator->() const { return get(); }

  value_type& operator*() { return *get(); }
  const value_type& operator*() const { return *get(); }
};

//
//
//

//MARK: pool_container
/**
 * pool_container
 *
 * fixed size data pool
 * - no reallocation at runtime
 * - weak pointer to active data
 * - loop over active data
 */
template <typename InternalBaseType,
          typename PublicBaseType /*= InternalBaseType*/,
          std::size_t initial_size /*= 256*/,
          bool no_realloc /*= true*/,
          template <typename...> class Allocator /*= std::allocator*/
>
class pool_container
{
public:
  using value_type = internals::base_class::non_movable<PublicBaseType>;
  using weak_ref = pool_weak_ref<InternalBaseType, PublicBaseType, initial_size, no_realloc, Allocator>;
  friend weak_ref;

private:
  using internal_data = internals::pool_internal_element<InternalBaseType, PublicBaseType, initial_size, no_realloc, Allocator>;
  using allocator_type = Allocator<internal_data>;
  friend internal_data;

private:
  dynamic_heap_array<internal_data, internal_data, initial_size, allocator_type> _itemsPool;

private:
  virtual internal_data& _get_itemsPool_data_by_index(std::size_t inIndex) {
    return _itemsPool.at(inIndex);
  }
  virtual PublicBaseType& _get_itemsPool_public_data_by_index(std::size_t inIndex) {
    return _itemsPool.at(inIndex);
  }
  virtual const PublicBaseType& _get_itemsPool_public_data_by_index(std::size_t inIndex) const {
    return _itemsPool.at(inIndex);
  }
  virtual bool _is_out_of_range(std::size_t inIndex) { return _itemsPool.is_out_of_range(inIndex); }

public:
  pool_container() = default;
  ~pool_container() { clear(); }

  // disable copy
  pool_container(const pool_container& other) = delete;
  pool_container& operator=(const pool_container& other) = delete;
  // disable copy

  pool_container(pool_container&& other) {
    _itemsPool = std::move(other._itemsPool);
    for (auto& item : _itemsPool)
      item.sync_all_ref_pool(this);
  }

  pool_container& operator=(pool_container&& other) {
    _itemsPool = std::move(other._itemsPool);
    for (auto& item : _itemsPool)
      item.sync_all_ref_pool(this);

    return *this;
  }

  void pre_allocate(std::size_t newCapacity) { _itemsPool.pre_allocate(newCapacity); }

  void clear() {
    for (internal_data& item : _itemsPool) {
      item.invalidate_all_ref();
    }

    _itemsPool.clear();
  }

  template <typename... Args>
  weak_ref acquire(Args&&... args) {
    if (no_realloc == true && _itemsPool.size() == _itemsPool.capacity()) {
      return weak_ref::make_invalid();
    }

    const int32_t index = int32_t(_itemsPool.size());

    internal_data& currData = _itemsPool.emplace_back(std::forward<Args>(args)...);

    currData._index = index;
    currData._is_valid = true;

    return weak_ref(this, index);
  }

  weak_ref get(uint32_t index) { return weak_ref(this, int32_t(index)); }
  weak_ref get(uint32_t index) const {
    return weak_ref(const_cast<pool_container*>(this), int32_t(index));
  }

  weak_ref get(const weak_ref& ref) { return weak_ref(this, get_index(ref)); }
  weak_ref get(const weak_ref& ref) const {
    return weak_ref(const_cast<pool_container*>(this), get_index(ref));
  }

  std::size_t size() const { return _itemsPool.size(); }
  std::size_t capacity() const { return _itemsPool.capacity(); }
  bool is_empty() const { return _itemsPool.is_empty(); }

public:
  uint32_t get_ref_count(uint32_t index) const {
    if (_itemsPool.is_out_of_range(index)) {
      return 0;
    }
    return _itemsPool.at(index)._weak_ref_list.size;
  }

  uint32_t get_ref_count(const weak_ref& ref) const { return get_ref_count(uint32_t(get_index(ref))); }

public:
  int32_t get_index(const weak_ref& ref) const { return ref._index; }

public:
  bool is_valid(const weak_ref& ref) { return ref.is_valid(); }
  bool is_valid(std::size_t index) const {
    return reinterpret_cast<const internal_data*>(&_itemsPool.at(index))->_is_valid;
  }

public:
  void release(const weak_ref& ref) {
    if (!ref.is_valid())
      return;
    release(ref._index);
  }

  void release(int32_t index) {
    if (index == -1) {
      return;
    }
    auto& currData = _itemsPool.at(std::size_t(index));
    _release(currData);
  }

private:
  void _release(internal_data& curr_item) {

    const int32_t index = curr_item._index;

    curr_item.invalidate_all_ref();
    curr_item._is_valid = false;

    const uint32_t totalSwapped = _itemsPool.unsorted_erase(std::size_t(index));

    if (totalSwapped > 0) {
      auto& item = _itemsPool.at(std::size_t(index));
      item._index = index;
      item.sync_all_ref_index();
    }
  }

public:
  void remove_unreferenced_items() {
    for (std::size_t index = 0; index < _itemsPool.size();) {
      if (_itemsPool.at(index)._weak_ref_list.size == 0) {
        release(int32_t(index));
      } else {
        ++index;
      }
    }
  }

public:
  void filter(std::function<bool(const value_type&)> callback) {
    for (std::size_t index = 0; index < _itemsPool.size();) {
      auto& item = _itemsPool.at(index);

      if (item._is_valid == false) {
        ++index;
        continue;
      }

      if (callback(reinterpret_cast<const value_type&>(item)) == false) {
        release(int32_t(index));
      } else {
        ++index;
      }
    }
  }

public:
  void for_each(std::function<void(value_type&)> callback) {
    for (auto& item : _itemsPool) {
      if (item._is_valid == true) {
        callback(reinterpret_cast<value_type&>(item));
      }
    }
  }

  void for_each(std::function<void(value_type&, std::size_t)> callback) {
    for (auto& item : _itemsPool) {
      if (item._is_valid == true) {
        callback(reinterpret_cast<value_type&>(item), std::size_t(item._index));
      }
    }
  }

  void for_each(std::function<void(weak_ref)> callback) {
    for (auto& item : _itemsPool) {
      if (item._is_valid == true) {
        callback(weak_ref(this, item._index));
      }
    }
  }

  void for_each(std::function<void(weak_ref, std::size_t)> callback) {
    for (auto& item : _itemsPool) {
      if (item._is_valid == true) {
        callback(weak_ref(this, item._index), std::size_t(item._index));
      }
    }
  }

  void for_each(std::function<void(const value_type&)> callback) const {
    for (auto& item : _itemsPool) {
      if (item._is_valid == true) {
        callback(reinterpret_cast<const value_type&>(item));
      }
    }
  }

  void for_each(std::function<void(const value_type&, std::size_t)> callback) const {
    for (auto& item : _itemsPool) {
      if (item._is_valid == true) {
        callback(reinterpret_cast<const value_type&>(item), std::size_t(item._index));
      }
    }
  }

  void for_each(std::function<void(const weak_ref)> callback) const {
    for (auto& item : _itemsPool) {
      if (item._is_valid == true) {
        callback(weak_ref(const_cast<pool_container*>(this), item._index));
      }
    }
  }

  void for_each(std::function<void(const weak_ref, std::size_t)> callback) const {
    for (auto& item : _itemsPool) {
      if (item._is_valid == true) {
        callback(weak_ref(const_cast<pool_container*>(this), item._index), std::size_t(item._index));
      }
    }
  }

public:
  weak_ref find_if(std::function<bool(const value_type&)> callback) const {
    for (std::size_t index = 0; index < _itemsPool.size(); ++index) {
      auto& item = _itemsPool.at(index);

      if (item._is_valid == false) {
        continue;
      }

      if (callback(reinterpret_cast<const value_type&>(item)) == true) {
        return weak_ref(const_cast<pool_container*>(this), int32_t(index));
      }
    }
    return weak_ref::make_invalid();
  }

  weak_ref find_if(std::function<bool(const weak_ref)> callback) const {
    for (std::size_t index = 0; index < _itemsPool.size(); ++index) {
      auto& item = _itemsPool.at(index);

      if (item._is_valid == false) {
        continue;
      }

      auto currRef = weak_ref(const_cast<pool_container*>(this), int32_t(index));

      if (callback(currRef) == true) {
        return currRef;
      }
    }
    return weak_ref::make_invalid();
  }
};

//
//
//

}; // namespace custom_containers
}; // namespace weak_ref_data_pool

