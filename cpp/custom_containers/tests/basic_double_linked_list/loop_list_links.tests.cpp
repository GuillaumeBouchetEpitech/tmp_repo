
#include "headers.hpp"

TEST(basic_double_linked_list, can_loop_list_a_list_of_1_link) {
  List list;
  Link link1;

  ASSERT_EQ(list.head_link, nullptr);
  ASSERT_EQ(list.size, 0);
  ASSERT_EQ(link1.prev_link, nullptr);
  ASSERT_EQ(link1.next_link, nullptr);

  int val1 = 100;
  link1.user_data = reinterpret_cast<void*>(&val1);

  list.add_link(link1);

  ASSERT_EQ(list.head_link, &link1);
  ASSERT_EQ(list.size, 1);
  ASSERT_EQ(link1.prev_link, nullptr);
  ASSERT_EQ(link1.next_link, nullptr);

  std::size_t totalIteration = 0;
  std::array<int, 1> expectedLinks = {{val1}};

  list.loop<int>([&expectedLinks, &totalIteration](int* inUserData) -> void {
    ASSERT_EQ(*inUserData, expectedLinks[totalIteration]);
    ++totalIteration;
  });
  ASSERT_EQ(totalIteration, 1);

  ASSERT_EQ(list.head_link, &link1);
  ASSERT_EQ(list.size, 1);
  ASSERT_EQ(link1.prev_link, nullptr);
  ASSERT_EQ(link1.next_link, nullptr);
}

TEST(basic_double_linked_list, can_loop_list_a_list_of_2_links) {
  List list;
  Link link1;
  Link link2;

  int val1 = 100;
  int val2 = 200;
  link1.user_data = reinterpret_cast<void*>(&val1);
  link2.user_data = reinterpret_cast<void*>(&val2);

  ASSERT_EQ(list.head_link, nullptr);
  ASSERT_EQ(list.size, 0);
  ASSERT_EQ(link1.prev_link, nullptr);
  ASSERT_EQ(link1.next_link, nullptr);
  ASSERT_EQ(link2.prev_link, nullptr);
  ASSERT_EQ(link2.next_link, nullptr);

  list.add_link(link1);
  list.add_link(link2);

  ASSERT_EQ(list.head_link, &link2);
  ASSERT_EQ(list.size, 2);
  ASSERT_EQ(link2.prev_link, nullptr);
  ASSERT_EQ(link2.next_link, &link1);
  ASSERT_EQ(link1.prev_link, &link2);
  ASSERT_EQ(link1.next_link, nullptr);

  std::size_t totalIteration = 0;
  std::array<int*, 2> expectedLinks = {{&val2, &val1}};

  list.loop<int>([&expectedLinks, &totalIteration](int* inUserData) -> void {
    ASSERT_EQ(inUserData, expectedLinks[totalIteration]);
    ++totalIteration;
  });
  ASSERT_EQ(totalIteration, 2);

  ASSERT_EQ(list.head_link, &link2);
  ASSERT_EQ(list.size, 2);
  ASSERT_EQ(link2.prev_link, nullptr);
  ASSERT_EQ(link2.next_link, &link1);
  ASSERT_EQ(link1.prev_link, &link2);
  ASSERT_EQ(link1.next_link, nullptr);
}

TEST(basic_double_linked_list, can_loop_list_a_list_of_3_links) {
  List list;
  Link link1;
  Link link2;
  Link link3;

  int val1 = 100;
  int val2 = 200;
  int val3 = 300;
  link1.user_data = reinterpret_cast<void*>(&val1);
  link2.user_data = reinterpret_cast<void*>(&val2);
  link3.user_data = reinterpret_cast<void*>(&val3);

  ASSERT_EQ(list.head_link, nullptr);
  ASSERT_EQ(list.size, 0);
  ASSERT_EQ(link1.prev_link, nullptr);
  ASSERT_EQ(link1.next_link, nullptr);
  ASSERT_EQ(link2.prev_link, nullptr);
  ASSERT_EQ(link2.next_link, nullptr);
  ASSERT_EQ(link3.prev_link, nullptr);
  ASSERT_EQ(link3.next_link, nullptr);

  list.add_link(link1);
  list.add_link(link2);
  list.add_link(link3);

  ASSERT_EQ(list.head_link, &link3);
  ASSERT_EQ(list.size, 3);
  ASSERT_EQ(link3.prev_link, nullptr);
  ASSERT_EQ(link3.next_link, &link2);
  ASSERT_EQ(link2.prev_link, &link3);
  ASSERT_EQ(link2.next_link, &link1);
  ASSERT_EQ(link1.prev_link, &link2);
  ASSERT_EQ(link1.next_link, nullptr);

  std::size_t totalIteration = 0;
  std::array<int*, 3> expectedLinks = {{&val3, &val2, &val1}};

  list.loop<int>([&expectedLinks, &totalIteration](int* inUserData) -> void {
    ASSERT_EQ(inUserData, expectedLinks[totalIteration]);
    ++totalIteration;
  });
  ASSERT_EQ(totalIteration, 3);

  ASSERT_EQ(list.head_link, &link3);
  ASSERT_EQ(list.size, 3);
  ASSERT_EQ(link3.prev_link, nullptr);
  ASSERT_EQ(link3.next_link, &link2);
  ASSERT_EQ(link2.prev_link, &link3);
  ASSERT_EQ(link2.next_link, &link1);
  ASSERT_EQ(link1.prev_link, &link2);
  ASSERT_EQ(link1.next_link, nullptr);
}
