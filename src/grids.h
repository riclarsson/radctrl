#ifndef grids_h
#define grids_h

#include <functional>
#include <numeric>
#include <vector>

template<class base, std::size_t N>
class Grid {
private:
  std::array<std::size_t, N> gridsize;
  std::vector<base> d;
public:
  Grid(const base& fillval, std::array<std::size_t, N> size) : gridsize(size),
  d(std::reduce(gridsize.cbegin(), gridsize.cend(), 1, std::multiplies<std::size_t>{}), fillval) {}
  
  template <typename... Inds> base& operator()(Inds ... inds) noexcept {return d[index(std::array<std::size_t, sizeof...(Inds)>{std::size_t(inds)...})];}
  template <typename... Inds> const base& operator()(Inds ... inds) const noexcept {return d[index(std::array<std::size_t, sizeof...(Inds)>{std::size_t(inds)...})];}
  
  std::array<std::size_t, N> sizes() const {return gridsize;}
  const std::vector<base>& data() const {return d;}
  
  void reset(std::size_t I, std::size_t J, std::size_t K, std::size_t M, const base& fillval) {
    gridsize = {I, J, K, M};
    d = std::vector<base>(std::reduce(gridsize.cbegin(), gridsize.cend(), 1, std::multiplies<std::size_t>{}), fillval);
  }
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
