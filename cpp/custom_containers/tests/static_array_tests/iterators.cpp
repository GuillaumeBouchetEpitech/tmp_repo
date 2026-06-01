
#include "headers.hpp"

// #include "system/containers/generic_array_container_commons/common.hpp"

TEST_F(static_array, raw_iterator) {

  custom_containers::static_array<common::TestStructureCopyable, 5, common::TestStructureCopyable> myStaticArray;
  for (std::size_t ii = 0; ii < myStaticArray.size(); ++ii)
  {
    myStaticArray[ii].value = 10 + ii;
  }

  {
    std::size_t ii = 0;
    for (auto it = myStaticArray.begin(); it != myStaticArray.end(); ++it)
    {
      ASSERT_EQ(it->value, 10 + ii);
      ii += 1;
    }
  }

}

TEST_F(static_array, raw_const_iterator) {

  custom_containers::static_array<common::TestStructureCopyable, 5, common::TestStructureCopyable> myStaticArray;
  for (std::size_t ii = 0; ii < myStaticArray.size(); ++ii)
  {
    myStaticArray[ii].value = 10 + ii;
  }

  {
    const auto& myConstStaticArray = myStaticArray;

    std::size_t ii = 0;
    for (const auto it = myConstStaticArray.begin(); it != myConstStaticArray.end(); ++it)
    {
      ASSERT_EQ(it->value, 10 + ii);
      ii += 1;
    }
  }

}

TEST_F(static_array, raw_reverse_iterator) {

  custom_containers::static_array<common::TestStructureCopyable, 5, common::TestStructureCopyable> myStaticArray;
  for (std::size_t ii = 0; ii < myStaticArray.size(); ++ii)
  {
    myStaticArray[ii].value = 10 + ii;
  }

  {
    std::size_t ii = 0;
    for (auto it = myStaticArray.rbegin(); it != myStaticArray.rend(); ++it)
    {
      ASSERT_EQ(it->value, 14 - ii);
      ii += 1;
    }
    ASSERT_EQ(ii, 5);
  }

}

TEST_F(static_array, raw_reverse_const_iterator) {

  custom_containers::static_array<common::TestStructureCopyable, 5, common::TestStructureCopyable> myStaticArray;
  for (std::size_t ii = 0; ii < myStaticArray.size(); ++ii)
  {
    myStaticArray[ii].value = 10 + ii;
  }

  {
    const auto& myConstStaticArray = myStaticArray;

    std::size_t ii = 0;
    for (const auto it = myConstStaticArray.rbegin(); it != myConstStaticArray.rend(); ++it)
    {
      ASSERT_EQ(it->value, 14 - ii);
      ii += 1;
    }
  }

}

TEST_F(static_array, loop_syntax) {

  custom_containers::static_array<common::TestStructureCopyable, 5, common::TestStructureCopyable> myStaticArray;
  for (std::size_t ii = 0; ii < myStaticArray.size(); ++ii)
  {
    myStaticArray[ii].value = 10 + ii;
  }

  {
    std::size_t ii = 0;
    for (const auto& it : myStaticArray)
    {
      ASSERT_EQ(it.value, 10 + ii);
      ii += 1;
    }
  }

}
