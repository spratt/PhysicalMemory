#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <random>

#include <cstdlib>

#include "bsd_allocator.hpp"
#include "linux_allocator.hpp"
#include "bitmap_allocator.hpp"

void test_allocator(FrameAllocator& all, size_t NPAGES) {
  const size_t trials = 30;

  std::random_device device;
  std::mt19937 generator(device());
  std::uniform_int_distribution<size_t> dist(0, NPAGES);

  std::vector<Page*> allocs;
  
  double total_alloc = 0.0, total_free = 0.0;
  for(size_t trial = 0; trial < trials; ++trial) {
    // Init
    all.init();

    for(size_t i = 0; i < NPAGES; ++i) {
      size_t r = dist(generator);

      // Free
      if(r < allocs.size()) {
        auto start = std::chrono::high_resolution_clock::now();
        all.free(*allocs.back());
        allocs.pop_back();
        auto end = std::chrono::high_resolution_clock::now();
        auto diff = end - start;
        total_free += diff.count();
      } else { // alloc
        auto start = std::chrono::high_resolution_clock::now();
        allocs.push_back(&all.alloc());      
        auto end = std::chrono::high_resolution_clock::now();
        auto diff = end - start;
        total_alloc += diff.count();
      }
    }

    // Free all frames
    while(allocs.size() > 0) {
      all.free(*allocs.back());
      allocs.pop_back();      
    }
  }
  double total_trials = NPAGES * trials;
  std::cout << std::setprecision(2) << std::fixed
            << "," << (total_alloc / total_trials)
            << "," << (total_free / total_trials) << std::flush;
}

int main() {
  std::cout << "npages,bitmap-allocate,bitmap-free,bsd-allocate,bsd-free,linux-allocate,linux-free" << std::endl;
  for(int p = 0; p <= 22; ++p) {
    size_t npages = pow(2, p);
    std::cout << npages << std::flush;
    {
      BitmapAllocator bitmapall(npages);
      test_allocator(bitmapall, npages);
    }
    {
      BSDAllocator bsdall(npages);
      test_allocator(bsdall, npages);
    }
    {
      LinuxAllocator linuxall(npages);
      test_allocator(linuxall, npages);
    }
    std::cout << std::endl;
  }
  return 0;
}
