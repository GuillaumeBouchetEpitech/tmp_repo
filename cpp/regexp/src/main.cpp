
#include <cstdio> // <= EXIT_SUCCESS
#include <iostream>
#include <string>
#include <memory>
#include <cstring>
#include <cassert>
#include <stdint.h>

template<typename Type, typename Allocator = std::allocator<Type>>
class dynamic_numeric_heap_array
{
private:
  using traits_t = std::allocator_traits<Allocator>; // The matching trait

private:
  std::size_t _capacity = 0;
  std::size_t _size = 0;
  Type* _data = nullptr;

private:

  // allocate memory only, will not call any constructor
  Type* allocateMemory(std::size_t size)
  {
    Allocator alloc;
    Type* newData = alloc.allocate(size);

    return newData;
  }

  // deallocate memory only, will not call any destructor
  void deallocateMemory(Type* data, std::size_t size)
  {
    Allocator alloc;
    alloc.deallocate(data, size);
  }

  // call the move constructor only, do not allocate memory
  void callConstructor(Type* data, std::size_t index, Type& value)
  {
    Allocator alloc;
    traits_t::construct(alloc, data + index, value);
  }

  // call the destructor only, do not deallocate memory
  void callDestructor(std::size_t index)
  {
    Allocator alloc;
    traits_t::destroy(alloc, _data + index);
  }

public:
  dynamic_numeric_heap_array() = default;

  // disable copy
  dynamic_numeric_heap_array(const dynamic_numeric_heap_array& other) = delete;
  dynamic_numeric_heap_array& operator=(const dynamic_numeric_heap_array& other) = delete;
  // disable copy

  dynamic_numeric_heap_array(dynamic_numeric_heap_array&& other)
  {
    std::swap(_capacity, other._capacity);
    std::swap(_size, other._size);
    std::swap(_data, other._data);
  }

  dynamic_numeric_heap_array& operator=(dynamic_numeric_heap_array&& other)
  {
    std::swap(_capacity, other._capacity);
    std::swap(_size, other._size);
    std::swap(_data, other._data);
    return *this;
  }

  virtual ~dynamic_numeric_heap_array()
  {
    clear();
    deallocateMemory(_data, _capacity);
  }

public:
  void push_back(const Type& value)
  {
    if (_size == _capacity)
      _realloc(_capacity * 2);

    _data[_size++] = value; // copy
  }

  void pop_back()
  {
    if (_size == 0)
      return;

    --_size;

    callDestructor(_size);
  }

  void clear()
  {
    for (std::size_t ii = 0; ii < _size; ++ii)
      callDestructor(ii);

    _size = 0;
  }

  void pre_allocate(std::size_t capacity)
  {
    _realloc(capacity);
  }

public:
  bool empty() const { return _size == 0; }
  std::size_t size() const { return _size; }
  std::size_t capacity() const { return _capacity; }

public:
  const Type& operator[](std::size_t index) const { return _data[index]; }
  Type& operator[](std::size_t index) { return _data[index]; }

private:
  void _realloc(std::size_t newCapacity)
  {
    if (newCapacity == 0) // true the first time
      newCapacity = 1;

    if (newCapacity < _capacity)
      return;

    Type* newData = allocateMemory(newCapacity);

    for (std::size_t ii = 0; ii < _size; ++ii)
      callConstructor(newData, ii, _data[ii]); // copy

    for (std::size_t ii = 0; ii < _size; ++ii)
      callDestructor(ii);

    deallocateMemory(_data, _capacity);

    _data = newData;
    _capacity = newCapacity;
  }
};

int main()
{

    {
        // int
        dynamic_numeric_heap_array<int> test;

        assert(test.empty());
        assert(test.size() == 0);
        assert(test.capacity() == 0);

        test.push_back(1111);

        assert(!test.empty());
        assert(test.size() == 1);
        assert(test.capacity() == 1);
        assert(test[0] == 1111);

        test.push_back(2222);
        test.push_back(3333);

        assert(!test.empty());
        assert(test.size() == 3);
        assert(test.capacity() == 4);
        assert(test[0] == 1111);
        assert(test[1] == 2222);
        assert(test[2] == 3333);

        std::cout << "test.size() -> " << test.size() << std::endl;
        for (std::size_t index = 0; index < test.size(); ++index)
            std::cout << "test[" << index << "] -> " << test[index] << std::endl;

        test.clear();

        assert(test.empty());
        assert(test.size() == 0);
        assert(test.capacity() == 4);
    }

    {
        // float
        dynamic_numeric_heap_array<float> test;

        assert(test.empty());
        assert(test.size() == 0);
        assert(test.capacity() == 0);

        test.push_back(1111.1111f);

        assert(!test.empty());
        assert(test.size() == 1);
        assert(test.capacity() == 1);
        assert(test[0] == 1111.1111f);

        test.push_back(2222.2222f);
        test.push_back(3333.3333f);

        assert(!test.empty());
        assert(test.size() == 3);
        assert(test.capacity() == 4);
        assert(test[0] == 1111.1111f);
        assert(test[1] == 2222.2222f);
        assert(test[2] == 3333.3333f);

        std::cout << "test.size() -> " << test.size() << std::endl;
        for (std::size_t index = 0; index < test.size(); ++index)
            std::cout << "test[" << index << "] -> " << test[index] << std::endl;

        test.clear();

        assert(test.empty());
        assert(test.size() == 0);
        assert(test.capacity() == 4);
    }

    {
        // double
        dynamic_numeric_heap_array<double> test;

        assert(test.empty());
        assert(test.size() == 0);
        assert(test.capacity() == 0);

        test.push_back(1111.1111);

        assert(!test.empty());
        assert(test.size() == 1);
        assert(test.capacity() == 1);
        assert(test[0] == 1111.1111);

        test.push_back(2222.2222);
        test.push_back(3333.3333);

        assert(!test.empty());
        assert(test.size() == 3);
        assert(test.capacity() == 4);
        assert(test[0] == 1111.1111);
        assert(test[1] == 2222.2222);
        assert(test[2] == 3333.3333);

        std::cout << "test.size() -> " << test.size() << std::endl;
        for (std::size_t index = 0; index < test.size(); ++index)
            std::cout << "test[" << index << "] -> " << test[index] << std::endl;

        test.clear();

        assert(test.empty());
        assert(test.size() == 0);
        assert(test.capacity() == 4);
    }


    {
        // test pre-allocate
        dynamic_numeric_heap_array<int> test;
        test.pre_allocate(64);

        assert(test.empty());
        assert(test.size() == 0);
        assert(test.capacity() == 64);

        for (int index = 0; index < 32; ++index)
            test.push_back(index);

        assert(!test.empty());
        assert(test.size() == 32);
        assert(test.capacity() == 64);
        for (int index = 0; index < 32; ++index)
            assert(test[index] == index);

        for (int index = 0; index < 16; ++index)
            test.pop_back();

        assert(!test.empty());
        assert(test.size() == 16);
        assert(test.capacity() == 64);
        for (int index = 0; index < 16; ++index)
            assert(test[index] == index);

        test.clear();

        assert(test.empty());
        assert(test.size() == 0);
        assert(test.capacity() == 64);
    }

    return EXIT_SUCCESS;
}
