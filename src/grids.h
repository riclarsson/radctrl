#ifndef grids_h
#define grids_h

#include <vector>

#include "enums.h"
#include "units.h"

template<class base, std::size_t N>
class Grid {
private:
  std::array<long, N> gridsize;
  std::vector<base> data;
public:
  template <typename... Inds> base& operator()(Inds ... inds) noexcept {return data[index(std::array<std::size_t, sizeof...(Inds)>{std::size_t(inds)...})];}
  template <typename... Inds> const base& operator()(Inds ... inds) const noexcept {return data[index(std::array<std::size_t, sizeof...(Inds)>{std::size_t(inds)...})];}
  
private:
  template <std::size_t Nd> std::size_t index(std::array<std::size_t, Nd>&& ind) const noexcept {
    static_assert(Nd <= N, "Bad number of inputs");
    std::size_t posmul{1};
    std::size_t pos{0};
    for (std::size_t i{0}; i<Nd; i++) {
      pos += posmul * ind[i];
      posmul *= gridsize[i];
    }
    return pos;
  }
};  // Grid

#endif  // grids_h
