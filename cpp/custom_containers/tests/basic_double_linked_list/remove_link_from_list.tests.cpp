
#include "headers.hpp"

#include <list>

TEST(basic_double_linked_list, can_remove_only_link) {
  List list;
  Link link1;

  ASSERT_EQ(list.head_link, nullptr);
  ASSERT_EQ(list.size, 0);
  ASSERT_EQ(link1.prev_link, nullptr);
  ASSERT_EQ(link1.next_link, nullptr);

  list.add_link(link1);

  ASSERT_EQ(list.head_link, &link1);
  ASSERT_EQ(list.size, 1);
  ASSERT_EQ(link1.prev_link, nullptr);
  ASSERT_EQ(link1.next_link, nullptr);

  list.remove_link(link1);

  ASSERT_EQ(list.head_link, nullptr);
  ASSERT_EQ(list.size, 0);
  ASSERT_EQ(link1.prev_link, nullptr);
  ASSERT_EQ(link1.next_link, nullptr);
}

TEST(basic_double_linked_list, can_remove_both_link_link1_first) {
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

  ASSERT_EQ(list.head_link, &link2);
  ASSERT_EQ(list.size, 2);
  ASSERT_EQ(link2.prev_link, nullptr);
  ASSERT_EQ(link2.next_link, &link1);
  ASSERT_EQ(link1.prev_link, &link2);
  ASSERT_EQ(link1.next_link, nullptr);

  list.remove_link(link1);

  ASSERT_EQ(list.head_link, &link2);
  ASSERT_EQ(list.size, 1);
  ASSERT_EQ(link1.prev_link, nullptr);
  ASSERT_EQ(link1.next_link, nullptr);
  ASSERT_EQ(link2.prev_link, nullptr);
  ASSERT_EQ(link2.next_link, nullptr);

  list.remove_link(link2);

  ASSERT_EQ(list.head_link, nullptr);
  ASSERT_EQ(list.size, 0);
  ASSERT_EQ(link1.prev_link, nullptr);
  ASSERT_EQ(link1.next_link, nullptr);
  ASSERT_EQ(link2.prev_link, nullptr);
  ASSERT_EQ(link2.next_link, nullptr);
}

TEST(basic_double_linked_list, can_remove_both_link_link2_first) {
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

  ASSERT_EQ(list.head_link, &link2);
  ASSERT_EQ(list.size, 2);
  ASSERT_EQ(link2.prev_link, nullptr);
  ASSERT_EQ(link2.next_link, &link1);
  ASSERT_EQ(link1.prev_link, &link2);
  ASSERT_EQ(link1.next_link, nullptr);

  list.remove_link(link2);

  ASSERT_EQ(list.head_link, &link1);
  ASSERT_EQ(list.size, 1);
  ASSERT_EQ(link1.prev_link, nullptr);
  ASSERT_EQ(link1.next_link, nullptr);
  ASSERT_EQ(link2.prev_link, nullptr);
  ASSERT_EQ(link2.next_link, nullptr);

  list.remove_link(link1);

  ASSERT_EQ(list.head_link, nullptr);
  ASSERT_EQ(list.size, 0);
  ASSERT_EQ(link1.prev_link, nullptr);
  ASSERT_EQ(link1.next_link, nullptr);
  ASSERT_EQ(link2.prev_link, nullptr);
  ASSERT_EQ(link2.next_link, nullptr);
}

namespace {

void rec_expand(const std::vector<int>& inData, std::vector<std::vector<int>>& inoutResults) {
  if (inData.empty())
    return;

  if (inData.size() == 1) {
    std::vector<int> tmpRes;
    tmpRes.push_back(inData[0]);
    inoutResults.push_back(tmpRes);
    return;
  }

  for (std::size_t ii = 0; ii < inData.size(); ++ii) {
    std::vector<int> copiedData = inData;

    copiedData.erase(copiedData.begin() + int(ii));

    std::vector<std::vector<int>> subRes;
    rec_expand(copiedData, subRes);

    for (const auto& currRes : subRes) {
      std::vector<int> tmpRes;

      tmpRes.push_back(inData[ii]);

      for (int currVal : currRes)
        tmpRes.push_back(currVal);

      inoutResults.push_back(tmpRes);
    }
  }
}

} // namespace

TEST(basic_double_linked_list, can_remove_several_links_in_all_removal_combinations) {

  constexpr int k_totalIndices = 6;

  std::vector<int> indices;
  indices.reserve(k_totalIndices);
  for (int ii = 0; ii < k_totalIndices; ++ii)
    indices.push_back(ii);

  std::vector<std::vector<int>> allResults;
  rec_expand(indices, allResults);

  for (std::vector<int>& currResult : allResults) {
    List list;

    // D_MYERR("--------------");

    std::list<Link> allLinksList;
    for (std::size_t ii = 0; ii < indices.size(); ++ii)
      allLinksList.push_back(Link());

    ASSERT_EQ(list.head_link, nullptr);
    ASSERT_EQ(list.size, 0);

    for (Link& currLink : allLinksList) {
      ASSERT_EQ(currLink.prev_link, nullptr);
      ASSERT_EQ(currLink.next_link, nullptr);
      ASSERT_EQ(currLink.user_data, nullptr);
    }

    for (Link& currLink : allLinksList) {
      list.add_link(currLink);
    }

    ASSERT_EQ(list.head_link, &allLinksList.back());
    ASSERT_EQ(list.size, allLinksList.size());

    std::vector<Link*> allLinksArray;
    allLinksArray.reserve(allLinksList.size());
    for (Link& tmpLink : allLinksList)
      allLinksArray.push_back(&tmpLink);

    for (int currIndex : currResult) {

      // {
      //   D_MYERR("===========");
      //   for (const Link* currLink : allLinksArray)
      //   {
      //     // D_MYERR(" --> " << ii);
      //     D_MYERR("    curr " << currLink);

      //     std::size_t prevIndex = 0;
      //     for (; prevIndex < allLinksArray.size(); ++prevIndex)
      //       if (currLink->prev_link == allLinksArray[prevIndex])
      //         break;

      //     std::size_t nextIndex = 0;
      //     for (; nextIndex < allLinksArray.size(); ++nextIndex)
      //       if (currLink->next_link == allLinksArray[nextIndex])
      //         break;

      //     if (currLink->prev_link)
      //     {
      //       D_MYERR("    prev " << currLink->prev_link << " prevIndex " << prevIndex);
      //     }
      //     else
      //     {
      //       D_MYERR("    prev " << currLink->prev_link);
      //     }

      //     if (currLink->next_link)
      //     {
      //       D_MYERR("    prev " << currLink->next_link << " nextIndex " << nextIndex);
      //     }
      //     else
      //     {
      //       D_MYERR("    prev " << currLink->next_link);
      //     }

      //   }
      //   D_MYERR("===========");
      // }

      {
        for (std::size_t ii = 0; ii < allLinksArray.size(); ++ii) {
          if (ii == 0) {
            ASSERT_EQ(allLinksArray[ii]->next_link, nullptr);
          } else {
            ASSERT_EQ(allLinksArray[ii]->next_link, allLinksArray[ii - 1]);
          }

          if (ii + 1 == allLinksArray.size()) {
            ASSERT_EQ(allLinksArray[ii]->prev_link, nullptr);
          } else {
            ASSERT_EQ(allLinksArray[ii]->prev_link, allLinksArray[ii + 1]);
          }
        }
      }

      //
      //
      //

      list.remove_link(*allLinksArray[std::size_t(currIndex)]);

      allLinksArray.erase(allLinksArray.begin() + currIndex);

      for (int& currVal : currResult)
        if (currVal > currIndex)
          currVal -= 1;

      ASSERT_EQ(list.size, allLinksArray.size());

      //
      //
      //

      // {
      //   D_MYERR("===========");
      //   for (const Link* currLink : allLinksArray)
      //   {
      //     // D_MYERR(" --> " << ii);
      //     D_MYERR("    curr " << currLink);

      //     std::size_t prevIndex = 0;
      //     for (; prevIndex < allLinksArray.size(); ++prevIndex)
      //       if (currLink->prev_link == allLinksArray[prevIndex])
      //         break;

      //     std::size_t nextIndex = 0;
      //     for (; nextIndex < allLinksArray.size(); ++nextIndex)
      //       if (currLink->next_link == allLinksArray[nextIndex])
      //         break;

      //     if (currLink->prev_link)
      //     {
      //       D_MYERR("    prev " << currLink->prev_link << " prevIndex " << prevIndex);
      //     }
      //     else
      //     {
      //       D_MYERR("    prev " << currLink->prev_link);
      //     }

      //     if (currLink->next_link)
      //     {
      //       D_MYERR("    prev " << currLink->next_link << " nextIndex " << nextIndex);
      //     }
      //     else
      //     {
      //       D_MYERR("    prev " << currLink->next_link);
      //     }

      //   }
      //   D_MYERR("===========");
      // }

      {
        for (std::size_t ii = 0; ii < allLinksArray.size(); ++ii) {
          if (ii == 0) {
            ASSERT_EQ(allLinksArray[ii]->next_link, nullptr);
          } else {
            ASSERT_EQ(allLinksArray[ii]->next_link, allLinksArray[ii - 1]);
          }

          if (ii + 1 == allLinksArray.size()) {
            ASSERT_EQ(allLinksArray[ii]->prev_link, nullptr);
          } else {
            ASSERT_EQ(allLinksArray[ii]->prev_link, allLinksArray[ii + 1]);
          }
        }
      }
    }

    ASSERT_EQ(list.head_link, nullptr);
    ASSERT_EQ(list.size, 0);

    for (Link& currLink : allLinksList) {
      ASSERT_EQ(currLink.prev_link, nullptr);
      ASSERT_EQ(currLink.next_link, nullptr);
      ASSERT_EQ(currLink.user_data, nullptr);
    }
  }
}
