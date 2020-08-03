#include "multithread.h"
#include "timeclass.h"

std::mutex mtx;  // Global mutex to lock-guard important things

int test001(double time, int n, char x) {
  
  for (int i=0; i<n; i++) {
    std::this_thread::sleep_for(TimeStep(time));
    mtx.lock();
    std::cout<<x<<i<<'\n';
    mtx.unlock();
  }
  return n;
}

int main () {
  auto x1 = AsyncThread(test001, 0.00005, 120, 'x');
  auto x2 = AsyncThread(test001, 0.0001, 60, 'a');
  auto x3 = AsyncThread(test001, 0.0002, 30, 'b');
  auto x4 = AsyncThread(test001, 0.0004, 15, 'c');
  auto x5 = AsyncThread(test001, 0.0008, 8, 'd');
}
