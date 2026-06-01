
#include "common.tests.hpp"

#include <functional>
#include <mutex>
#include <vector>

namespace common {

// MARK: Globals

int g_totalCtor = 0;
int g_totalCopyCtor = 0;
int g_totalMoveCtor = 0;
int g_totalDtor = 0;
int g_totalAlloc = 0;
int g_totalDealloc = 0;

std::mutex g_singleThread;

int getTotalCtor() { return g_totalCtor; }
int getTotalCopyCtor() { return g_totalCopyCtor; }
int getTotalMoveCtor() { return g_totalMoveCtor; }
int getTotalDtor() { return g_totalDtor; }
int getTotalAlloc() { return g_totalAlloc; }
int getTotalDealloc() { return g_totalDealloc; }
void setTotalAlloc(int val) { g_totalAlloc = val; }
void setTotalDealloc(int val) { g_totalDealloc = val; }

void reset() {
  g_totalCtor = 0;
  g_totalCopyCtor = 0;
  g_totalMoveCtor = 0;
  g_totalDtor = 0;

  g_totalAlloc = 0;
  g_totalDealloc = 0;
}

void lockThread() { g_singleThread.lock(); }
void unlockThread() { g_singleThread.unlock(); }

//
//
//

// MARK: Copyable

TestStructureCopyable::TestStructureCopyable(int inValue /* = 0 */, const char* inString /*= ""*/)
  : value(inValue), my_string(inString) {
  ++g_totalCtor;
}

TestStructureCopyable::~TestStructureCopyable() { ++g_totalDtor; }

TestStructureCopyable::TestStructureCopyable(const TestStructureCopyable& other) {
  ++g_totalCopyCtor;
  value = other.value;
  my_string = other.my_string;
}

TestStructureCopyable::TestStructureCopyable(TestStructureCopyable&& other) {
  ++g_totalMoveCtor;
  std::swap(value, other.value);
  my_string = std::move(other.my_string);
}

TestStructureCopyable& TestStructureCopyable::operator=(const TestStructureCopyable& other) {
  ++g_totalCopyCtor;
  value = other.value;
  my_string = other.my_string;
  return *this;
}

TestStructureCopyable& TestStructureCopyable::operator=(TestStructureCopyable&& other) {
  ++g_totalMoveCtor;
  std::swap(value, other.value);
  my_string = std::move(other.my_string);
  return *this;
}

void TestStructureCopyable::set_value(int inVal) { value = inVal; }

int TestStructureCopyable::get_value() const { return value; }

const std::string& TestStructureCopyable::get_my_string() const { return my_string; }

void TestStructureCopyable::set_my_string(const std::string& inVal) { my_string = inVal; }

//
//
//

// MARK: NonCopyable

TestStructureNonCopyable::TestStructureNonCopyable(int inValue /* = 0 */, const char* inString /*= ""*/)
  : value(inValue), my_string(inString)
  // : TestStructure(inValue, inString)
{
  ++g_totalCtor;
}

TestStructureNonCopyable::~TestStructureNonCopyable() { ++g_totalDtor; }

// TestStructureNonCopyable::TestStructureNonCopyable(const TestStructureNonCopyable& other) {
//   ++g_totalCopyCtor;
//   value = other.value;
//   my_string = other.my_string;
// }

TestStructureNonCopyable::TestStructureNonCopyable(TestStructureNonCopyable&& other) {
  ++g_totalMoveCtor;
  std::swap(value, other.value);
  my_string = std::move(other.my_string);
}

// TestStructureNonCopyable& TestStructureNonCopyable::operator=(const TestStructureNonCopyable& other) {
//   ++g_totalCopyCtor;
//   value = other.value;
//   my_string = other.my_string;
//   return *this;
// }

TestStructureNonCopyable& TestStructureNonCopyable::operator=(TestStructureNonCopyable&& other) {
  ++g_totalMoveCtor;
  std::swap(value, other.value);
  my_string = std::move(other.my_string);
  return *this;
}

void TestStructureNonCopyable::set_value(int inVal) { value = inVal; }

int TestStructureNonCopyable::get_value() const { return value; }

const std::string& TestStructureNonCopyable::get_my_string() const { return my_string; }

void TestStructureNonCopyable::set_my_string(const std::string& inVal) { my_string = inVal; }

} // namespace common
