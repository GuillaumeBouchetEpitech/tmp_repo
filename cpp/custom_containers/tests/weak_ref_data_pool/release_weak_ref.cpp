
#include "headers.hpp"

#include <list>



TEST_F(weak_ref_data_pool, release_one_pool_element) {
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

    // auto ref1 = myPool.acquire(111, "111");
    auto ref1 = myPool.acquire(111, "111");

    ASSERT_EQ(common::getTotalCtor(), 1);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 0);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 1);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1), 0);
    ASSERT_EQ(myPool.get_ref_count(0), 1);

    ASSERT_EQ(ref1.is_valid(), true);
    ASSERT_EQ(ref1, true);
    ASSERT_EQ(ref1.get(), myPool.get(0).get());
    ASSERT_EQ(ref1->get_value(), 111);
    ASSERT_EQ(ref1->get_my_string(), "111");

    myPool.release(ref1);

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 1);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 0);
    ASSERT_EQ(myPool.is_empty(), true);
    ASSERT_EQ(myPool.get_index(ref1), -1);
    ASSERT_EQ(myPool.get_ref_count(0), 0);

    ASSERT_EQ(ref1.is_valid(), false);
    ASSERT_EQ(ref1, false);
  }

  ASSERT_EQ(common::getTotalCtor(), 0);
  ASSERT_EQ(common::getTotalCopyCtor(), 0);
  ASSERT_EQ(common::getTotalMoveCtor(), 0);
  ASSERT_EQ(common::getTotalDtor(), 0); // elements from the pool
  ASSERT_EQ(common::getTotalAlloc(), 0);
  ASSERT_EQ(common::getTotalDealloc(), 1); // deallocation of the pool's memory
  common::reset();
}



TEST_F(weak_ref_data_pool, release_multiple_pool_elements_one_by_one) {
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

    myPool.release(ref2); // release element 1 (in the middle of the pool)

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 3);
    ASSERT_EQ(common::getTotalDtor(), 2);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 3);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1), 0);
    ASSERT_EQ(myPool.get_index(ref2), -1);
    ASSERT_EQ(myPool.get_index(ref3), 2);
    ASSERT_EQ(myPool.get_index(ref4), 1); // swapped ref4<->ref2 pool elements
    ASSERT_EQ(myPool.get_ref_count(0), 1);
    ASSERT_EQ(myPool.get_ref_count(1), 1); // where ref4 element got moved
    ASSERT_EQ(myPool.get_ref_count(2), 1);
    ASSERT_EQ(myPool.get_ref_count(3), 0); // where ref2 element got moved

    ASSERT_EQ(ref1.is_valid(), true);
    ASSERT_EQ(ref1, true);
    ASSERT_EQ(ref1.get(), myPool.get(0).get());
    ASSERT_EQ(ref1->get_value(), 111);
    ASSERT_EQ(ref1->get_my_string(), "111");

    ASSERT_EQ(ref2.is_valid(), false);
    ASSERT_EQ(ref2, false);

    ASSERT_EQ(ref3.is_valid(), true);
    ASSERT_EQ(ref3, true);
    ASSERT_EQ(ref3.get(), myPool.get(2).get());
    ASSERT_EQ(ref3->get_value(), 333);
    ASSERT_EQ(ref3->get_my_string(), "333");

    ASSERT_EQ(ref4.is_valid(), true);
    ASSERT_EQ(ref4, true);
    ASSERT_EQ(ref4.get(), myPool.get(1).get());
    ASSERT_EQ(ref4->get_value(), 444);
    ASSERT_EQ(ref4->get_my_string(), "444");

    myPool.release(ref3); // release element 2 (at the back of the pool)

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 1);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 2);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1), 0);
    ASSERT_EQ(myPool.get_index(ref2), -1);
    ASSERT_EQ(myPool.get_index(ref3), -1);
    ASSERT_EQ(myPool.get_index(ref4), 1);
    ASSERT_EQ(myPool.get_ref_count(0), 1);
    ASSERT_EQ(myPool.get_ref_count(1), 1);
    ASSERT_EQ(myPool.get_ref_count(2), 0); // where ref3 element was
    ASSERT_EQ(myPool.get_ref_count(3), 0);

    ASSERT_EQ(ref1.is_valid(), true);
    ASSERT_EQ(ref1, true);
    ASSERT_EQ(ref1.get(), myPool.get(0).get());
    ASSERT_EQ(ref1->get_value(), 111);
    ASSERT_EQ(ref1->get_my_string(), "111");

    ASSERT_EQ(ref2.is_valid(), false);
    ASSERT_EQ(ref2, false);

    ASSERT_EQ(ref3.is_valid(), false);
    ASSERT_EQ(ref3, false);

    ASSERT_EQ(ref4.is_valid(), true);
    ASSERT_EQ(ref4, true);
    ASSERT_EQ(ref4.get(), myPool.get(1).get());
    ASSERT_EQ(ref4->get_value(), 444);
    ASSERT_EQ(ref4->get_my_string(), "444");

    myPool.release(ref1); // release element 0 (at the start of the pool)

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 3);
    ASSERT_EQ(common::getTotalDtor(), 2);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 1);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1), -1);
    ASSERT_EQ(myPool.get_index(ref2), -1);
    ASSERT_EQ(myPool.get_index(ref3), -1);
    ASSERT_EQ(myPool.get_index(ref4), 0); // new position of ref4
    ASSERT_EQ(myPool.get_ref_count(0), 1);
    ASSERT_EQ(myPool.get_ref_count(1), 0); // where ref4 element was
    ASSERT_EQ(myPool.get_ref_count(2), 0);
    ASSERT_EQ(myPool.get_ref_count(3), 0);

    ASSERT_EQ(ref1.is_valid(), false);
    ASSERT_EQ(ref1, false);

    ASSERT_EQ(ref2.is_valid(), false);
    ASSERT_EQ(ref2, false);

    ASSERT_EQ(ref3.is_valid(), false);
    ASSERT_EQ(ref3, false);

    ASSERT_EQ(ref4.is_valid(), true);
    ASSERT_EQ(ref4, true);
    ASSERT_EQ(ref4.get(), myPool.get(0).get());
    ASSERT_EQ(ref4->get_value(), 444);
    ASSERT_EQ(ref4->get_my_string(), "444");

    myPool.release(ref4); // release last element 1 (at the start of the pool)

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 1);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 0);
    ASSERT_EQ(myPool.is_empty(), true);
    ASSERT_EQ(myPool.get_index(ref1), -1);
    ASSERT_EQ(myPool.get_index(ref2), -1);
    ASSERT_EQ(myPool.get_index(ref3), -1);
    ASSERT_EQ(myPool.get_index(ref4), -1);
    ASSERT_EQ(myPool.get_ref_count(0), 0);
    ASSERT_EQ(myPool.get_ref_count(1), 0);
    ASSERT_EQ(myPool.get_ref_count(2), 0);
    ASSERT_EQ(myPool.get_ref_count(3), 0);

    ASSERT_EQ(ref1.is_valid(), false);
    ASSERT_EQ(ref1, false);

    ASSERT_EQ(ref2.is_valid(), false);
    ASSERT_EQ(ref2, false);

    ASSERT_EQ(ref3.is_valid(), false);
    ASSERT_EQ(ref3, false);

    ASSERT_EQ(ref4.is_valid(), false);
    ASSERT_EQ(ref4, false);
  }

  ASSERT_EQ(common::getTotalCtor(), 0);
  ASSERT_EQ(common::getTotalCopyCtor(), 0);
  ASSERT_EQ(common::getTotalMoveCtor(), 0);
  ASSERT_EQ(common::getTotalDtor(), 0); // elements from the pool
  ASSERT_EQ(common::getTotalAlloc(), 0);
  ASSERT_EQ(common::getTotalDealloc(), 1); // deallocation of the pool's memory
  common::reset();
}



TEST_F(weak_ref_data_pool, release_multiple_pool_elements_one_by_one__by_index) {
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

    myPool.release(1); // release element 1 (in the middle of the pool)

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 3);
    ASSERT_EQ(common::getTotalDtor(), 2);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 3);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1), 0);
    ASSERT_EQ(myPool.get_index(ref2), -1);
    ASSERT_EQ(myPool.get_index(ref3), 2);
    ASSERT_EQ(myPool.get_index(ref4), 1); // swapped ref4<->ref2 pool elements
    ASSERT_EQ(myPool.get_ref_count(0), 1);
    ASSERT_EQ(myPool.get_ref_count(1), 1); // where ref4 element got moved
    ASSERT_EQ(myPool.get_ref_count(2), 1);
    ASSERT_EQ(myPool.get_ref_count(3), 0); // where ref2 element got moved

    ASSERT_EQ(ref1.is_valid(), true);
    ASSERT_EQ(ref1, true);
    ASSERT_EQ(ref1.get(), myPool.get(0).get());
    ASSERT_EQ(ref1->get_value(), 111);
    ASSERT_EQ(ref1->get_my_string(), "111");

    ASSERT_EQ(ref2.is_valid(), false);
    ASSERT_EQ(ref2, false);

    ASSERT_EQ(ref3.is_valid(), true);
    ASSERT_EQ(ref3, true);
    ASSERT_EQ(ref3.get(), myPool.get(2).get());
    ASSERT_EQ(ref3->get_value(), 333);
    ASSERT_EQ(ref3->get_my_string(), "333");

    ASSERT_EQ(ref4.is_valid(), true);
    ASSERT_EQ(ref4, true);
    ASSERT_EQ(ref4.get(), myPool.get(1).get());
    ASSERT_EQ(ref4->get_value(), 444);
    ASSERT_EQ(ref4->get_my_string(), "444");

    myPool.release(2); // release element 2 (at the back of the pool)

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 1);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 2);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1), 0);
    ASSERT_EQ(myPool.get_index(ref2), -1);
    ASSERT_EQ(myPool.get_index(ref3), -1);
    ASSERT_EQ(myPool.get_index(ref4), 1);
    ASSERT_EQ(myPool.get_ref_count(0), 1);
    ASSERT_EQ(myPool.get_ref_count(1), 1);
    ASSERT_EQ(myPool.get_ref_count(2), 0); // where ref3 element was
    ASSERT_EQ(myPool.get_ref_count(3), 0);

    ASSERT_EQ(ref1.is_valid(), true);
    ASSERT_EQ(ref1, true);
    ASSERT_EQ(ref1.get(), myPool.get(0).get());
    ASSERT_EQ(ref1->get_value(), 111);
    ASSERT_EQ(ref1->get_my_string(), "111");

    ASSERT_EQ(ref2.is_valid(), false);
    ASSERT_EQ(ref2, false);

    ASSERT_EQ(ref3.is_valid(), false);
    ASSERT_EQ(ref3, false);

    ASSERT_EQ(ref4.is_valid(), true);
    ASSERT_EQ(ref4, true);
    ASSERT_EQ(ref4.get(), myPool.get(1).get());
    ASSERT_EQ(ref4->get_value(), 444);
    ASSERT_EQ(ref4->get_my_string(), "444");

    myPool.release(0); // release element 0 (at the start of the pool)

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 3);
    ASSERT_EQ(common::getTotalDtor(), 2);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 1);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1), -1);
    ASSERT_EQ(myPool.get_index(ref2), -1);
    ASSERT_EQ(myPool.get_index(ref3), -1);
    ASSERT_EQ(myPool.get_index(ref4), 0); // new position of ref4
    ASSERT_EQ(myPool.get_ref_count(0), 1);
    ASSERT_EQ(myPool.get_ref_count(1), 0); // where ref4 element was
    ASSERT_EQ(myPool.get_ref_count(2), 0);
    ASSERT_EQ(myPool.get_ref_count(3), 0);

    ASSERT_EQ(ref1.is_valid(), false);
    ASSERT_EQ(ref1, false);

    ASSERT_EQ(ref2.is_valid(), false);
    ASSERT_EQ(ref2, false);

    ASSERT_EQ(ref3.is_valid(), false);
    ASSERT_EQ(ref3, false);

    ASSERT_EQ(ref4.is_valid(), true);
    ASSERT_EQ(ref4, true);
    ASSERT_EQ(ref4.get(), myPool.get(0).get());
    ASSERT_EQ(ref4->get_value(), 444);
    ASSERT_EQ(ref4->get_my_string(), "444");

    myPool.release(0); // release last element (at the start of the pool)

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 1);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 0);
    ASSERT_EQ(myPool.is_empty(), true);
    ASSERT_EQ(myPool.get_index(ref1), -1);
    ASSERT_EQ(myPool.get_index(ref2), -1);
    ASSERT_EQ(myPool.get_index(ref3), -1);
    ASSERT_EQ(myPool.get_index(ref4), -1);
    ASSERT_EQ(myPool.get_ref_count(0), 0);
    ASSERT_EQ(myPool.get_ref_count(1), 0);
    ASSERT_EQ(myPool.get_ref_count(2), 0);
    ASSERT_EQ(myPool.get_ref_count(3), 0);

    ASSERT_EQ(ref1.is_valid(), false);
    ASSERT_EQ(ref1, false);

    ASSERT_EQ(ref2.is_valid(), false);
    ASSERT_EQ(ref2, false);

    ASSERT_EQ(ref3.is_valid(), false);
    ASSERT_EQ(ref3, false);

    ASSERT_EQ(ref4.is_valid(), false);
    ASSERT_EQ(ref4, false);
  }

  ASSERT_EQ(common::getTotalCtor(), 0);
  ASSERT_EQ(common::getTotalCopyCtor(), 0);
  ASSERT_EQ(common::getTotalMoveCtor(), 0);
  ASSERT_EQ(common::getTotalDtor(), 0); // elements from the pool
  ASSERT_EQ(common::getTotalAlloc(), 0);
  ASSERT_EQ(common::getTotalDealloc(), 1); // deallocation of the pool's memory
  common::reset();
}



TEST_F(weak_ref_data_pool, release_all_pool_elements_all_at_once) {
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

    myPool.clear();

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 4);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 0);
    ASSERT_EQ(myPool.is_empty(), true);
    ASSERT_EQ(myPool.get_index(ref1), -1);
    ASSERT_EQ(myPool.get_index(ref2), -1);
    ASSERT_EQ(myPool.get_index(ref3), -1);
    ASSERT_EQ(myPool.get_index(ref4), -1);
    ASSERT_EQ(myPool.get_ref_count(0), 0);
    ASSERT_EQ(myPool.get_ref_count(1), 0);
    ASSERT_EQ(myPool.get_ref_count(2), 0);
    ASSERT_EQ(myPool.get_ref_count(3), 0);

    ASSERT_EQ(ref1.is_valid(), false);
    ASSERT_EQ(ref1, false);

    ASSERT_EQ(ref2.is_valid(), false);
    ASSERT_EQ(ref2, false);

    ASSERT_EQ(ref3.is_valid(), false);
    ASSERT_EQ(ref3, false);

    ASSERT_EQ(ref4.is_valid(), false);
    ASSERT_EQ(ref4, false);
  }

  ASSERT_EQ(common::getTotalCtor(), 0);
  ASSERT_EQ(common::getTotalCopyCtor(), 0);
  ASSERT_EQ(common::getTotalMoveCtor(), 0);
  ASSERT_EQ(common::getTotalDtor(), 0); // elements from the pool
  ASSERT_EQ(common::getTotalAlloc(), 0);
  ASSERT_EQ(common::getTotalDealloc(), 1); // deallocation of the pool's memory
  common::reset();
}

