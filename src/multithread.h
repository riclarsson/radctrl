#ifndef multithread_h
#define multithread_h

#include <future>

template <class Function, class ... Args>
class AsyncThread {
  std::future<std::invoke_result_t<std::decay_t<Function>, std::decay_t<Args>...>> r;
public:
  AsyncThread(Function&& f, Args&&... args) : r(std::async(std::launch::async, f, args...)) {}
  decltype(r.get()) results() {return r.get();}
};

#endif  // multithread_h
