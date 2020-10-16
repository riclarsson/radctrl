/* Copyright (C) 2013
   Oliver Lemke <olemke@core-dump.info>

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA. */

/**
 * @file quantum.cc
 * @author Oliver Lemke
 * @date 2013-04-12
 * 
 * @brief Classes to handle quantum numbers
 */

#ifndef quantum_h
#define quantum_h

#include <array>
#include <iostream>
#include <map>
#include <numeric>
#include <stdexcept>
#include "array.h"
#include "interpolation.h"
#include "matpack.h"
#include "mystring.h"
#include "rational.h"

/** Enum for Quantum Numbers used for indexing
 *
 * If you add anything here, remember to also adapt
 * operator<<(ostream&, const QuantumNumbers&) and
 * operator>>(istream&, QuantumNumbers&)
 * to handle the added numbers.
 */
enum class QuantumNumberType : Index {
  J = 0,   // Total angular momentum
  dJ,      // Delta total angular momentum
  M,       // Projection of J along magnetic field
  N,       // J minus spin
  dN,      // Delta J minus spin
  S,       // Spin angular momentum (from electrons) NOTE: S_global for HITRAN S
  tau,
  n,
  F,       // J + nuclear spin
  Ka,      //(This is a projection of J along one axis)
  Kc,      //(This is a projection of J along another axis)
  Omega,   // This is an absolute projection of J and S
  i,       //(Is related to Omega)
  Lambda,  // This is Sigma or Pi or Lambda states (as seen in literature)
  alpha,   // Alpha from HITRAN
  Sym,     // Symmetry expression
  parity,  // parity value (+/-)
  kronigParity,  // ???
  v1,      // Vibrational mode 1
  v2,      // Vibrational mode 2
  v3,      // Vibrational mode 3
  v4,      // Vibrational mode 4
  v5,      // Vibrational mode 5
  v6,      // Vibrational mode 6
  v7,
  v8,
  v9,
  v10,
  v11,
  v12,
  l1,      // The absolute sum of l_j for v_j
  l2,      // Vibrational angular momentum associated with v2
  l3,
  l4,
  l5,
  l6,
  l7,
  l8,
  l9,
  l10,
  l11,
  l12,
  pm,      // Symmetry type for l=0
  r,       // Rank of the level within a set of the same vibrational symmetry
  S_global,  // Symmetry of the level
  ElectronState,  // Electronic state
  n_global,  // Torosional quanta
  C,         // Another symmetry expression
  Hund,  // Flag for Hund case type.  This flag lets Zeeman know what to expect
  FINAL_ENTRY  // We need this to determine the number of elements in this enum
};

inline QuantumNumberType string2quantumnumbertype(const String& s) {
  #define INPUT_QUANTUM(ID) \
  if (s == #ID) return QuantumNumberType::ID
  INPUT_QUANTUM(J);
  else INPUT_QUANTUM(dJ);
  else INPUT_QUANTUM(M);
  else INPUT_QUANTUM(N);
  else INPUT_QUANTUM(dN);
  else INPUT_QUANTUM(tau);
  else INPUT_QUANTUM(n);
  else INPUT_QUANTUM(S);
  else INPUT_QUANTUM(F);
  else if (s.find("F#") < s.length()) return QuantumNumberType::F;  // HITRAN has many names for F
  else if (s == "K") return QuantumNumberType::Ka;  // HITRAN name
  else INPUT_QUANTUM(Ka);
  else INPUT_QUANTUM(Kc);
  else INPUT_QUANTUM(Omega);
  else INPUT_QUANTUM(i);
  else INPUT_QUANTUM(Lambda);
  else INPUT_QUANTUM(alpha);
  else INPUT_QUANTUM(Sym);
  else INPUT_QUANTUM(parity);
  else INPUT_QUANTUM(kronigParity);
  else if (s == "v") return QuantumNumberType::v1;  // HITRAN name
  else INPUT_QUANTUM(v1);
  else INPUT_QUANTUM(v2);
  else INPUT_QUANTUM(v3);
  else INPUT_QUANTUM(v4);
  else INPUT_QUANTUM(v5);
  else INPUT_QUANTUM(v6);
  else INPUT_QUANTUM(v7);
  else INPUT_QUANTUM(v8);
  else INPUT_QUANTUM(v9);
  else INPUT_QUANTUM(v10);
  else INPUT_QUANTUM(v11);
  else INPUT_QUANTUM(v12);
  else if (s == "l") return QuantumNumberType::l1;  // HITRAN name
  else INPUT_QUANTUM(l1);
  else INPUT_QUANTUM(l2);
  else INPUT_QUANTUM(l3);
  else INPUT_QUANTUM(l4);
  else INPUT_QUANTUM(l5);
  else INPUT_QUANTUM(l6);
  else INPUT_QUANTUM(l7);
  else INPUT_QUANTUM(l8);
  else INPUT_QUANTUM(l9);
  else INPUT_QUANTUM(l10);
  else INPUT_QUANTUM(l11);
  else INPUT_QUANTUM(l12);
  else INPUT_QUANTUM(pm);
  else INPUT_QUANTUM(r);
  else INPUT_QUANTUM(S_global);
  else INPUT_QUANTUM(ElectronState);
  else if (s == "ElecStateLabel") return QuantumNumberType::ElectronState;  // HITRAN name
  else INPUT_QUANTUM(n_global);
  else INPUT_QUANTUM(C);
  else INPUT_QUANTUM(Hund);
  else return QuantumNumberType::FINAL_ENTRY;
  #undef INPUT_QUANTUM
}

inline String quantumnumbertype2string(QuantumNumberType s) {
  #define INPUT_QUANTUM(ID) \
  if (s == QuantumNumberType::ID) return #ID
  INPUT_QUANTUM(J);
  else INPUT_QUANTUM(dJ);
  else INPUT_QUANTUM(M);
  else INPUT_QUANTUM(N);
  else INPUT_QUANTUM(dN);
  else INPUT_QUANTUM(tau);
  else INPUT_QUANTUM(n);
  else INPUT_QUANTUM(S);
  else INPUT_QUANTUM(F);
  else INPUT_QUANTUM(Ka);
  else INPUT_QUANTUM(Kc);
  else INPUT_QUANTUM(Omega);
  else INPUT_QUANTUM(i);
  else INPUT_QUANTUM(Lambda);
  else INPUT_QUANTUM(alpha);
  else INPUT_QUANTUM(Sym);
  else INPUT_QUANTUM(parity);
  else INPUT_QUANTUM(kronigParity);
  else INPUT_QUANTUM(v1);
  else INPUT_QUANTUM(v2);
  else INPUT_QUANTUM(v3);
  else INPUT_QUANTUM(v4);
  else INPUT_QUANTUM(v5);
  else INPUT_QUANTUM(v6);
  else INPUT_QUANTUM(v7);
  else INPUT_QUANTUM(v8);
  else INPUT_QUANTUM(v9);
  else INPUT_QUANTUM(v10);
  else INPUT_QUANTUM(v11);
  else INPUT_QUANTUM(v12);
  else INPUT_QUANTUM(l1);
  else INPUT_QUANTUM(l2);
  else INPUT_QUANTUM(l3);
  else INPUT_QUANTUM(l4);
  else INPUT_QUANTUM(l5);
  else INPUT_QUANTUM(l6);
  else INPUT_QUANTUM(l7);
  else INPUT_QUANTUM(l8);
  else INPUT_QUANTUM(l9);
  else INPUT_QUANTUM(l10);
  else INPUT_QUANTUM(l11);
  else INPUT_QUANTUM(l12);
  else INPUT_QUANTUM(pm);
  else INPUT_QUANTUM(r);
  else INPUT_QUANTUM(S_global);
  else INPUT_QUANTUM(ElectronState);
  else INPUT_QUANTUM(n_global);
  else INPUT_QUANTUM(C);
  else INPUT_QUANTUM(Hund);
  throw std::runtime_error("Bad quantum number type");
  #undef INPUT_QUANTUM
}

/** Enum for Hund cases */
enum class Hund : Index { CaseA = int('a'), CaseB = int('b') };

/** Container class for Quantum Numbers */
class QuantumNumbers {
 public:
  typedef std::array<Rational, Index(QuantumNumberType::FINAL_ENTRY)>
      QuantumContainer;

  /** Initializer with undefined values */
  constexpr QuantumNumbers() noexcept
      : mqnumbers({RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 3
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 6
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 9
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 12
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 15
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 18
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 21
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 24
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 27
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 30
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 33
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 36
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 39
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 42
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 45
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 48
                   RATIONAL_UNDEFINED}) {}
  
  // Temporary initialization until there is a more reliable way to setup 
  // a full initialization for all quantum numbers but only choose a few
  // select ones based on the problem at hand
  constexpr QuantumNumbers(Rational J, Rational N, Rational v) noexcept
      : mqnumbers({J, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 3
                   N, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 6
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 9
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 12
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 15
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 18
                   v, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 21
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 24
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 27
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 30
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 33
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 36
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 39
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 42
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 45
                   RATIONAL_UNDEFINED, RATIONAL_UNDEFINED, RATIONAL_UNDEFINED,  // 48
                   RATIONAL_UNDEFINED}) {}

  /** Access operator
   * 
   * @param[in] qn Index Pos to access
   * @return constexpr Rational Copy of value at pos
   */
  constexpr Rational operator[](const Index qn) const noexcept {
    return mqnumbers[qn];
  }

  /** Access operator
   * 
   * @param qn[in] Index Pos to access
   * @return constexpr Rational Copy of value at pos
   */
  constexpr Rational operator[](const QuantumNumberType qn) const noexcept {
    return mqnumbers[Index(qn)];
  }
  
  /** Access operator
   * 
   * @param[in] qn Index Pos to access
   * @return constexpr Rational Copy of value at pos
   */
  Rational& operator[](const Index qn) noexcept {
    return mqnumbers[qn];
  }
  
  /** Access operator
   * 
   * @param qn[in] Index Pos to access
   * @return constexpr Rational Copy of value at pos
   */
  Rational& operator[](const QuantumNumberType qn) noexcept {
    return mqnumbers[Index(qn)];
  }

  /** Set quantum number at position
   * 
   * @param[in] qn Index Pos to set at
   * @param[in] r Rational to set
   */
  void Set(Index qn, Rational r) {
    assert(qn < Index(QuantumNumberType::FINAL_ENTRY));
    mqnumbers[qn] = r;
  }

  /** Set quantum number at position
   * 
   * @param[in] qn Index Pos to set at
   * @param[in] r Rational to set
   */
  void Set(QuantumNumberType qn, Rational r) {
    assert(qn != QuantumNumberType::FINAL_ENTRY);
    mqnumbers[Index(qn)] = r;
  }

  /** Set quantum number at position
   * 
   * @param[in] qn String Pos to set at by name
   * @param[in] r Rational to set
   */
  void Set(String qn, Rational r) {
    mqnumbers[Index(string2quantumnumbertype(qn))] = r;
  }

  /** Get the numbers
   * 
   * @return const QuantumContainer& All the numbers
   */
  const QuantumContainer& GetNumbers() const { return mqnumbers; }

  /** The number of defined quantum numbers
   * 
   * @return Index Count of defined numbers
   */
  Index nNumbers() const {
    return std::accumulate(
        mqnumbers.cbegin(), mqnumbers.cend(), 0, [](Index i, Rational r) {
          return r.isUndefined() ? i : i + 1;
        });
  }

  /** Compare Quantum Numbers
   * Ignores any undefined numbers in the comparison
   *
   * @param[in] qn  Quantum Numbers to compare to
   *
   * @return true For a match
   * @return false Otherwise
   */
  bool Compare(const QuantumNumbers& qn) const;

 private:
  QuantumContainer mqnumbers;
};

typedef Array<QuantumNumbers> ArrayOfQuantumNumbers;

/** Class to identify and match lines by their quantum numbers
 * 
 * Describes a transition, an energy level, all numbers or none
 * as matchable.  Useful to match to line energy levels and
 * to identify lines themselves.
 * 
 * For transitions, the QI contains upper and lower quantum numbers.
 * For energy levels, it only holds one set of quantum numbers which
 * are then matched against the upper and lower qns of the lines. For
 * all and none no quantum numbers are considered
 * 
 * File format:
 *   Transition:   SPECIES_NAME-ISOTOPE TR UP QUANTUMNUMBERS LO QUANTUMNUMBERS
 *   Energy level: SPECIES_NAME-ISOTOPE EN QUANTUMNUMBERS
 *   All lines:    SPECIES_NAME-ISOTOPE ALL
 *   No lines:    SPECIES_NAME-ISOTOPE NONE
 * 
 * Example written out:
 *   H2O-161 TR UP J 0/1 v1 2/3 LO J 1/1 v2 1/2
 *   H2O-161 EN J 0/1 v1 2/3
 *   H2O-161 ALL
 *   H2O-161 NONE
*/
class QuantumIdentifier {
 public:
  /** Ways to identify quantum numbers */
  typedef enum : Index { TRANSITION, ENERGY_LEVEL, ALL, NONE } QType;

  /** Initialize with no matches */
  constexpr QuantumIdentifier() noexcept
      : mqtype(QuantumIdentifier::NONE), mspecies(-1), miso(-1) {}

  /** Initialize with no quantum numbers defined but known species and matching type
   * 
   * @param[in] qt Way to identify quantum numbers
   * @param[in] species Species index-mapped
   * @param[in] iso Isotopologue index-mapped
   */
  constexpr QuantumIdentifier(const QuantumIdentifier::QType& qt,
                              const Index species,
                              const Index iso) noexcept
      : mqtype(qt), mspecies(species), miso(iso) {}

  /** Initialize with transition identifier type
   * 
   * @param[in] species Species index-mapped
   * @param[in] iso Isotopologue index-mapped
   * @param[in] upper Upper state quantum numbers
   * @param[in] lower Lower state quantum numbers
   */
  constexpr QuantumIdentifier(const Index spec,
                              const Index isot,
                              const QuantumNumbers& upper,
                              const QuantumNumbers& lower) noexcept
      : mqtype(QuantumIdentifier::TRANSITION),
        mspecies(spec),
        miso(isot),
        mqm({upper, lower}) {}

  /** Initialize with transition identifier type
   * 
   * @param[in] species Species index-mapped
   * @param[in] iso Isotopologue index-mapped
   * @param[in] ids List of quantum number keys
   * @param[in] upper Upper state quantum numbers
   * @param[in] lower Lower state quantum numbers
   */
  QuantumIdentifier(const Index spec,
                    const Index isot,
                    const std::vector<QuantumNumberType>& keys,
                    const std::vector<Rational>& upper,
                    const std::vector<Rational>& lower)
      : mqtype(QuantumIdentifier::TRANSITION),
        mspecies(spec),
        miso(isot) {
          for(size_t i=0; i<keys.size(); i++) {
            mqm[TRANSITION_UPPER_INDEX][keys[i]] = upper[i];
            mqm[TRANSITION_LOWER_INDEX][keys[i]] = lower[i];
          }
        }

  /** Initialize with energy level identifier type
   * 
   * @param[in] species Species index-mapped
   * @param[in] iso Isotopologue index-mapped
   * @param[in] qnr Quantum numbers
   */
  constexpr QuantumIdentifier(const Index spec,
                              const Index isot,
                              const QuantumNumbers& qnr) noexcept
      : mqtype(QuantumIdentifier::ENERGY_LEVEL),
        mspecies(spec),
        miso(isot),
        mqm({qnr}) {}

  /** Construct a new Quantum Identifier object from text
   * 
   * @param[in] x Text
   */
  explicit QuantumIdentifier(String x) { SetFromString(x); }

  /** Upper level index */
  static constexpr Index TRANSITION_UPPER_INDEX = 0;

  /** Lower level index */
  static constexpr Index TRANSITION_LOWER_INDEX = 1;

  /** Energy level index */
  static constexpr Index ENERGY_LEVEL_INDEX = 0;

  /** Set the Species
   * 
   * @param[in] sp Species index-mapped
   */
  void Species(Index sp) { mspecies = sp; }

  /** Set the Isotopologue
   * 
   * @param[in] is Isotopologue index-mapped
   */
  void Isotopologue(Index iso) { miso = iso; }

  /** Set to transition type identifier
   * 
   * @param[in] upper Upper state quantum numbers
   * @param[in] lower Lower state quantum numbers
   */
  void SetTransition(const QuantumNumbers& upper, const QuantumNumbers& lower);

  /** Set tp energy level identifier
   * 
   * @param[in] q Quantum numbers
   */
  void SetEnergyLevel(const QuantumNumbers& q);

  /** Set to All identifier */
  void SetAll() { mqtype = QuantumIdentifier::ALL; };
  
  /** Set to NONE identifier */
  void SetNone() { mqtype = QuantumIdentifier::NONE; };

  /** Set key to transition type */
  void SetTransition() { mqtype = QuantumIdentifier::TRANSITION; };

  /** Set from a String object
   * 
   * @param[in] str The string to set this from
   */
  void SetFromString(String str);

  /** Set CO2 transition from String objects
   * 
   * @param[in] upper Upper state quantum numbers
   * @param[in] lower Lower state quantum numbers
   * @param[in] iso Isotopologue by string
   */
  void SetFromStringForCO2Band(String upper, String lower, String iso);

  /** @return constexpr QType Type of identifier */
  constexpr QType Type() const { return mqtype; }
  
  /** Set Type */
  void Type(QType x) { mqtype = x; }
  
  /** Checks if input is a valid Type */
  static bool validIndexForType(Index x) noexcept {
    constexpr auto keys = stdarrayify(Index(TRANSITION), ENERGY_LEVEL, ALL, NONE);
    return std::any_of(keys.cbegin(), keys.cend(), [x](auto y){return x == y;});
  }
  
  /** @return QType if string is a Type or -1 if not */
  static QType string2Type(const String& str) noexcept {
    if (std::string("ENERGY_LEVEL") == str) {
      return QuantumIdentifier::ENERGY_LEVEL;
    } else if (std::string("TRANSITION") == str) {
      return QuantumIdentifier::TRANSITION;
    } else if (std::string("ALL") == str) {
      return QuantumIdentifier::ALL;
    } else if (std::string("NONE") == str) {
      return QuantumIdentifier::NONE;
    } else {
      return QType(-1);
    }
  }

  /** @return QType as String */
  String TypeStr() const;

  /** Return the Species by name */
  String SpeciesName() const;

  /** Return the Species by index */
  constexpr Index Species() const { return mspecies; }

  /** Return the Species by index reference */
  Index& Species() { return mspecies; }

  /** Return the Isotopologue by index */
  constexpr Index Isotopologue() const { return miso; }

  /** Return the Isotopologue by index reference */
  Index& Isotopologue() { return miso; }

  /** Return the quantum numbers array const reference */
  const std::array<QuantumNumbers, 2>& QuantumMatch() const { return mqm; }

  /** Return the quantum numbers array reference */
  std::array<QuantumNumbers, 2>& QuantumMatch() { return mqm; }

  /** Return a quantum identifer as if it wants to match to upper energy level */
  constexpr QuantumIdentifier UpperQuantumId() const noexcept {
    return QuantumIdentifier(mspecies, miso, mqm[TRANSITION_UPPER_INDEX]);
  };

  /** Return a quantum identifer as if it wants to match to lower energy level */
  constexpr QuantumIdentifier LowerQuantumId() const noexcept {
    return QuantumIdentifier(mspecies, miso, mqm[TRANSITION_LOWER_INDEX]);
  };

  /** Return the upper quantum numbers by const reference */
  const QuantumNumbers& UpperQuantumNumbers() const noexcept {
    return mqm[TRANSITION_UPPER_INDEX];
  };

  /** Return the lower quantum numbers by const reference */
  const QuantumNumbers& LowerQuantumNumbers() const noexcept {
    return mqm[TRANSITION_LOWER_INDEX];
  };

  /** Return a upper quantum number by copy */
  constexpr Rational UpperQuantumNumber(QuantumNumberType X) const noexcept {
    return mqm[TRANSITION_UPPER_INDEX][X];
  };

  /** Return a lower quantum number by copy */
  constexpr Rational LowerQuantumNumber(QuantumNumberType X) const noexcept {
    return mqm[TRANSITION_LOWER_INDEX][X];
  };
  
  /** Return a upper quantum number by copy */
  Rational& UpperQuantumNumber(QuantumNumberType X) noexcept {
    return mqm[TRANSITION_UPPER_INDEX][X];
  };
  
  /** Return a lower quantum number by copy */
  Rational& LowerQuantumNumber(QuantumNumberType X) noexcept {
    return mqm[TRANSITION_LOWER_INDEX][X];
  };

  /** Return the energy level quantum numbers by const reference */
  const QuantumNumbers& EnergyLevelQuantumNumbers() const noexcept {
    return mqm[ENERGY_LEVEL_INDEX];
  }
  
  /** Return a energy level quantum number by copy */
  constexpr Rational EnergyLevelQuantumNumber(QuantumNumberType X) const noexcept {
    return mqm[ENERGY_LEVEL_INDEX][X];
  };

  /** Return the upper quantum numbers by reference */
  QuantumNumbers& UpperQuantumNumbers() {
    return mqm[TRANSITION_UPPER_INDEX];
  };

  /** Return the lower quantum numbers by reference */
  QuantumNumbers& LowerQuantumNumbers() {
    return mqm[TRANSITION_LOWER_INDEX];
  };

  /** Return the energy level quantum numbers by reference */
  QuantumNumbers& EnergyLevelQuantumNumbers() {
    return mqm[ENERGY_LEVEL_INDEX];
  }

  /** Return if this is in other
   * 
   * All quantum numbers defined in *this must be the
   * same as the quantum numbers in other for a call
   * to In() to return true.  The numbers in other
   * must not all be defined in *this.
   * 
   * @param[in] other Another quantum identifier
   * @return true If the above description holds
   * @return false Otherwise
   */
  bool In(const QuantumIdentifier& other) const;

  /** Return if this is in other's lower energy state
   * 
   * All quantum numbers defined in *this must be the
   * same as the quantum numbers in other for a call
   * to In() to return true.  The numbers in other
   * must not all be defined in *this.
   * 
   * @param[in] other Another quantum identifier
   * @return true If the above description holds
   * @return false Otherwise
   */
  bool InLower(const QuantumIdentifier& other) const;

  /** Return if this is in other's upper energy state
   * 
   * All quantum numbers defined in *this must be the
   * same as the quantum numbers in other for a call
   * to In() to return true.  The numbers in other
   * must not all be defined in *this.
   * 
   * @param[in] other Another quantum identifier
   * @return true If the above description holds
   * @return false Otherwise
   */
  bool InUpper(const QuantumIdentifier& other) const;

  /** Check if there are any quantum numbers defined */
  bool any_quantumnumbers() const;

  /** Check if *this is a energy level type of identifier */
  bool IsEnergyLevelType() const { return mqtype == ENERGY_LEVEL; }

 private:
  QType mqtype;
  Index mspecies;
  Index miso;
  std::array<QuantumNumbers, 2> mqm;
};

/** Is everything the same between the identifiers
 * 
 * May throw if different Qtypes are compared.
 * 
 * @param[in] a One identifier
 * @param[in] b Another identifier
 * @return true If all quantum numbers match
 * @return false Otherwise
 */
inline bool operator==(const QuantumIdentifier& a, const QuantumIdentifier& b) {
  if (!(a.Isotopologue() == b.Isotopologue() && a.Species() == b.Species() &&
        a.Type() == b.Type()))
    return false;

  if (a.Type() == QuantumIdentifier::ENERGY_LEVEL)
    return a.QuantumMatch()[a.ENERGY_LEVEL_INDEX].Compare(
        b.QuantumMatch()[b.ENERGY_LEVEL_INDEX]);
  else if (a.Type() == QuantumIdentifier::TRANSITION)
    return a.QuantumMatch()[a.TRANSITION_LOWER_INDEX].Compare(
               b.QuantumMatch()[b.TRANSITION_LOWER_INDEX]) &&
           a.QuantumMatch()[a.TRANSITION_UPPER_INDEX].Compare(
               b.QuantumMatch()[b.TRANSITION_UPPER_INDEX]);
  else if (a.Type() == QuantumIdentifier::ALL)
    return true;
  else if (a.Type() == QuantumIdentifier::NONE)
    return false;
  else
    throw std::runtime_error("Programmer error --- added type is missing");
}

/** Is anything different between the identifiers
 * 
 * May throw if different Qtypes are compared.
 * 
 * @param[in] a One identifier
 * @param[in] b Another identifier
 * @return true If some quantum numbers mismatch
 * @return false Otherwise
 */
inline bool operator!=(const QuantumIdentifier& a, const QuantumIdentifier& b) {
  return not operator==(a, b);
}

/** Check if all quantum numbers are the same between a and b
 * 
 * @param[in] a One set of quantum numbers
 * @param[in] b Another set of quantum numbers
 * @return true If all quantum numbers match
 * @return false Otherwise
 */
inline bool operator==(const QuantumNumbers& a, const QuantumNumbers& b) {
  return a.Compare(b) and b.Compare(a);
}

typedef Array<QuantumIdentifier> ArrayOfQuantumIdentifier;

/** Check for valid quantum number name
 * 
 * @param[in] name Parameter
 * @return true If the parameter exist
 * @return false Otherwise
 */
bool IsValidQuantumNumberName(String name);

/** Check for valid quantum number name and throws if it is invalid
 * 
 * @param[in] name Parameter
 */
void ThrowIfQuantumNumberNameInvalid(String name);

/** Input operator */
std::istream& operator>>(std::istream& is, QuantumNumbers& qn);

/** Output operator */
std::ostream& operator<<(std::ostream& os, const QuantumNumbers& qn);

/** Output operator */
std::ostream& operator<<(std::ostream& os, const QuantumIdentifier& qi);

std::ostream& operator<<(std::ostream& os, QuantumNumberType t);

/** Updates the quantum identifier based on a lists of strings
 * 
 * The input lists of strings should be paired as {key, value}
 * 
 * \param[in,out] qid Identifier to update
 * \param[in] upper_list List of strings to update upper state
 * \param[in] lower_list List of strings to update lower state
 */
void update_id(QuantumIdentifier& qid, const std::vector<std::array<String, 2> >& upper_list, const std::vector<std::array<String, 2> >& lower_list);

#endif
