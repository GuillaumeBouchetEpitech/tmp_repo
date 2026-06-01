
#include "headers.hpp"

// #include "system/containers/generic_array_container_commons/common.hpp"

TEST_F(dynamic_heap_array, push_back__by_ref__without_pre_allocation) {

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

    {
      common::TestStructureCopyable tmpData(1, "1");

      ASSERT_EQ(common::getTotalCtor(), 1); // ctor the rvalue
      ASSERT_EQ(common::getTotalCopyCtor(), 0);
      ASSERT_EQ(common::getTotalMoveCtor(), 0); // moved the rvalue in the container
      ASSERT_EQ(common::getTotalDtor(), 0); // dtor the rvalue
      ASSERT_EQ(common::getTotalAlloc(), 0);
      ASSERT_EQ(common::getTotalDealloc(), 0);
      common::reset();

      myArray.push_back(tmpData);

      ASSERT_EQ(myArray.is_empty(), false);
      ASSERT_EQ(myArray.size(), 1);
      ASSERT_EQ(myArray.capacity(), 1);
      ASSERT_EQ(myArray.is_out_of_range(0), false);
      ASSERT_EQ(myArray.is_out_of_range(1), true);

      ASSERT_EQ(common::getTotalCtor(), 0);
      ASSERT_EQ(common::getTotalCopyCtor(), 1); // ctor the ref value in the container
      ASSERT_EQ(common::getTotalMoveCtor(), 0);
      ASSERT_EQ(common::getTotalDtor(), 0);
      ASSERT_EQ(common::getTotalAlloc(), 1);
      ASSERT_EQ(common::getTotalDealloc(), 0);
      common::reset();
    }

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 1); // dtor tmpData
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    {
      common::TestStructureCopyable tmpData(2, "2");

      ASSERT_EQ(common::getTotalCtor(), 1); // ctor the rvalue
      ASSERT_EQ(common::getTotalCopyCtor(), 0);
      ASSERT_EQ(common::getTotalMoveCtor(), 0); // moved the rvalue in the container
      ASSERT_EQ(common::getTotalDtor(), 0); // dtor the rvalue
      ASSERT_EQ(common::getTotalAlloc(), 0);
      ASSERT_EQ(common::getTotalDealloc(), 0);
      common::reset();

      myArray.push_back(tmpData);

      ASSERT_EQ(myArray.is_empty(), false);
      ASSERT_EQ(myArray.size(), 2);
      ASSERT_EQ(myArray.capacity(), 2);
      ASSERT_EQ(myArray.is_out_of_range(0), false);
      ASSERT_EQ(myArray.is_out_of_range(1), false);
      ASSERT_EQ(myArray.is_out_of_range(2), true);

      ASSERT_EQ(common::getTotalCtor(), 0);
      ASSERT_EQ(common::getTotalCopyCtor(), 1); // ctor the ref value in the container
      ASSERT_EQ(common::getTotalMoveCtor(), 1);
      ASSERT_EQ(common::getTotalDtor(), 1);
      ASSERT_EQ(common::getTotalAlloc(), 1);
      ASSERT_EQ(common::getTotalDealloc(), 1);
      common::reset();
    }

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 1); // dtor tmpData
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    {
      common::TestStructureCopyable tmpData(3, "3");

      ASSERT_EQ(common::getTotalCtor(), 1); // ctor the rvalue
      ASSERT_EQ(common::getTotalCopyCtor(), 0);
      ASSERT_EQ(common::getTotalMoveCtor(), 0); // moved the rvalue in the container
      ASSERT_EQ(common::getTotalDtor(), 0); // dtor the rvalue
      ASSERT_EQ(common::getTotalAlloc(), 0);
      ASSERT_EQ(common::getTotalDealloc(), 0);
      common::reset();

      myArray.push_back(tmpData);

      ASSERT_EQ(myArray.is_empty(), false);
      ASSERT_EQ(myArray.size(), 3);
      ASSERT_EQ(myArray.capacity(), 4);
      ASSERT_EQ(myArray.is_out_of_range(0), false);
      ASSERT_EQ(myArray.is_out_of_range(1), false);
      ASSERT_EQ(myArray.is_out_of_range(2), false);
      ASSERT_EQ(myArray.is_out_of_range(3), true);

      ASSERT_EQ(common::getTotalCtor(), 0);
      ASSERT_EQ(common::getTotalCopyCtor(), 1); // ctor the ref value in the container
      ASSERT_EQ(common::getTotalMoveCtor(), 2);
      ASSERT_EQ(common::getTotalDtor(), 2);
      ASSERT_EQ(common::getTotalAlloc(), 1);
      ASSERT_EQ(common::getTotalDealloc(), 1);
      common::reset();
    }

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 1); // dtor tmpData
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    {
      common::TestStructureCopyable tmpData(4, "4");

      ASSERT_EQ(common::getTotalCtor(), 1); // ctor the rvalue
      ASSERT_EQ(common::getTotalCopyCtor(), 0);
      ASSERT_EQ(common::getTotalMoveCtor(), 0); // moved the rvalue in the container
      ASSERT_EQ(common::getTotalDtor(), 0); // dtor the rvalue
      ASSERT_EQ(common::getTotalAlloc(), 0);
      ASSERT_EQ(common::getTotalDealloc(), 0);
      common::reset();

      myArray.push_back(tmpData);

      ASSERT_EQ(myArray.is_empty(), false);
      ASSERT_EQ(myArray.size(), 4);
      ASSERT_EQ(myArray.capacity(), 4);
      ASSERT_EQ(myArray.is_out_of_range(0), false);
      ASSERT_EQ(myArray.is_out_of_range(1), false);
      ASSERT_EQ(myArray.is_out_of_range(2), false);
      ASSERT_EQ(myArray.is_out_of_range(3), false);
      ASSERT_EQ(myArray.is_out_of_range(4), true);

      ASSERT_EQ(common::getTotalCtor(), 0);
      ASSERT_EQ(common::getTotalCopyCtor(), 1); // ctor the ref value in the container
      ASSERT_EQ(common::getTotalMoveCtor(), 0);
      ASSERT_EQ(common::getTotalDtor(), 0);
      ASSERT_EQ(common::getTotalAlloc(), 0);
      ASSERT_EQ(common::getTotalDealloc(), 0);
      common::reset();
    }

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 1); // dtor tmpData
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

  }

  ASSERT_EQ(common::getTotalCtor(), 0);
  ASSERT_EQ(common::getTotalCopyCtor(), 0);
  ASSERT_EQ(common::getTotalMoveCtor(), 0);
  ASSERT_EQ(common::getTotalDtor(), 4);
  ASSERT_EQ(common::getTotalAlloc(), 0);
  ASSERT_EQ(common::getTotalDealloc(), 1);
}




TEST_F(dynamic_heap_array, push_back__by_ref__with_pre_allocation) {

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

    {
      common::TestStructureCopyable tmpData(1, "1");

      ASSERT_EQ(common::getTotalCtor(), 1); // ctor the rvalue
      ASSERT_EQ(common::getTotalCopyCtor(), 0);
      ASSERT_EQ(common::getTotalMoveCtor(), 0); // moved the rvalue in the container
      ASSERT_EQ(common::getTotalDtor(), 0); // dtor the rvalue
      ASSERT_EQ(common::getTotalAlloc(), 0);
      ASSERT_EQ(common::getTotalDealloc(), 0);
      common::reset();

      myArray.push_back(tmpData);

      ASSERT_EQ(myArray.is_empty(), false);
      ASSERT_EQ(myArray.size(), 1);
      ASSERT_EQ(myArray.capacity(), 5);
      ASSERT_EQ(myArray.is_out_of_range(0), false);
      ASSERT_EQ(myArray.is_out_of_range(1), true);

      ASSERT_EQ(common::getTotalCtor(), 0);
      ASSERT_EQ(common::getTotalCopyCtor(), 1); // ctor the ref value in the container
      ASSERT_EQ(common::getTotalMoveCtor(), 0);
      ASSERT_EQ(common::getTotalDtor(), 0);
      ASSERT_EQ(common::getTotalAlloc(), 0);
      ASSERT_EQ(common::getTotalDealloc(), 0);
      common::reset();
    }

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 1); // dtor tmpData
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    {
      common::TestStructureCopyable tmpData(2, "2");

      ASSERT_EQ(common::getTotalCtor(), 1); // ctor the rvalue
      ASSERT_EQ(common::getTotalCopyCtor(), 0);
      ASSERT_EQ(common::getTotalMoveCtor(), 0); // moved the rvalue in the container
      ASSERT_EQ(common::getTotalDtor(), 0); // dtor the rvalue
      ASSERT_EQ(common::getTotalAlloc(), 0);
      ASSERT_EQ(common::getTotalDealloc(), 0);
      common::reset();

      myArray.push_back(tmpData);

      ASSERT_EQ(myArray.is_empty(), false);
      ASSERT_EQ(myArray.size(), 2);
      ASSERT_EQ(myArray.capacity(), 5);
      ASSERT_EQ(myArray.is_out_of_range(0), false);
      ASSERT_EQ(myArray.is_out_of_range(1), false);
      ASSERT_EQ(myArray.is_out_of_range(2), true);

      ASSERT_EQ(common::getTotalCtor(), 0);
      ASSERT_EQ(common::getTotalCopyCtor(), 1); // ctor the ref value in the container
      ASSERT_EQ(common::getTotalMoveCtor(), 0);
      ASSERT_EQ(common::getTotalDtor(), 0);
      ASSERT_EQ(common::getTotalAlloc(), 0);
      ASSERT_EQ(common::getTotalDealloc(), 0);
      common::reset();
    }

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 1); // dtor tmpData
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    {
      common::TestStructureCopyable tmpData(3, "3");

      ASSERT_EQ(common::getTotalCtor(), 1); // ctor the rvalue
      ASSERT_EQ(common::getTotalCopyCtor(), 0);
      ASSERT_EQ(common::getTotalMoveCtor(), 0); // moved the rvalue in the container
      ASSERT_EQ(common::getTotalDtor(), 0); // dtor the rvalue
      ASSERT_EQ(common::getTotalAlloc(), 0);
      ASSERT_EQ(common::getTotalDealloc(), 0);
      common::reset();

      myArray.push_back(tmpData);

      ASSERT_EQ(myArray.is_empty(), false);
      ASSERT_EQ(myArray.size(), 3);
      ASSERT_EQ(myArray.capacity(), 5);
      ASSERT_EQ(myArray.is_out_of_range(0), false);
      ASSERT_EQ(myArray.is_out_of_range(1), false);
      ASSERT_EQ(myArray.is_out_of_range(2), false);
      ASSERT_EQ(myArray.is_out_of_range(3), true);

      ASSERT_EQ(common::getTotalCtor(), 0);
      ASSERT_EQ(common::getTotalCopyCtor(), 1); // ctor the ref value in the container
      ASSERT_EQ(common::getTotalMoveCtor(), 0);
      ASSERT_EQ(common::getTotalDtor(), 0);
      ASSERT_EQ(common::getTotalAlloc(), 0);
      ASSERT_EQ(common::getTotalDealloc(), 0);
      common::reset();
    }

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 1); // dtor tmpData
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    {
      common::TestStructureCopyable tmpData(4, "4");

      ASSERT_EQ(common::getTotalCtor(), 1); // ctor the rvalue
      ASSERT_EQ(common::getTotalCopyCtor(), 0);
      ASSERT_EQ(common::getTotalMoveCtor(), 0); // moved the rvalue in the container
      ASSERT_EQ(common::getTotalDtor(), 0); // dtor the rvalue
      ASSERT_EQ(common::getTotalAlloc(), 0);
      ASSERT_EQ(common::getTotalDealloc(), 0);
      common::reset();

      myArray.push_back(tmpData);

      ASSERT_EQ(myArray.is_empty(), false);
      ASSERT_EQ(myArray.size(), 4);
      ASSERT_EQ(myArray.capacity(), 5);
      ASSERT_EQ(myArray.is_out_of_range(0), false);
      ASSERT_EQ(myArray.is_out_of_range(1), false);
      ASSERT_EQ(myArray.is_out_of_range(2), false);
      ASSERT_EQ(myArray.is_out_of_range(3), false);
      ASSERT_EQ(myArray.is_out_of_range(4), true);

      ASSERT_EQ(common::getTotalCtor(), 0);
      ASSERT_EQ(common::getTotalCopyCtor(), 1); // ctor the ref value in the container
      ASSERT_EQ(common::getTotalMoveCtor(), 0);
      ASSERT_EQ(common::getTotalDtor(), 0);
      ASSERT_EQ(common::getTotalAlloc(), 0);
      ASSERT_EQ(common::getTotalDealloc(), 0);
      common::reset();
    }

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 1); // dtor tmpData
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

  }

  ASSERT_EQ(common::getTotalCtor(), 0);
  ASSERT_EQ(common::getTotalCopyCtor(), 0);
  ASSERT_EQ(common::getTotalMoveCtor(), 0);
  ASSERT_EQ(common::getTotalDtor(), 4);
  ASSERT_EQ(common::getTotalAlloc(), 0);
  ASSERT_EQ(common::getTotalDealloc(), 1);
}
