
#pragma once

#include <functional>
#include <stdint.h>

namespace custom_containers {

struct basic_double_linked_list {
  struct link {
    link* prev_link = nullptr;
    link* next_link = nullptr;
    void* user_data = nullptr;

    void reset() {
      this->prev_link = nullptr;
      this->next_link = nullptr;
      this->user_data = nullptr;
    }

  };

  link* head_link = nullptr;
  uint32_t size = 0;

  void add_link(link& new_link) {
    // add as head of list

    // FROM: (headLink) -> (next/null-link)
    // TO:   (headLink) -> (NEW_LINK) <--> (next/null-link)

    new_link.prev_link = nullptr;
    new_link.next_link = this->head_link;
    if (this->head_link) {
      this->head_link->prev_link = &new_link;
    }
    this->head_link = &new_link;

    this->size += 1;
  }

  void remove_link(link& old_link) {
    // remove from list

    if (this->is_empty()) {
      return;
    }

    // FROM: (prev-link) <--> (OLD_LINK) <--> (next/null-link)
    // TO:   (prev-link) <--> (next/null-link)

    if (old_link.prev_link) {
      old_link.prev_link->next_link = old_link.next_link;
    }
    if (old_link.next_link) {
      old_link.next_link->prev_link = old_link.prev_link;
    }

    // FROM: (headLink) -> (OLD_LINK) -> (next/null-link)
    // TO:   (headLink) -> (next/null-link)
    if (this->head_link == &old_link) {
      this->head_link = old_link.next_link;
    }

    old_link.reset();

    this->size -= 1;
  }

  void swap_two_links_from_same_list(link& link_a, link& link_b) {
    // swap in list

    if (this->is_empty())
      return;

    // FROM: ... <--> (link_a) <--> ... <--> (link_b) <--> ...
    // TO:   ... <--> (link_b) <--> ... <--> (link_a) <--> ...

    std::swap(link_a.prev_link, link_b.prev_link);
    if (link_a.prev_link) {
      link_a.prev_link->next_link = &link_a;
    }
    if (link_b.prev_link) {
      link_b.prev_link->next_link = &link_b;
    }

    std::swap(link_a.next_link, link_b.next_link);
    if (link_a.next_link) {
      link_a.next_link->prev_link = &link_a;
    }
    if (link_b.next_link) {
      link_b.next_link->prev_link = &link_b;
    }

    // FROM: (headLink) -> (link_a/b) -> ...
    // TO:   (headLink) -> (link_b/a) -> ...
    if (this->head_link == &link_a) {
      this->head_link = &link_b;
    }
    else if (this->head_link == &link_b) {
      this->head_link = &link_a;
    }
  }

  bool is_empty() {
    return (this->head_link == nullptr || this->size == 0);
  }

  void reset() {
    if (this->is_empty()) {
      return;
    }
    link* curr_link = this->head_link;
    while (curr_link) {
      link* to_reset_link = curr_link;
      curr_link = curr_link->next_link;
      to_reset_link->reset();
    }
    this->head_link = nullptr;
    this->size = 0;
  }

  template <typename T>
  void loop(const std::function<void(T*)>& callback) {

    if (this->is_empty()) {
      return;
    }

    link* curr_link = this->head_link;
    while (curr_link) {
      callback(static_cast<T*>(curr_link->user_data));
      curr_link = curr_link->next_link;
    }
  }

  template <typename T>
  void loop_and_reset(const std::function<void(T*)>& callback) {

    if (this->is_empty()) {
      this->head_link = nullptr;
      this->size = 0;
      return;
    }

    link* curr_link = this->head_link;
    while (curr_link) {
      link* to_reset_link = curr_link;
      curr_link = curr_link->next_link;

      callback(static_cast<T*>(to_reset_link->user_data));
      to_reset_link->reset();
    }
    this->head_link = nullptr;
    this->size = 0;
  }
};

} // namespace gero