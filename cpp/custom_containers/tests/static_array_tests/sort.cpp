
#include "headers.hpp"

#include <algorithm>

TEST_F(static_array, sort) {

  custom_containers::static_array<int, 5, int> myStaticArray;

  myStaticArray[0] = 4;
  myStaticArray[1] = 3;
  myStaticArray[2] = 0;
  myStaticArray[3] = 2;
  myStaticArray[4] = 1;

  std::sort(myStaticArray.begin(), myStaticArray.end());

  ASSERT_EQ(myStaticArray[0], 0);
  ASSERT_EQ(myStaticArray[1], 1);
  ASSERT_EQ(myStaticArray[2], 2);
  ASSERT_EQ(myStaticArray[3], 3);
  ASSERT_EQ(myStaticArray[4], 4);
}
