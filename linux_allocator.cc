#include <iostream>
#include <cmath>
#include "linux_allocator.hpp"

int main() {
  std::vector<Page*> allocs;
  
  for(int p = 0; p < 22; ++p) {
    size_t nframes = pow(2, p);
    std::cout << "p = " << p
              << ", nframes = " << nframes << std::endl;
    
    LinuxAllocator linuxall(nframes);
    if(p <= 4) linuxall.set_debug(true);
    
    std::cout << "\tInitializing..." << std::flush;
    linuxall.init();
    std::cout << "done." << std::endl;

    std::cout << "\tAllocating..." << std::flush;
    for(int i = 0; i < nframes; ++i)
      allocs.push_back(&linuxall.alloc());
    std::cout << "done." << std::endl;

    std::cout << "\tFreeing..." << std::flush;
    while(allocs.size() > 0) {
      linuxall.free(*allocs.back());
      allocs.pop_back();
    }
    std::cout << "done." << std::endl;
  }
}
