#ifndef BITMAP_ALLOCATOR_HPP
#define BITMAP_ALLOCATOR_HPP

#include <vector>
#include "frame_allocator.hpp"

class BitmapAllocator : public FrameAllocator {
protected:
  std::vector<Page> pages;
  std::vector<bool> bitmap;
  size_t hint;
  
public:
  BitmapAllocator(size_t npages) : hint(0),
                                   pages(npages),
                                   bitmap(npages)
  {}
  
  virtual void init() {
    // Initialize pages
    for(size_t i = 0; i < pages.size(); ++i) {
      pages[i].padd = i;
    }

    // Initialize bitmap
    for(size_t i = 0; i < bitmap.size(); ++i) {
      bitmap[i] = 0;
    }
  }

  virtual Page& alloc() {
    size_t N = pages.size();
    for(size_t i = 0; i < N; ++i) {
      size_t index = (i + hint) % N;
      if(bitmap[index] == 1) continue;
      bitmap[index] = 1;
      hint = (index + 1) % N;
      return pages[index];
    }
    throw "OOM";
  }

  virtual void free(Page& p) {
    bitmap[p.padd] = 0;
  }
};

#endif
