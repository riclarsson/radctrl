/* Copyright (C) 2012
 * Richard Larsson <ric.larsson@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA. */

/**
 * @file wigner_functions.h
 * @author Richard Larsson
 * @date 2013-06-19
 * 
 * @brief Wigner symbol interactions
 */

#include <wigner/wigxjpf/inc/wigxjpf.h>
#include "rational.h"

#ifdef FAST_WIGNER_PATH_3J
#define DO_FAST_WIGNER 1
#include <wigner/fastwigxj/inc/fastwigxj.h>
#else
#define DO_FAST_WIGNER 0
#endif

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
  * @return Numeric Symbol value
  */
Numeric wigner3j(const Rational j1,
                 const Rational j2,
                 const Rational j3,
                 const Rational m1,
                 const Rational m2,
                 const Rational m3);

/** Wigner 6J symbol
  * 
  * Run wigxjpf wig6jj for Rational symbol
  *
  * /                \
  * |  j1   j2   j3  |
  * <                >
  * |  l1   l2   l3  |
  * \                /
  *
  * See for definition: http://dlmf.nist.gov/34.4
  * 
  * @param[in] j1 as above
  * @param[in] j2 as above
  * @param[in] j3 as above
  * @param[in] l1 as above
  * @param[in] l2 as above
  * @param[in] l3 as above
  * @return Numeric Symbol value
  */
Numeric wigner6j(const Rational j1,
                 const Rational j2,
                 const Rational j3,
                 const Rational l1,
                 const Rational l2,
                 const Rational l3);

/** Returns the wigner symbol used in Niro etal 2004
 *
 * Symbol:
 * /              \  /              \  /               \
 * | Ji_p  L   Ji |  |  Jf_p  L  Jf |  | Ji    Jf    1 |
 * |              |  |              |  <               >  (2L + 1)
 * | li    0  -li |  | -lf    0  lf |  | Jf_p  Ji_p  L |
 * \              /  \              /  \               /
 *
 * Note: The wigner library takes two times the physical values
 *       so, e.g., the 1 must be 2.  This hold true for all user inputs as well!
 *
 * Reference: 
 * Spectra calculations in central and wing regions of CO2 IR bands between 10 and 20 mcrons.
 * I: model and laboratory measurements. F. Niro, C. Boulet, J.-M. Hartmann. 
 * JQSRT 88 (2004) 483 – 498. Equation 4 page 488.
 *
 * Note: Ignore typos, the above is tested in relmat
 *
 * Warning:  Must have called wig_temp_init(j) with appropriate j before 
 *           using this function.  Failure to do so will cause segfault.
 *
 * @param[in] Ji as above times 2
 * @param[in] Jf as above times 2
 * @param[in] Ji_p as above times 2
 * @param[in] Jf_p as above times 2
 * @param[in] L as above times 2
 * @param[in] li as above times 2
 * @param[in] lf as above times 2
 * @return Numeric Symbol value
 */
Numeric co2_ecs_wigner_symbol(
    int Ji, int Jf, int Ji_p, int Jf_p, int L, int li, int lf);


/** Energy of the J=N line at J
 * 
 * @param[in]  J Rotational quantum number
 * @return  O2 energy at this level
 */
Numeric o2_ecs_erot_jn_same(Rational J);

/** Returns the wigner symbol used in Makarov etal 2013
 *
 * Symbol:
 * /             \  /             \  /                 \  /                 \
 * |  Nl  Nk  L  |  |  L  Jk  Jl  |  |  L  Jk_p  Jl_p  |  |  L  Jk    Jl    |
 * |             |  <             >  <                 >  <                 >
 * |  0   0   0  |  |  1  Nl  Nk  |  |  1  Nl    Nk    |  |  1  Jl_p  Jk_p  |
 * \             /  \             /  \                 /  \                 /
 *
 *
 * Note: The wigner library takes two times the physical values
 *       so, e.g., the 1 must be 2.  This hold true for all user inputs as well!
 * 
 * Reference:
 * D.S. Makarov, M.Yu. Tretyakov, C. Boulet,
 * Line mixing in the 60-GHz atmospheric oxygen band: Comparison of the MPM and ECS model,
 * Journal of Quantitative Spectroscopy and Radiative Transfer,
 * Volume 124,
 * 2013,
 * Pages 1-10,
 * ISSN 0022-4073,
 * https://doi.org/10.1016/j.jqsrt.2013.02.019.
 * (http://www.sciencedirect.com/science/article/pii/S0022407313000745)
 * Abstract: Precise profiles of the 60-GHz molecular oxygen band recorded earlier in a wide temperature range by means of a resonator spectrometer at atmospheric pressure were treated. High signal-to-noise ratio allows careful study of the band shape taking into consideration the mixing effect. Comparative analysis of the band profile calculated by an extended MPM (Millimeter-wave Propagation Model) and by the ECS (Energy Corrected Sudden) approximation model is presented. Some limitations of the MPM approach are discussed on the basis of the comparison of the two models. Interbranch coupling is shown to make a noticeable contribution to absorption which means that it should be taken into account as it is expected to improve band profile modeling accuracy.
 * Keywords: Molecular oxygen; Microwave spectroscopy; Profile shape modeling; Collisional coupling
 * 
 * Note:  The ARTS implementation has not been tested in detail
 *
 * Warning:  Must have called wig_temp_init(j) with appropriate j before 
 *           using this function.  Failure to do so will cause segfault.
 * 
 * @param[in] Nl as above times 2
 * @param[in] Nk as above times 2
 * @param[in] Jl as above times 2
 * @param[in] Jk as above times 2
 * @param[in] Jl_p as above times 2
 * @param[in] Jk_p as above times 2
 * @param[in] L as above times 2
 * @return Numeric Symbol value
 */
Numeric o2_ecs_wigner_symbol(
    int Nl, int Nk, int Jl, int Jk, int Jl_p, int Jk_p, int L);

/** Returns the wigner symbol used in Tran etal 2006
 * 
 * Symbol:
 * /               \  /               \  /                  \  /                  \  /                 \
 * |  Ni_p  Ni  L  |  |  Nf_p  Nf  L  |  |  L   Ji    Ji_p  |  |  L   Jf    Jf_p  |  |  L  Ji    Ji_p  |
 * |               |  |               |  <                  >  <                  >  <                 >  (2L + 1)  (OmegaNi / OmegaL) * QL
 * |  0     0   0  |  |  0     0   0  |  |  Si  Ni_p  Ni    |  <  Sf  Nf_p  Nf    |  |  n  Jf_p  Jf    |
 * \               /  \               /  \                  /  \                  /  \                 /
 * 
 * Reference:
 * H. Tran, C. Boulet, and J.-M. Hartmann
 * Line mixing and collision-induced absorption by oxygen in the A
 * band: Laboratory mea*surements, model, and tools for atmospheric
 * spectra computations,
 * Journal Of Geophysical Research,
 * Volume 111,
 * 2006,
 * doi:10.1029/2005JD006869.
 * 
 * Note:  The ARTS implementation has not been tested in detail
 *
 * Warning:  Must have called wig_temp_init(j) with appropriate j before 
 *           using this function.  Failure to do so will cause segfault.
 * 
 * @param[in] Ji Initial J of level
 * @param[in] Jf Final J of level
 * @param[in] Ni Initial N of level
 * @param[in] Nf Final N of level
 * @param[in] Si Initial S of level
 * @param[in] Sf Final S of level
 * @param[in] Ji_p Initial J of pseudo-level
 * @param[in] Jf_p Final J of pseudo-level
 * @param[in] Ni_p Initial N of pseudo-level
 * @param[in] Nf_p Final N of pseudo-level
 * @param[in] L Coupling to pseudo-level
 * @param[in] n Order of the tensor (n=1 is magnetic dipole)
 * @param[in] OmegaNi Adiabatic factor at initial N
 * @param[in] OmegaL Adiabatic factor at L
 * @param[in] QL Adiabatic factor at L
 * @return Numeric Symbol value
 */
Numeric o2_ecs_wigner_symbol_tran(
  const Rational& Ji,
  const Rational& Jf,
  const Rational& Ni,
  const Rational& Nf,
  const Rational& Si,
  const Rational& Sf,
  const Rational& Ji_p,
  const Rational& Jf_p,
  const Rational& Ni_p,
  const Rational& Nf_p,
  const Rational& n,
  const Numeric& T);

/** Returns the reduced dipole moment following Makarov etal 2013
 * 
 * Only for N+ and N- lines
 * 
 * @param[in] Jup Upper state's J
 * @param[in] Jlo Lower state's J
 * @param[in] N Upp/low states' N
 * @return The reduced dipole moment
 */
Numeric o2_makarov2013_reduced_dipole(
  const Rational& Jup,
  const Rational& Jlo,
  const Rational& N);

/** Ready Wigner
 * 
 * @param[in] largest
 * @param[in] fastest
 * @param[in] size [3 or 6]
 * @return largest if successful
 */
Index make_wigner_ready(int largest, int fastest, int size);

/** Tells if the function can deal with the input integer
 * 
 * @param[in] j 
 * @return true If j is less than max allowed j
 * @return false Otherwise
 */
bool is_wigner_ready(int j);

/** Tells if the function is ready for Wigner 3J calculations
 * 
 * @param[in] J Largest input into a Wigner 3J function call
 * @return true If is_wigner_ready(3J + 1) does
 * @return false Otherwise
 */
bool is_wigner3_ready(const Rational& J);

/** Tells if the function is ready for Wigner 6J calculations
 * 
 * @param[in] J Largest input into a Wigner 6J function call
 * @return true If is_wigner_ready(4J + 1)
 * @return false Otherwise
 */
bool is_wigner6_ready(const Rational& J);
