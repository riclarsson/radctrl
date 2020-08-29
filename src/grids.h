#ifndef grids_h
#define grids_h

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <numeric>
#include <vector>

template <typename... Inds>
constexpr std::size_t mul(std::size_t val, Inds... inds) {
  if constexpr (sizeof...(Inds) == 0)
    return val;
  else
    return val * mul(inds...);
}

/** Row-major grid creation */
template <typename base, std::size_t N>
class Grid {
  std::unique_ptr<base[]> ptr;
  std::array<std::size_t, N> gridsize;

  std::size_t nelem() const {
    return std::reduce(gridsize.cbegin(), gridsize.cend(), 1,
                       std::multiplies<std::size_t>());
  }

 public:
  static constexpr std::size_t dim = N;

  template <typename... Inds>
  Grid(const base &fillval, Inds... inds)
      : ptr(std::make_unique<base[]>(mul(inds...))),
        gridsize({std::size_t(inds)...}) {
    static_assert(sizeof...(Inds) == N,
                  "Must have same size for initialization");
    for (auto &x : *this) x = fillval;
  }

  Grid(const Grid &g) noexcept
      : ptr(std::make_unique<base[]>(
            mul(std::reduce(g.gridsize.cbegin(), g.gridsize.cend(), 1,
                            std::multiplies<std::size_t>())))),
        gridsize(g.gridsize) {
    const std::size_t end = nelem();
    for (size_t i = 0; i < end; i++) ptr[i] = g.ptr[i];
  }

  Grid(Grid &&g) noexcept : ptr(g.ptr.release()), gridsize(g.gridsize) {
    g.ptr = nullptr;
  }

  template <typename... Inds>
  base &operator()(Inds... inds) noexcept {
    return ptr[index(
        std::array<std::size_t, sizeof...(Inds)>{std::size_t(inds)...})];
  }

  template <typename... Inds>
  const base &operator()(Inds... inds) const noexcept {
    return ptr[index(
        std::array<std::size_t, sizeof...(Inds)>{std::size_t(inds)...})];
  }

  std::array<std::size_t, N> sizes() const { return gridsize; }
  std::size_t size(std::size_t pos) const { return gridsize[pos]; }

  const base *data() const { return ptr.get(); }

  base *data() { return ptr.get(); }

  template <typename... Inds>
  void resize(Inds... inds) {
    ptr = std::make_unique<base[]>(mul(inds...));
    gridsize = {std::size_t(inds)...};
  }

  auto begin() { return data(); }
  auto end() { return data() + nelem(); }

  auto begin() const { return data(); }
  auto end() const { return data() + nelem(); }

  const auto cbegin() const { return data(); }
  auto cend() const { return data() + nelem(); }

  friend std::ostream &operator<<(std::ostream &os, const Grid &g) {
    std::size_t i = 0;
    const std::size_t n = g.nelem();
    while (i < n) {
      os << g.ptr[i];
      i++;
      if (i not_eq 0 and i not_eq n and i % g.gridsize.back() == 0)
        os << '\n';
      else if (i not_eq n)
        os << ' ';
    }

    return os;
  }

 private:
  std::size_t index(std::array<std::size_t, N> ind) const noexcept {
    [[unlikely]] if (std::any_of(ind.cbegin(), ind.cend(),
                                 [](auto &a) { return a < 0; }) or
                     gridsize.back() <= ind.back()) {
      std::cerr << "Out of range\n";
      std::exit(1);
    }

    std::size_t posmul{gridsize.back()};
    std::size_t pos{ind.back()};
    for (std::size_t i{N - 2}; i < N; i--) {
      pos += posmul * ind[i];
      posmul *= gridsize[i];
      [[unlikely]] if (ind[i] >= gridsize[i]) {
        std::cerr << "Out of range\n";
        std::exit(1);
      }
    }
    //     std::cout << pos << '\n';
    return pos;
  }
};  // Grid

#endif  // grids_h
