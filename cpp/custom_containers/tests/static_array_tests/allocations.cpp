
#include "headers.hpp"

// #include "system/containers/generic_array_container_commons/common.hpp"

TEST_F(static_array, total_ctor_calls) {

  ASSERT_EQ(common::getTotalCtor(), 0);
  ASSERT_EQ(common::getTotalCopyCtor(), 0);
  ASSERT_EQ(common::getTotalMoveCtor(), 0);
  ASSERT_EQ(common::getTotalDtor(), 0);
  ASSERT_EQ(common::getTotalAlloc(), 0);
  ASSERT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  custom_containers::static_array<common::TestStructureCopyable, 5, common::TestStructureCopyable> myDefaultStaticArray;

  ASSERT_EQ(common::getTotalCtor(), 5);
  ASSERT_EQ(common::getTotalCopyCtor(), 0);
  ASSERT_EQ(common::getTotalMoveCtor(), 0);
  ASSERT_EQ(common::getTotalDtor(), 0);
  ASSERT_EQ(common::getTotalAlloc(), 0);
  ASSERT_EQ(common::getTotalDealloc(), 0);

  ASSERT_EQ(myDefaultStaticArray.is_empty(), false);
  ASSERT_EQ(myDefaultStaticArray.size(), 5);
  ASSERT_EQ(myDefaultStaticArray.is_out_of_range(0), false);
  ASSERT_EQ(myDefaultStaticArray.is_out_of_range(1), false);
  ASSERT_EQ(myDefaultStaticArray.is_out_of_range(2), false);
  ASSERT_EQ(myDefaultStaticArray.is_out_of_range(3), false);
  ASSERT_EQ(myDefaultStaticArray.is_out_of_range(4), false);
  ASSERT_EQ(myDefaultStaticArray.is_out_of_range(5), true);
}

TEST_F(static_array, total_dtor_calls) {

  {
    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 0);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    custom_containers::static_array<common::TestStructureCopyable, 5, common::TestStructureCopyable> myDefaultStaticArray;

    ASSERT_EQ(common::getTotalCtor(), 5);
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
  ASSERT_EQ(common::getTotalDtor(), 5);
  ASSERT_EQ(common::getTotalAlloc(), 0);
  ASSERT_EQ(common::getTotalDealloc(), 0);
}
