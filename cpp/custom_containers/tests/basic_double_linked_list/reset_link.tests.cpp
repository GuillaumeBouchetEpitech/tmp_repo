
#include "headers.hpp"

TEST(basic_double_linked_list, link_can_be_reset) {
  Link link1;
  link1.prev_link = (Link*)666;
  link1.next_link = (Link*)666;

  link1.reset();

  ASSERT_EQ(link1.prev_link, nullptr);
  ASSERT_EQ(link1.next_link, nullptr);
}
