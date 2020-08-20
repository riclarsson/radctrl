#ifndef wigner_h
#define wigner_h

#include <wigxjpf/inc/wigxjpf.h>

#ifdef FAST_WIGNER_PATH_3J
#define DO_FAST_WIGNER 1
#include <fastwigxj/inc/fastwigxj.h>
#else
#define DO_FAST_WIGNER 0
#endif

#include "rational.h"



/** Ready Wigner
  * 
  * @param[in] largest
  * @param[in] fastest
  * @param[in] size [3 or 6]
  * @return largest if successful
  */
int make_wigner_ready(int largest, int fastest, int size);

/** Wigner 3J symbol
 * 
 * Run wigxjpf wig3jj for Rational symbol
 * 
 * /                \
 * |  j1   j2   j3  |
 * |                |
 * |  m1   m2   m3  |
 * \                /
 * 
 * See for definition: http://dlmf.nist.gov/34.2
 * 
 * @param[in] j1 as above
 * @param[in] j2 as above
 * @param[in] j3 as above
 * @param[in] m1 as above
 * @param[in] m2 as above
 * @param[in] m3 as above
 * @return Symbol value
 */
double wigner3j(const Rational j1,
                const Rational j2,
                const Rational j3,
                const Rational m1,
                const Rational m2,
                const Rational m3);

#endif  // wigner_h
