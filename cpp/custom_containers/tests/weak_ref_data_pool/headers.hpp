
#pragma once

#include "weak_ref_data_pool.hpp"

#include "../utils/generic_array_container_commons/common.tests.hpp"

#include <functional>
#include <memory>
#include <vector>

#include "gtest/gtest.h"

template <std::size_t N, bool no_realloc>
using shorthand_weak_ref_data_pool =
custom_containers::weak_ref_data_pool::pool_container<
  common::TestStructureNonCopyable,
  common::ITestStructure,
  N, // initial size
  no_realloc, // no realloc
  common::MyAllocator
>;

struct weak_ref_data_pool : public common::threadsafe_fixture {};
