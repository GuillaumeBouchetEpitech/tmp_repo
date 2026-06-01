
#pragma once

// #include "fixed_size_array.hpp"
// #include "utils/generic_array_container.hpp"
#include "dynamic_heap_array.hpp"

#include "../tests/utils/generic_array_container_commons/common.tests.hpp"

#include <functional>
#include <memory>
#include <vector>

#include "gtest/gtest.h"

template <std::size_t N>
using shorthand_dynamic_heap_array =
custom_containers::dynamic_heap_array<
  common::TestStructureCopyable,
  common::ITestStructure,
  N,
  common::MyAllocator<common::TestStructureCopyable>
>;

struct dynamic_heap_array : public common::threadsafe_fixture {};
