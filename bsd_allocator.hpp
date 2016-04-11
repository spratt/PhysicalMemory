#ifndef BSD_ALLOCATOR_HPP
#define BSD_ALLOCATOR_HPP

#include <vector>
#include "frame_allocator.hpp"



class BSDAllocator : public FrameAllocator {
protected:
  std::vector<Page> pages;
  Page* head;
  
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
    Page& ret = *head;
    if(head->next != NULL) head->next->prev = head->prev;
    if(head->prev != NULL) head->prev->next = head->next;
    head = head->next;
    ret.next = NULL;
    ret.prev = NULL;
    return ret;
  }

  virtual void free(Page& p) {
    p.next = head;
    if(head != NULL) p.prev = head->prev;
    head = &p;
  }
};

#endif
