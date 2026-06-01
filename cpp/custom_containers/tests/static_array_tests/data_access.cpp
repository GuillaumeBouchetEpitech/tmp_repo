
#include "headers.hpp"

// #include "system/containers/generic_array_container_commons/common.hpp"

TEST_F(static_array, data_access) {

  custom_containers::static_array<common::TestStructureCopyable, 5, common::TestStructureCopyable> myDefaultStaticArray;

  ASSERT_EQ(myDefaultStaticArray.is_empty(), false);
  ASSERT_EQ(myDefaultStaticArray.size(), 5);
  ASSERT_EQ(myDefaultStaticArray.is_out_of_range(0), false);
  ASSERT_EQ(myDefaultStaticArray.is_out_of_range(1), false);
  ASSERT_EQ(myDefaultStaticArray.is_out_of_range(2), false);
  ASSERT_EQ(myDefaultStaticArray.is_out_of_range(3), false);
  ASSERT_EQ(myDefaultStaticArray.is_out_of_range(4), false);
  ASSERT_EQ(myDefaultStaticArray.is_out_of_range(5), true);

  myDefaultStaticArray[0].value = 666;
  myDefaultStaticArray.at(1).value = 777;

  ASSERT_EQ(myDefaultStaticArray[0].value, 666);
  ASSERT_EQ(myDefaultStaticArray[1].value, 777);
  ASSERT_EQ(myDefaultStaticArray[2].value, 0);
  ASSERT_EQ(myDefaultStaticArray[3].value, 0);
  ASSERT_EQ(myDefaultStaticArray[4].value, 0);
  ASSERT_EQ(myDefaultStaticArray.front().value, 666);
  ASSERT_EQ(myDefaultStaticArray.back().value, 0);

  ASSERT_EQ(myDefaultStaticArray[-5].value, 666); // loop back
  ASSERT_EQ(myDefaultStaticArray[5].value, 666); // loop back
}
