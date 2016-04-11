#ifndef FRAME_ALLOCATOR_HPP
#define FRAME_ALLOCATOR_HPP

class Page;

class Page {
public:
  uint64_t padd;
  Page* next;
  Page* prev;
};

// Define a standard interface
class FrameAllocator {
public:
  // allocate any structures used by the allocator
  virtual void init() = 0;

  // return an unused physical address, and
  // modify Allocation to include the new mapping
  virtual Page& alloc() = 0;

  // free 
  virtual void free(Page&) = 0;
};

#endif
