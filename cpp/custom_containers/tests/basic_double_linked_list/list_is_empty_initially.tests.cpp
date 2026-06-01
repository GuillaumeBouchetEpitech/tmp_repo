
#include "headers.hpp"

TEST(basic_double_linked_list, list_is_initially_empty) {
  List list;

  ASSERT_EQ(list.head_link, nullptr);
  ASSERT_EQ(list.size, 0);
}

TEST(basic_double_linked_list, list_link_is_initially_null) {
  Link link1;

  ASSERT_EQ(link1.prev_link, nullptr);
  ASSERT_EQ(link1.next_link, nullptr);
  ASSERT_EQ(link1.user_data, nullptr);
}
