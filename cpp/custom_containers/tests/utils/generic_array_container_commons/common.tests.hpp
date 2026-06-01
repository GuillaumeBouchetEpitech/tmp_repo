
#pragma once

#include "weak_ref_data_pool.hpp"

#include "gtest/gtest.h"

#include <string>

namespace common {

void reset();

int getTotalCtor();
int getTotalCopyCtor();
int getTotalMoveCtor();
int getTotalDtor();

int getTotalAlloc();
int getTotalDealloc();
void setTotalAlloc(int val);
void setTotalDealloc(int val);

void lockThread();
void unlockThread();

//
//
//
//
//

struct ITestStructure {

public:
  // ITestStructure(const ITestStructure& other) = default;
  // ITestStructure(ITestStructure&& other) = default;
  // ITestStructure& operator=(const ITestStructure& other) = default;
  // ITestStructure& operator=(ITestStructure&& other) = default;
  virtual ~ITestStructure() = default;

  virtual void set_value(int inVal) = 0;
  virtual int get_value() const = 0;
  virtual const std::string& get_my_string() const = 0;
  virtual void set_my_string(const std::string& inVal) = 0;
};

// struct IPublicTestStructure : public ITestStructure, public custom_containers::weak_ref_data_pool_interface_base_class {
// };

struct TestStructureCopyable : public ITestStructure {
  TestStructureCopyable(int inValue = 0, const char* inString = "");
  ~TestStructureCopyable();
  TestStructureCopyable(const TestStructureCopyable& other);
  TestStructureCopyable(TestStructureCopyable&& other);
  TestStructureCopyable& operator=(const TestStructureCopyable& other);
  TestStructureCopyable& operator=(TestStructureCopyable&& other);

  int value = -1;

  std::string my_string;

  void set_value(int inVal) override;
  int get_value() const override;
  const std::string& get_my_string() const override;
  void set_my_string(const std::string& inVal) override;
};

struct TestStructureNonCopyable : public ITestStructure {
  TestStructureNonCopyable(int inValue = 0, const char* inString = "");
  ~TestStructureNonCopyable();
  TestStructureNonCopyable(const TestStructureNonCopyable& other) = delete; // block copy
  TestStructureNonCopyable(TestStructureNonCopyable&& other);
  TestStructureNonCopyable& operator=(const TestStructureNonCopyable& other) = delete; // block copy
  TestStructureNonCopyable& operator=(TestStructureNonCopyable&& other);

  int value = -1;

  std::string my_string;

  void set_value(int inVal) override;
  int get_value() const override;
  const std::string& get_my_string() const override;
  void set_my_string(const std::string& inVal) override;
};

//
//
//
//
//

template <typename T> struct MyAllocator {
  using value_type = T;

  MyAllocator() = default;
  template <class U> MyAllocator(const MyAllocator<U>&) {}

  T* allocate(std::size_t n) {
    setTotalAlloc(getTotalAlloc() + 1);
    if (n <= std::numeric_limits<std::size_t>::max() / sizeof(T)) {
      if (auto ptr = std::malloc(n * sizeof(T))) {
        return static_cast<T*>(ptr);
      }
    }
    throw std::bad_alloc();
  }
  void deallocate(T* ptr, std::size_t n) {
    setTotalDealloc(getTotalDealloc() + 1);
    static_cast<void>(n); // unused
    std::free(ptr);
  }
};

//
//
//
//
//

class threadsafe_fixture : public ::testing::Test {
protected:
  virtual void SetUp() override {
    lockThread();
    common::reset();
  }

  virtual void TearDown() override {
    common::reset();
    unlockThread();
  }
};

} // namespace common
