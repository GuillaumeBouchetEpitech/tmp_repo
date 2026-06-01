
#include "headers.hpp"

#include <list>



TEST_F(weak_ref_data_pool, multiple_weak_ref_that_can_be_copied) {
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

    my_pool_type::weak_ref ref1_a = myPool.acquire(111, "111");

    ASSERT_EQ(common::getTotalCtor(), 1);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 0);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 1);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1_a), 0);
    ASSERT_EQ(myPool.get_ref_count(0), 1);

    ASSERT_EQ(ref1_a.is_valid(), true);
    ASSERT_EQ(ref1_a, true);
    ASSERT_EQ(ref1_a.get(), myPool.get(0).get());
    ASSERT_EQ(ref1_a->get_value(), 111);
    ASSERT_EQ(ref1_a->get_my_string(), "111");


    auto ref1_b = ref1_a;


    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 0);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 1);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1_a), 0);
    ASSERT_EQ(myPool.get_index(ref1_b), 0);
    ASSERT_EQ(myPool.get_ref_count(0), 2); // 2 weak ref for this pool element

    ASSERT_EQ(ref1_a.is_valid(), true);
    ASSERT_EQ(ref1_a, true);
    ASSERT_EQ(ref1_a.get(), myPool.get(0).get());
    ASSERT_EQ(ref1_a->get_value(), 111);
    ASSERT_EQ(ref1_a->get_my_string(), "111");

    ASSERT_EQ(ref1_b.is_valid(), true);
    ASSERT_EQ(ref1_b, true);
    ASSERT_EQ(ref1_b.get(), myPool.get(0).get());
    ASSERT_EQ(ref1_b->get_value(), 111);
    ASSERT_EQ(ref1_b->get_my_string(), "111");


    auto ref1_c = ref1_b;


    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 0);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 1);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1_a), 0);
    ASSERT_EQ(myPool.get_index(ref1_b), 0);
    ASSERT_EQ(myPool.get_index(ref1_c), 0);
    ASSERT_EQ(myPool.get_ref_count(0), 3); // 3 weak ref for this pool element

    ASSERT_EQ(ref1_a.is_valid(), true);
    ASSERT_EQ(ref1_a, true);
    ASSERT_EQ(ref1_a.get(), myPool.get(0).get());
    ASSERT_EQ(ref1_a->get_value(), 111);
    ASSERT_EQ(ref1_a->get_my_string(), "111");

    ASSERT_EQ(ref1_b.is_valid(), true);
    ASSERT_EQ(ref1_b, true);
    ASSERT_EQ(ref1_b.get(), myPool.get(0).get());
    ASSERT_EQ(ref1_b->get_value(), 111);
    ASSERT_EQ(ref1_b->get_my_string(), "111");

    ASSERT_EQ(ref1_c.is_valid(), true);
    ASSERT_EQ(ref1_c, true);
    ASSERT_EQ(ref1_c.get(), myPool.get(0).get());
    ASSERT_EQ(ref1_c->get_value(), 111);
    ASSERT_EQ(ref1_c->get_my_string(), "111");
  }

  ASSERT_EQ(common::getTotalCtor(), 0);
  ASSERT_EQ(common::getTotalCopyCtor(), 0);
  ASSERT_EQ(common::getTotalMoveCtor(), 0);
  ASSERT_EQ(common::getTotalDtor(), 1); // elements from the pool
  ASSERT_EQ(common::getTotalAlloc(), 0);
  ASSERT_EQ(common::getTotalDealloc(), 1); // deallocation of the pool's memory
  common::reset();
}



TEST_F(weak_ref_data_pool, multiple_weak_ref_that_can_be_moved) {
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

    auto ref1_a = myPool.acquire(111, "111");

    ASSERT_EQ(common::getTotalCtor(), 1);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 0);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 1);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1_a), 0);
    ASSERT_EQ(myPool.get_ref_count(0), 1);

    ASSERT_EQ(ref1_a.is_valid(), true);
    ASSERT_EQ(ref1_a, true);
    ASSERT_EQ(ref1_a.get(), myPool.get(0).get());
    ASSERT_EQ(ref1_a->get_value(), 111);
    ASSERT_EQ(ref1_a->get_my_string(), "111");


    auto ref1_b = std::move(ref1_a);


    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 0);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 1);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1_a), -1);
    ASSERT_EQ(myPool.get_index(ref1_b), 0);
    ASSERT_EQ(myPool.get_ref_count(0), 1); // 1 weak ref for this pool element

    ASSERT_EQ(ref1_a.is_valid(), false);
    ASSERT_EQ(ref1_a, false);

    ASSERT_EQ(ref1_b.is_valid(), true);
    ASSERT_EQ(ref1_b, true);
    ASSERT_EQ(ref1_b.get(), myPool.get(0).get());
    ASSERT_EQ(ref1_b->get_value(), 111);
    ASSERT_EQ(ref1_b->get_my_string(), "111");


    auto ref1_c = std::move(ref1_b);


    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 0);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 1);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1_a), -1);
    ASSERT_EQ(myPool.get_index(ref1_b), -1);
    ASSERT_EQ(myPool.get_index(ref1_c), 0);
    ASSERT_EQ(myPool.get_ref_count(0), 1); // 1 weak ref for this pool element

    ASSERT_EQ(ref1_a.is_valid(), false);
    ASSERT_EQ(ref1_a, false);

    ASSERT_EQ(ref1_b.is_valid(), false);
    ASSERT_EQ(ref1_b, false);

    ASSERT_EQ(ref1_c.is_valid(), true);
    ASSERT_EQ(ref1_c, true);
    ASSERT_EQ(ref1_c.get(), myPool.get(0).get());
    ASSERT_EQ(ref1_c->get_value(), 111);
    ASSERT_EQ(ref1_c->get_my_string(), "111");
  }

  ASSERT_EQ(common::getTotalCtor(), 0);
  ASSERT_EQ(common::getTotalCopyCtor(), 0);
  ASSERT_EQ(common::getTotalMoveCtor(), 0);
  ASSERT_EQ(common::getTotalDtor(), 1); // elements from the pool
  ASSERT_EQ(common::getTotalAlloc(), 0);
  ASSERT_EQ(common::getTotalDealloc(), 1); // deallocation of the pool's memory
  common::reset();
}



TEST_F(weak_ref_data_pool, multiple_weak_ref_that_can_be_swapped) {
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

    auto ref1_a = myPool.acquire(111, "111");

    ASSERT_EQ(common::getTotalCtor(), 1);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 0);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 1);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1_a), 0);
    ASSERT_EQ(myPool.get_ref_count(0), 1);

    ASSERT_EQ(ref1_a.is_valid(), true);
    ASSERT_EQ(ref1_a, true);
    ASSERT_EQ(ref1_a.get(), myPool.get(0).get());
    ASSERT_EQ(ref1_a->get_value(), 111);
    ASSERT_EQ(ref1_a->get_my_string(), "111");


    auto ref1_b = my_pool_type::weak_ref::make_invalid();
    std::swap(ref1_b, ref1_a);


    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 0);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 1);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1_a), -1);
    ASSERT_EQ(myPool.get_index(ref1_b), 0);
    ASSERT_EQ(myPool.get_ref_count(0), 1); // 1 weak ref for this pool element

    ASSERT_EQ(ref1_a.is_valid(), false);
    ASSERT_EQ(ref1_a, false);

    ASSERT_EQ(ref1_b.is_valid(), true);
    ASSERT_EQ(ref1_b, true);
    ASSERT_EQ(ref1_b.get(), myPool.get(0).get());
    ASSERT_EQ(ref1_b->get_value(), 111);
    ASSERT_EQ(ref1_b->get_my_string(), "111");


    auto ref1_c = my_pool_type::weak_ref::make_invalid();
    std::swap(ref1_c, ref1_b);


    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 0);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 1);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1_a), -1);
    ASSERT_EQ(myPool.get_index(ref1_b), -1);
    ASSERT_EQ(myPool.get_index(ref1_c), 0);
    ASSERT_EQ(myPool.get_ref_count(0), 1); // 1 weak ref for this pool element

    ASSERT_EQ(ref1_a.is_valid(), false);
    ASSERT_EQ(ref1_a, false);

    ASSERT_EQ(ref1_b.is_valid(), false);
    ASSERT_EQ(ref1_b, false);

    ASSERT_EQ(ref1_c.is_valid(), true);
    ASSERT_EQ(ref1_c, true);
    ASSERT_EQ(ref1_c.get(), myPool.get(0).get());
    ASSERT_EQ(ref1_c->get_value(), 111);
    ASSERT_EQ(ref1_c->get_my_string(), "111");
  }

  ASSERT_EQ(common::getTotalCtor(), 0);
  ASSERT_EQ(common::getTotalCopyCtor(), 0);
  ASSERT_EQ(common::getTotalMoveCtor(), 0);
  ASSERT_EQ(common::getTotalDtor(), 1); // elements from the pool
  ASSERT_EQ(common::getTotalAlloc(), 0);
  ASSERT_EQ(common::getTotalDealloc(), 1); // deallocation of the pool's memory
  common::reset();
}



TEST_F(weak_ref_data_pool, multiple_weak_ref_that_are_updated_when_the_value_is_changed) {
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

    auto ref1_a = myPool.acquire(111, "111");

    ASSERT_EQ(common::getTotalCtor(), 1);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 0);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 1);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1_a), 0);
    ASSERT_EQ(myPool.get_ref_count(0), 1);

    ASSERT_EQ(ref1_a.is_valid(), true);
    ASSERT_EQ(ref1_a, true);
    ASSERT_EQ(ref1_a.get(), myPool.get(0).get());
    ASSERT_EQ(ref1_a->get_value(), 111);
    ASSERT_EQ(ref1_a->get_my_string(), "111");


    auto ref1_b = ref1_a;


    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 0);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 1);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1_a), 0);
    ASSERT_EQ(myPool.get_index(ref1_b), 0);
    ASSERT_EQ(myPool.get_ref_count(0), 2); // 2 weak ref for this pool element

    ASSERT_EQ(ref1_a.is_valid(), true);
    ASSERT_EQ(ref1_a, true);
    ASSERT_EQ(ref1_a.get(), myPool.get(0).get());
    ASSERT_EQ(ref1_a->get_value(), 111);
    ASSERT_EQ(ref1_a->get_my_string(), "111");

    ASSERT_EQ(ref1_b.is_valid(), true);
    ASSERT_EQ(ref1_b, true);
    ASSERT_EQ(ref1_b.get(), myPool.get(0).get());
    ASSERT_EQ(ref1_b->get_value(), 111);
    ASSERT_EQ(ref1_b->get_my_string(), "111");


    auto ref1_c = ref1_b;


    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 0);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 1);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1_a), 0);
    ASSERT_EQ(myPool.get_index(ref1_b), 0);
    ASSERT_EQ(myPool.get_index(ref1_c), 0);
    ASSERT_EQ(myPool.get_ref_count(0), 3); // 3 weak ref for this pool element

    ASSERT_EQ(ref1_a.is_valid(), true);
    ASSERT_EQ(ref1_a, true);
    ASSERT_EQ(ref1_a.get(), myPool.get(0).get());
    ASSERT_EQ(ref1_a->get_value(), 111);
    ASSERT_EQ(ref1_a->get_my_string(), "111");

    ASSERT_EQ(ref1_b.is_valid(), true);
    ASSERT_EQ(ref1_b, true);
    ASSERT_EQ(ref1_b.get(), myPool.get(0).get());
    ASSERT_EQ(ref1_b->get_value(), 111);
    ASSERT_EQ(ref1_b->get_my_string(), "111");

    ASSERT_EQ(ref1_c.is_valid(), true);
    ASSERT_EQ(ref1_c, true);
    ASSERT_EQ(ref1_c.get(), myPool.get(0).get());
    ASSERT_EQ(ref1_c->get_value(), 111);
    ASSERT_EQ(ref1_c->get_my_string(), "111");


    // ref1 b will update the value
    ref1_b.get()->set_value(111111);
    ref1_b.get()->set_my_string("111111");


    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 0);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 1);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1_a), 0);
    ASSERT_EQ(myPool.get_index(ref1_b), 0);
    ASSERT_EQ(myPool.get_index(ref1_c), 0);
    ASSERT_EQ(myPool.get_ref_count(0), 3); // 3 weak ref for this pool element

    ASSERT_EQ(ref1_a.is_valid(), true);
    ASSERT_EQ(ref1_a, true);
    ASSERT_EQ(ref1_a.get(), myPool.get(0).get());
    ASSERT_EQ(ref1_a->get_value(), 111111);
    ASSERT_EQ(ref1_a->get_my_string(), "111111");

    ASSERT_EQ(ref1_b.is_valid(), true);
    ASSERT_EQ(ref1_b, true);
    ASSERT_EQ(ref1_b.get(), myPool.get(0).get());
    ASSERT_EQ(ref1_b->get_value(), 111111);
    ASSERT_EQ(ref1_b->get_my_string(), "111111");

    ASSERT_EQ(ref1_c.is_valid(), true);
    ASSERT_EQ(ref1_c, true);
    ASSERT_EQ(ref1_c.get(), myPool.get(0).get());
    ASSERT_EQ(ref1_c->get_value(), 111111);
    ASSERT_EQ(ref1_c->get_my_string(), "111111");
  }

  ASSERT_EQ(common::getTotalCtor(), 0);
  ASSERT_EQ(common::getTotalCopyCtor(), 0);
  ASSERT_EQ(common::getTotalMoveCtor(), 0);
  ASSERT_EQ(common::getTotalDtor(), 1); // elements from the pool
  ASSERT_EQ(common::getTotalAlloc(), 0);
  ASSERT_EQ(common::getTotalDealloc(), 1); // deallocation of the pool's memory
  common::reset();
}



TEST_F(weak_ref_data_pool, multiple_weak_ref_that_can_be_invalidated_one_by_one) {
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


    ref2.invalidate();
    ref3.invalidate();


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
    ASSERT_EQ(myPool.get_index(ref2), -1);
    ASSERT_EQ(myPool.get_index(ref3), -1);
    ASSERT_EQ(myPool.get_index(ref4), 3);
    ASSERT_EQ(myPool.get_ref_count(0), 1);
    ASSERT_EQ(myPool.get_ref_count(1), 0);
    ASSERT_EQ(myPool.get_ref_count(2), 0);
    ASSERT_EQ(myPool.get_ref_count(3), 1);

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
}



TEST_F(weak_ref_data_pool, multiple_weak_ref_that_can_be_copied_and_then_all_invalidated) {
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
    auto ref2 = ref1;
    auto ref3 = ref1;
    auto ref4 = ref1;

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
    ASSERT_EQ(myPool.get_index(ref2), 0);
    ASSERT_EQ(myPool.get_index(ref3), 0);
    ASSERT_EQ(myPool.get_index(ref4), 0);
    ASSERT_EQ(myPool.get_ref_count(0), 4);

    ASSERT_EQ(ref1.is_valid(), true);
    ASSERT_EQ(ref1, true);
    ASSERT_EQ(ref1.get(), myPool.get(0).get());
    ASSERT_EQ(ref1->get_value(), 111);
    ASSERT_EQ(ref1->get_my_string(), "111");

    ASSERT_EQ(ref2.is_valid(), true);
    ASSERT_EQ(ref2, true);
    ASSERT_EQ(ref2.get(), myPool.get(0).get());
    ASSERT_EQ(ref2->get_value(), 111);
    ASSERT_EQ(ref2->get_my_string(), "111");

    ASSERT_EQ(ref3.is_valid(), true);
    ASSERT_EQ(ref3, true);
    ASSERT_EQ(ref3.get(), myPool.get(0).get());
    ASSERT_EQ(ref3->get_value(), 111);
    ASSERT_EQ(ref3->get_my_string(), "111");

    ASSERT_EQ(ref4.is_valid(), true);
    ASSERT_EQ(ref4, true);
    ASSERT_EQ(ref4.get(), myPool.get(0).get());
    ASSERT_EQ(ref4->get_value(), 111);
    ASSERT_EQ(ref4->get_my_string(), "111");

    ref2.invalidate_all();

    ASSERT_EQ(common::getTotalCtor(), 0);
    ASSERT_EQ(common::getTotalCopyCtor(), 0);
    ASSERT_EQ(common::getTotalMoveCtor(), 0);
    ASSERT_EQ(common::getTotalDtor(), 0);
    ASSERT_EQ(common::getTotalAlloc(), 0);
    ASSERT_EQ(common::getTotalDealloc(), 0);
    common::reset();

    ASSERT_EQ(myPool.size(), 1);
    ASSERT_EQ(myPool.is_empty(), false);
    ASSERT_EQ(myPool.get_index(ref1), -1);
    ASSERT_EQ(myPool.get_index(ref2), -1);
    ASSERT_EQ(myPool.get_index(ref3), -1);
    ASSERT_EQ(myPool.get_index(ref4), -1);
    ASSERT_EQ(myPool.get_ref_count(0), 0);

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
  ASSERT_EQ(common::getTotalDtor(), 1); // elements from the pool
  ASSERT_EQ(common::getTotalAlloc(), 0);
  ASSERT_EQ(common::getTotalDealloc(), 1); // deallocation of the pool's memory
}


