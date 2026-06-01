
#include "headers.hpp"

// #include "system/containers/generic_array_container_commons/common.hpp"

TEST_F(dynamic_heap_array, default_template_args) {

  ASSERT_EQ(common::getTotalCtor(), 0);
  ASSERT_EQ(common::getTotalCopyCtor(), 0);
  ASSERT_EQ(common::getTotalMoveCtor(), 0);
  ASSERT_EQ(common::getTotalDtor(), 0);
  ASSERT_EQ(common::getTotalAlloc(), 0);
  ASSERT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  {
    shorthand_dynamic_heap_array<0> myArray;

    ASSERT_EQ(myArray.is_empty(), true);
    ASSERT_EQ(myArray.size(), 0);
    ASSERT_EQ(myArray.capacity(), 0);
    ASSERT_EQ(myArray.is_out_of_range(0), true);

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 0);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();
  }

  ASSERT_EQ(common::getTotalCtor(), 0);
  ASSERT_EQ(common::getTotalCopyCtor(), 0);
  ASSERT_EQ(common::getTotalMoveCtor(), 0);
  ASSERT_EQ(common::getTotalDtor(), 0);
  ASSERT_EQ(common::getTotalAlloc(), 0);
  ASSERT_EQ(common::getTotalDealloc(), 1);
}




TEST_F(dynamic_heap_array, pre_allocate_template_args) {

  ASSERT_EQ(common::getTotalCtor(), 0);
  ASSERT_EQ(common::getTotalCopyCtor(), 0);
  ASSERT_EQ(common::getTotalMoveCtor(), 0);
  ASSERT_EQ(common::getTotalDtor(), 0);
  ASSERT_EQ(common::getTotalAlloc(), 0);
  ASSERT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  {
    shorthand_dynamic_heap_array<5> myArray;

    ASSERT_EQ(myArray.is_empty(), true);
    ASSERT_EQ(myArray.size(), 0);
    ASSERT_EQ(myArray.capacity(), 5);
    ASSERT_EQ(myArray.is_out_of_range(0), true);

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 0);
    ASSERT_EQ(common::getTotalAlloc(), 1);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();
  }

  ASSERT_EQ(common::getTotalCtor(), 0);
  ASSERT_EQ(common::getTotalCopyCtor(), 0);
  ASSERT_EQ(common::getTotalMoveCtor(), 0);
  ASSERT_EQ(common::getTotalDtor(), 0);
  ASSERT_EQ(common::getTotalAlloc(), 0);
  ASSERT_EQ(common::getTotalDealloc(), 1);
}
