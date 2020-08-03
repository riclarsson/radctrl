#ifndef vectors_h
#define vectors_h

#include <algorithm>
#include <functional>
#include <vector>

namespace Vector {
template<class T, int order=1>
class UniqueSorted {
  std::vector<T> data;
public:
  UniqueSorted (const std::vector<T>& in) : data(in) {sortself();}

  const T& operator[](decltype(data.size()) i) const noexcept {return data[i];}  // Cannot have the non-const version!
  decltype(data.size()) size() const noexcept {return data.size();}
  decltype(data.cbegin()) cbegin() const noexcept {return data.cbegin();}
  decltype(data.cend()) cend() const noexcept {return data.cend();}
private:
  void sortself() noexcept {
    data.erase(std::unique(data.begin(), data.end()), data.end());
    static_assert(order == 1 or order == -1, "+1 for ascending, -1 for descending, nothing else permitted");
    if (order == 1)
      std::sort(data.begin(), data.end());
    else
      std::sort(data.begin(), data.end(), std::greater<T>());
  }
};  // UniqueSorted

template<class T, int order=1>
class CyclicDegreesUniqueSorted {
  std::vector<T> data;
public:
  CyclicDegreesUniqueSorted (const std::vector<T>& in) : data(in) {sortself();}
  
  const T& operator[](decltype(data.size()) i) const noexcept {return data[i];}  // Cannot have the non-const version!
  decltype(data.size()) size() const noexcept {return data.size();}
  decltype(data.cbegin()) cbegin() const noexcept {return data.cbegin();}
  decltype(data.cend()) cend() const noexcept {return data.cend();}
private:
  void sortself() noexcept {
    T min{0};
    T max{360};
    for (T& x: data) {
      while (x < min) x += max;
      while (x >= max) x -= max;  // Also remove max
    }
    
    data.erase(std::unique(data.begin(), data.end()), data.end());
    static_assert(order == 1 or order == -1, "+1 for ascending, -1 for descending, nothing else permitted");
    if (order == 1)
      std::sort(data.begin(), data.end());
    else
      std::sort(data.begin(), data.end(), std::greater<T>());
  }
};  // CyclicUniqueSorted
}  // Vectors

#endif  // vectors_h
