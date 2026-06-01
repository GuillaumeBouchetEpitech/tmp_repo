

#include "headers.hpp"


// TEST_F(weak_ref_data_pool, can_move_data_into_a_weak_ref) {

//   using my_pool_type = custom_containers::weak_ref_data_pool<
//     common::TestStructureNonCopyable,
//     common::TestStructureNonCopyable, // <- no public interface type
//     10, // initial size
//     true, // no realloc
//     common::MyAllocator
//   >;

//   my_pool_type myPool;

//   auto mainRef = myPool.acquire(666, "test");

//   ASSERT_EQ(mainRef->get_value(), 666);
//   ASSERT_EQ(mainRef->get_my_string(), "test");

//   common::TestStructureNonCopyable tmpData(777, "test test");
//   *mainRef = std::move(tmpData);

//   ASSERT_EQ(mainRef->get_value(), 777);
//   ASSERT_EQ(mainRef->get_my_string(), "test test");

//   // ASSERT_EQ(tmpData.get_value(), 666);
//   // ASSERT_EQ(tmpData.get_my_string(), "test");
// }

// TEST_F(weak_ref_data_pool, cannot_move_data_into_a_weak_ref_when_using_an_interface) {

//   using my_pool_type = custom_containers::weak_ref_data_pool<
//     common::TestStructureNonCopyable,
//     common::ITestStructure, // <- public interface type
//     10, // initial size
//     true, // no realloc
//     common::MyAllocator
//   >;

//   my_pool_type myPool;

//   auto mainRef = myPool.acquire(666, "test");

//   ASSERT_EQ(mainRef->get_value(), 666);
//   ASSERT_EQ(mainRef->get_my_string(), "test");

//   common::TestStructureNonCopyable tmpData(777, "test test");
//   *mainRef = std::move(tmpData);

//   ASSERT_EQ(mainRef->get_value(), 666);
//   ASSERT_EQ(mainRef->get_my_string(), "test");
// }


