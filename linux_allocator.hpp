#ifndef LINUX_ALLOCATOR_HPP
#define LINUX_ALLOCATOR_HPP

#include <vector>
#include <cmath>
#include <cassert>
#include "frame_allocator.hpp"

class LinuxPage : public Page {
public:
  size_t order;
};

class LinuxAllocator : public FrameAllocator {
protected:
  std::vector<LinuxPage> pages;
  std::vector<LinuxPage*> order;
  size_t max_order;

/* This buddy code is taken from the linux kernel.
 * 
 * Locate the struct page for both the matching buddy in our
 * pair (buddy1) and the combined O(n+1) page they form (page).
 *
 * 1) Any buddy B1 will have an order O twin B2 which satisfies
 * the following equation:
 *     B2 = B1 ^ (1 << O)
 * For example, if the starting buddy (buddy2) is #8 its order
 * 1 buddy is #10:
 *     B2 = 8 ^ (1 << 1) = 8 ^ 2 = 10
 *
 * 2) Any buddy B will have an order O+1 parent P which
 * satisfies the following equation:
 *     P = B & ~(1 << O)
 */  
  size_t get_buddy(uint64_t b, size_t order) { return b ^ (1 << order); }
  bool is_allocated(LinuxPage& p) { return (p.next == &p && p.prev == &p); };
  bool is_free(LinuxPage& p) { return !is_allocated(p); }
  void set_allocated(LinuxPage* p) {
    p->next = p;
    p->prev = p;
  }

public:
  LinuxAllocator(size_t n) : max_order(ceil(log2(n))),
                             pages(pow(2, ceil(log2(n)))),
                             order(ceil(log2(n)) + 1)
  {}

  virtual void init() {
    // initialize pages
    for(size_t i = 0; i < pages.size(); ++i) {
      pages[i].padd = i;
      pages[i].next = NULL;
      pages[i].prev = NULL;
      pages[i].order = -1;
    }

    // initialize orders
    for(size_t i = 0; i <= max_order; ++i) {
      order[i] = NULL;
    }

    // initialize first block
    pages[0].order = max_order;
    order[max_order] = &pages[0];
  }

  Page& allocate(size_t ord) {
    for(size_t curr = ord; curr <= max_order; ++curr) {
      // if no pages of curr order, continue to next order
      if(order[curr] == NULL) continue;

      // remove first from list
      LinuxPage* first = order[curr];
      if(first->next == NULL)
        order[curr] = NULL;
      else {
        order[curr] = (LinuxPage*)first->next;
        order[curr]->prev = NULL;
      }

      // split until we have the correct order
      while(first->order > ord) {
        --(first->order);
        LinuxPage* buddy = &pages[get_buddy(first->padd, first->order)];
        buddy->order = first->order;
        // we know this order must be empty, or we would have started there
        buddy->next = NULL;
        buddy->prev = NULL;
        order[first->order] = buddy;
      }

      set_allocated(first);
      return *first;
    }
    throw "OOM";
  }
  virtual Page& alloc() {
    return allocate(0);
  }
  virtual void free(Page& p) {
    LinuxPage* ptr = &((LinuxPage&)p);
    
    // merge as much as possible
    while(ptr->order < max_order) {
      size_t buddy_index = get_buddy(ptr->padd, ptr->order);
      LinuxPage& buddy = pages[buddy_index];
      if(is_allocated(buddy)) break;
      
      // remove page from its list
      if(buddy.next != NULL)
        buddy.next->prev = buddy.prev;
      if(buddy.prev != NULL)
        buddy.prev->next = buddy.next;
      else {
        assert(order[buddy.order] == &buddy);
        order[buddy.order] = (LinuxPage*)buddy.next;
      }
      
      // merge
      if(buddy_index < p.padd)
        ptr = &buddy;
      ++(ptr->order);
    }
    
    // add to current order
    ptr->prev = NULL;
    ptr->next = order[ptr->order];
    order[ptr->order] = ptr;
    if(ptr->next != NULL) {
      ptr->next->prev = ptr;
    }
  }
};

#endif
