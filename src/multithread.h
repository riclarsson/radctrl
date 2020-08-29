#ifndef multithread_h
#define multithread_h

#include <future>

template <class Function, class... Args>
auto AsyncConstRef(Function &&f, const Args &... args) {
  return std::async(std::launch::async, f, std::cref(args)...);
}

template <class Function, class... Args>
auto AsyncRef(Function &&f, Args &... args) {
  return std::async(std::launch::async, f, std::ref(args)...);
}

template <class Function, class... Args>
auto Async(Function &&f, Args... args) {
  return std::async(std::launch::async, f, args...);
}

#endif  // multithread_h
