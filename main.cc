#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include "bsd_allocator.hpp"

const size_t KB = 1024;
const size_t MB = KB * KB;
const size_t GB = MB * KB;
const size_t NPAGES = (16 * GB) / (4 * KB);

void test_allocator(const char* name, FrameAllocator& all) {
  const size_t trials = 30;

  std::vector<Page*> allocs;
  
  double total_init = 0.0, total_alloc = 0.0, total_free = 0.0;
  for(size_t trial = 0; trial < trials; ++trial) {
    // Init
    auto start = std::chrono::high_resolution_clock::now();
    all.init();
    auto end = std::chrono::high_resolution_clock::now();
    auto diff = end - start;
    total_init += diff.count();

    // alloc
    start = std::chrono::high_resolution_clock::now();
    for(size_t i = 0; i < NPAGES; ++i) {
      allocs.push_back(&all.alloc());
    }
    end = std::chrono::high_resolution_clock::now();
    diff = end - start;
    total_alloc += diff.count();

    // Free
    start = std::chrono::high_resolution_clock::now();
    while(allocs.size() > 0) {
      all.free(*allocs.back());
      allocs.pop_back();
    }
    end = std::chrono::high_resolution_clock::now();
    diff = end - start;
    total_free += diff.count();
  }
  double total_trials = NPAGES * trials;
  std::cout << name << std::setprecision(2) << std::fixed
            << "," << (total_init / trials)
            << "," << (total_alloc / total_trials)
            << "," << (total_free / total_trials) << std::endl;
}

int main() {
  std::cout << "allocator,init,allocate,free" << std::endl;
  {
    BSDAllocator bsdall(NPAGES);
    test_allocator("bsd", bsdall);
  }
  return 0;
}
