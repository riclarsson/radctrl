#ifndef grids_h
#define grids_h

#include <algorithm>
#include <functional>
#include <iostream>
#include <numeric>
#include <vector>

template <typename... Inds>
constexpr std::size_t mul(std::size_t val, Inds... inds) {
  if constexpr (sizeof...(Inds) == 0)
    return val;
  else
    return val * mul(inds...);
}

template <class base, std::size_t N>
class Grid {
 private:
  std::array<std::size_t, N> gridsize;
  std::vector<base> d;

 public:
  template <typename... Inds>
  Grid(const base& fillval, Inds... inds)
      : gridsize({std::size_t(inds)...}), d(mul(inds...), fillval) {
    static_assert(sizeof...(Inds) == N,
                  "Must have same size for initialization");
  }

  template <typename... Inds>
  base& operator()(Inds... inds) noexcept {
    return d[index(
        std::array<std::size_t, sizeof...(Inds)>{std::size_t(inds)...})];
  }

  template <typename... Inds>
  const base& operator()(Inds... inds) const noexcept {
    return d[index(
        std::array<std::size_t, sizeof...(Inds)>{std::size_t(inds)...})];
  }

  std::array<std::size_t, N> sizes() const { return gridsize; }

  const std::vector<base>& data() const { return d; }

  std::vector<base>& data() { return d; }

  template <typename... Inds>
  void reset(const base& fillval, Inds... inds) {
    *this = Grid(fillval, inds...);
  }

  template <typename... Inds>
  static Grid initLastFromVector(const std::vector<base>& v, Inds... inds) {
    auto out = Grid(v[0], inds...);

    if (out.gridsize.back() not_eq v.size())
      throw std::runtime_error("Bad size of last vector");

    for (std::size_t i = 0; i < out.d.size(); i += v.size())
      std::copy(v.cbegin(), v.cend(), out.d.begin() + i);

    return out;
  }

  friend std::ostream& operator<<(std::ostream& os, const Grid& g) {
    auto grid = g.gridsize;
    grid.fill(0);
    std::size_t i = 0;
    std::size_t n = grid.size();
    for (;;) {
      os << g.d[i];

      i++;
      grid[n - 1]++;
      if (grid[n - 1] == g.gridsize[n - 1]) {
        for (std::size_t j = grid.size() - 1; j >= 0; j--) {
          if (grid[j] == g.gridsize[j]) {
            if (j == 0) {
              return os;
            } else {
              grid[j] = 0;
              grid[j - 1]++;
            }
          }
        }
        os << '\n';
      } else {
        os << ' ';
      }
    }
  }

 private:
  template <std::size_t Nd>
  std::size_t index(std::array<std::size_t, Nd>&& ind) const noexcept {
    static_assert(Nd <= N, "Bad number of inputs");
    std::size_t posmul{1};
    std::size_t pos{0};
    for (std::size_t i{0}; i < Nd; i++) {
      pos += posmul * ind[i];
      posmul *= gridsize[i];
    }
    return pos;
  }
};  // Grid

#endif  // grids_h
