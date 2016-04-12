#ifndef BSD_ALLOCATOR_HPP
#define BSD_ALLOCATOR_HPP

#include <vector>
#include "frame_allocator.hpp"

class BSDPage : public Page {
public:
  BSDPage* next;
  BSDPage* prev;

  BSDPage() : next(NULL), prev(NULL) {}
};

class BSDAllocator : public FrameAllocator {
protected:
  std::vector<BSDPage> pages;
  BSDPage* head;
  
public:
  BSDAllocator(size_t n) : pages(n) {}
  
  virtual void init() {
    size_t npages = pages.size();
    head = &pages[0];
    for(size_t i = 0; i < npages; ++i) {
      size_t next_index = i + 1;
      size_t prev_index = i - 1;
      
      pages[i].padd = i;
      pages[i].next = (next_index == npages) ? NULL : &pages[next_index];
      pages[i].prev = (prev_index == -1) ? NULL : &pages[prev_index];
    }
  }

  virtual Page& alloc() {
    if(head == NULL) throw "Cannot allocate from empty list";
    BSDPage& ret = *head;
    if(head->next != NULL) head->next->prev = head->prev;
    if(head->prev != NULL) head->prev->next = head->next;
    head = head->next;
    ret.next = NULL;
    ret.prev = NULL;
    return ret;
  }

  virtual void free(Page& p) {
    BSDPage& bp = (BSDPage&)p;
    bp.next = head;
    if(head != NULL) bp.prev = head->prev;
    head = &bp;
  }
};

#endif
