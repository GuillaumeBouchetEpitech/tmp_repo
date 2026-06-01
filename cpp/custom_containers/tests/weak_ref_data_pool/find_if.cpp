
#include "headers.hpp"

#include <list>


TEST_F(weak_ref_data_pool, find_if) {
  common::reset();

  ASSERT_EQ(common::getTotalCtor(), 0);
  ASSERT_EQ(common::getTotalCopyCtor(), 0);
  ASSERT_EQ(common::getTotalMoveCtor(), 0);
  ASSERT_EQ(common::getTotalDtor(), 0);
  ASSERT_EQ(common::getTotalAlloc(), 0);
  ASSERT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  {

    using my_pool_type = shorthand_weak_ref_data_pool<10, true>;

    my_pool_type myPool;

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 0);
    ASSERT_EQ(common::getTotalAlloc(), 1); // allocation of the pool's memory
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 0);
    ASSERT_EQ(myPool.is_empty(), true);
    ASSERT_EQ(myPool.get_ref_count(0), 0);

    auto ref1 = myPool.acquire(111, "111");
    auto ref2 = myPool.acquire(222, "222");
    auto ref3 = myPool.acquire(333, "333");
    auto ref4 = myPool.acquire(444, "444");

    ASSERT_EQ(common::getTotalCtor(), 4);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 0);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 4);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1), 0);
    ASSERT_EQ(myPool.get_index(ref2), 1);
    ASSERT_EQ(myPool.get_index(ref3), 2);
    ASSERT_EQ(myPool.get_index(ref4), 3);
    ASSERT_EQ(myPool.get_ref_count(0), 1);
    ASSERT_EQ(myPool.get_ref_count(1), 1);
    ASSERT_EQ(myPool.get_ref_count(2), 1);
    ASSERT_EQ(myPool.get_ref_count(3), 1);

    ASSERT_EQ(ref1.is_valid(), true);
    ASSERT_EQ(ref1, true);
    ASSERT_EQ(ref1.get(), myPool.get(0).get());
    ASSERT_EQ(ref1->get_value(), 111);
    ASSERT_EQ(ref1->get_my_string(), "111");

    ASSERT_EQ(ref2.is_valid(), true);
    ASSERT_EQ(ref2, true);
    ASSERT_EQ(ref2.get(), myPool.get(1).get());
    ASSERT_EQ(ref2->get_value(), 222);
    ASSERT_EQ(ref2->get_my_string(), "222");

    ASSERT_EQ(ref3.is_valid(), true);
    ASSERT_EQ(ref3, true);
    ASSERT_EQ(ref3.get(), myPool.get(2).get());
    ASSERT_EQ(ref3->get_value(), 333);
    ASSERT_EQ(ref3->get_my_string(), "333");

    ASSERT_EQ(ref4.is_valid(), true);
    ASSERT_EQ(ref4, true);
    ASSERT_EQ(ref4.get(), myPool.get(3).get());
    ASSERT_EQ(ref4->get_value(), 444);
    ASSERT_EQ(ref4->get_my_string(), "444");

    //
    //
    //

    {
      auto foundRef = myPool.find_if([](const my_pool_type::value_type& inValue) -> bool {
        return (
          inValue.get_value() > 300 &&
          inValue.get_value() < 400
        );
      });

      ASSERT_LE(foundRef->get_value(), 333);
      ASSERT_LE(foundRef->get_my_string(), "333");
    }

    //
    //
    //

    {
      auto foundRef = myPool.find_if([](const my_pool_type::weak_ref& inRef) -> bool {
        return (
          inRef->get_value() > 300 &&
          inRef->get_value() < 400
        );
      });

      ASSERT_LE(foundRef->get_value(), 333);
      ASSERT_LE(foundRef->get_my_string(), "333");
    }

    //
    //
    //

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 0);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 4);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1), 0);
    ASSERT_EQ(myPool.get_index(ref2), 1);
    ASSERT_EQ(myPool.get_index(ref3), 2);
    ASSERT_EQ(myPool.get_index(ref4), 3);
    ASSERT_EQ(myPool.get_ref_count(0), 1);
    ASSERT_EQ(myPool.get_ref_count(1), 1);
    ASSERT_EQ(myPool.get_ref_count(2), 1);
    ASSERT_EQ(myPool.get_ref_count(3), 1);

    ASSERT_EQ(ref1.is_valid(), true);
    ASSERT_EQ(ref1, true);
    ASSERT_EQ(ref1.get(), myPool.get(0).get());
    ASSERT_EQ(ref1->get_value(), 111);
    ASSERT_EQ(ref1->get_my_string(), "111");

    ASSERT_EQ(ref2.is_valid(), true);
    ASSERT_EQ(ref2, true);
    ASSERT_EQ(ref2.get(), myPool.get(1).get());
    ASSERT_EQ(ref2->get_value(), 222);
    ASSERT_EQ(ref2->get_my_string(), "222");

    ASSERT_EQ(ref3.is_valid(), true);
    ASSERT_EQ(ref3, true);
    ASSERT_EQ(ref3.get(), myPool.get(2).get());
    ASSERT_EQ(ref3->get_value(), 333);
    ASSERT_EQ(ref3->get_my_string(), "333");

    ASSERT_EQ(ref4.is_valid(), true);
    ASSERT_EQ(ref4, true);
    ASSERT_EQ(ref4.get(), myPool.get(3).get());
    ASSERT_EQ(ref4->get_value(), 444);
    ASSERT_EQ(ref4->get_my_string(), "444");
  }

  ASSERT_EQ(common::getTotalCtor(), 0);
  ASSERT_EQ(common::getTotalCopyCtor(), 0);
  ASSERT_EQ(common::getTotalMoveCtor(), 0);
  ASSERT_EQ(common::getTotalDtor(), 4); // elements from the pool
  ASSERT_EQ(common::getTotalAlloc(), 0);
  ASSERT_EQ(common::getTotalDealloc(), 1); // deallocation of the pool's memory
  common::reset();
}


