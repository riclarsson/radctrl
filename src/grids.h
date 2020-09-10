#ifndef grids_h
#define grids_h

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <numeric>
#include <vector>

template <typename... Inds>
constexpr std::size_t mul(Inds... inds) {
  return (std::size_t(inds) * ...);
}

/** Row-major grid creation */
template <typename b, std::size_t n>
class Grid {
  std::unique_ptr<b[]> ptr;
  std::array<std::size_t, n> gridsize;

  std::size_t nelem() const {
    return std::reduce(gridsize.cbegin(), gridsize.cend(), 1,
                       std::multiplies<std::size_t>());
  }

 public:
  static constexpr std::size_t N = n;
  using base = b;
  static_assert(N, "Must have size");

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

  base &operator()(std::array<std::size_t, N> inds) noexcept {
    return ptr[index(inds)];
  }

  const base &operator()(std::array<std::size_t, N> inds) const noexcept {
    return ptr[index(inds)];
  }

  template <typename... Inds>
  base &operator()(Inds... inds) noexcept {
    return ptr[index(std::array<std::size_t, N>{std::size_t(inds)...})];
  }

  template <typename... Inds>
  const base &operator()(Inds... inds) const noexcept {
    return ptr[index(std::array<std::size_t, N>{std::size_t(inds)...})];
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

  auto cbegin() const { return data(); }
  auto cend() const { return data() + nelem(); }

  friend std::ostream &operator<<(std::ostream &os, const Grid &g) {
    std::size_t i = 0;
    const std::size_t nel = g.nelem();
    while (i < nel) {
      os << g.ptr[i];
      i++;
      if (i not_eq 0 and i not_eq nel and i % g.gridsize.back() == 0)
        os << '\n';
      else if (i not_eq nel)
        os << ' ';
    }

    return os;
  }

 private:
  std::size_t index(std::array<std::size_t, N> ind) const noexcept {
    std::size_t posmul{gridsize.back()};
    std::size_t pos{ind.back()};
    for (std::size_t i{N - 2}; i < N; i--) {
      pos += posmul * ind[i];
      posmul *= gridsize[i];
      [[unlikely]] if (ind[i] >= gridsize[i]) {
        std::cerr << "Out of range\n";
        std::terminate();
      }
    }
    return pos;
  }
};  // Grid

template <class GridClass>
class Sub {
  const std::size_t axis;
  const std::size_t axispos;
  GridClass &grid;

 public:
  static constexpr std::size_t N = GridClass::N - 1;
  static_assert(N, "Must (still?) have size");

  Sub(GridClass &g, std::size_t a, std::size_t p) noexcept
      : axis(a), axispos(p), grid(g) {}

  template <typename... Inds>
  typename GridClass::base &operator()(Inds... inds) noexcept {
    return grid(index(std::array<std::size_t, N>{std::size_t(inds)...}));
  }

  template <typename... Inds>
  const typename GridClass::base &operator()(Inds... inds) const noexcept {
    return grid(index(std::array<std::size_t, N>{std::size_t(inds)...}));
  }

  typename GridClass::base &operator[](std::size_t ind) noexcept {
    static_assert(N == 1);
    if (axis == 0)
      return grid(axispos, ind);
    else
      return grid(ind, axispos);
  }

  const typename GridClass::base &operator[](std::size_t ind) const noexcept {
    static_assert(N == 1);
    if (axis == 0)
      return grid(axispos, ind);
    else
      return grid(ind, axispos);
  }

  std::array<std::size_t, N> sizes() const {
    const std::array<std::size_t, N + 1> out_orig = grid.sizes();
    std::array<std::size_t, N> out;
    for (std::size_t i = 0; i < N; i++) {
      out[i] =
          out_orig[i + (i >= axis)];  // at i before axis, at i+1 after axis
    }
    return out;
  }

  friend std::ostream &operator<<(std::ostream &os, const Sub &s) {
    const auto size = s.sizes();
    if constexpr (Sub::N == 1) {
      for (std::size_t pos = 0; pos < size[0]; pos++) {
        os << s[pos];
        if (pos not_eq size[0] - 1) {
          os << ' ';
        }
      }
    } else {
      for (std::size_t dim = 0; dim < Sub::N; dim++) {
        for (std::size_t pos = 0; pos < size[dim]; pos++) {
          os << Sub(s, dim, pos);
          if (pos not_eq size[dim] - 1) {
            os << '\n';
          }
        }
      }
    }
    return os;
  }

 private:
  std::array<std::size_t, N + 1> index(
      std::array<std::size_t, N> inds) const noexcept {
    std::array<std::size_t, N + 1> out;
    for (std::size_t i = 0; i < N; i++) {
      out[i + (i >= axis)] = inds[i];  // at i before axis, at i+1 after axis
    }
    out[axis] = axispos;
    return out;
  }
};

#endif  // grids_h
