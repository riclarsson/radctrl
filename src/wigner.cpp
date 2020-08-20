#include <algorithm>

#include "wigner.h"

#if DO_FAST_WIGNER
#define WIGNER3 fw3jja6
#define WIGNER6 fw6jja
#else
#define WIGNER3 wig3jj
#define WIGNER6 wig6jj
#endif

int make_wigner_ready(int largest, 
     [[maybe_unused]] int fastest,
                      int size)
{
  if (size == 3) {
    #if DO_FAST_WIGNER
    fastwigxj_load(FAST_WIGNER_PATH_3J, 3, NULL);
    #ifdef _OPENMP
    fastwigxj_thread_dyn_init(3, fastest);
    #else
    fastwigxj_dyn_init(3, fastest);
    #endif
    #endif
    wig_table_init(largest, 3);
    
    return largest;
  } else if (size == 6) {
    #if DO_FAST_WIGNER
    fastwigxj_load(FAST_WIGNER_PATH_3J, 3, NULL);
    fastwigxj_load(FAST_WIGNER_PATH_6J, 6, NULL);
    #ifdef _OPENMP
    fastwigxj_thread_dyn_init(3, fastest);
    fastwigxj_thread_dyn_init(6, fastest);
    #else
    fastwigxj_dyn_init(3, fastest);
    fastwigxj_dyn_init(6, fastest);
    #endif
    #endif
    wig_table_init(largest * 2, 6);
    
    return largest;
  } else {
    return 0;
  }
}

double wigner3j(const Rational j1,
                const Rational j2,
                const Rational j3,
                const Rational m1,
                const Rational m2,
                const Rational m3) {
  const int a = int(2 * j1), b = int(2 * j2), c = int(2 * j3),
            d = int(2 * m1), e = int(2 * m2), f = int(2 * m3);
  double g;
  const int j = std::max({std::abs(a),
                          std::abs(b),
                          std::abs(c),
                          std::abs(d),
                          std::abs(e),
                          std::abs(f)}) *
                          3 / 2 + 1;

  wig_temp_init(j);
  g = WIGNER3(a, b, c, d, e, f);
  wig_temp_free();

  return g;
}
