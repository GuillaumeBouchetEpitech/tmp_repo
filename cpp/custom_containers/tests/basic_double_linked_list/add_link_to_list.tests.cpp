
#include "headers.hpp"

TEST(basic_double_linked_list, can_add_link_to_list_once) {
  List list;
  Link link1;

  // start at null and size 0
  ASSERT_EQ(list.head_link, nullptr);
  ASSERT_EQ(list.size, 0);
  ASSERT_EQ(link1.prev_link, nullptr);
  ASSERT_EQ(link1.next_link, nullptr);

  list.add_link(link1);

  // end at not-null and size 1
  ASSERT_EQ(list.head_link, &link1); // head is NEW_LINK
  ASSERT_EQ(list.size, 1);
  ASSERT_EQ(link1.prev_link, nullptr);
  ASSERT_EQ(link1.next_link, nullptr);
}

TEST(basic_double_linked_list, can_add_link_to_list_twice) {
  List list;
  Link link1;
  Link link2;

  ASSERT_EQ(list.head_link, nullptr);
  ASSERT_EQ(list.size, 0);
  ASSERT_EQ(link1.prev_link, nullptr);
  ASSERT_EQ(link1.next_link, nullptr);
  ASSERT_EQ(link2.prev_link, nullptr);
  ASSERT_EQ(link2.next_link, nullptr);

  list.add_link(link1);
  list.add_link(link2);

  ASSERT_EQ(list.head_link, &link2); // head is NEW_LINK_2
  ASSERT_EQ(list.size, 2);
  ASSERT_EQ(link2.prev_link, nullptr);
  ASSERT_EQ(link2.next_link, &link1); // NEW_LINK_2 next is NEW_LINK_1
  ASSERT_EQ(link1.prev_link, &link2); // NEW_LINK_1 prev is NEW_LINK_2
  ASSERT_EQ(link1.next_link, nullptr);
}

TEST(basic_double_linked_list, can_add_link_to_list_three_times) {
  List list;
  Link link1;
  Link link2;
  Link link3;

  ASSERT_EQ(list.head_link, nullptr);
  ASSERT_EQ(list.size, 0);
  ASSERT_EQ(link1.prev_link, nullptr);
  ASSERT_EQ(link1.next_link, nullptr);
  ASSERT_EQ(link2.prev_link, nullptr);
  ASSERT_EQ(link2.next_link, nullptr);

  list.add_link(link1);
  list.add_link(link2);
  list.add_link(link3);

  ASSERT_EQ(list.head_link, &link3); // head is NEW_LINK_3
  ASSERT_EQ(list.size, 3);

  ASSERT_EQ(link3.prev_link, nullptr);
  ASSERT_EQ(link3.next_link, &link2); // NEW_LINK_3 next is NEW_LINK_2

  ASSERT_EQ(link2.prev_link, &link3); // NEW_LINK_2 prev is NEW_LINK_3
  ASSERT_EQ(link2.next_link, &link1); // NEW_LINK_2 next is NEW_LINK_1

  ASSERT_EQ(link1.prev_link, &link2); // NEW_LINK_1 prev is NEW_LINK_2
  ASSERT_EQ(link1.next_link, nullptr);
}
