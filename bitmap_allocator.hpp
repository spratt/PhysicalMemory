#ifndef BITMAP_ALLOCATOR_HPP
#define BITMAP_ALLOCATOR_HPP

#include <bitset>
#include <array>
#include "frame_allocator.hpp"

template <size_t N>
class BitmapAllocator : public FrameAllocator {
protected:
  std::array<Page, N> pages;
  std::bitset<N> bitmap;
  size_t hint;
  
public:
  BitmapAllocator() : hint(0) {}
  
  virtual void init() {
    // Initialize pages
    for(size_t i = 0; i < N; ++i) {
      pages[i].padd = i;
    }

    // Initialize bitmap
    bitmap.reset();
  }

  virtual Page& alloc() {
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
