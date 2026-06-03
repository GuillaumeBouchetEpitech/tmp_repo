
# Custom Containers

## weak_ref_data_pool

```C++
weak_ref_data_pool<
  typename InternalBaseType,
  typename PublicBaseType = InternalBaseType,
  std::size_t initial_size = 256,
  bool no_realloc = true,
  template <typename...> class Allocator = std::allocator
>;
```

### Small Example

```C++

#include "gtest/gtest.h"

#include "weak_ref_data_pool.hpp"

#include <iostream>

namespace /*anonymous*/ {

  // external / public class (interface)
  class SomePublicEntity {
  public:
    virtual ~SomePublicEntity() = default;
    virtual void update(float deltaTimeSec) = 0;
    virtual void render() const = 0;
  };

  // internal / private class (implementation)
  class SomePrivateEntity : public SomePublicEntity {
    public:
      SomePrivateEntity(std::string_view inSomeStr, int32_t inSomeInt) {
      }
      void update(float deltaTimeSec) override {
        // complex logic here
      }
      void render() const override {
        // complex logic here
      }
    private:
      // attributes here
  };

  // pool type
  using some_entities_pool = custom_containers::weak_ref_data_pool<
    SomePrivateEntity, // internal / private - type / implementation
    SomePublicEntity, // external / public - type / interface
    512, // initial size (default: 256)
    true, // no realloc (default: true)
    std::allocator // custom allocator (default: std::allocator)
  >;

  using some_weak_ref = some_entities_pool::weak_ref;
  using some_value_type = some_entities_pool::value_type; // <- SomePublicEntity
};

TEST(weak_ref_data_pool, simple_usecase) {

  some_entities_pool someEntitiesPool;

  some_weak_ref helloRef = someEntitiesPool.acquire("hello", 777);
  some_weak_ref worldRef = someEntitiesPool.acquire("world", 888);

  ASSERT_EQ(helloRef.is_valid(), true);
  ASSERT_EQ(helloRef, true); // operator bool() supported (when valid -> true)

  auto invalidRef = some_weak_ref::make_invalid(); // created invalid
  ASSERT_EQ(invalidRef.is_valid(), false);
  ASSERT_EQ(invalidRef, false);

  auto copiedRef = helloRef; // copy
  ASSERT_EQ(copiedRef.is_valid(), true);
  ASSERT_EQ(copiedRef, true);

  invalidRef = std::move(copiedRef); // move semantic supported
  ASSERT_EQ(invalidRef.is_valid(), true);
  ASSERT_EQ(invalidRef, true);

  std::swap(invalidRef, copiedRef); // ...can also use std::swap
  ASSERT_EQ(copiedRef.is_valid(), true);
  ASSERT_EQ(copiedRef, true);

  ASSERT_EQ(helloRef.is_valid(), true);
  ASSERT_EQ(copiedRef.is_valid(), true);
  ASSERT_EQ(invalidRef.is_valid(), false);

  // non-constant for-each
  someEntitiesPool.for_each([](some_value_type& inVal) -> void {
    constexpr float k_fixedStep = 1.0f / 60.0f;
    inVal.update(k_fixedStep);
  });

  // constant for-each
  someEntitiesPool.for_each([](const some_value_type& inVal) -> void {
    inVal.render();
  });

  // filtering
  someEntitiesPool.filter([](const some_value_type& inVal) -> bool {
    return true; // returning true wil keep the element, else -> fast unsorted removal
  });

  // search
  auto foundRef = someEntitiesPool.find_if([](const some_value_type& inVal) -> bool {
    return true; // returning true to get a weak_ref of the element
  });

  // all weak_ref(s) of this element are now invalid
  helloRef.invalidate_all();
  ASSERT_EQ(copiedRef.is_valid(), false);

  ASSERT_EQ(someEntitiesPool.size(), 2);
  ASSERT_EQ(someEntitiesPool.is_empty(), false);

  // fast unsorted removal of any unreferenced element(s)
  someEntitiesPool.remove_unreferenced_items();

  ASSERT_EQ(someEntitiesPool.size(), 1);
  ASSERT_EQ(someEntitiesPool.is_empty(), false);

  // manual fast unsorted removal of a referenced element
  // -> all weak_ref(s) of this element are now invalid
  someEntitiesPool.release(worldRef);

  ASSERT_EQ(someEntitiesPool.size(), 0);
  ASSERT_EQ(someEntitiesPool.is_empty(), true);

  // pool is emptied -> all existing weak_ref are invalidated
  someEntitiesPool.clear();
}

```

## Testing

```
###########
# TESTING #
###########

==1080823== Memcheck, a memory error detector
==1080823== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==1080823== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==1080823== Command: ./tests/_bin/custom-container-unit-tests
==1080823==
Running main() from ./googletest/src/gtest_main.cc
[==========] Running 57 tests from 5 test suites.
[----------] Global test environment set-up.
[----------] 22 tests from basic_double_linked_list
[ RUN      ] basic_double_linked_list.can_add_link_to_list_once
[       OK ] basic_double_linked_list.can_add_link_to_list_once (15 ms)
[ RUN      ] basic_double_linked_list.can_add_link_to_list_twice
[       OK ] basic_double_linked_list.can_add_link_to_list_twice (8 ms)
[ RUN      ] basic_double_linked_list.can_add_link_to_list_three_times
[       OK ] basic_double_linked_list.can_add_link_to_list_three_times (9 ms)
[ RUN      ] basic_double_linked_list.list_is_initially_empty
[       OK ] basic_double_linked_list.list_is_initially_empty (2 ms)
[ RUN      ] basic_double_linked_list.list_link_is_initially_null
[       OK ] basic_double_linked_list.list_link_is_initially_null (3 ms)
[ RUN      ] basic_double_linked_list.can_loop_list_a_list_of_1_link
[       OK ] basic_double_linked_list.can_loop_list_a_list_of_1_link (15 ms)
[ RUN      ] basic_double_linked_list.can_loop_list_a_list_of_2_links
[       OK ] basic_double_linked_list.can_loop_list_a_list_of_2_links (17 ms)
[ RUN      ] basic_double_linked_list.can_loop_list_a_list_of_3_links
[       OK ] basic_double_linked_list.can_loop_list_a_list_of_3_links (20 ms)
[ RUN      ] basic_double_linked_list.can_loop_and_reset_list_a_list_of_1_link
[       OK ] basic_double_linked_list.can_loop_and_reset_list_a_list_of_1_link (13 ms)
[ RUN      ] basic_double_linked_list.can_loop_and_reset_list_a_list_of_2_links
[       OK ] basic_double_linked_list.can_loop_and_reset_list_a_list_of_2_links (17 ms)
[ RUN      ] basic_double_linked_list.can_loop_and_reset_list_a_list_of_3_links
[       OK ] basic_double_linked_list.can_loop_and_reset_list_a_list_of_3_links (21 ms)
[ RUN      ] basic_double_linked_list.can_remove_only_link
[       OK ] basic_double_linked_list.can_remove_only_link (9 ms)
[ RUN      ] basic_double_linked_list.can_remove_both_link_link1_first
[       OK ] basic_double_linked_list.can_remove_both_link_link1_first (17 ms)
[ RUN      ] basic_double_linked_list.can_remove_both_link_link2_first
[       OK ] basic_double_linked_list.can_remove_both_link_link2_first (16 ms)
[ RUN      ] basic_double_linked_list.can_remove_several_links_in_all_removal_combinations
[       OK ] basic_double_linked_list.can_remove_several_links_in_all_removal_combinations (713 ms)
[ RUN      ] basic_double_linked_list.link_can_be_reset
[       OK ] basic_double_linked_list.link_can_be_reset (2 ms)
[ RUN      ] basic_double_linked_list.can_reset_a_list_of_1_link
[       OK ] basic_double_linked_list.can_reset_a_list_of_1_link (8 ms)
[ RUN      ] basic_double_linked_list.can_reset_a_list_of_2_links
[       OK ] basic_double_linked_list.can_reset_a_list_of_2_links (12 ms)
[ RUN      ] basic_double_linked_list.can_reset_a_list_of_3_links
[       OK ] basic_double_linked_list.can_reset_a_list_of_3_links (16 ms)
[ RUN      ] basic_double_linked_list.can_swap_link_in_2_links_list
[       OK ] basic_double_linked_list.can_swap_link_in_2_links_list (13 ms)
[ RUN      ] basic_double_linked_list.can_swap_link_in_3_links_list
[       OK ] basic_double_linked_list.can_swap_link_in_3_links_list (26 ms)
[ RUN      ] basic_double_linked_list.can_swap_dead_link_in_3_links_list
[       OK ] basic_double_linked_list.can_swap_dead_link_in_3_links_list (32 ms)
[----------] 22 tests from basic_double_linked_list (1021 ms total)

[----------] 9 tests from static_array
[ RUN      ] static_array.total_ctor_calls
[       OK ] static_array.total_ctor_calls (20 ms)
[ RUN      ] static_array.total_dtor_calls
[       OK ] static_array.total_dtor_calls (13 ms)
[ RUN      ] static_array.data_access
[       OK ] static_array.data_access (14 ms)
[ RUN      ] static_array.raw_iterator
[       OK ] static_array.raw_iterator (6 ms)
[ RUN      ] static_array.raw_const_iterator
[       OK ] static_array.raw_const_iterator (4 ms)
[ RUN      ] static_array.raw_reverse_iterator
[       OK ] static_array.raw_reverse_iterator (4 ms)
[ RUN      ] static_array.raw_reverse_const_iterator
[       OK ] static_array.raw_reverse_const_iterator (4 ms)
[ RUN      ] static_array.loop_syntax
[       OK ] static_array.loop_syntax (3 ms)
[ RUN      ] static_array.sort
[       OK ] static_array.sort (30 ms)
[----------] 9 tests from static_array (105 ms total)

[----------] 8 tests from dynamic_heap_array
[ RUN      ] dynamic_heap_array.default_template_args
[       OK ] dynamic_heap_array.default_template_args (18 ms)
[ RUN      ] dynamic_heap_array.pre_allocate_template_args
[       OK ] dynamic_heap_array.pre_allocate_template_args (18 ms)
[ RUN      ] dynamic_heap_array.emplace_back__without_pre_allocation
[       OK ] dynamic_heap_array.emplace_back__without_pre_allocation (59 ms)
[ RUN      ] dynamic_heap_array.emplace_back__with_pre_allocation
[       OK ] dynamic_heap_array.emplace_back__with_pre_allocation (52 ms)
[ RUN      ] dynamic_heap_array.push_back__by_ref__without_pre_allocation
[       OK ] dynamic_heap_array.push_back__by_ref__without_pre_allocation (92 ms)
[ RUN      ] dynamic_heap_array.push_back__by_ref__with_pre_allocation
[       OK ] dynamic_heap_array.push_back__by_ref__with_pre_allocation (82 ms)
[ RUN      ] dynamic_heap_array.push_back__by_rvalue__without_pre_allocation
[       OK ] dynamic_heap_array.push_back__by_rvalue__without_pre_allocation (51 ms)
[ RUN      ] dynamic_heap_array.push_back__by_rvalue__with_pre_allocation
[       OK ] dynamic_heap_array.push_back__by_rvalue__with_pre_allocation (51 ms)
[----------] 8 tests from dynamic_heap_array (428 ms total)

[----------] 17 tests from weak_ref_data_pool
[ RUN      ] weak_ref_data_pool.acquire_one_weak_ref
[       OK ] weak_ref_data_pool.acquire_one_weak_ref (50 ms)
[ RUN      ] weak_ref_data_pool.try_to_acquire_weak_ref_beyond_the_limit__no_realloc
[       OK ] weak_ref_data_pool.try_to_acquire_weak_ref_beyond_the_limit__no_realloc (58 ms)
[ RUN      ] weak_ref_data_pool.try_to_acquire_weak_ref_beyond_the_limit__with_realloc
[       OK ] weak_ref_data_pool.try_to_acquire_weak_ref_beyond_the_limit__with_realloc (69 ms)
[ RUN      ] weak_ref_data_pool.filter
[       OK ] weak_ref_data_pool.filter (80 ms)
[ RUN      ] weak_ref_data_pool.for_each
[       OK ] weak_ref_data_pool.for_each (129 ms)
[ RUN      ] weak_ref_data_pool.multiple_weak_ref_that_can_be_copied
[       OK ] weak_ref_data_pool.multiple_weak_ref_that_can_be_copied (64 ms)
[ RUN      ] weak_ref_data_pool.multiple_weak_ref_that_can_be_moved
[       OK ] weak_ref_data_pool.multiple_weak_ref_that_can_be_moved (54 ms)
[ RUN      ] weak_ref_data_pool.multiple_weak_ref_that_can_be_swapped
[       OK ] weak_ref_data_pool.multiple_weak_ref_that_can_be_swapped (56 ms)
[ RUN      ] weak_ref_data_pool.multiple_weak_ref_that_are_updated_when_the_value_is_changed
[       OK ] weak_ref_data_pool.multiple_weak_ref_that_are_updated_when_the_value_is_changed (78 ms)
[ RUN      ] weak_ref_data_pool.multiple_weak_ref_that_can_be_invalidated_one_by_one
[       OK ] weak_ref_data_pool.multiple_weak_ref_that_can_be_invalidated_one_by_one (61 ms)
[ RUN      ] weak_ref_data_pool.multiple_weak_ref_that_can_be_copied_and_then_all_invalidated
[       OK ] weak_ref_data_pool.multiple_weak_ref_that_can_be_copied_and_then_all_invalidated (53 ms)
[ RUN      ] weak_ref_data_pool.release_one_pool_element
[       OK ] weak_ref_data_pool.release_one_pool_element (36 ms)
[ RUN      ] weak_ref_data_pool.release_multiple_pool_elements_one_by_one
[       OK ] weak_ref_data_pool.release_multiple_pool_elements_one_by_one (119 ms)
[ RUN      ] weak_ref_data_pool.release_multiple_pool_elements_one_by_one__by_index
[       OK ] weak_ref_data_pool.release_multiple_pool_elements_one_by_one__by_index (116 ms)
[ RUN      ] weak_ref_data_pool.release_all_pool_elements_all_at_once
[       OK ] weak_ref_data_pool.release_all_pool_elements_all_at_once (56 ms)
[ RUN      ] weak_ref_data_pool.remove_unreferenced_items_one_by_one
[       OK ] weak_ref_data_pool.remove_unreferenced_items_one_by_one (120 ms)
[ RUN      ] weak_ref_data_pool.remove_unreferenced_items_all_at_once
[       OK ] weak_ref_data_pool.remove_unreferenced_items_all_at_once (59 ms)
[----------] 17 tests from weak_ref_data_pool (1269 ms total)

[----------] 1 test from weak_ref_data_pool_usecase1
[ RUN      ] weak_ref_data_pool_usecase1.simple_usecase
[       OK ] weak_ref_data_pool_usecase1.simple_usecase (74 ms)
[----------] 1 test from weak_ref_data_pool_usecase1 (74 ms total)

[----------] Global test environment tear-down
[==========] 57 tests from 5 test suites ran. (2945 ms total)
[  PASSED  ] 57 tests.
==1080823==
==1080823== HEAP SUMMARY:
==1080823==     in use at exit: 0 bytes in 0 blocks
==1080823==   total heap usage: 26,360 allocs, 26,360 frees, 884,905 bytes allocated
==1080823==
==1080823== All heap blocks were freed -- no leaks are possible
==1080823==
==1080823== For lists of detected and suppressed errors, rerun with: -s
==1080823== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```
