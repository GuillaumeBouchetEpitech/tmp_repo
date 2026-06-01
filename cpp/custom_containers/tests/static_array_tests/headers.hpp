
#pragma once

// #include "fixed_size_array.hpp"
// #include "utils/generic_array_container.hpp"
#include "static_array.hpp"

#include "../tests/utils/generic_array_container_commons/common.tests.hpp"

#include <functional>
#include <memory>
#include <vector>

#include "gtest/gtest.h"

template <std::size_t N>
using shorthand_static_size_array =
custom_containers::static_array<
  common::TestStructureCopyable,
  N,
  common::TestStructureCopyable
>;

struct static_array : public common::threadsafe_fixture {};
