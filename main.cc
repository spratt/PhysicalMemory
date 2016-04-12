#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <cstdlib>
#include "bsd_allocator.hpp"
#include "linux_allocator.hpp"
#include "bitmap_allocator.hpp"

void test_allocator(FrameAllocator& all, size_t NPAGES) {
  const size_t trials = 30;

  std::vector<Page*> allocs;
  
  double total_alloc = 0.0, total_free = 0.0;
  for(size_t trial = 0; trial < trials; ++trial) {
    // Init
    all.init();

    // alloc
    auto start = std::chrono::high_resolution_clock::now();
    for(size_t i = 0; i < NPAGES; ++i) {
      try {
        allocs.push_back(&all.alloc());
      } catch(std::string s) {
        std::cout << s << std::endl;
        exit(-1);
      }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto diff = end - start;
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
  std::cout << std::setprecision(2) << std::fixed
            << "," << (total_alloc / total_trials)
            << "," << (total_free / total_trials) << std::flush;
}

/******************************************************************************/
// x^n
// By Alexandre Daigle
template <std::size_t X, std::size_t N>
struct pow_t {
    static constexpr std::size_t value = X * pow_t<X, N-1>::value;
};

template<std::size_t X>
struct pow_t<X, 0> {
    static constexpr std::size_t value = 1;
};
/******************************************************************************/

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
