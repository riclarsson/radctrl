#ifndef species_h
#define species_h

#include <vector>

#include "enums.h"
#include "quantum.h"
#include "species_partfun.h"

namespace Species {
/** named species */
ENUMCLASS(Species, unsigned char, Bath, Water, CarbonDioxide, Ozone,
          NitrogenOxide, CarbonMonoxide, Methane, Oxygen, NitricOxide,
          SulfurDioxide, NitrogenDioxide, Ammonia, NitricAcid, Hydroxyl,
          HydrogenFluoride, HydrogenChloride, HydrogenBromide, HydrogenIodide,
          ChlorineMonoxide, CarbonylSulfide, Formaldehyde, HypochlorousAcid,
          Nitrogen, HydrogenCyanide, MethylChloride, HydrogenPeroxide,
          Acetylene, Ethane, Phosphine, CarbonylFluoride, SulfurHexafluoride,
          HydrogenSulfide, FormicAcid, Hydroperoxyl, OxygenAtom,
          ChlorineNitrate, NitricOxideCation, HypobromousAcid, Ethylene,
          Methanol, MethylBromide, Acetonitrile, Pfc14, Diacetylene,
          Cyanoacetylene, Hydrogen, CarbonMonosulfide, SulfurTrioxide, Cyanogen,
          Phosgene)  // Species

std::string toShortName(Species x) noexcept;
Species fromShortName(const std::string& x) noexcept;

/** named atoms */
ENUMCLASS(Atom, unsigned char, H, D, T, He4, He3, Li7, Li6, Be9, B11, B10, C12,
          C13, N14, N15, O16, O17, O18, F19, Ne20, Ne21, Ne22, Na23, Mg24, Mg25,
          Mg26, Al27, Si28, Si29, Si30, P31, S32, S33, S34, Cl35, Cl37, Ar40,
          Ar36, Ar38, K39, K40, K41, Ca40, Ca42, Ca43, Ca44, Ca46, Ca48, Sc45,
          Ti46, Ti47, Ti48, Ti49, Ti50, V51, V50, Cr52, Cr50, Cr53, Cr54, Mn55,
          Fe56, Fe54, Fe57, Fe58, Co59, Ni58, Cu63, Zn64, Ga69, Ge74, As75,
          Se80, Br79, Br81, Kr84, Rb85, Rb87, Sr88, Y89, Zr90, Nb93, Mo98, Mo92,
          Mo94, Mo95, Mo96, Mo100, Tc99, Ru102, Ru99, Ru100, Ru101, Ru104,
          Rh103, Pd106, Pd104, Pd105, Pd108, Ag107, Ag109, Cd114, Cd110, Cd111,
          Cd112, Cd113, Cd116, In115, In113, Sn120, Sb121, Sb123, Te130, I127,
          Xe132, Xe129, Xe131, Cs133, Ba138, La139, Ce140, Pr141, Nd142, Nd143,
          Nd144, Nd145, Nd146, Pm0, Sm144, Sm150, Sm152, Sm154, Eu151, Eu153,
          Gd154, Gd155, Gd156, Gd157, Gd158, Gd160, Tb159, Dy156, Dy158, Dy160,
          Dy161, Dy162, Dy163, Dy164, Ho165, Er162, Er164, Er166, Er167, Er168,
          Er170, Tm169, Yb168, Yb170, Yb171, Yb172, Yb173, Yb174, Yb176, Lu175,
          Hf176, Hf177, Hf178, Hf179, Hf180, Ta181, W180, W182, W184, W186,
          Re185, Os187, Os188, Os189, Os190, Os192, Ir191, Ir193, Pt192, Pt194,
          Pt195, Pt196, Pt198, Au197, Hg196, Hg198, Hg199, Hg200, Hg201, Hg202,
          Hg204, Tl203, Tl205, Pb206, Pb207, Pb208, Bi209, Po0, At0, Rn0, Fr0,
          Ra0, Ac0, Th232, Pa0, U235, U238, Np0, Pu0, Am0, Cm0, Bk0, Cf0, Es0,
          Fm0)

/** Information about atoms */
struct AtomInfo {
  std::string name;
  double nuclear_spin;
};

/** Number of atoms for a species */
constexpr unsigned char getAtomCount(Species s) {
  switch (s) {
    case Species::Bath:
      return 0;
    case Species::Water:
      return 3;
    case Species::CarbonDioxide:
      return 3;
    case Species::Ozone:
      return 3;
    case Species::NitrogenOxide:
      return 3;
    case Species::CarbonMonoxide:
      return 2;
    case Species::Methane:
      return 5;
    case Species::Oxygen:
      return 2;
    case Species::NitricOxide:
      return 2;
    case Species::SulfurDioxide:
      return 3;
    case Species::NitrogenDioxide:
      return 3;
    case Species::Ammonia:
      return 4;
    case Species::NitricAcid:
      return 5;
    case Species::Hydroxyl:
      return 2;
    case Species::HydrogenFluoride:
      return 2;
    case Species::HydrogenChloride:
      return 2;
    case Species::HydrogenBromide:
      return 2;
    case Species::HydrogenIodide:
      return 2;
    case Species::ChlorineMonoxide:
      return 2;
    case Species::CarbonylSulfide:
      return 3;
    case Species::Formaldehyde:
      return 4;
    case Species::HypochlorousAcid:
      return 3;
    case Species::Nitrogen:
      return 2;
    case Species::HydrogenCyanide:
      return 3;
    case Species::MethylChloride:
      return 5;
    case Species::HydrogenPeroxide:
      return 4;
    case Species::Acetylene:
      return 4;
    case Species::Ethane:
      return 8;
    case Species::Phosphine:
      return 4;
    case Species::CarbonylFluoride:
      return 4;
    case Species::SulfurHexafluoride:
      return 7;
    case Species::HydrogenSulfide:
      return 3;
    case Species::FormicAcid:
      return 5;
    case Species::Hydroperoxyl:
      return 3;
    case Species::OxygenAtom:
      return 1;
    case Species::ChlorineNitrate:
      return 5;
    case Species::NitricOxideCation:
      return 2;
    case Species::HypobromousAcid:
      return 3;
    case Species::Ethylene:
      return 6;
    case Species::Methanol:
      return 6;
    case Species::MethylBromide:
      return 5;
    case Species::Acetonitrile:
      return 6;
    case Species::Pfc14:
      return 5;
    case Species::Diacetylene:
      return 6;
    case Species::Cyanoacetylene:
      return 5;
    case Species::Hydrogen:
      return 2;
    case Species::CarbonMonosulfide:
      return 2;
    case Species::SulfurTrioxide:
      return 4;
    case Species::Cyanogen:
      return 4;
    case Species::Phosgene:
      return 4;
    case Species::FINAL: { /*leave last*/
    }
  }
  return std::numeric_limits<unsigned char>::max();
}

/** Number of isotopes considered for a species */
constexpr unsigned char getIsotopologueCount(Species s) {
  switch (s) {
    case Species::Bath:
      return 0;
    case Species::Water:
      return 7;
    case Species::CarbonDioxide:
      return 12;
    case Species::Ozone:
      return 5;
    case Species::NitrogenOxide:
      return 5;
    case Species::CarbonMonoxide:
      return 6;
    case Species::Methane:
      return 4;
    case Species::Oxygen:
      return 3;
    case Species::NitricOxide:
      return 3;
    case Species::SulfurDioxide:
      return 2;
    case Species::NitrogenDioxide:
      return 2;
    case Species::Ammonia:
      return 2;
    case Species::NitricAcid:
      return 2;
    case Species::Hydroxyl:
      return 3;
    case Species::HydrogenFluoride:
      return 2;
    case Species::HydrogenChloride:
      return 4;
    case Species::HydrogenBromide:
      return 4;
    case Species::HydrogenIodide:
      return 2;
    case Species::ChlorineMonoxide:
      return 2;
    case Species::CarbonylSulfide:
      return 5;
    case Species::Formaldehyde:
      return 3;
    case Species::HypochlorousAcid:
      return 2;
    case Species::Nitrogen:
      return 2;
    case Species::HydrogenCyanide:
      return 3;
    case Species::MethylChloride:
      return 2;
    case Species::HydrogenPeroxide:
      return 1;
    case Species::Acetylene:
      return 3;
    case Species::Ethane:
      return 2;
    case Species::Phosphine:
      return 1;
    case Species::CarbonylFluoride:
      return 2;
    case Species::SulfurHexafluoride:
      return 1;
    case Species::HydrogenSulfide:
      return 3;
    case Species::FormicAcid:
      return 1;
    case Species::Hydroperoxyl:
      return 1;
    case Species::OxygenAtom:
      return 1;
    case Species::ChlorineNitrate:
      return 2;
    case Species::NitricOxideCation:
      return 1;
    case Species::HypobromousAcid:
      return 2;
    case Species::Ethylene:
      return 2;
    case Species::Methanol:
      return 1;
    case Species::MethylBromide:
      return 2;
    case Species::Acetonitrile:
      return 1;
    case Species::Pfc14:
      return 1;
    case Species::Diacetylene:
      return 1;
    case Species::Cyanoacetylene:
      return 1;
    case Species::Hydrogen:
      return 2;
    case Species::CarbonMonosulfide:
      return 4;
    case Species::SulfurTrioxide:
      return 1;
    case Species::Cyanogen:
      return 1;
    case Species::Phosgene:
      return 2;
    case Species::FINAL: { /*leave last*/
    }
  }
  return std::numeric_limits<unsigned char>::max();
}

/** Number of global quantum numbers considered for a species */
constexpr unsigned char getGlobalQuantumNumberCount(Species s) {
  switch (s) {
    case Species::Bath:
      return 0;
    case Species::Water:
      return 4;
    case Species::CarbonDioxide:
      return 7;
    case Species::Ozone:
      return 5;
    case Species::NitrogenOxide:
      return 6;
    case Species::CarbonMonoxide:
      return 2;
    case Species::Methane:
      return 11;
    case Species::Oxygen:
      return 4;
    case Species::NitricOxide:
      return 6;
    case Species::SulfurDioxide:
      return 4;
    case Species::NitrogenDioxide:
      return 5;
    case Species::Ammonia:
      return 12;
    case Species::NitricAcid:
      return 10;
    case Species::Hydroxyl:
      return 6;
    case Species::HydrogenFluoride:
      return 2;
    case Species::HydrogenChloride:
      return 2;
    case Species::HydrogenBromide:
      return 2;
    case Species::HydrogenIodide:
      return 2;
    case Species::ChlorineMonoxide:
      return 6;
    case Species::CarbonylSulfide:
      return 6;
    case Species::Formaldehyde:
      return 7;
    case Species::HypochlorousAcid:
      return 4;
    case Species::Nitrogen:
      return 2;
    case Species::HydrogenCyanide:
      return 6;
    case Species::MethylChloride:
      return 9;
    case Species::HydrogenPeroxide:
      return 8;
    case Species::Acetylene:
      return 12;
    case Species::Ethane:
      return 9;
    case Species::Phosphine:
      return 7;
    case Species::CarbonylFluoride:
      return 7;
    case Species::SulfurHexafluoride:
      return 0;
    case Species::HydrogenSulfide:
      return 4;
    case Species::FormicAcid:
      return 10;
    case Species::Hydroperoxyl:
      return 5;
    case Species::OxygenAtom:
      return 3;
    case Species::ChlorineNitrate:
      return 0;
    case Species::NitricOxideCation:
      return 2;
    case Species::HypobromousAcid:
      return 4;
    case Species::Ethylene:
      return 10;
    case Species::Methanol:
      return 11;
    case Species::MethylBromide:
      return 6;
    case Species::Acetonitrile:
      return 3;
    case Species::Pfc14:
      return 0;
    case Species::Diacetylene:
      return 17;
    case Species::Cyanoacetylene:
      return 13;
    case Species::Hydrogen:
      return 2;
    case Species::CarbonMonosulfide:
      return 2;
    case Species::SulfurTrioxide:
      return 1;
    case Species::Cyanogen:
      return 11;
    case Species::Phosgene:
      return 12;
    case Species::FINAL: { /*leave last*/
    }
  }
  return std::numeric_limits<unsigned char>::max();
}

/** Number of local quantum numbers considered for a species */
constexpr unsigned char getLocalQuantumNumberCount(Species s) {
  switch (s) {
    case Species::Bath:
      return 0;
    case Species::Water:
      return 3;
    case Species::CarbonDioxide:
      return 2;
    case Species::Ozone:
      return 4;
    case Species::NitrogenOxide:
      return 1;
    case Species::CarbonMonoxide:
      return 1;
    case Species::Methane:
      return 2;
    case Species::Oxygen:
      return 3;
    case Species::NitricOxide:
      return 3;
    case Species::SulfurDioxide:
      return 3;
    case Species::NitrogenDioxide:
      return 5;
    case Species::Ammonia:
      return 2;
    case Species::NitricAcid:
      return 4;
    case Species::Hydroxyl:
      return 4;
    case Species::HydrogenFluoride:
      return 1;
    case Species::HydrogenChloride:
      return 2;
    case Species::HydrogenBromide:
      return 2;
    case Species::HydrogenIodide:
      return 2;
    case Species::ChlorineMonoxide:
      return 3;
    case Species::CarbonylSulfide:
      return 1;
    case Species::Formaldehyde:
      return 3;
    case Species::HypochlorousAcid:
      return 3;
    case Species::Nitrogen:
      return 1;
    case Species::HydrogenCyanide:
      return 2;
    case Species::MethylChloride:
      return 3;
    case Species::HydrogenPeroxide:
      return 3;
    case Species::Acetylene:
      return 1;
    case Species::Ethane:
      return 2;
    case Species::Phosphine:
      return 2;
    case Species::CarbonylFluoride:
      return 3;
    case Species::SulfurHexafluoride:
      return 0;
    case Species::HydrogenSulfide:
      return 3;
    case Species::FormicAcid:
      return 3;
    case Species::Hydroperoxyl:
      return 5;
    case Species::OxygenAtom:
      return 2;
    case Species::ChlorineNitrate:
      return 0;
    case Species::NitricOxideCation:
      return 1;
    case Species::HypobromousAcid:
      return 3;
    case Species::Ethylene:
      return 3;
    case Species::Methanol:
      return 2;
    case Species::MethylBromide:
      return 2;
    case Species::Acetonitrile:
      return 2;
    case Species::Pfc14:
      return 0;
    case Species::Diacetylene:
      return 1;
    case Species::Cyanoacetylene:
      return 1;
    case Species::Hydrogen:
      return 1;
    case Species::CarbonMonosulfide:
      return 1;
    case Species::SulfurTrioxide:
      return 2;
    case Species::Cyanogen:
      return 1;
    case Species::Phosgene:
      return 3;
    case Species::FINAL: { /*leave last*/
    }
  }
  return std::numeric_limits<unsigned char>::max();
}

/** A struct representing an atom and a charge (defaults to 0 charge) */
struct ChargedAtom {
  Atom atom;
  char charge;
  constexpr ChargedAtom() noexcept : atom(Atom::FINAL), charge(0) {}
  constexpr ChargedAtom(Atom a, char c = 0) noexcept : atom(a), charge(c) {}

  friend std::ostream& operator<<(std::ostream& os, ChargedAtom ca) {
    return os << ca.atom << ' ' << int(ca.charge);
  }
  friend std::istream& operator>>(std::istream& is, ChargedAtom& ca) {
    return is >> ca.atom >> ca.charge;
  }
  friend constexpr bool operator==(ChargedAtom a, ChargedAtom b) {
    return (a.atom == b.atom) and (a.charge == b.charge);
  }
};
constexpr ChargedAtom operator+(Atom a, int b) { return ChargedAtom(a, b); }

/** Struct containing all information needed about one isotope */
template <Species S>
struct IsotopeRecord {
  static constexpr unsigned char N = getAtomCount(S);
  std::array<ChargedAtom, N> atoms;
  double mass;
  long gi;
  long afgl;
  constexpr IsotopeRecord() noexcept : atoms(), mass(0), gi(0) {}
  constexpr IsotopeRecord(std::array<ChargedAtom, N> a, double b, long c,
                          long d) noexcept
      : atoms(a), mass(b), gi(c), afgl(d) {}
};

/** Static data variable for looking up information about the species at runtime
 * and at compile time */
template <Species S>
constexpr std::array<IsotopeRecord<S>, getIsotopologueCount(S)>
isotopeRecords() {
  using CA = ChargedAtom;
  using IR = IsotopeRecord<S>;
  using SA = std::array<CA, IR::N>;
  if constexpr (S == Species::Water) {  // H2O
    return {
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::O16},
           },
           18.010565 /*grams/mol*/, 1 /*gi*/, 161 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::O18},
           },
           20.014811 /*grams/mol*/, 1 /*gi*/, 181 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::O17},
           },
           19.01478 /*grams/mol*/, 6 /*gi*/, 171 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::D},
               CA{Atom::O16},
           },
           19.01674 /*grams/mol*/, 6 /*gi*/, 162 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::D},
               CA{Atom::O18},
           },
           21.020985 /*grams/mol*/, 6 /*gi*/, 182 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::D},
               CA{Atom::O17},
           },
           20.020956 /*grams/mol*/, 36 /*gi*/, 172 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::D},
               CA{Atom::D},
               CA{Atom::O16},
           },
           20.022915 /*grams/mol*/, 1 /*gi*/, 262 /*afgl*/},
    };
  } else if constexpr (S == Species::CarbonDioxide) {  // CO2
    return {
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::O16},
               CA{Atom::O16},
           },
           43.98983 /*grams/mol*/, 1 /*gi*/, 626 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C13},
               CA{Atom::O16},
               CA{Atom::O16},
           },
           44.993185 /*grams/mol*/, 2 /*gi*/, 636 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::O16},
               CA{Atom::C12},
               CA{Atom::O18},
           },
           45.994076 /*grams/mol*/, 1 /*gi*/, 628 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::O16},
               CA{Atom::C12},
               CA{Atom::O17},
           },
           44.994045 /*grams/mol*/, 6 /*gi*/, 627 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::O16},
               CA{Atom::C13},
               CA{Atom::O18},
           },
           46.997431 /*grams/mol*/, 2 /*gi*/, 638 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::O16},
               CA{Atom::C13},
               CA{Atom::O17},
           },
           45.9974 /*grams/mol*/, 12 /*gi*/, 637 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::O18},
               CA{Atom::O18},
           },
           47.998322 /*grams/mol*/, 1 /*gi*/, 828 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::O17},
               CA{Atom::C12},
               CA{Atom::O18},
           },
           46.998291 /*grams/mol*/, 6 /*gi*/, 827 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::O17},
               CA{Atom::O17},
           },
           45.998262 /*grams/mol*/, 1 /*gi*/, 727 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C13},
               CA{Atom::O18},
               CA{Atom::O18},
           },
           49.001675 /*grams/mol*/, 2 /*gi*/, 838 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::O18},
               CA{Atom::C13},
               CA{Atom::O17},
           },
           48.001646 /*grams/mol*/, 12 /*gi*/, 837 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C13},
               CA{Atom::O17},
               CA{Atom::O17},
           },
           47.0016182378 /*grams/mol*/, 2 /*gi*/, 737 /*afgl*/},
    };
  } else if constexpr (S == Species::Ozone) {  // O3
    return {
        IR{/*atoms=*/SA{
               CA{Atom::O16},
               CA{Atom::O16},
               CA{Atom::O16},
           },
           47.984745 /*grams/mol*/, 1 /*gi*/, 666 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::O16},
               CA{Atom::O16},
               CA{Atom::O18},
           },
           49.988991 /*grams/mol*/, 1 /*gi*/, 668 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::O16},
               CA{Atom::O18},
               CA{Atom::O16},
           },
           49.988991 /*grams/mol*/, 1 /*gi*/, 686 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::O16},
               CA{Atom::O16},
               CA{Atom::O17},
           },
           48.98896 /*grams/mol*/, 6 /*gi*/, 667 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::O16},
               CA{Atom::O17},
               CA{Atom::O16},
           },
           48.98896 /*grams/mol*/, 6 /*gi*/, 676 /*afgl*/},
    };
  } else if constexpr (S == Species::NitrogenOxide) {  // N2O
    return {
        IR{/*atoms=*/SA{
               CA{Atom::N14},
               CA{Atom::N14},
               CA{Atom::O16},
           },
           44.001062 /*grams/mol*/, 9 /*gi*/, 446 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::N14},
               CA{Atom::N15},
               CA{Atom::O16},
           },
           44.998096 /*grams/mol*/, 6 /*gi*/, 456 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::N15},
               CA{Atom::N14},
               CA{Atom::O16},
           },
           44.998096 /*grams/mol*/, 6 /*gi*/, 546 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::N14},
               CA{Atom::N14},
               CA{Atom::O18},
           },
           46.005308 /*grams/mol*/, 9 /*gi*/, 448 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::N14},
               CA{Atom::N14},
               CA{Atom::O17},
           },
           45.005278 /*grams/mol*/, 54 /*gi*/, 447 /*afgl*/},
    };
  } else if constexpr (S == Species::CarbonMonoxide) {  // CO
    return {
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::O16},
           },
           27.994915 /*grams/mol*/, 1 /*gi*/, 26 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C13},
               CA{Atom::O16},
           },
           28.99827 /*grams/mol*/, 2 /*gi*/, 36 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::O18},
           },
           29.999161 /*grams/mol*/, 1 /*gi*/, 28 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::O17},
           },
           28.99913 /*grams/mol*/, 6 /*gi*/, 27 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C13},
               CA{Atom::O18},
           },
           31.002516 /*grams/mol*/, 2 /*gi*/, 38 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C13},
               CA{Atom::O17},
           },
           30.002485 /*grams/mol*/, 12 /*gi*/, 37 /*afgl*/},
    };
  } else if constexpr (S == Species::Methane) {  // CH4
    return {
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
           },
           16.0313 /*grams/mol*/, 1 /*gi*/, 211 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C13},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
           },
           17.034655 /*grams/mol*/, 2 /*gi*/, 311 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::D},
           },
           17.037475 /*grams/mol*/, 3 /*gi*/, 212 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C13},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::D},
           },
           18.04083 /*grams/mol*/, 6 /*gi*/, 312 /*afgl*/},
    };
  } else if constexpr (S == Species::Oxygen) {  // O2
    return {
        IR{/*atoms=*/SA{
               CA{Atom::O16},
               CA{Atom::O16},
           },
           31.98983 /*grams/mol*/, 1 /*gi*/, 66 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::O16},
               CA{Atom::O18},
           },
           33.994076 /*grams/mol*/, 1 /*gi*/, 68 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::O16},
               CA{Atom::O17},
           },
           32.994045 /*grams/mol*/, 6 /*gi*/, 67 /*afgl*/},
    };
  } else if constexpr (S == Species::NitricOxide) {  // NO
    return {
        IR{/*atoms=*/SA{
               CA{Atom::N14},
               CA{Atom::O16},
           },
           29.997989 /*grams/mol*/, 3 /*gi*/, 46 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::N15},
               CA{Atom::O16},
           },
           30.995023 /*grams/mol*/, 2 /*gi*/, 56 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::N14},
               CA{Atom::O18},
           },
           32.002234 /*grams/mol*/, 3 /*gi*/, 48 /*afgl*/},
    };
  } else if constexpr (S == Species::SulfurDioxide) {  // SO2
    return {
        IR{/*atoms=*/SA{
               CA{Atom::S32},
               CA{Atom::O16},
               CA{Atom::O16},
           },
           63.961901 /*grams/mol*/, 1 /*gi*/, 626 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::S34},
               CA{Atom::O16},
               CA{Atom::O16},
           },
           65.957695 /*grams/mol*/, 1 /*gi*/, 646 /*afgl*/},
    };
  } else if constexpr (S == Species::NitrogenDioxide) {  // NO2
    return {
        IR{/*atoms=*/SA{
               CA{Atom::N14},
               CA{Atom::O16},
               CA{Atom::O16},
           },
           45.992904 /*grams/mol*/, 3 /*gi*/, 646 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::N15},
               CA{Atom::O16},
               CA{Atom::O16},
           },
           46.989938 /*grams/mol*/, 2 /*gi*/, 656 /*afgl*/},
    };
  } else if constexpr (S == Species::Ammonia) {  // NH3
    return {
        IR{/*atoms=*/SA{
               CA{Atom::N14},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
           },
           17.026549 /*grams/mol*/, 3 /*gi*/, 4111 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::N15},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
           },
           18.023583 /*grams/mol*/, 2 /*gi*/, 5111 /*afgl*/},
    };
  } else if constexpr (S == Species::NitricAcid) {  // HNO3
    return {
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::N14},
               CA{Atom::O16},
               CA{Atom::O16},
               CA{Atom::O16},
           },
           62.995644 /*grams/mol*/, 6 /*gi*/, 146 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::N15},
               CA{Atom::O16},
               CA{Atom::O16},
               CA{Atom::O16},
           },
           63.99268 /*grams/mol*/, 4 /*gi*/, 156 /*afgl*/},
    };
  } else if constexpr (S == Species::Hydroxyl) {  // OH
    return {
        IR{/*atoms=*/SA{
               CA{Atom::O16},
               CA{Atom::H},
           },
           17.00274 /*grams/mol*/, 2 /*gi*/, 61 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::O18},
               CA{Atom::H},
           },
           19.006986 /*grams/mol*/, 2 /*gi*/, 81 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::O16},
               CA{Atom::D},
           },
           18.008915 /*grams/mol*/, 3 /*gi*/, 62 /*afgl*/},
    };
  } else if constexpr (S == Species::HydrogenFluoride) {  // HF
    return {
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::F19},
           },
           20.006229 /*grams/mol*/, 4 /*gi*/, 19 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::D},
               CA{Atom::F19},
           },
           21.012404 /*grams/mol*/, 6 /*gi*/, 29 /*afgl*/},
    };
  } else if constexpr (S == Species::HydrogenChloride) {  // HCl
    return {
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::Cl35},
           },
           35.976678 /*grams/mol*/, 8 /*gi*/, 15 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::Cl37},
           },
           37.973729 /*grams/mol*/, 8 /*gi*/, 17 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::D},
               CA{Atom::Cl35},
           },
           36.982853 /*grams/mol*/, 12 /*gi*/, 25 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::D},
               CA{Atom::Cl37},
           },
           38.979904 /*grams/mol*/, 12 /*gi*/, 27 /*afgl*/},
    };
  } else if constexpr (S == Species::HydrogenBromide) {  // HBr
    return {
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::Br79},
           },
           79.92616 /*grams/mol*/, 8 /*gi*/, 19 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::Br81},
           },
           81.924115 /*grams/mol*/, 8 /*gi*/, 11 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::D},
               CA{Atom::Br79},
           },
           80.932336 /*grams/mol*/, 12 /*gi*/, 29 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::D},
               CA{Atom::Br81},
           },
           82.930289 /*grams/mol*/, 12 /*gi*/, 21 /*afgl*/},
    };
  } else if constexpr (S == Species::HydrogenIodide) {  // HI
    return {
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::I127},
           },
           127.912297 /*grams/mol*/, 12 /*gi*/, 17 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::D},
               CA{Atom::I127},
           },
           128.918472 /*grams/mol*/, 18 /*gi*/, 27 /*afgl*/},
    };
  } else if constexpr (S == Species::ChlorineMonoxide) {  // ClO
    return {
        IR{/*atoms=*/SA{
               CA{Atom::Cl35},
               CA{Atom::O16},
           },
           50.963768 /*grams/mol*/, 4 /*gi*/, 56 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::Cl37},
               CA{Atom::O16},
           },
           52.960819 /*grams/mol*/, 4 /*gi*/, 76 /*afgl*/},
    };
  } else if constexpr (S == Species::CarbonylSulfide) {  // OCS
    return {
        IR{/*atoms=*/SA{
               CA{Atom::O16},
               CA{Atom::C12},
               CA{Atom::S32},
           },
           59.966986 /*grams/mol*/, 1 /*gi*/, 622 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::O16},
               CA{Atom::C12},
               CA{Atom::S34},
           },
           61.96278 /*grams/mol*/, 1 /*gi*/, 624 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::O16},
               CA{Atom::C13},
               CA{Atom::S32},
           },
           60.970341 /*grams/mol*/, 2 /*gi*/, 632 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::O16},
               CA{Atom::C12},
               CA{Atom::S33},
           },
           60.966371 /*grams/mol*/, 4 /*gi*/, 623 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::O18},
               CA{Atom::C12},
               CA{Atom::S32},
           },
           61.971231 /*grams/mol*/, 1 /*gi*/, 822 /*afgl*/},
    };
  } else if constexpr (S == Species::Formaldehyde) {  // H2CO
    return {
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::C12},
               CA{Atom::O16},
           },
           30.010565 /*grams/mol*/, 1 /*gi*/, 126 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::C13},
               CA{Atom::O16},
           },
           31.01392 /*grams/mol*/, 2 /*gi*/, 136 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::C12},
               CA{Atom::O18},
           },
           32.014811 /*grams/mol*/, 1 /*gi*/, 128 /*afgl*/},
    };
  } else if constexpr (S == Species::HypochlorousAcid) {  // HOCl
    return {
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::O16},
               CA{Atom::Cl35},
           },
           51.971593 /*grams/mol*/, 8 /*gi*/, 165 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::O16},
               CA{Atom::Cl37},
           },
           53.968644 /*grams/mol*/, 8 /*gi*/, 167 /*afgl*/},
    };
  } else if constexpr (S == Species::Nitrogen) {  // N2
    return {
        IR{/*atoms=*/SA{
               CA{Atom::N14},
               CA{Atom::N14},
           },
           28.006148 /*grams/mol*/, 1 /*gi*/, 44 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::N14},
               CA{Atom::N15},
           },
           29.003182 /*grams/mol*/, 6 /*gi*/, 45 /*afgl*/},
    };
  } else if constexpr (S == Species::HydrogenCyanide) {  // HCN
    return {
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::C12},
               CA{Atom::N14},
           },
           27.010899 /*grams/mol*/, 6 /*gi*/, 124 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::C13},
               CA{Atom::N14},
           },
           28.014254 /*grams/mol*/, 12 /*gi*/, 134 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::C12},
               CA{Atom::N15},
           },
           28.007933 /*grams/mol*/, 4 /*gi*/, 125 /*afgl*/},
    };
  } else if constexpr (S == Species::MethylChloride) {  // CH3Cl
    return {
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::Cl35},
           },
           49.992328 /*grams/mol*/, 4 /*gi*/, 215 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::Cl37},
           },
           51.989379 /*grams/mol*/, 4 /*gi*/, 217 /*afgl*/},
    };
  } else if constexpr (S == Species::HydrogenPeroxide) {  // H2O2
    return {
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::O16},
               CA{Atom::O16},
           },
           34.00548 /*grams/mol*/, 1 /*gi*/, 1661 /*afgl*/},
    };
  } else if constexpr (S == Species::Acetylene) {  // C2H2
    return {
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::C12},
               CA{Atom::H},
               CA{Atom::H},
           },
           26.01565 /*grams/mol*/, 1 /*gi*/, 1221 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::C12},
               CA{Atom::C13},
               CA{Atom::H},
           },
           27.019005 /*grams/mol*/, 8 /*gi*/, 1231 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::C12},
               CA{Atom::C12},
               CA{Atom::D},
           },
           27.021825 /*grams/mol*/, 6 /*gi*/, 1222 /*afgl*/},
    };
  } else if constexpr (S == Species::Ethane) {  // C2H6
    return {
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::C12},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
           },
           30.04695 /*grams/mol*/, 1 /*gi*/, 1221 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::C13},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
           },
           31.050305 /*grams/mol*/, 2 /*gi*/, 1231 /*afgl*/},
    };
  } else if constexpr (S == Species::Phosphine) {  // PH3
    return {
        IR{/*atoms=*/SA{
               CA{Atom::P31},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
           },
           33.997238 /*grams/mol*/, 2 /*gi*/, 1111 /*afgl*/},
    };
  } else if constexpr (S == Species::CarbonylFluoride) {  // COF2
    return {
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::O16},
               CA{Atom::F19},
               CA{Atom::F19},
           },
           65.991722 /*grams/mol*/, 1 /*gi*/, 269 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C13},
               CA{Atom::O16},
               CA{Atom::F19},
               CA{Atom::F19},
           },
           66.995083 /*grams/mol*/, 2 /*gi*/, 369 /*afgl*/},
    };
  } else if constexpr (S == Species::SulfurHexafluoride) {  // SF6
    return {
        IR{/*atoms=*/SA{
               CA{Atom::S32},
               CA{Atom::F19},
               CA{Atom::F19},
               CA{Atom::F19},
               CA{Atom::F19},
               CA{Atom::F19},
               CA{Atom::F19},
           },
           145.962492 /*grams/mol*/, 1 /*gi*/, 29 /*afgl*/},
    };
  } else if constexpr (S == Species::HydrogenSulfide) {  // H2S
    return {
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::S32},
           },
           33.987721 /*grams/mol*/, 1 /*gi*/, 121 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::S34},
           },
           35.983515 /*grams/mol*/, 1 /*gi*/, 141 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::S33},
           },
           34.987105 /*grams/mol*/, 4 /*gi*/, 131 /*afgl*/},
    };
  } else if constexpr (S == Species::FormicAcid) {  // HCOOH
    return {
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::C12},
               CA{Atom::O16},
               CA{Atom::O16},
               CA{Atom::H},
           },
           46.00548 /*grams/mol*/, 4 /*gi*/, 126 /*afgl*/},
    };
  } else if constexpr (S == Species::Hydroperoxyl) {  // HO2
    return {
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::O16},
               CA{Atom::O16},
           },
           32.997655 /*grams/mol*/, 2 /*gi*/, 166 /*afgl*/},
    };
  } else if constexpr (S == Species::OxygenAtom) {  // O
    return {
        IR{/*atoms=*/SA{
               CA{Atom::O16},
           },
           15.994915 /*grams/mol*/, 1 /*gi*/, 6 /*afgl*/},
    };
  } else if constexpr (S == Species::ChlorineNitrate) {  // ClONO2
    return {
        IR{/*atoms=*/SA{
               CA{Atom::Cl35},
               CA{Atom::O16},
               CA{Atom::N14},
               CA{Atom::O16},
               CA{Atom::O16},
           },
           96.956672 /*grams/mol*/, 12 /*gi*/, 5646 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::Cl37},
               CA{Atom::O16},
               CA{Atom::N14},
               CA{Atom::O16},
               CA{Atom::O16},
           },
           98.953723 /*grams/mol*/, 12 /*gi*/, 7646 /*afgl*/},
    };
  } else if constexpr (S == Species::NitricOxideCation) {  // NO+
    return {
        IR{/*atoms=*/SA{
               CA{Atom::N14},
               CA{Atom::O16, 1},
           },
           29.997989 /*grams/mol*/, 3 /*gi*/, 46 /*afgl*/},
    };
  } else if constexpr (S == Species::HypobromousAcid) {  // HOBr
    return {
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::O16},
               CA{Atom::Br79},
           },
           95.921076 /*grams/mol*/, 8 /*gi*/, 169 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::O16},
               CA{Atom::Br81},
           },
           97.919027 /*grams/mol*/, 8 /*gi*/, 161 /*afgl*/},
    };
  } else if constexpr (S == Species::Ethylene) {  // C2H4
    return {
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::C12},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
           },
           28.0313 /*grams/mol*/, 1 /*gi*/, 221 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::C13},
               CA{Atom::H},
               CA{Atom::H},
           },
           29.034655 /*grams/mol*/, 2 /*gi*/, 231 /*afgl*/},
    };
  } else if constexpr (S == Species::Methanol) {  // CH3OH
    return {
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::O16},
               CA{Atom::H},
           },
           32.026215 /*grams/mol*/, 2 /*gi*/, 2161 /*afgl*/},
    };
  } else if constexpr (S == Species::MethylBromide) {  // CH3Br
    return {
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::Br79},
           },
           93.941811 /*grams/mol*/, 4 /*gi*/, 219 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::Br81},
           },
           95.939764 /*grams/mol*/, 4 /*gi*/, 211 /*afgl*/},
    };
  } else if constexpr (S == Species::Acetonitrile) {  // CH3CN
    return {
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::H},
               CA{Atom::C12},
               CA{Atom::N14},
           },
           41.026549 /*grams/mol*/, 3 /*gi*/, 2124 /*afgl*/},
    };
  } else if constexpr (S == Species::Pfc14) {  // CF4
    return {
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::F19},
               CA{Atom::F19},
               CA{Atom::F19},
               CA{Atom::F19},
           },
           87.993616 /*grams/mol*/, 1 /*gi*/, 29 /*afgl*/},
    };
  } else if constexpr (S == Species::Diacetylene) {  // C4H2
    return {
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::C12},
               CA{Atom::C12},
               CA{Atom::C12},
               CA{Atom::H},
               CA{Atom::H},
           },
           50.01565 /*grams/mol*/, 1 /*gi*/, 2211 /*afgl*/},
    };
  } else if constexpr (S == Species::Cyanoacetylene) {  // HC3N
    return {
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::C12},
               CA{Atom::C12},
               CA{Atom::C12},
               CA{Atom::N14},
           },
           51.010899 /*grams/mol*/, 6 /*gi*/, 1224 /*afgl*/},
    };
  } else if constexpr (S == Species::Hydrogen) {  // H2
    return {
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::H},
           },
           2.01565 /*grams/mol*/, 1 /*gi*/, 11 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::H},
               CA{Atom::D},
           },
           3.021825 /*grams/mol*/, 6 /*gi*/, 12 /*afgl*/},
    };
  } else if constexpr (S == Species::CarbonMonosulfide) {  // CS
    return {
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::S32},
           },
           43.971036 /*grams/mol*/, 1 /*gi*/, 22 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::S34},
           },
           45.966787 /*grams/mol*/, 1 /*gi*/, 24 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C13},
               CA{Atom::S32},
           },
           44.974368 /*grams/mol*/, 2 /*gi*/, 32 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::S33},
           },
           44.970399 /*grams/mol*/, 4 /*gi*/, 23 /*afgl*/},
    };
  } else if constexpr (S == Species::SulfurTrioxide) {  // SO3
    return {
        IR{/*atoms=*/SA{
               CA{Atom::S32},
               CA{Atom::O16},
               CA{Atom::O16},
               CA{Atom::O16},
           },
           79.95682 /*grams/mol*/, 1 /*gi*/, 26 /*afgl*/},
    };
  } else if constexpr (S == Species::Cyanogen) {  // C2N2
    return {
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::C12},
               CA{Atom::N14},
               CA{Atom::N14},
           },
           52.006148 /*grams/mol*/, 1 /*gi*/, 4224 /*afgl*/},
    };
  } else if constexpr (S == Species::Phosgene) {  // COCl2
    return {
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::O16},
               CA{Atom::Cl35},
               CA{Atom::Cl35},
           },
           97.9326199796 /*grams/mol*/, 1 /*gi*/, 2655 /*afgl*/},
        IR{/*atoms=*/SA{
               CA{Atom::C12},
               CA{Atom::O16},
               CA{Atom::Cl35},
               CA{Atom::Cl37},
           },
           99.9296698896 /*grams/mol*/, 16 /*gi*/, 2657 /*afgl*/},
    };
  } else {
    return {};
  }
}

template <Species S>
constexpr std::array<Quantum::Type, getGlobalQuantumNumberCount(S)>
globalQuantumNumberTypesRecord() {
  if constexpr (S == Species::Water) {  // H2O
    return {Quantum::Type::v1, Quantum::Type::v2, Quantum::Type::v3,
            Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::CarbonDioxide) {  // CO2
    return {Quantum::Type::v1,          Quantum::Type::v2,
            Quantum::Type::v3,          Quantum::Type::l2,
            Quantum::Type::r,           Quantum::Type::ElecStateLabel,
            Quantum::Type::kronigParity};
  } else if constexpr (S == Species::Ozone) {  // O3
    return {Quantum::Type::v1, Quantum::Type::v2, Quantum::Type::v3,
            Quantum::Type::r, Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::NitrogenOxide) {  // N2O
    return {Quantum::Type::v1,
            Quantum::Type::v2,
            Quantum::Type::v3,
            Quantum::Type::l2,
            Quantum::Type::ElecStateLabel,
            Quantum::Type::kronigParity};
  } else if constexpr (S == Species::CarbonMonoxide) {  // CO
    return {Quantum::Type::v, Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Methane) {  // CH4
    return {Quantum::Type::v1,       Quantum::Type::v2,
            Quantum::Type::v3,       Quantum::Type::v4,
            Quantum::Type::v5,       Quantum::Type::v6,
            Quantum::Type::rovibSym, Quantum::Type::n,
            Quantum::Type::alpha,    Quantum::Type::ElecStateLabel,
            Quantum::Type::vibSym};
  } else if constexpr (S == Species::Oxygen) {  // O2
    return {Quantum::Type::S, Quantum::Type::v, Quantum::Type::Lambda,
            Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::NitricOxide) {  // NO
    return {Quantum::Type::S,
            Quantum::Type::v,
            Quantum::Type::Lambda,
            Quantum::Type::ElecStateLabel,
            Quantum::Type::kronigParity,
            Quantum::Type::parity};
  } else if constexpr (S == Species::SulfurDioxide) {  // SO2
    return {Quantum::Type::v1, Quantum::Type::v2, Quantum::Type::v3,
            Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::NitrogenDioxide) {  // NO2
    return {Quantum::Type::S, Quantum::Type::v1, Quantum::Type::v2,
            Quantum::Type::v3, Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Ammonia) {  // NH3
    return {Quantum::Type::v1,
            Quantum::Type::v2,
            Quantum::Type::v3,
            Quantum::Type::v4,
            Quantum::Type::l,
            Quantum::Type::l3,
            Quantum::Type::l4,
            Quantum::Type::rovibSym,
            Quantum::Type::ElecStateLabel,
            Quantum::Type::vibSym,
            Quantum::Type::vibInv,
            Quantum::Type::rotSym};
  } else if constexpr (S == Species::NitricAcid) {  // HNO3
    return {Quantum::Type::v1, Quantum::Type::v2,
            Quantum::Type::v3, Quantum::Type::v4,
            Quantum::Type::v5, Quantum::Type::v6,
            Quantum::Type::v7, Quantum::Type::v8,
            Quantum::Type::v9, Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Hydroxyl) {  // OH
    return {Quantum::Type::S,
            Quantum::Type::v,
            Quantum::Type::Lambda,
            Quantum::Type::ElecStateLabel,
            Quantum::Type::kronigParity,
            Quantum::Type::parity};
  } else if constexpr (S == Species::HydrogenFluoride) {  // HF
    return {Quantum::Type::v, Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::HydrogenChloride) {  // HCl
    return {Quantum::Type::v, Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::HydrogenBromide) {  // HBr
    return {Quantum::Type::v, Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::HydrogenIodide) {  // HI
    return {Quantum::Type::v, Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::ChlorineMonoxide) {  // ClO
    return {Quantum::Type::S,
            Quantum::Type::v,
            Quantum::Type::Lambda,
            Quantum::Type::ElecStateLabel,
            Quantum::Type::kronigParity,
            Quantum::Type::parity};
  } else if constexpr (S == Species::CarbonylSulfide) {  // OCS
    return {Quantum::Type::v1,
            Quantum::Type::v2,
            Quantum::Type::v3,
            Quantum::Type::l2,
            Quantum::Type::ElecStateLabel,
            Quantum::Type::kronigParity};
  } else if constexpr (S == Species::Formaldehyde) {  // H2CO
    return {Quantum::Type::v1,
            Quantum::Type::v2,
            Quantum::Type::v3,
            Quantum::Type::v4,
            Quantum::Type::v5,
            Quantum::Type::v6,
            Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::HypochlorousAcid) {  // HOCl
    return {Quantum::Type::v1, Quantum::Type::v2, Quantum::Type::v3,
            Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Nitrogen) {  // N2
    return {Quantum::Type::v, Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::HydrogenCyanide) {  // HCN
    return {Quantum::Type::v1,
            Quantum::Type::v2,
            Quantum::Type::v3,
            Quantum::Type::l2,
            Quantum::Type::ElecStateLabel,
            Quantum::Type::kronigParity};
  } else if constexpr (S == Species::MethylChloride) {  // CH3Cl
    return {Quantum::Type::v1,
            Quantum::Type::v2,
            Quantum::Type::v3,
            Quantum::Type::v4,
            Quantum::Type::v5,
            Quantum::Type::v6,
            Quantum::Type::l,
            Quantum::Type::rovibSym,
            Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::HydrogenPeroxide) {  // H2O2
    return {Quantum::Type::v1,  Quantum::Type::v2,
            Quantum::Type::v3,  Quantum::Type::v5,
            Quantum::Type::v6,  Quantum::Type::n,
            Quantum::Type::tau, Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Acetylene) {  // C2H2
    return {Quantum::Type::v1,
            Quantum::Type::v2,
            Quantum::Type::v3,
            Quantum::Type::v4,
            Quantum::Type::v5,
            Quantum::Type::l,
            Quantum::Type::r,
            Quantum::Type::ElecStateLabel,
            Quantum::Type::kronigParity,
            Quantum::Type::parity,
            Quantum::Type::vibInv,
            Quantum::Type::vibRefl};
  } else if constexpr (S == Species::Ethane) {  // C2H6
    return {Quantum::Type::v4,
            Quantum::Type::v6,
            Quantum::Type::v7,
            Quantum::Type::v8,
            Quantum::Type::v9,
            Quantum::Type::v12,
            Quantum::Type::l,
            Quantum::Type::rovibSym,
            Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Phosphine) {  // PH3
    return {Quantum::Type::v1,    Quantum::Type::v2,
            Quantum::Type::v3,    Quantum::Type::v4,
            Quantum::Type::l,     Quantum::Type::ElecStateLabel,
            Quantum::Type::vibSym};
  } else if constexpr (S == Species::CarbonylFluoride) {  // COF2
    return {Quantum::Type::v1,
            Quantum::Type::v2,
            Quantum::Type::v3,
            Quantum::Type::v4,
            Quantum::Type::v5,
            Quantum::Type::v6,
            Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::SulfurHexafluoride) {  // SF6
    return {};
  } else if constexpr (S == Species::HydrogenSulfide) {  // H2S
    return {Quantum::Type::v1, Quantum::Type::v2, Quantum::Type::v3,
            Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::FormicAcid) {  // HCOOH
    return {Quantum::Type::v1, Quantum::Type::v2,
            Quantum::Type::v3, Quantum::Type::v4,
            Quantum::Type::v5, Quantum::Type::v6,
            Quantum::Type::v7, Quantum::Type::v8,
            Quantum::Type::v9, Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Hydroperoxyl) {  // HO2
    return {Quantum::Type::S, Quantum::Type::v1, Quantum::Type::v2,
            Quantum::Type::v3, Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::OxygenAtom) {  // O
    return {Quantum::Type::S, Quantum::Type::config, Quantum::Type::term};
  } else if constexpr (S == Species::ChlorineNitrate) {  // ClONO2
    return {};
  } else if constexpr (S == Species::NitricOxideCation) {  // NO+
    return {Quantum::Type::v, Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::HypobromousAcid) {  // HOBr
    return {Quantum::Type::v1, Quantum::Type::v2, Quantum::Type::v3,
            Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Ethylene) {  // C2H4
    return {Quantum::Type::v2,  Quantum::Type::v4,
            Quantum::Type::v6,  Quantum::Type::v7,
            Quantum::Type::v8,  Quantum::Type::v9,
            Quantum::Type::v10, Quantum::Type::v11,
            Quantum::Type::v12, Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Methanol) {  // CH3OH
    return {Quantum::Type::v1,
            Quantum::Type::v2,
            Quantum::Type::v3,
            Quantum::Type::v4,
            Quantum::Type::v5,
            Quantum::Type::v6,
            Quantum::Type::v7,
            Quantum::Type::v8,
            Quantum::Type::v12,
            Quantum::Type::rovibSym,
            Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::MethylBromide) {  // CH3Br
    return {Quantum::Type::v2,       Quantum::Type::v5,
            Quantum::Type::v6,       Quantum::Type::l,
            Quantum::Type::rovibSym, Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Acetonitrile) {  // CH3CN
    return {Quantum::Type::v4, Quantum::Type::rovibSym,
            Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Pfc14) {  // CF4
    return {};
  } else if constexpr (S == Species::Diacetylene) {  // C4H2
    return {Quantum::Type::v1,           Quantum::Type::v2,
            Quantum::Type::v3,           Quantum::Type::v4,
            Quantum::Type::v5,           Quantum::Type::v6,
            Quantum::Type::v7,           Quantum::Type::v8,
            Quantum::Type::v9,           Quantum::Type::l6,
            Quantum::Type::l7,           Quantum::Type::l8,
            Quantum::Type::l9,           Quantum::Type::ElecStateLabel,
            Quantum::Type::kronigParity, Quantum::Type::parity,
            Quantum::Type::vibInv};
  } else if constexpr (S == Species::Cyanoacetylene) {  // HC3N
    return {Quantum::Type::v1,
            Quantum::Type::v2,
            Quantum::Type::v3,
            Quantum::Type::v4,
            Quantum::Type::v5,
            Quantum::Type::v6,
            Quantum::Type::v7,
            Quantum::Type::l5,
            Quantum::Type::l6,
            Quantum::Type::l7,
            Quantum::Type::ElecStateLabel,
            Quantum::Type::kronigParity,
            Quantum::Type::parity};
  } else if constexpr (S == Species::Hydrogen) {  // H2
    return {Quantum::Type::v, Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::CarbonMonosulfide) {  // CS
    return {Quantum::Type::v, Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::SulfurTrioxide) {  // SO3
    return {Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Cyanogen) {  // C2N2
    return {Quantum::Type::v1,
            Quantum::Type::v2,
            Quantum::Type::v3,
            Quantum::Type::v4,
            Quantum::Type::v5,
            Quantum::Type::l,
            Quantum::Type::l2,
            Quantum::Type::r,
            Quantum::Type::ElecStateLabel,
            Quantum::Type::kronigParity,
            Quantum::Type::parity};
  } else if constexpr (S == Species::Phosgene) {  // COCl2
    return {Quantum::Type::v1,
            Quantum::Type::v2,
            Quantum::Type::v3,
            Quantum::Type::v4,
            Quantum::Type::v5,
            Quantum::Type::v6,
            Quantum::Type::l,
            Quantum::Type::l2,
            Quantum::Type::r,
            Quantum::Type::ElecStateLabel,
            Quantum::Type::kronigParity,
            Quantum::Type::parity};
  } else {
    return {};
  }
}

template <Species S>
constexpr std::array<Quantum::Type, getLocalQuantumNumberCount(S)>
localQuantumNumberTypesRecord() {
  if constexpr (S == Species::Water) {  // H2O
    return {Quantum::Type::J, Quantum::Type::Ka, Quantum::Type::Kc};
  } else if constexpr (S == Species::CarbonDioxide) {  // CO2
    return {Quantum::Type::J, Quantum::Type::F};
  } else if constexpr (S == Species::Ozone) {  // O3
    return {Quantum::Type::J, Quantum::Type::F, Quantum::Type::Ka,
            Quantum::Type::Kc};
  } else if constexpr (S == Species::NitrogenOxide) {  // N2O
    return {Quantum::Type::J};
  } else if constexpr (S == Species::CarbonMonoxide) {  // CO
    return {Quantum::Type::J};
  } else if constexpr (S == Species::Methane) {  // CH4
    return {Quantum::Type::J, Quantum::Type::K};
  } else if constexpr (S == Species::Oxygen) {  // O2
    return {Quantum::Type::J, Quantum::Type::N, Quantum::Type::F};
  } else if constexpr (S == Species::NitricOxide) {  // NO
    return {Quantum::Type::J, Quantum::Type::F, Quantum::Type::Omega};
  } else if constexpr (S == Species::SulfurDioxide) {  // SO2
    return {Quantum::Type::J, Quantum::Type::Ka, Quantum::Type::Kc};
  } else if constexpr (S == Species::NitrogenDioxide) {  // NO2
    return {Quantum::Type::J, Quantum::Type::N, Quantum::Type::F,
            Quantum::Type::Ka, Quantum::Type::Kc};
  } else if constexpr (S == Species::Ammonia) {  // NH3
    return {Quantum::Type::J, Quantum::Type::K};
  } else if constexpr (S == Species::NitricAcid) {  // HNO3
    return {Quantum::Type::J, Quantum::Type::F, Quantum::Type::Ka,
            Quantum::Type::Kc};
  } else if constexpr (S == Species::Hydroxyl) {  // OH
    return {Quantum::Type::J, Quantum::Type::N, Quantum::Type::F,
            Quantum::Type::Omega};
  } else if constexpr (S == Species::HydrogenFluoride) {  // HF
    return {Quantum::Type::J};
  } else if constexpr (S == Species::HydrogenChloride) {  // HCl
    return {Quantum::Type::J, Quantum::Type::F};
  } else if constexpr (S == Species::HydrogenBromide) {  // HBr
    return {Quantum::Type::J, Quantum::Type::F};
  } else if constexpr (S == Species::HydrogenIodide) {  // HI
    return {Quantum::Type::J, Quantum::Type::F};
  } else if constexpr (S == Species::ChlorineMonoxide) {  // ClO
    return {Quantum::Type::J, Quantum::Type::F, Quantum::Type::Omega};
  } else if constexpr (S == Species::CarbonylSulfide) {  // OCS
    return {Quantum::Type::J};
  } else if constexpr (S == Species::Formaldehyde) {  // H2CO
    return {Quantum::Type::J, Quantum::Type::Ka, Quantum::Type::Kc};
  } else if constexpr (S == Species::HypochlorousAcid) {  // HOCl
    return {Quantum::Type::J, Quantum::Type::Ka, Quantum::Type::Kc};
  } else if constexpr (S == Species::Nitrogen) {  // N2
    return {Quantum::Type::J};
  } else if constexpr (S == Species::HydrogenCyanide) {  // HCN
    return {Quantum::Type::J, Quantum::Type::F};
  } else if constexpr (S == Species::MethylChloride) {  // CH3Cl
    return {Quantum::Type::J, Quantum::Type::F, Quantum::Type::K};
  } else if constexpr (S == Species::HydrogenPeroxide) {  // H2O2
    return {Quantum::Type::J, Quantum::Type::Ka, Quantum::Type::Kc};
  } else if constexpr (S == Species::Acetylene) {  // C2H2
    return {Quantum::Type::J};
  } else if constexpr (S == Species::Ethane) {  // C2H6
    return {Quantum::Type::J, Quantum::Type::K};
  } else if constexpr (S == Species::Phosphine) {  // PH3
    return {Quantum::Type::J, Quantum::Type::K};
  } else if constexpr (S == Species::CarbonylFluoride) {  // COF2
    return {Quantum::Type::J, Quantum::Type::Ka, Quantum::Type::Kc};
  } else if constexpr (S == Species::SulfurHexafluoride) {  // SF6
    return {};
  } else if constexpr (S == Species::HydrogenSulfide) {  // H2S
    return {Quantum::Type::J, Quantum::Type::Ka, Quantum::Type::Kc};
  } else if constexpr (S == Species::FormicAcid) {  // HCOOH
    return {Quantum::Type::J, Quantum::Type::Ka, Quantum::Type::Kc};
  } else if constexpr (S == Species::Hydroperoxyl) {  // HO2
    return {Quantum::Type::J, Quantum::Type::N, Quantum::Type::F,
            Quantum::Type::Ka, Quantum::Type::Kc};
  } else if constexpr (S == Species::OxygenAtom) {  // O
    return {Quantum::Type::J, Quantum::Type::L};
  } else if constexpr (S == Species::ChlorineNitrate) {  // ClONO2
    return {};
  } else if constexpr (S == Species::NitricOxideCation) {  // NO+
    return {Quantum::Type::J};
  } else if constexpr (S == Species::HypobromousAcid) {  // HOBr
    return {Quantum::Type::J, Quantum::Type::Ka, Quantum::Type::Kc};
  } else if constexpr (S == Species::Ethylene) {  // C2H4
    return {Quantum::Type::J, Quantum::Type::Ka, Quantum::Type::Kc};
  } else if constexpr (S == Species::Methanol) {  // CH3OH
    return {Quantum::Type::J, Quantum::Type::K};
  } else if constexpr (S == Species::MethylBromide) {  // CH3Br
    return {Quantum::Type::J, Quantum::Type::K};
  } else if constexpr (S == Species::Acetonitrile) {  // CH3CN
    return {Quantum::Type::J, Quantum::Type::K};
  } else if constexpr (S == Species::Pfc14) {  // CF4
    return {};
  } else if constexpr (S == Species::Diacetylene) {  // C4H2
    return {Quantum::Type::J};
  } else if constexpr (S == Species::Cyanoacetylene) {  // HC3N
    return {Quantum::Type::J};
  } else if constexpr (S == Species::Hydrogen) {  // H2
    return {Quantum::Type::J};
  } else if constexpr (S == Species::CarbonMonosulfide) {  // CS
    return {Quantum::Type::J};
  } else if constexpr (S == Species::SulfurTrioxide) {  // SO3
    return {Quantum::Type::Ka, Quantum::Type::Kc};
  } else if constexpr (S == Species::Cyanogen) {  // C2N2
    return {Quantum::Type::J};
  } else if constexpr (S == Species::Phosgene) {  // COCl2
    return {Quantum::Type::J, Quantum::Type::Ka, Quantum::Type::Kc};
  } else {
    return {};
  }
}

template <Species S>
struct IsotopologueRecord {
  static constexpr unsigned char N = getIsotopologueCount(S);
  static constexpr unsigned char G = getGlobalQuantumNumberCount(S);
  static constexpr unsigned char L = getLocalQuantumNumberCount(S);
  std::array<IsotopeRecord<S>, N> isot;
  std::array<Quantum::Type, G> global;
  std::array<Quantum::Type, L> local;
  constexpr IsotopologueRecord() noexcept
      : isot(isotopeRecords<S>()),
        global(globalQuantumNumberTypesRecord<S>()),
        local(localQuantumNumberTypesRecord<S>()) {}
};

template <size_t N>
constexpr bool check_quantumnumbers(const std::array<Quantum::Type, N>& data) {
  if constexpr (N > 1) {
    for (size_t i = 0; i < N - 1; i++) {
      if (long(data[i]) >= long(data[i + 1])) {
        return false;
      }
    }
  }
  return true;
}

template <Species S, size_t N>
constexpr bool check_unique_afgl(const std::array<IsotopeRecord<S>, N>& data) {
  if constexpr (N > 1) {
    for (size_t i = 0; i < N; i++) {
      for (size_t j = 0; j < N; j++) {
        if (j not_eq i and data[i].afgl == data[j].afgl) {
          return false;
        }
      }
    }
  }
  return true;
}

template <Species S, size_t N>
constexpr bool check_unique_atoms(const std::array<IsotopeRecord<S>, N>& data) {
  if constexpr (N > 1) {
    for (size_t i = 0; i < N; i++) {
      for (size_t j = 0; j < N; j++) {
        if (j not_eq i) {
          size_t matches = 0;
          for (size_t k = 0; k < data[i].atoms.size(); k++) {
            matches += data[i].atoms[k] == data[j].atoms[k];
          }
          if (matches == data[i].atoms.size()) return false;
        }
      }
    }
  }
  return true;
}

template <Species S, size_t N>
constexpr bool check_positive_gi(const std::array<IsotopeRecord<S>, N>& data) {
  if constexpr (N > 1) {
    for (size_t i = 0; i < N; i++) {
      if (data[i].gi < 1) {
        return false;
      }
    }
  }
  return true;
}

/** A class of static isotope data */
template <Species S>
class Isotopologue {
  static constexpr unsigned char NI = getIsotopologueCount(S);
  static constexpr unsigned char NA = getAtomCount(S);
  static constexpr unsigned char NG = getGlobalQuantumNumberCount(S);
  static constexpr unsigned char NL = getLocalQuantumNumberCount(S);
  static constexpr IsotopologueRecord<S> data{};
  static_assert(check_quantumnumbers(data.local),
                "Error, quantum numbers must be in increasing order (you might "
                "be missing a number)");
  static_assert(check_quantumnumbers(data.global),
                "Error, quantum numbers must be in increasing order (you might "
                "be missing a number)");
  static_assert(check_unique_afgl(data.isot), "Error, AFGL numbers are copies");
  static_assert(check_positive_gi(data.isot), "Error, non-positive gi");
  static_assert(check_unique_atoms(data.isot),
                "Error, non-unique atom configuration");

 public:
  constexpr Isotopologue() noexcept {}

  constexpr double Mass(long i) const noexcept { return data.isot[i].mass; }
  constexpr long G(long i) const noexcept { return data.isot[i].gi; }
  std::vector<ChargedAtom> Atoms(long i) const noexcept {
    return {data.isot[i].atoms.cbegin(), data.isot[i].atoms.cend()};
  }
  std::vector<Quantum::Type> globalQuantumNumbers() const noexcept {
    return {data.global.cbegin(), data.global.cend()};
  }
  std::vector<Quantum::Type> localQuantumNumbers() const noexcept {
    return {data.local.cbegin(), data.local.cend()};
  }

  constexpr unsigned char findNum(
      std::array<ChargedAtom, NA> atoms) const noexcept {
    // For all defined isotopes
    for (unsigned char i = 0; i < NI; i++) {
      // a list saying we haven't seen the atom so far
      std::array<bool, NA> found{0};
      for (unsigned char j = 0; j < NA; j++) {
        found[j] = false;
      }

      for (unsigned char j = 0; j < NA; j++) {
        if (found[j]) continue;  // Guard against finding the same atom twice
        if (atoms[j] == data.isot[i].atoms[j]) found[j] = true;
      }

      // Return if "all_of" (which is not constexpr before C++20)
      bool all = true;
      for (bool here : found) {
        all = all and here;
      }
      if (all) return i;
    }

    // Placeholder for out of range
    return NI;
  }

  unsigned char findNum(const std::vector<ChargedAtom>& atoms) const noexcept {
    std::array<ChargedAtom, NA> a;
    for (unsigned char i = 0; i < NA; i++) a[i] = atoms[i];
    return findNum(a);
  }

  template <typename... A>
  constexpr unsigned char findNum(A... atoms) const noexcept {
    // FIXME: Make consteval when possible
    static_assert(sizeof...(A) == NA, "Wrong number of atoms");
    return findNum(std::array<ChargedAtom, NA>{ChargedAtom(atoms)...});
  }

  std::string Name() const noexcept { return toString(S); }
  std::string ShortName() const noexcept { return toShortName(S); }

  bool set_local(std::vector<Quantum::Number>& v, const Quantum::Number& n,
                 Quantum::Type t) noexcept {
    for (size_t i = 0; i < data.local.size(); i++) {
      if (data.local[i] == t) {
        v[i] = n;
        return true;
      }
    }
    return false;
  }

  bool set_global(std::vector<Quantum::Number>& v, const Quantum::Number& n,
                  Quantum::Type t) noexcept {
    for (size_t i = 0; i < data.global.size(); i++) {
      if (data.global[i] == t) {
        v[i] = n;
        return true;
      }
    }
    return false;
  }

  constexpr size_t get_local_pos(Quantum::Type t) const noexcept {
    for (size_t i = 0; i < data.local.size(); i++) {
      if (data.local[i] == t) {
        return i;
      }
    }
    return std::numeric_limits<size_t>::max();
  }

  constexpr size_t get_global_pos(Quantum::Type t) const noexcept {
    for (size_t i = 0; i < data.global.size(); i++) {
      if (data.global[i] == t) {
        return i;
      }
    }
    return std::numeric_limits<size_t>::max();
  }

  constexpr double QT(double T, unsigned char num) const noexcept {
    if constexpr (S == Species::Water) {
      if (data.isot[num].afgl == 161) {
        return PartitionFunction::compute_partfun_H2O161(T);
      } else if (data.isot[num].afgl == 181) {
        return PartitionFunction::compute_partfun_H2O181(T);
      } else if (data.isot[num].afgl == 171) {
        return PartitionFunction::compute_partfun_H2O171(T);
      } else if (data.isot[num].afgl == 162) {
        return PartitionFunction::compute_partfun_H2O162(T);
      } else if (data.isot[num].afgl == 182) {
        return PartitionFunction::compute_partfun_H2O182(T);
      } else if (data.isot[num].afgl == 172) {
        return PartitionFunction::compute_partfun_H2O172(T);
      } else if (data.isot[num].afgl == 262) {
        return PartitionFunction::compute_partfun_H2O262(T);
      } else if (data.isot[num].afgl == 282) {
        return PartitionFunction::compute_partfun_H2O282(T);
      } else if (data.isot[num].afgl == 272) {
        return PartitionFunction::compute_partfun_H2O272(T);
      }
    } else if constexpr (S == Species::CarbonDioxide) {
      if (data.isot[num].afgl == 626) {
        return PartitionFunction::compute_partfun_CO2626(T);
      } else if (data.isot[num].afgl == 636) {
        return PartitionFunction::compute_partfun_CO2636(T);
      } else if (data.isot[num].afgl == 628) {
        return PartitionFunction::compute_partfun_CO2628(T);
      } else if (data.isot[num].afgl == 627) {
        return PartitionFunction::compute_partfun_CO2627(T);
      } else if (data.isot[num].afgl == 638) {
        return PartitionFunction::compute_partfun_CO2638(T);
      } else if (data.isot[num].afgl == 637) {
        return PartitionFunction::compute_partfun_CO2637(T);
      } else if (data.isot[num].afgl == 828) {
        return PartitionFunction::compute_partfun_CO2828(T);
      } else if (data.isot[num].afgl == 728) {
        return PartitionFunction::compute_partfun_CO2728(T);
      } else if (data.isot[num].afgl == 727) {
        return PartitionFunction::compute_partfun_CO2727(T);
      } else if (data.isot[num].afgl == 838) {
        return PartitionFunction::compute_partfun_CO2838(T);
      } else if (data.isot[num].afgl == 837) {
        return PartitionFunction::compute_partfun_CO2837(T);
      } else if (data.isot[num].afgl == 737) {
        return PartitionFunction::compute_partfun_CO2737(T);
      } else if (data.isot[num].afgl == 646) {
        return PartitionFunction::compute_partfun_CO2646(T);
      }
    } else if constexpr (S == Species::Ozone) {
      if (data.isot[num].afgl == 666) {
        return PartitionFunction::compute_partfun_O3666(T);
      } else if (data.isot[num].afgl == 668) {
        return PartitionFunction::compute_partfun_O3668(T);
      } else if (data.isot[num].afgl == 686) {
        return PartitionFunction::compute_partfun_O3686(T);
      } else if (data.isot[num].afgl == 667) {
        return PartitionFunction::compute_partfun_O3667(T);
      } else if (data.isot[num].afgl == 676) {
        return PartitionFunction::compute_partfun_O3676(T);
      } else if (data.isot[num].afgl == 886) {
        return PartitionFunction::compute_partfun_O3886(T);
      } else if (data.isot[num].afgl == 868) {
        return PartitionFunction::compute_partfun_O3868(T);
      } else if (data.isot[num].afgl == 678) {
        return PartitionFunction::compute_partfun_O3678(T);
      } else if (data.isot[num].afgl == 768) {
        return PartitionFunction::compute_partfun_O3768(T);
      } else if (data.isot[num].afgl == 786) {
        return PartitionFunction::compute_partfun_O3786(T);
      } else if (data.isot[num].afgl == 776) {
        return PartitionFunction::compute_partfun_O3776(T);
      } else if (data.isot[num].afgl == 767) {
        return PartitionFunction::compute_partfun_O3767(T);
      } else if (data.isot[num].afgl == 888) {
        return PartitionFunction::compute_partfun_O3888(T);
      } else if (data.isot[num].afgl == 887) {
        return PartitionFunction::compute_partfun_O3887(T);
      } else if (data.isot[num].afgl == 878) {
        return PartitionFunction::compute_partfun_O3878(T);
      } else if (data.isot[num].afgl == 778) {
        return PartitionFunction::compute_partfun_O3778(T);
      } else if (data.isot[num].afgl == 787) {
        return PartitionFunction::compute_partfun_O3787(T);
      } else if (data.isot[num].afgl == 777) {
        return PartitionFunction::compute_partfun_O3777(T);
      }
    } else if constexpr (S == Species::NitrogenOxide) {
      if (data.isot[num].afgl == 446) {
        return PartitionFunction::compute_partfun_N2O446(T);
      } else if (data.isot[num].afgl == 456) {
        return PartitionFunction::compute_partfun_N2O456(T);
      } else if (data.isot[num].afgl == 546) {
        return PartitionFunction::compute_partfun_N2O546(T);
      } else if (data.isot[num].afgl == 448) {
        return PartitionFunction::compute_partfun_N2O448(T);
      } else if (data.isot[num].afgl == 447) {
        return PartitionFunction::compute_partfun_N2O447(T);
      }
    } else if constexpr (S == Species::CarbonMonoxide) {
      if (data.isot[num].afgl == 26) {
        return PartitionFunction::compute_partfun_CO26(T);
      } else if (data.isot[num].afgl == 36) {
        return PartitionFunction::compute_partfun_CO36(T);
      } else if (data.isot[num].afgl == 28) {
        return PartitionFunction::compute_partfun_CO28(T);
      } else if (data.isot[num].afgl == 27) {
        return PartitionFunction::compute_partfun_CO27(T);
      } else if (data.isot[num].afgl == 38) {
        return PartitionFunction::compute_partfun_CO38(T);
      } else if (data.isot[num].afgl == 37) {
        return PartitionFunction::compute_partfun_CO37(T);
      } else if (data.isot[num].afgl == 46) {
        return PartitionFunction::compute_partfun_CO46(T);
      } else if (data.isot[num].afgl == 48) {
        return PartitionFunction::compute_partfun_CO48(T);
      } else if (data.isot[num].afgl == 47) {
        return PartitionFunction::compute_partfun_CO47(T);
      }
    } else if constexpr (S == Species::Methane) {
      if (data.isot[num].afgl == 211) {
        return PartitionFunction::compute_partfun_CH4211(T);
      } else if (data.isot[num].afgl == 311) {
        return PartitionFunction::compute_partfun_CH4311(T);
      } else if (data.isot[num].afgl == 212) {
        return PartitionFunction::compute_partfun_CH4212(T);
      } else if (data.isot[num].afgl == 312) {
        return PartitionFunction::compute_partfun_CH4312(T);
      }
    } else if constexpr (S == Species::Oxygen) {
      if (data.isot[num].afgl == 66) {
        return PartitionFunction::compute_partfun_O266(T);
      } else if (data.isot[num].afgl == 68) {
        return PartitionFunction::compute_partfun_O268(T);
      } else if (data.isot[num].afgl == 67) {
        return PartitionFunction::compute_partfun_O267(T);
      } else if (data.isot[num].afgl == 88) {
        return PartitionFunction::compute_partfun_O288(T);
      } else if (data.isot[num].afgl == 87) {
        return PartitionFunction::compute_partfun_O287(T);
      } else if (data.isot[num].afgl == 77) {
        return PartitionFunction::compute_partfun_O277(T);
      }
    } else if constexpr (S == Species::NitricOxide) {
      if (data.isot[num].afgl == 46) {
        return PartitionFunction::compute_partfun_NO46(T);
      } else if (data.isot[num].afgl == 56) {
        return PartitionFunction::compute_partfun_NO56(T);
      } else if (data.isot[num].afgl == 48) {
        return PartitionFunction::compute_partfun_NO48(T);
      }
    } else if constexpr (S == Species::SulfurDioxide) {
      if (data.isot[num].afgl == 626) {
        return PartitionFunction::compute_partfun_SO2626(T);
      } else if (data.isot[num].afgl == 646) {
        return PartitionFunction::compute_partfun_SO2646(T);
      }
    } else if constexpr (S == Species::NitrogenDioxide) {
      if (data.isot[num].afgl == 646) {
        return PartitionFunction::compute_partfun_NO2646(T);
      }
    } else if constexpr (S == Species::Ammonia) {
      if (data.isot[num].afgl == 4111) {
        return PartitionFunction::compute_partfun_NH34111(T);
      } else if (data.isot[num].afgl == 5111) {
        return PartitionFunction::compute_partfun_NH35111(T);
      }
    } else if constexpr (S == Species::NitricAcid) {
      if (data.isot[num].afgl == 146) {
        return PartitionFunction::compute_partfun_HNO3146(T);
      } else if (data.isot[num].afgl == 156) {
        return PartitionFunction::compute_partfun_HNO3156(T);
      }
    } else if constexpr (S == Species::Hydroxyl) {
      if (data.isot[num].afgl == 61) {
        return PartitionFunction::compute_partfun_OH61(T);
      } else if (data.isot[num].afgl == 81) {
        return PartitionFunction::compute_partfun_OH81(T);
      } else if (data.isot[num].afgl == 62) {
        return PartitionFunction::compute_partfun_OH62(T);
      }
    } else if constexpr (S == Species::HydrogenFluoride) {
      if (data.isot[num].afgl == 19) {
        return PartitionFunction::compute_partfun_HF19(T);
      } else if (data.isot[num].afgl == 29) {
        return PartitionFunction::compute_partfun_HF29(T);
      }
    } else if constexpr (S == Species::HydrogenChloride) {
      if (data.isot[num].afgl == 15) {
        return PartitionFunction::compute_partfun_HCl15(T);
      } else if (data.isot[num].afgl == 17) {
        return PartitionFunction::compute_partfun_HCl17(T);
      } else if (data.isot[num].afgl == 25) {
        return PartitionFunction::compute_partfun_HCl25(T);
      } else if (data.isot[num].afgl == 27) {
        return PartitionFunction::compute_partfun_HCl27(T);
      }
    } else if constexpr (S == Species::HydrogenBromide) {
      if (data.isot[num].afgl == 19) {
        return PartitionFunction::compute_partfun_HBr19(T);
      } else if (data.isot[num].afgl == 11) {
        return PartitionFunction::compute_partfun_HBr11(T);
      } else if (data.isot[num].afgl == 29) {
        return PartitionFunction::compute_partfun_HBr29(T);
      } else if (data.isot[num].afgl == 21) {
        return PartitionFunction::compute_partfun_HBr21(T);
      }
    } else if constexpr (S == Species::HydrogenIodide) {
      if (data.isot[num].afgl == 17) {
        return PartitionFunction::compute_partfun_HI17(T);
      } else if (data.isot[num].afgl == 27) {
        return PartitionFunction::compute_partfun_HI27(T);
      }
    } else if constexpr (S == Species::ChlorineMonoxide) {
      if (data.isot[num].afgl == 56) {
        return PartitionFunction::compute_partfun_ClO56(T);
      } else if (data.isot[num].afgl == 76) {
        return PartitionFunction::compute_partfun_ClO76(T);
      }
    } else if constexpr (S == Species::CarbonylSulfide) {
      if (data.isot[num].afgl == 622) {
        return PartitionFunction::compute_partfun_OCS622(T);
      } else if (data.isot[num].afgl == 624) {
        return PartitionFunction::compute_partfun_OCS624(T);
      } else if (data.isot[num].afgl == 632) {
        return PartitionFunction::compute_partfun_OCS632(T);
      } else if (data.isot[num].afgl == 623) {
        return PartitionFunction::compute_partfun_OCS623(T);
      } else if (data.isot[num].afgl == 822) {
        return PartitionFunction::compute_partfun_OCS822(T);
      }
    } else if constexpr (S == Species::Formaldehyde) {
      if (data.isot[num].afgl == 126) {
        return PartitionFunction::compute_partfun_H2CO126(T);
      } else if (data.isot[num].afgl == 136) {
        return PartitionFunction::compute_partfun_H2CO136(T);
      } else if (data.isot[num].afgl == 128) {
        return PartitionFunction::compute_partfun_H2CO128(T);
      }
    } else if constexpr (S == Species::HypochlorousAcid) {
      if (data.isot[num].afgl == 165) {
        return PartitionFunction::compute_partfun_HOCl165(T);
      } else if (data.isot[num].afgl == 167) {
        return PartitionFunction::compute_partfun_HOCl167(T);
      }
    } else if constexpr (S == Species::Nitrogen) {
      if (data.isot[num].afgl == 44) {
        return PartitionFunction::compute_partfun_N244(T);
      } else if (data.isot[num].afgl == 45) {
        return PartitionFunction::compute_partfun_N245(T);
      } else if (data.isot[num].afgl == 55) {
        return PartitionFunction::compute_partfun_N255(T);
      }
    } else if constexpr (S == Species::HydrogenCyanide) {
      if (data.isot[num].afgl == 124) {
        return PartitionFunction::compute_partfun_HCN124(T);
      } else if (data.isot[num].afgl == 134) {
        return PartitionFunction::compute_partfun_HCN134(T);
      } else if (data.isot[num].afgl == 125) {
        return PartitionFunction::compute_partfun_HCN125(T);
      }
    } else if constexpr (S == Species::MethylChloride) {
      if (data.isot[num].afgl == 215) {
        return PartitionFunction::compute_partfun_CH3Cl215(T);
      } else if (data.isot[num].afgl == 217) {
        return PartitionFunction::compute_partfun_CH3Cl217(T);
      }
    } else if constexpr (S == Species::HydrogenPeroxide) {
      if (data.isot[num].afgl == 1661) {
        return PartitionFunction::compute_partfun_H2O21661(T);
      }
    } else if constexpr (S == Species::Acetylene) {
      if (data.isot[num].afgl == 1221) {
        return PartitionFunction::compute_partfun_C2H21221(T);
      } else if (data.isot[num].afgl == 1231) {
        return PartitionFunction::compute_partfun_C2H21231(T);
      } else if (data.isot[num].afgl == 1222) {
        return PartitionFunction::compute_partfun_C2H21222(T);
      }
    } else if constexpr (S == Species::Ethane) {
      if (data.isot[num].afgl == 1221) {
        return PartitionFunction::compute_partfun_C2H61221(T);
      } else if (data.isot[num].afgl == 1231) {
        return PartitionFunction::compute_partfun_C2H61231(T);
      }
    } else if constexpr (S == Species::Phosphine) {
      if (data.isot[num].afgl == 1111) {
        return PartitionFunction::compute_partfun_PH31111(T);
      }
    } else if constexpr (S == Species::CarbonylFluoride) {
      if (data.isot[num].afgl == 269) {
        return PartitionFunction::compute_partfun_COF2269(T);
      } else if (data.isot[num].afgl == 369) {
        return PartitionFunction::compute_partfun_COF2369(T);
      }
    } else if constexpr (S == Species::SulfurHexafluoride) {
      if (data.isot[num].afgl == 29) {
        return PartitionFunction::compute_partfun_SF629(T);
      }
    } else if constexpr (S == Species::HydrogenSulfide) {
      if (data.isot[num].afgl == 121) {
        return PartitionFunction::compute_partfun_H2S121(T);
      } else if (data.isot[num].afgl == 141) {
        return PartitionFunction::compute_partfun_H2S141(T);
      } else if (data.isot[num].afgl == 131) {
        return PartitionFunction::compute_partfun_H2S131(T);
      }
    } else if constexpr (S == Species::FormicAcid) {
      if (data.isot[num].afgl == 126) {
        return PartitionFunction::compute_partfun_HCOOH126(T);
      }
    } else if constexpr (S == Species::Hydroperoxyl) {
      if (data.isot[num].afgl == 166) {
        return PartitionFunction::compute_partfun_HO2166(T);
      }
    } else if constexpr (S == Species::OxygenAtom) {
      if (data.isot[num].afgl == 6) {
        return PartitionFunction::compute_partfun_O6(T);
      }
    } else if constexpr (S == Species::ChlorineNitrate) {
      if (data.isot[num].afgl == 5646) {
        return PartitionFunction::compute_partfun_ClONO25646(T);
      } else if (data.isot[num].afgl == 7646) {
        return PartitionFunction::compute_partfun_ClONO27646(T);
      }
    } else if constexpr (S == Species::NitricOxideCation) {
      if (data.isot[num].afgl == 46) {
        return PartitionFunction::compute_partfun_NO46_plus(T);
      }
    } else if constexpr (S == Species::HypobromousAcid) {
      if (data.isot[num].afgl == 169) {
        return PartitionFunction::compute_partfun_HOBr169(T);
      } else if (data.isot[num].afgl == 161) {
        return PartitionFunction::compute_partfun_HOBr161(T);
      }
    } else if constexpr (S == Species::Ethylene) {
      if (data.isot[num].afgl == 221) {
        return PartitionFunction::compute_partfun_C2H4221(T);
      } else if (data.isot[num].afgl == 231) {
        return PartitionFunction::compute_partfun_C2H4231(T);
      }
    } else if constexpr (S == Species::Methanol) {
      if (data.isot[num].afgl == 2161) {
        return PartitionFunction::compute_partfun_CH3OH2161(T);
      }
    } else if constexpr (S == Species::MethylBromide) {
      if (data.isot[num].afgl == 219) {
        return PartitionFunction::compute_partfun_CH3Br219(T);
      } else if (data.isot[num].afgl == 211) {
        return PartitionFunction::compute_partfun_CH3Br211(T);
      }
    } else if constexpr (S == Species::Acetonitrile) {
      if (data.isot[num].afgl == 2124) {
        return PartitionFunction::compute_partfun_CH3CN2124(T);
      } else if (data.isot[num].afgl == 3124) {
        return PartitionFunction::compute_partfun_CH3CN3124(T);
      } else if (data.isot[num].afgl == 2134) {
        return PartitionFunction::compute_partfun_CH3CN2134(T);
      } else if (data.isot[num].afgl == 3134) {
        return PartitionFunction::compute_partfun_CH3CN3134(T);
      }
    } else if constexpr (S == Species::Pfc14) {
      if (data.isot[num].afgl == 29) {
        return PartitionFunction::compute_partfun_CF429(T);
      }
    } else if constexpr (S == Species::Diacetylene) {
      if (data.isot[num].afgl == 2211) {
        return PartitionFunction::compute_partfun_C4H22211(T);
      }
    } else if constexpr (S == Species::Cyanoacetylene) {
      if (data.isot[num].afgl == 12224) {
        return PartitionFunction::compute_partfun_HC3N12224(T);
      } else if (data.isot[num].afgl == 12225) {
        return PartitionFunction::compute_partfun_HC3N12225(T);
      } else if (data.isot[num].afgl == 12234) {
        return PartitionFunction::compute_partfun_HC3N12234(T);
      } else if (data.isot[num].afgl == 12324) {
        return PartitionFunction::compute_partfun_HC3N12324(T);
      } else if (data.isot[num].afgl == 13224) {
        return PartitionFunction::compute_partfun_HC3N13224(T);
      } else if (data.isot[num].afgl == 22224) {
        return PartitionFunction::compute_partfun_HC3N22224(T);
      }
    } else if constexpr (S == Species::Hydrogen) {
      if (data.isot[num].afgl == 11) {
        return PartitionFunction::compute_partfun_H211(T);
      } else if (data.isot[num].afgl == 12) {
        return PartitionFunction::compute_partfun_H212(T);
      }
    } else if constexpr (S == Species::CarbonMonosulfide) {
      if (data.isot[num].afgl == 22) {
        return PartitionFunction::compute_partfun_CS22(T);
      } else if (data.isot[num].afgl == 24) {
        return PartitionFunction::compute_partfun_CS24(T);
      } else if (data.isot[num].afgl == 32) {
        return PartitionFunction::compute_partfun_CS32(T);
      } else if (data.isot[num].afgl == 23) {
        return PartitionFunction::compute_partfun_CS23(T);
      }
    } else if constexpr (S == Species::SulfurTrioxide) {
      if (data.isot[num].afgl == 26) {
        return PartitionFunction::compute_partfun_SO326(T);
      }
    } else if constexpr (S == Species::Cyanogen) {
      if (data.isot[num].afgl == 4224) {
        return PartitionFunction::compute_partfun_C2N24224(T);
      } else if (data.isot[num].afgl == 5225) {
        return PartitionFunction::compute_partfun_C2N25225(T);
      }
    } else if constexpr (S == Species::Phosgene) {
      if (data.isot[num].afgl == 2655) {
        return PartitionFunction::compute_partfun_COCl22655(T);
      } else if (data.isot[num].afgl == 2657) {
        return PartitionFunction::compute_partfun_COCl22657(T);
      }
    }
    std::exit(1);
  }
};

class Isotope {
  Species s;
  unsigned char num;

 public:
  constexpr Isotope(Species S, unsigned char n) noexcept : s(S), num(n) {}
  constexpr Isotope() noexcept : s(Species::FINAL), num(-1) {}
  Isotope(Species S, const std::vector<ChargedAtom>& ca) : s(S), num(0) {
    switch (s) {
      case Species::Bath:
        num = 0;
        break;
      case Species::Water:
        num = Isotopologue<Species::Water>().findNum(ca);
        break;
      case Species::CarbonDioxide:
        num = Isotopologue<Species::CarbonDioxide>().findNum(ca);
        break;
      case Species::Ozone:
        num = Isotopologue<Species::Ozone>().findNum(ca);
        break;
      case Species::NitrogenOxide:
        num = Isotopologue<Species::NitrogenOxide>().findNum(ca);
        break;
      case Species::CarbonMonoxide:
        num = Isotopologue<Species::CarbonMonoxide>().findNum(ca);
        break;
      case Species::Methane:
        num = Isotopologue<Species::Methane>().findNum(ca);
        break;
      case Species::Oxygen:
        num = Isotopologue<Species::Oxygen>().findNum(ca);
        break;
      case Species::NitricOxide:
        num = Isotopologue<Species::NitricOxide>().findNum(ca);
        break;
      case Species::SulfurDioxide:
        num = Isotopologue<Species::SulfurDioxide>().findNum(ca);
        break;
      case Species::NitrogenDioxide:
        num = Isotopologue<Species::NitrogenDioxide>().findNum(ca);
        break;
      case Species::Ammonia:
        num = Isotopologue<Species::Ammonia>().findNum(ca);
        break;
      case Species::NitricAcid:
        num = Isotopologue<Species::NitricAcid>().findNum(ca);
        break;
      case Species::Hydroxyl:
        num = Isotopologue<Species::Hydroxyl>().findNum(ca);
        break;
      case Species::HydrogenFluoride:
        num = Isotopologue<Species::HydrogenFluoride>().findNum(ca);
        break;
      case Species::HydrogenChloride:
        num = Isotopologue<Species::HydrogenChloride>().findNum(ca);
        break;
      case Species::HydrogenBromide:
        num = Isotopologue<Species::HydrogenBromide>().findNum(ca);
        break;
      case Species::HydrogenIodide:
        num = Isotopologue<Species::HydrogenIodide>().findNum(ca);
        break;
      case Species::ChlorineMonoxide:
        num = Isotopologue<Species::ChlorineMonoxide>().findNum(ca);
        break;
      case Species::CarbonylSulfide:
        num = Isotopologue<Species::CarbonylSulfide>().findNum(ca);
        break;
      case Species::Formaldehyde:
        num = Isotopologue<Species::Formaldehyde>().findNum(ca);
        break;
      case Species::HypochlorousAcid:
        num = Isotopologue<Species::HypochlorousAcid>().findNum(ca);
        break;
      case Species::Nitrogen:
        num = Isotopologue<Species::Nitrogen>().findNum(ca);
        break;
      case Species::HydrogenCyanide:
        num = Isotopologue<Species::HydrogenCyanide>().findNum(ca);
        break;
      case Species::MethylChloride:
        num = Isotopologue<Species::MethylChloride>().findNum(ca);
        break;
      case Species::HydrogenPeroxide:
        num = Isotopologue<Species::HydrogenPeroxide>().findNum(ca);
        break;
      case Species::Acetylene:
        num = Isotopologue<Species::Acetylene>().findNum(ca);
        break;
      case Species::Ethane:
        num = Isotopologue<Species::Ethane>().findNum(ca);
        break;
      case Species::Phosphine:
        num = Isotopologue<Species::Phosphine>().findNum(ca);
        break;
      case Species::CarbonylFluoride:
        num = Isotopologue<Species::CarbonylFluoride>().findNum(ca);
        break;
      case Species::SulfurHexafluoride:
        num = Isotopologue<Species::SulfurHexafluoride>().findNum(ca);
        break;
      case Species::HydrogenSulfide:
        num = Isotopologue<Species::HydrogenSulfide>().findNum(ca);
        break;
      case Species::FormicAcid:
        num = Isotopologue<Species::FormicAcid>().findNum(ca);
        break;
      case Species::Hydroperoxyl:
        num = Isotopologue<Species::Hydroperoxyl>().findNum(ca);
        break;
      case Species::OxygenAtom:
        num = Isotopologue<Species::OxygenAtom>().findNum(ca);
        break;
      case Species::ChlorineNitrate:
        num = Isotopologue<Species::ChlorineNitrate>().findNum(ca);
        break;
      case Species::NitricOxideCation:
        num = Isotopologue<Species::NitricOxideCation>().findNum(ca);
        break;
      case Species::HypobromousAcid:
        num = Isotopologue<Species::HypobromousAcid>().findNum(ca);
        break;
      case Species::Ethylene:
        num = Isotopologue<Species::Ethylene>().findNum(ca);
        break;
      case Species::Methanol:
        num = Isotopologue<Species::Methanol>().findNum(ca);
        break;
      case Species::MethylBromide:
        num = Isotopologue<Species::MethylBromide>().findNum(ca);
        break;
      case Species::Acetonitrile:
        num = Isotopologue<Species::Acetonitrile>().findNum(ca);
        break;
      case Species::Pfc14:
        num = Isotopologue<Species::Pfc14>().findNum(ca);
        break;
      case Species::Diacetylene:
        num = Isotopologue<Species::Diacetylene>().findNum(ca);
        break;
      case Species::Cyanoacetylene:
        num = Isotopologue<Species::Cyanoacetylene>().findNum(ca);
        break;
      case Species::Hydrogen:
        num = Isotopologue<Species::Hydrogen>().findNum(ca);
        break;
      case Species::CarbonMonosulfide:
        num = Isotopologue<Species::CarbonMonosulfide>().findNum(ca);
        break;
      case Species::SulfurTrioxide:
        num = Isotopologue<Species::SulfurTrioxide>().findNum(ca);
        break;
      case Species::Cyanogen:
        num = Isotopologue<Species::Cyanogen>().findNum(ca);
        break;
      case Species::Phosgene:
        num = Isotopologue<Species::Phosgene>().findNum(ca);
        break;
      case Species::FINAL:
        num = 0;
    }

    if (num == getIsotopologueCount(S))
      throw std::runtime_error("Cannot find atoms");
  }

  friend std::ostream& operator<<(std::ostream& os, Isotope x) {
    return os << x.s << '-' << int(x.num);
  }
  friend std::istream& operator>>(std::istream& is, Isotope& x) {
    std::string isot;
    is >> isot;
    size_t bindes = isot.find('-');
    x.s = toSpecies(isot.substr(0, bindes));
    x.num = std::stoi(isot.substr(bindes + 1, isot.size()));
    return is;
  }
  friend bool operator==(Isotope x, Isotope y) {
    return (x.s == y.s) and (x.num == y.num);
  }

  constexpr Species Spec() const noexcept { return s; }

  constexpr double mass() const noexcept {
    switch (s) {
      case Species::Bath:
        return std::numeric_limits<double>::quiet_NaN();
      case Species::Water:
        return Isotopologue<Species::Water>().Mass(long(num));
      case Species::CarbonDioxide:
        return Isotopologue<Species::CarbonDioxide>().Mass(long(num));
      case Species::Ozone:
        return Isotopologue<Species::Ozone>().Mass(long(num));
      case Species::NitrogenOxide:
        return Isotopologue<Species::NitrogenOxide>().Mass(long(num));
      case Species::CarbonMonoxide:
        return Isotopologue<Species::CarbonMonoxide>().Mass(long(num));
      case Species::Methane:
        return Isotopologue<Species::Methane>().Mass(long(num));
      case Species::Oxygen:
        return Isotopologue<Species::Oxygen>().Mass(long(num));
      case Species::NitricOxide:
        return Isotopologue<Species::NitricOxide>().Mass(long(num));
      case Species::SulfurDioxide:
        return Isotopologue<Species::SulfurDioxide>().Mass(long(num));
      case Species::NitrogenDioxide:
        return Isotopologue<Species::NitrogenDioxide>().Mass(long(num));
      case Species::Ammonia:
        return Isotopologue<Species::Ammonia>().Mass(long(num));
      case Species::NitricAcid:
        return Isotopologue<Species::NitricAcid>().Mass(long(num));
      case Species::Hydroxyl:
        return Isotopologue<Species::Hydroxyl>().Mass(long(num));
      case Species::HydrogenFluoride:
        return Isotopologue<Species::HydrogenFluoride>().Mass(long(num));
      case Species::HydrogenChloride:
        return Isotopologue<Species::HydrogenChloride>().Mass(long(num));
      case Species::HydrogenBromide:
        return Isotopologue<Species::HydrogenBromide>().Mass(long(num));
      case Species::HydrogenIodide:
        return Isotopologue<Species::HydrogenIodide>().Mass(long(num));
      case Species::ChlorineMonoxide:
        return Isotopologue<Species::ChlorineMonoxide>().Mass(long(num));
      case Species::CarbonylSulfide:
        return Isotopologue<Species::CarbonylSulfide>().Mass(long(num));
      case Species::Formaldehyde:
        return Isotopologue<Species::Formaldehyde>().Mass(long(num));
      case Species::HypochlorousAcid:
        return Isotopologue<Species::HypochlorousAcid>().Mass(long(num));
      case Species::Nitrogen:
        return Isotopologue<Species::Nitrogen>().Mass(long(num));
      case Species::HydrogenCyanide:
        return Isotopologue<Species::HydrogenCyanide>().Mass(long(num));
      case Species::MethylChloride:
        return Isotopologue<Species::MethylChloride>().Mass(long(num));
      case Species::HydrogenPeroxide:
        return Isotopologue<Species::HydrogenPeroxide>().Mass(long(num));
      case Species::Acetylene:
        return Isotopologue<Species::Acetylene>().Mass(long(num));
      case Species::Ethane:
        return Isotopologue<Species::Ethane>().Mass(long(num));
      case Species::Phosphine:
        return Isotopologue<Species::Phosphine>().Mass(long(num));
      case Species::CarbonylFluoride:
        return Isotopologue<Species::CarbonylFluoride>().Mass(long(num));
      case Species::SulfurHexafluoride:
        return Isotopologue<Species::SulfurHexafluoride>().Mass(long(num));
      case Species::HydrogenSulfide:
        return Isotopologue<Species::HydrogenSulfide>().Mass(long(num));
      case Species::FormicAcid:
        return Isotopologue<Species::FormicAcid>().Mass(long(num));
      case Species::Hydroperoxyl:
        return Isotopologue<Species::Hydroperoxyl>().Mass(long(num));
      case Species::OxygenAtom:
        return Isotopologue<Species::OxygenAtom>().Mass(long(num));
      case Species::ChlorineNitrate:
        return Isotopologue<Species::ChlorineNitrate>().Mass(long(num));
      case Species::NitricOxideCation:
        return Isotopologue<Species::NitricOxideCation>().Mass(long(num));
      case Species::HypobromousAcid:
        return Isotopologue<Species::HypobromousAcid>().Mass(long(num));
      case Species::Ethylene:
        return Isotopologue<Species::Ethylene>().Mass(long(num));
      case Species::Methanol:
        return Isotopologue<Species::Methanol>().Mass(long(num));
      case Species::MethylBromide:
        return Isotopologue<Species::MethylBromide>().Mass(long(num));
      case Species::Acetonitrile:
        return Isotopologue<Species::Acetonitrile>().Mass(long(num));
      case Species::Pfc14:
        return Isotopologue<Species::Pfc14>().Mass(long(num));
      case Species::Diacetylene:
        return Isotopologue<Species::Diacetylene>().Mass(long(num));
      case Species::Cyanoacetylene:
        return Isotopologue<Species::Cyanoacetylene>().Mass(long(num));
      case Species::Hydrogen:
        return Isotopologue<Species::Hydrogen>().Mass(long(num));
      case Species::CarbonMonosulfide:
        return Isotopologue<Species::CarbonMonosulfide>().Mass(long(num));
      case Species::SulfurTrioxide:
        return Isotopologue<Species::SulfurTrioxide>().Mass(long(num));
      case Species::Cyanogen:
        return Isotopologue<Species::Cyanogen>().Mass(long(num));
      case Species::Phosgene:
        return Isotopologue<Species::Phosgene>().Mass(long(num));
      case Species::FINAL: { /*leave last*/
      }
    }
    return {};
  }

  constexpr double gi() const noexcept {
    switch (s) {
      case Species::Bath:
        return std::numeric_limits<double>::quiet_NaN();
      case Species::Water:
        return Isotopologue<Species::Water>().G(long(num));
      case Species::CarbonDioxide:
        return Isotopologue<Species::CarbonDioxide>().G(long(num));
      case Species::Ozone:
        return Isotopologue<Species::Ozone>().G(long(num));
      case Species::NitrogenOxide:
        return Isotopologue<Species::NitrogenOxide>().G(long(num));
      case Species::CarbonMonoxide:
        return Isotopologue<Species::CarbonMonoxide>().G(long(num));
      case Species::Methane:
        return Isotopologue<Species::Methane>().G(long(num));
      case Species::Oxygen:
        return Isotopologue<Species::Oxygen>().G(long(num));
      case Species::NitricOxide:
        return Isotopologue<Species::NitricOxide>().G(long(num));
      case Species::SulfurDioxide:
        return Isotopologue<Species::SulfurDioxide>().G(long(num));
      case Species::NitrogenDioxide:
        return Isotopologue<Species::NitrogenDioxide>().G(long(num));
      case Species::Ammonia:
        return Isotopologue<Species::Ammonia>().G(long(num));
      case Species::NitricAcid:
        return Isotopologue<Species::NitricAcid>().G(long(num));
      case Species::Hydroxyl:
        return Isotopologue<Species::Hydroxyl>().G(long(num));
      case Species::HydrogenFluoride:
        return Isotopologue<Species::HydrogenFluoride>().G(long(num));
      case Species::HydrogenChloride:
        return Isotopologue<Species::HydrogenChloride>().G(long(num));
      case Species::HydrogenBromide:
        return Isotopologue<Species::HydrogenBromide>().G(long(num));
      case Species::HydrogenIodide:
        return Isotopologue<Species::HydrogenIodide>().G(long(num));
      case Species::ChlorineMonoxide:
        return Isotopologue<Species::ChlorineMonoxide>().G(long(num));
      case Species::CarbonylSulfide:
        return Isotopologue<Species::CarbonylSulfide>().G(long(num));
      case Species::Formaldehyde:
        return Isotopologue<Species::Formaldehyde>().G(long(num));
      case Species::HypochlorousAcid:
        return Isotopologue<Species::HypochlorousAcid>().G(long(num));
      case Species::Nitrogen:
        return Isotopologue<Species::Nitrogen>().G(long(num));
      case Species::HydrogenCyanide:
        return Isotopologue<Species::HydrogenCyanide>().G(long(num));
      case Species::MethylChloride:
        return Isotopologue<Species::MethylChloride>().G(long(num));
      case Species::HydrogenPeroxide:
        return Isotopologue<Species::HydrogenPeroxide>().G(long(num));
      case Species::Acetylene:
        return Isotopologue<Species::Acetylene>().G(long(num));
      case Species::Ethane:
        return Isotopologue<Species::Ethane>().G(long(num));
      case Species::Phosphine:
        return Isotopologue<Species::Phosphine>().G(long(num));
      case Species::CarbonylFluoride:
        return Isotopologue<Species::CarbonylFluoride>().G(long(num));
      case Species::SulfurHexafluoride:
        return Isotopologue<Species::SulfurHexafluoride>().G(long(num));
      case Species::HydrogenSulfide:
        return Isotopologue<Species::HydrogenSulfide>().G(long(num));
      case Species::FormicAcid:
        return Isotopologue<Species::FormicAcid>().G(long(num));
      case Species::Hydroperoxyl:
        return Isotopologue<Species::Hydroperoxyl>().G(long(num));
      case Species::OxygenAtom:
        return Isotopologue<Species::OxygenAtom>().G(long(num));
      case Species::ChlorineNitrate:
        return Isotopologue<Species::ChlorineNitrate>().G(long(num));
      case Species::NitricOxideCation:
        return Isotopologue<Species::NitricOxideCation>().G(long(num));
      case Species::HypobromousAcid:
        return Isotopologue<Species::HypobromousAcid>().G(long(num));
      case Species::Ethylene:
        return Isotopologue<Species::Ethylene>().G(long(num));
      case Species::Methanol:
        return Isotopologue<Species::Methanol>().G(long(num));
      case Species::MethylBromide:
        return Isotopologue<Species::MethylBromide>().G(long(num));
      case Species::Acetonitrile:
        return Isotopologue<Species::Acetonitrile>().G(long(num));
      case Species::Pfc14:
        return Isotopologue<Species::Pfc14>().G(long(num));
      case Species::Diacetylene:
        return Isotopologue<Species::Diacetylene>().G(long(num));
      case Species::Cyanoacetylene:
        return Isotopologue<Species::Cyanoacetylene>().G(long(num));
      case Species::Hydrogen:
        return Isotopologue<Species::Hydrogen>().G(long(num));
      case Species::CarbonMonosulfide:
        return Isotopologue<Species::CarbonMonosulfide>().G(long(num));
      case Species::SulfurTrioxide:
        return Isotopologue<Species::SulfurTrioxide>().G(long(num));
      case Species::Cyanogen:
        return Isotopologue<Species::Cyanogen>().G(long(num));
      case Species::Phosgene:
        return Isotopologue<Species::Phosgene>().G(long(num));
      case Species::FINAL: { /*leave last*/
      }
    }
    return {};
  }

  constexpr double QT(double T) const noexcept {
    switch (s) {
      case Species::Water:
        return Isotopologue<Species::Water>().QT(T, num);
      case Species::CarbonDioxide:
        return Isotopologue<Species::CarbonDioxide>().QT(T, num);
      case Species::Ozone:
        return Isotopologue<Species::Ozone>().QT(T, num);
      case Species::NitrogenOxide:
        return Isotopologue<Species::NitrogenOxide>().QT(T, num);
      case Species::CarbonMonoxide:
        return Isotopologue<Species::CarbonMonoxide>().QT(T, num);
      case Species::Methane:
        return Isotopologue<Species::Methane>().QT(T, num);
      case Species::Oxygen:
        return Isotopologue<Species::Oxygen>().QT(T, num);
      case Species::NitricOxide:
        return Isotopologue<Species::NitricOxide>().QT(T, num);
      case Species::SulfurDioxide:
        return Isotopologue<Species::SulfurDioxide>().QT(T, num);
      case Species::NitrogenDioxide:
        return Isotopologue<Species::NitrogenDioxide>().QT(T, num);
      case Species::Ammonia:
        return Isotopologue<Species::Ammonia>().QT(T, num);
      case Species::NitricAcid:
        return Isotopologue<Species::NitricAcid>().QT(T, num);
      case Species::Hydroxyl:
        return Isotopologue<Species::Hydroxyl>().QT(T, num);
      case Species::HydrogenFluoride:
        return Isotopologue<Species::HydrogenFluoride>().QT(T, num);
      case Species::HydrogenChloride:
        return Isotopologue<Species::HydrogenChloride>().QT(T, num);
      case Species::HydrogenBromide:
        return Isotopologue<Species::HydrogenBromide>().QT(T, num);
      case Species::HydrogenIodide:
        return Isotopologue<Species::HydrogenIodide>().QT(T, num);
      case Species::ChlorineMonoxide:
        return Isotopologue<Species::ChlorineMonoxide>().QT(T, num);
      case Species::CarbonylSulfide:
        return Isotopologue<Species::CarbonylSulfide>().QT(T, num);
      case Species::Formaldehyde:
        return Isotopologue<Species::Formaldehyde>().QT(T, num);
      case Species::HypochlorousAcid:
        return Isotopologue<Species::HypochlorousAcid>().QT(T, num);
      case Species::Nitrogen:
        return Isotopologue<Species::Nitrogen>().QT(T, num);
      case Species::HydrogenCyanide:
        return Isotopologue<Species::HydrogenCyanide>().QT(T, num);
      case Species::MethylChloride:
        return Isotopologue<Species::MethylChloride>().QT(T, num);
      case Species::HydrogenPeroxide:
        return Isotopologue<Species::HydrogenPeroxide>().QT(T, num);
      case Species::Acetylene:
        return Isotopologue<Species::Acetylene>().QT(T, num);
      case Species::Ethane:
        return Isotopologue<Species::Ethane>().QT(T, num);
      case Species::Phosphine:
        return Isotopologue<Species::Phosphine>().QT(T, num);
      case Species::CarbonylFluoride:
        return Isotopologue<Species::CarbonylFluoride>().QT(T, num);
      case Species::SulfurHexafluoride:
        return Isotopologue<Species::SulfurHexafluoride>().QT(T, num);
      case Species::HydrogenSulfide:
        return Isotopologue<Species::HydrogenSulfide>().QT(T, num);
      case Species::FormicAcid:
        return Isotopologue<Species::FormicAcid>().QT(T, num);
      case Species::Hydroperoxyl:
        return Isotopologue<Species::Hydroperoxyl>().QT(T, num);
      case Species::OxygenAtom:
        return Isotopologue<Species::OxygenAtom>().QT(T, num);
      case Species::ChlorineNitrate:
        return Isotopologue<Species::ChlorineNitrate>().QT(T, num);
      case Species::NitricOxideCation:
        return Isotopologue<Species::NitricOxideCation>().QT(T, num);
      case Species::HypobromousAcid:
        return Isotopologue<Species::HypobromousAcid>().QT(T, num);
      case Species::Ethylene:
        return Isotopologue<Species::Ethylene>().QT(T, num);
      case Species::Methanol:
        return Isotopologue<Species::Methanol>().QT(T, num);
      case Species::MethylBromide:
        return Isotopologue<Species::MethylBromide>().QT(T, num);
      case Species::Acetonitrile:
        return Isotopologue<Species::Acetonitrile>().QT(T, num);
      case Species::Pfc14:
        return Isotopologue<Species::Pfc14>().QT(T, num);
      case Species::Diacetylene:
        return Isotopologue<Species::Diacetylene>().QT(T, num);
      case Species::Cyanoacetylene:
        return Isotopologue<Species::Cyanoacetylene>().QT(T, num);
      case Species::Hydrogen:
        return Isotopologue<Species::Hydrogen>().QT(T, num);
      case Species::CarbonMonosulfide:
        return Isotopologue<Species::CarbonMonosulfide>().QT(T, num);
      case Species::SulfurTrioxide:
        return Isotopologue<Species::SulfurTrioxide>().QT(T, num);
      case Species::Cyanogen:
        return Isotopologue<Species::Cyanogen>().QT(T, num);
      case Species::Phosgene:
        return Isotopologue<Species::Phosgene>().QT(T, num);
      case Species::Bath:
      case Species::FINAL: { /* leave last */
      }
    }
    return {};
  }

  std::vector<ChargedAtom> atoms() const noexcept {
    switch (s) {
      case Species::Bath: {
      };
      case Species::Water:
        return Isotopologue<Species::Water>().Atoms(long(num));
      case Species::CarbonDioxide:
        return Isotopologue<Species::CarbonDioxide>().Atoms(long(num));
      case Species::Ozone:
        return Isotopologue<Species::Ozone>().Atoms(long(num));
      case Species::NitrogenOxide:
        return Isotopologue<Species::NitrogenOxide>().Atoms(long(num));
      case Species::CarbonMonoxide:
        return Isotopologue<Species::CarbonMonoxide>().Atoms(long(num));
      case Species::Methane:
        return Isotopologue<Species::Methane>().Atoms(long(num));
      case Species::Oxygen:
        return Isotopologue<Species::Oxygen>().Atoms(long(num));
      case Species::NitricOxide:
        return Isotopologue<Species::NitricOxide>().Atoms(long(num));
      case Species::SulfurDioxide:
        return Isotopologue<Species::SulfurDioxide>().Atoms(long(num));
      case Species::NitrogenDioxide:
        return Isotopologue<Species::NitrogenDioxide>().Atoms(long(num));
      case Species::Ammonia:
        return Isotopologue<Species::Ammonia>().Atoms(long(num));
      case Species::NitricAcid:
        return Isotopologue<Species::NitricAcid>().Atoms(long(num));
      case Species::Hydroxyl:
        return Isotopologue<Species::Hydroxyl>().Atoms(long(num));
      case Species::HydrogenFluoride:
        return Isotopologue<Species::HydrogenFluoride>().Atoms(long(num));
      case Species::HydrogenChloride:
        return Isotopologue<Species::HydrogenChloride>().Atoms(long(num));
      case Species::HydrogenBromide:
        return Isotopologue<Species::HydrogenBromide>().Atoms(long(num));
      case Species::HydrogenIodide:
        return Isotopologue<Species::HydrogenIodide>().Atoms(long(num));
      case Species::ChlorineMonoxide:
        return Isotopologue<Species::ChlorineMonoxide>().Atoms(long(num));
      case Species::CarbonylSulfide:
        return Isotopologue<Species::CarbonylSulfide>().Atoms(long(num));
      case Species::Formaldehyde:
        return Isotopologue<Species::Formaldehyde>().Atoms(long(num));
      case Species::HypochlorousAcid:
        return Isotopologue<Species::HypochlorousAcid>().Atoms(long(num));
      case Species::Nitrogen:
        return Isotopologue<Species::Nitrogen>().Atoms(long(num));
      case Species::HydrogenCyanide:
        return Isotopologue<Species::HydrogenCyanide>().Atoms(long(num));
      case Species::MethylChloride:
        return Isotopologue<Species::MethylChloride>().Atoms(long(num));
      case Species::HydrogenPeroxide:
        return Isotopologue<Species::HydrogenPeroxide>().Atoms(long(num));
      case Species::Acetylene:
        return Isotopologue<Species::Acetylene>().Atoms(long(num));
      case Species::Ethane:
        return Isotopologue<Species::Ethane>().Atoms(long(num));
      case Species::Phosphine:
        return Isotopologue<Species::Phosphine>().Atoms(long(num));
      case Species::CarbonylFluoride:
        return Isotopologue<Species::CarbonylFluoride>().Atoms(long(num));
      case Species::SulfurHexafluoride:
        return Isotopologue<Species::SulfurHexafluoride>().Atoms(long(num));
      case Species::HydrogenSulfide:
        return Isotopologue<Species::HydrogenSulfide>().Atoms(long(num));
      case Species::FormicAcid:
        return Isotopologue<Species::FormicAcid>().Atoms(long(num));
      case Species::Hydroperoxyl:
        return Isotopologue<Species::Hydroperoxyl>().Atoms(long(num));
      case Species::OxygenAtom:
        return Isotopologue<Species::OxygenAtom>().Atoms(long(num));
      case Species::ChlorineNitrate:
        return Isotopologue<Species::ChlorineNitrate>().Atoms(long(num));
      case Species::NitricOxideCation:
        return Isotopologue<Species::NitricOxideCation>().Atoms(long(num));
      case Species::HypobromousAcid:
        return Isotopologue<Species::HypobromousAcid>().Atoms(long(num));
      case Species::Ethylene:
        return Isotopologue<Species::Ethylene>().Atoms(long(num));
      case Species::Methanol:
        return Isotopologue<Species::Methanol>().Atoms(long(num));
      case Species::MethylBromide:
        return Isotopologue<Species::MethylBromide>().Atoms(long(num));
      case Species::Acetonitrile:
        return Isotopologue<Species::Acetonitrile>().Atoms(long(num));
      case Species::Pfc14:
        return Isotopologue<Species::Pfc14>().Atoms(long(num));
      case Species::Diacetylene:
        return Isotopologue<Species::Diacetylene>().Atoms(long(num));
      case Species::Cyanoacetylene:
        return Isotopologue<Species::Cyanoacetylene>().Atoms(long(num));
      case Species::Hydrogen:
        return Isotopologue<Species::Hydrogen>().Atoms(long(num));
      case Species::CarbonMonosulfide:
        return Isotopologue<Species::CarbonMonosulfide>().Atoms(long(num));
      case Species::SulfurTrioxide:
        return Isotopologue<Species::SulfurTrioxide>().Atoms(long(num));
      case Species::Cyanogen:
        return Isotopologue<Species::Cyanogen>().Atoms(long(num));
      case Species::Phosgene:
        return Isotopologue<Species::Phosgene>().Atoms(long(num));
      case Species::FINAL: { /*leave last*/
      }
    }
    return {};
  }

  constexpr unsigned char localQuantumNumberCount() const noexcept {
    return getLocalQuantumNumberCount(s);
  }

  constexpr unsigned char globalQuantumNumberCount() const noexcept {
    return getGlobalQuantumNumberCount(s);
  }

  Quantum::Number get_local(const std::vector<Quantum::Number>& v,
                            Quantum::Type t) const noexcept {
    size_t pos = std::numeric_limits<size_t>::max();
    switch (s) {
      case Species::Bath:
        break;
      case Species::Water:
        pos = Isotopologue<Species::Water>().get_local_pos(t);
        break;
      case Species::CarbonDioxide:
        pos = Isotopologue<Species::CarbonDioxide>().get_local_pos(t);
        break;
      case Species::Ozone:
        pos = Isotopologue<Species::Ozone>().get_local_pos(t);
        break;
      case Species::NitrogenOxide:
        pos = Isotopologue<Species::NitrogenOxide>().get_local_pos(t);
        break;
      case Species::CarbonMonoxide:
        pos = Isotopologue<Species::CarbonMonoxide>().get_local_pos(t);
        break;
      case Species::Methane:
        pos = Isotopologue<Species::Methane>().get_local_pos(t);
        break;
      case Species::Oxygen:
        pos = Isotopologue<Species::Oxygen>().get_local_pos(t);
        break;
      case Species::NitricOxide:
        pos = Isotopologue<Species::NitricOxide>().get_local_pos(t);
        break;
      case Species::SulfurDioxide:
        pos = Isotopologue<Species::SulfurDioxide>().get_local_pos(t);
        break;
      case Species::NitrogenDioxide:
        pos = Isotopologue<Species::NitrogenDioxide>().get_local_pos(t);
        break;
      case Species::Ammonia:
        pos = Isotopologue<Species::Ammonia>().get_local_pos(t);
        break;
      case Species::NitricAcid:
        pos = Isotopologue<Species::NitricAcid>().get_local_pos(t);
        break;
      case Species::Hydroxyl:
        pos = Isotopologue<Species::Hydroxyl>().get_local_pos(t);
        break;
      case Species::HydrogenFluoride:
        pos = Isotopologue<Species::HydrogenFluoride>().get_local_pos(t);
        break;
      case Species::HydrogenChloride:
        pos = Isotopologue<Species::HydrogenChloride>().get_local_pos(t);
        break;
      case Species::HydrogenBromide:
        pos = Isotopologue<Species::HydrogenBromide>().get_local_pos(t);
        break;
      case Species::HydrogenIodide:
        pos = Isotopologue<Species::HydrogenIodide>().get_local_pos(t);
        break;
      case Species::ChlorineMonoxide:
        pos = Isotopologue<Species::ChlorineMonoxide>().get_local_pos(t);
        break;
      case Species::CarbonylSulfide:
        pos = Isotopologue<Species::CarbonylSulfide>().get_local_pos(t);
        break;
      case Species::Formaldehyde:
        pos = Isotopologue<Species::Formaldehyde>().get_local_pos(t);
        break;
      case Species::HypochlorousAcid:
        pos = Isotopologue<Species::HypochlorousAcid>().get_local_pos(t);
        break;
      case Species::Nitrogen:
        pos = Isotopologue<Species::Nitrogen>().get_local_pos(t);
        break;
      case Species::HydrogenCyanide:
        pos = Isotopologue<Species::HydrogenCyanide>().get_local_pos(t);
        break;
      case Species::MethylChloride:
        pos = Isotopologue<Species::MethylChloride>().get_local_pos(t);
        break;
      case Species::HydrogenPeroxide:
        pos = Isotopologue<Species::HydrogenPeroxide>().get_local_pos(t);
        break;
      case Species::Acetylene:
        pos = Isotopologue<Species::Acetylene>().get_local_pos(t);
        break;
      case Species::Ethane:
        pos = Isotopologue<Species::Ethane>().get_local_pos(t);
        break;
      case Species::Phosphine:
        pos = Isotopologue<Species::Phosphine>().get_local_pos(t);
        break;
      case Species::CarbonylFluoride:
        pos = Isotopologue<Species::CarbonylFluoride>().get_local_pos(t);
        break;
      case Species::SulfurHexafluoride:
        pos = Isotopologue<Species::SulfurHexafluoride>().get_local_pos(t);
        break;
      case Species::HydrogenSulfide:
        pos = Isotopologue<Species::HydrogenSulfide>().get_local_pos(t);
        break;
      case Species::FormicAcid:
        pos = Isotopologue<Species::FormicAcid>().get_local_pos(t);
        break;
      case Species::Hydroperoxyl:
        pos = Isotopologue<Species::Hydroperoxyl>().get_local_pos(t);
        break;
      case Species::OxygenAtom:
        pos = Isotopologue<Species::OxygenAtom>().get_local_pos(t);
        break;
      case Species::ChlorineNitrate:
        pos = Isotopologue<Species::ChlorineNitrate>().get_local_pos(t);
        break;
      case Species::NitricOxideCation:
        pos = Isotopologue<Species::NitricOxideCation>().get_local_pos(t);
        break;
      case Species::HypobromousAcid:
        pos = Isotopologue<Species::HypobromousAcid>().get_local_pos(t);
        break;
      case Species::Ethylene:
        pos = Isotopologue<Species::Ethylene>().get_local_pos(t);
        break;
      case Species::Methanol:
        pos = Isotopologue<Species::Methanol>().get_local_pos(t);
        break;
      case Species::MethylBromide:
        pos = Isotopologue<Species::MethylBromide>().get_local_pos(t);
        break;
      case Species::Acetonitrile:
        pos = Isotopologue<Species::Acetonitrile>().get_local_pos(t);
        break;
      case Species::Pfc14:
        pos = Isotopologue<Species::Pfc14>().get_local_pos(t);
        break;
      case Species::Diacetylene:
        pos = Isotopologue<Species::Diacetylene>().get_local_pos(t);
        break;
      case Species::Cyanoacetylene:
        pos = Isotopologue<Species::Cyanoacetylene>().get_local_pos(t);
        break;
      case Species::Hydrogen:
        pos = Isotopologue<Species::Hydrogen>().get_local_pos(t);
        break;
      case Species::CarbonMonosulfide:
        pos = Isotopologue<Species::CarbonMonosulfide>().get_local_pos(t);
        break;
      case Species::SulfurTrioxide:
        pos = Isotopologue<Species::SulfurTrioxide>().get_local_pos(t);
        break;
      case Species::Cyanogen:
        pos = Isotopologue<Species::Cyanogen>().get_local_pos(t);
        break;
      case Species::Phosgene:
        pos = Isotopologue<Species::Phosgene>().get_local_pos(t);
        break;
      case Species::FINAL: { /*leave last*/
      }
    }

    if (pos not_eq std::numeric_limits<size_t>::max())
      return v[pos];
    else
      return Rational(0, 0);
  }

  Quantum::Number get_global(const std::vector<Quantum::Number>& v,
                             Quantum::Type t) const noexcept {
    size_t pos = std::numeric_limits<size_t>::max();
    switch (s) {
      case Species::Bath:
        break;
      case Species::Water:
        pos = Isotopologue<Species::Water>().get_global_pos(t);
        break;
      case Species::CarbonDioxide:
        pos = Isotopologue<Species::CarbonDioxide>().get_global_pos(t);
        break;
      case Species::Ozone:
        pos = Isotopologue<Species::Ozone>().get_global_pos(t);
        break;
      case Species::NitrogenOxide:
        pos = Isotopologue<Species::NitrogenOxide>().get_global_pos(t);
        break;
      case Species::CarbonMonoxide:
        pos = Isotopologue<Species::CarbonMonoxide>().get_global_pos(t);
        break;
      case Species::Methane:
        pos = Isotopologue<Species::Methane>().get_global_pos(t);
        break;
      case Species::Oxygen:
        pos = Isotopologue<Species::Oxygen>().get_global_pos(t);
        break;
      case Species::NitricOxide:
        pos = Isotopologue<Species::NitricOxide>().get_global_pos(t);
        break;
      case Species::SulfurDioxide:
        pos = Isotopologue<Species::SulfurDioxide>().get_global_pos(t);
        break;
      case Species::NitrogenDioxide:
        pos = Isotopologue<Species::NitrogenDioxide>().get_global_pos(t);
        break;
      case Species::Ammonia:
        pos = Isotopologue<Species::Ammonia>().get_global_pos(t);
        break;
      case Species::NitricAcid:
        pos = Isotopologue<Species::NitricAcid>().get_global_pos(t);
        break;
      case Species::Hydroxyl:
        pos = Isotopologue<Species::Hydroxyl>().get_global_pos(t);
        break;
      case Species::HydrogenFluoride:
        pos = Isotopologue<Species::HydrogenFluoride>().get_global_pos(t);
        break;
      case Species::HydrogenChloride:
        pos = Isotopologue<Species::HydrogenChloride>().get_global_pos(t);
        break;
      case Species::HydrogenBromide:
        pos = Isotopologue<Species::HydrogenBromide>().get_global_pos(t);
        break;
      case Species::HydrogenIodide:
        pos = Isotopologue<Species::HydrogenIodide>().get_global_pos(t);
        break;
      case Species::ChlorineMonoxide:
        pos = Isotopologue<Species::ChlorineMonoxide>().get_global_pos(t);
        break;
      case Species::CarbonylSulfide:
        pos = Isotopologue<Species::CarbonylSulfide>().get_global_pos(t);
        break;
      case Species::Formaldehyde:
        pos = Isotopologue<Species::Formaldehyde>().get_global_pos(t);
        break;
      case Species::HypochlorousAcid:
        pos = Isotopologue<Species::HypochlorousAcid>().get_global_pos(t);
        break;
      case Species::Nitrogen:
        pos = Isotopologue<Species::Nitrogen>().get_global_pos(t);
        break;
      case Species::HydrogenCyanide:
        pos = Isotopologue<Species::HydrogenCyanide>().get_global_pos(t);
        break;
      case Species::MethylChloride:
        pos = Isotopologue<Species::MethylChloride>().get_global_pos(t);
        break;
      case Species::HydrogenPeroxide:
        pos = Isotopologue<Species::HydrogenPeroxide>().get_global_pos(t);
        break;
      case Species::Acetylene:
        pos = Isotopologue<Species::Acetylene>().get_global_pos(t);
        break;
      case Species::Ethane:
        pos = Isotopologue<Species::Ethane>().get_global_pos(t);
        break;
      case Species::Phosphine:
        pos = Isotopologue<Species::Phosphine>().get_global_pos(t);
        break;
      case Species::CarbonylFluoride:
        pos = Isotopologue<Species::CarbonylFluoride>().get_global_pos(t);
        break;
      case Species::SulfurHexafluoride:
        pos = Isotopologue<Species::SulfurHexafluoride>().get_global_pos(t);
        break;
      case Species::HydrogenSulfide:
        pos = Isotopologue<Species::HydrogenSulfide>().get_global_pos(t);
        break;
      case Species::FormicAcid:
        pos = Isotopologue<Species::FormicAcid>().get_global_pos(t);
        break;
      case Species::Hydroperoxyl:
        pos = Isotopologue<Species::Hydroperoxyl>().get_global_pos(t);
        break;
      case Species::OxygenAtom:
        pos = Isotopologue<Species::OxygenAtom>().get_global_pos(t);
        break;
      case Species::ChlorineNitrate:
        pos = Isotopologue<Species::ChlorineNitrate>().get_global_pos(t);
        break;
      case Species::NitricOxideCation:
        pos = Isotopologue<Species::NitricOxideCation>().get_global_pos(t);
        break;
      case Species::HypobromousAcid:
        pos = Isotopologue<Species::HypobromousAcid>().get_global_pos(t);
        break;
      case Species::Ethylene:
        pos = Isotopologue<Species::Ethylene>().get_global_pos(t);
        break;
      case Species::Methanol:
        pos = Isotopologue<Species::Methanol>().get_global_pos(t);
        break;
      case Species::MethylBromide:
        pos = Isotopologue<Species::MethylBromide>().get_global_pos(t);
        break;
      case Species::Acetonitrile:
        pos = Isotopologue<Species::Acetonitrile>().get_global_pos(t);
        break;
      case Species::Pfc14:
        pos = Isotopologue<Species::Pfc14>().get_global_pos(t);
        break;
      case Species::Diacetylene:
        pos = Isotopologue<Species::Diacetylene>().get_global_pos(t);
        break;
      case Species::Cyanoacetylene:
        pos = Isotopologue<Species::Cyanoacetylene>().get_global_pos(t);
        break;
      case Species::Hydrogen:
        pos = Isotopologue<Species::Hydrogen>().get_global_pos(t);
        break;
      case Species::CarbonMonosulfide:
        pos = Isotopologue<Species::CarbonMonosulfide>().get_global_pos(t);
        break;
      case Species::SulfurTrioxide:
        pos = Isotopologue<Species::SulfurTrioxide>().get_global_pos(t);
        break;
      case Species::Cyanogen:
        pos = Isotopologue<Species::Cyanogen>().get_global_pos(t);
        break;
      case Species::Phosgene:
        pos = Isotopologue<Species::Phosgene>().get_global_pos(t);
        break;
      case Species::FINAL: { /*leave last*/
      }
    }

    if (pos not_eq std::numeric_limits<size_t>::max())
      return v[pos];
    else
      return Rational(0, 0);
  }

  bool set_local(std::vector<Quantum::Number>& v, const Quantum::Number& n,
                 Quantum::Type t) const noexcept {
    switch (s) {
      case Species::Bath:
        return false;
      case Species::Water:
        return Isotopologue<Species::Water>().set_local(v, n, t);
      case Species::CarbonDioxide:
        return Isotopologue<Species::CarbonDioxide>().set_local(v, n, t);
      case Species::Ozone:
        return Isotopologue<Species::Ozone>().set_local(v, n, t);
      case Species::NitrogenOxide:
        return Isotopologue<Species::NitrogenOxide>().set_local(v, n, t);
      case Species::CarbonMonoxide:
        return Isotopologue<Species::CarbonMonoxide>().set_local(v, n, t);
      case Species::Methane:
        return Isotopologue<Species::Methane>().set_local(v, n, t);
      case Species::Oxygen:
        return Isotopologue<Species::Oxygen>().set_local(v, n, t);
      case Species::NitricOxide:
        return Isotopologue<Species::NitricOxide>().set_local(v, n, t);
      case Species::SulfurDioxide:
        return Isotopologue<Species::SulfurDioxide>().set_local(v, n, t);
      case Species::NitrogenDioxide:
        return Isotopologue<Species::NitrogenDioxide>().set_local(v, n, t);
      case Species::Ammonia:
        return Isotopologue<Species::Ammonia>().set_local(v, n, t);
      case Species::NitricAcid:
        return Isotopologue<Species::NitricAcid>().set_local(v, n, t);
      case Species::Hydroxyl:
        return Isotopologue<Species::Hydroxyl>().set_local(v, n, t);
      case Species::HydrogenFluoride:
        return Isotopologue<Species::HydrogenFluoride>().set_local(v, n, t);
      case Species::HydrogenChloride:
        return Isotopologue<Species::HydrogenChloride>().set_local(v, n, t);
      case Species::HydrogenBromide:
        return Isotopologue<Species::HydrogenBromide>().set_local(v, n, t);
      case Species::HydrogenIodide:
        return Isotopologue<Species::HydrogenIodide>().set_local(v, n, t);
      case Species::ChlorineMonoxide:
        return Isotopologue<Species::ChlorineMonoxide>().set_local(v, n, t);
      case Species::CarbonylSulfide:
        return Isotopologue<Species::CarbonylSulfide>().set_local(v, n, t);
      case Species::Formaldehyde:
        return Isotopologue<Species::Formaldehyde>().set_local(v, n, t);
      case Species::HypochlorousAcid:
        return Isotopologue<Species::HypochlorousAcid>().set_local(v, n, t);
      case Species::Nitrogen:
        return Isotopologue<Species::Nitrogen>().set_local(v, n, t);
      case Species::HydrogenCyanide:
        return Isotopologue<Species::HydrogenCyanide>().set_local(v, n, t);
      case Species::MethylChloride:
        return Isotopologue<Species::MethylChloride>().set_local(v, n, t);
      case Species::HydrogenPeroxide:
        return Isotopologue<Species::HydrogenPeroxide>().set_local(v, n, t);
      case Species::Acetylene:
        return Isotopologue<Species::Acetylene>().set_local(v, n, t);
      case Species::Ethane:
        return Isotopologue<Species::Ethane>().set_local(v, n, t);
      case Species::Phosphine:
        return Isotopologue<Species::Phosphine>().set_local(v, n, t);
      case Species::CarbonylFluoride:
        return Isotopologue<Species::CarbonylFluoride>().set_local(v, n, t);
      case Species::SulfurHexafluoride:
        return Isotopologue<Species::SulfurHexafluoride>().set_local(v, n, t);
      case Species::HydrogenSulfide:
        return Isotopologue<Species::HydrogenSulfide>().set_local(v, n, t);
      case Species::FormicAcid:
        return Isotopologue<Species::FormicAcid>().set_local(v, n, t);
      case Species::Hydroperoxyl:
        return Isotopologue<Species::Hydroperoxyl>().set_local(v, n, t);
      case Species::OxygenAtom:
        return Isotopologue<Species::OxygenAtom>().set_local(v, n, t);
      case Species::ChlorineNitrate:
        return Isotopologue<Species::ChlorineNitrate>().set_local(v, n, t);
      case Species::NitricOxideCation:
        return Isotopologue<Species::NitricOxideCation>().set_local(v, n, t);
      case Species::HypobromousAcid:
        return Isotopologue<Species::HypobromousAcid>().set_local(v, n, t);
      case Species::Ethylene:
        return Isotopologue<Species::Ethylene>().set_local(v, n, t);
      case Species::Methanol:
        return Isotopologue<Species::Methanol>().set_local(v, n, t);
      case Species::MethylBromide:
        return Isotopologue<Species::MethylBromide>().set_local(v, n, t);
      case Species::Acetonitrile:
        return Isotopologue<Species::Acetonitrile>().set_local(v, n, t);
      case Species::Pfc14:
        return Isotopologue<Species::Pfc14>().set_local(v, n, t);
      case Species::Diacetylene:
        return Isotopologue<Species::Diacetylene>().set_local(v, n, t);
      case Species::Cyanoacetylene:
        return Isotopologue<Species::Cyanoacetylene>().set_local(v, n, t);
      case Species::Hydrogen:
        return Isotopologue<Species::Hydrogen>().set_local(v, n, t);
      case Species::CarbonMonosulfide:
        return Isotopologue<Species::CarbonMonosulfide>().set_local(v, n, t);
      case Species::SulfurTrioxide:
        return Isotopologue<Species::SulfurTrioxide>().set_local(v, n, t);
      case Species::Cyanogen:
        return Isotopologue<Species::Cyanogen>().set_local(v, n, t);
      case Species::Phosgene:
        return Isotopologue<Species::Phosgene>().set_local(v, n, t);
      case Species::FINAL: { /*leave last*/
      }
    }
    return {};
  }

  bool set_global(std::vector<Quantum::Number>& v, const Quantum::Number& n,
                  Quantum::Type t) const noexcept {
    switch (s) {
      case Species::Bath:
        return false;
      case Species::Water:
        return Isotopologue<Species::Water>().set_global(v, n, t);
      case Species::CarbonDioxide:
        return Isotopologue<Species::CarbonDioxide>().set_global(v, n, t);
      case Species::Ozone:
        return Isotopologue<Species::Ozone>().set_global(v, n, t);
      case Species::NitrogenOxide:
        return Isotopologue<Species::NitrogenOxide>().set_global(v, n, t);
      case Species::CarbonMonoxide:
        return Isotopologue<Species::CarbonMonoxide>().set_global(v, n, t);
      case Species::Methane:
        return Isotopologue<Species::Methane>().set_global(v, n, t);
      case Species::Oxygen:
        return Isotopologue<Species::Oxygen>().set_global(v, n, t);
      case Species::NitricOxide:
        return Isotopologue<Species::NitricOxide>().set_global(v, n, t);
      case Species::SulfurDioxide:
        return Isotopologue<Species::SulfurDioxide>().set_global(v, n, t);
      case Species::NitrogenDioxide:
        return Isotopologue<Species::NitrogenDioxide>().set_global(v, n, t);
      case Species::Ammonia:
        return Isotopologue<Species::Ammonia>().set_global(v, n, t);
      case Species::NitricAcid:
        return Isotopologue<Species::NitricAcid>().set_global(v, n, t);
      case Species::Hydroxyl:
        return Isotopologue<Species::Hydroxyl>().set_global(v, n, t);
      case Species::HydrogenFluoride:
        return Isotopologue<Species::HydrogenFluoride>().set_global(v, n, t);
      case Species::HydrogenChloride:
        return Isotopologue<Species::HydrogenChloride>().set_global(v, n, t);
      case Species::HydrogenBromide:
        return Isotopologue<Species::HydrogenBromide>().set_global(v, n, t);
      case Species::HydrogenIodide:
        return Isotopologue<Species::HydrogenIodide>().set_global(v, n, t);
      case Species::ChlorineMonoxide:
        return Isotopologue<Species::ChlorineMonoxide>().set_global(v, n, t);
      case Species::CarbonylSulfide:
        return Isotopologue<Species::CarbonylSulfide>().set_global(v, n, t);
      case Species::Formaldehyde:
        return Isotopologue<Species::Formaldehyde>().set_global(v, n, t);
      case Species::HypochlorousAcid:
        return Isotopologue<Species::HypochlorousAcid>().set_global(v, n, t);
      case Species::Nitrogen:
        return Isotopologue<Species::Nitrogen>().set_global(v, n, t);
      case Species::HydrogenCyanide:
        return Isotopologue<Species::HydrogenCyanide>().set_global(v, n, t);
      case Species::MethylChloride:
        return Isotopologue<Species::MethylChloride>().set_global(v, n, t);
      case Species::HydrogenPeroxide:
        return Isotopologue<Species::HydrogenPeroxide>().set_global(v, n, t);
      case Species::Acetylene:
        return Isotopologue<Species::Acetylene>().set_global(v, n, t);
      case Species::Ethane:
        return Isotopologue<Species::Ethane>().set_global(v, n, t);
      case Species::Phosphine:
        return Isotopologue<Species::Phosphine>().set_global(v, n, t);
      case Species::CarbonylFluoride:
        return Isotopologue<Species::CarbonylFluoride>().set_global(v, n, t);
      case Species::SulfurHexafluoride:
        return Isotopologue<Species::SulfurHexafluoride>().set_global(v, n, t);
      case Species::HydrogenSulfide:
        return Isotopologue<Species::HydrogenSulfide>().set_global(v, n, t);
      case Species::FormicAcid:
        return Isotopologue<Species::FormicAcid>().set_global(v, n, t);
      case Species::Hydroperoxyl:
        return Isotopologue<Species::Hydroperoxyl>().set_global(v, n, t);
      case Species::OxygenAtom:
        return Isotopologue<Species::OxygenAtom>().set_global(v, n, t);
      case Species::ChlorineNitrate:
        return Isotopologue<Species::ChlorineNitrate>().set_global(v, n, t);
      case Species::NitricOxideCation:
        return Isotopologue<Species::NitricOxideCation>().set_global(v, n, t);
      case Species::HypobromousAcid:
        return Isotopologue<Species::HypobromousAcid>().set_global(v, n, t);
      case Species::Ethylene:
        return Isotopologue<Species::Ethylene>().set_global(v, n, t);
      case Species::Methanol:
        return Isotopologue<Species::Methanol>().set_global(v, n, t);
      case Species::MethylBromide:
        return Isotopologue<Species::MethylBromide>().set_global(v, n, t);
      case Species::Acetonitrile:
        return Isotopologue<Species::Acetonitrile>().set_global(v, n, t);
      case Species::Pfc14:
        return Isotopologue<Species::Pfc14>().set_global(v, n, t);
      case Species::Diacetylene:
        return Isotopologue<Species::Diacetylene>().set_global(v, n, t);
      case Species::Cyanoacetylene:
        return Isotopologue<Species::Cyanoacetylene>().set_global(v, n, t);
      case Species::Hydrogen:
        return Isotopologue<Species::Hydrogen>().set_global(v, n, t);
      case Species::CarbonMonosulfide:
        return Isotopologue<Species::CarbonMonosulfide>().set_global(v, n, t);
      case Species::SulfurTrioxide:
        return Isotopologue<Species::SulfurTrioxide>().set_global(v, n, t);
      case Species::Cyanogen:
        return Isotopologue<Species::Cyanogen>().set_global(v, n, t);
      case Species::Phosgene:
        return Isotopologue<Species::Phosgene>().set_global(v, n, t);
      case Species::FINAL: { /*leave last*/
      }
    }
    return {};
  }

  std::vector<Quantum::Type> get_globalquantumnumbers() const noexcept {
    switch (s) {
      case Species::Bath:
        return {};
      case Species::Water:
        return Isotopologue<Species::Water>().globalQuantumNumbers();
      case Species::CarbonDioxide:
        return Isotopologue<Species::CarbonDioxide>().globalQuantumNumbers();
      case Species::Ozone:
        return Isotopologue<Species::Ozone>().globalQuantumNumbers();
      case Species::NitrogenOxide:
        return Isotopologue<Species::NitrogenOxide>().globalQuantumNumbers();
      case Species::CarbonMonoxide:
        return Isotopologue<Species::CarbonMonoxide>().globalQuantumNumbers();
      case Species::Methane:
        return Isotopologue<Species::Methane>().globalQuantumNumbers();
      case Species::Oxygen:
        return Isotopologue<Species::Oxygen>().globalQuantumNumbers();
      case Species::NitricOxide:
        return Isotopologue<Species::NitricOxide>().globalQuantumNumbers();
      case Species::SulfurDioxide:
        return Isotopologue<Species::SulfurDioxide>().globalQuantumNumbers();
      case Species::NitrogenDioxide:
        return Isotopologue<Species::NitrogenDioxide>().globalQuantumNumbers();
      case Species::Ammonia:
        return Isotopologue<Species::Ammonia>().globalQuantumNumbers();
      case Species::NitricAcid:
        return Isotopologue<Species::NitricAcid>().globalQuantumNumbers();
      case Species::Hydroxyl:
        return Isotopologue<Species::Hydroxyl>().globalQuantumNumbers();
      case Species::HydrogenFluoride:
        return Isotopologue<Species::HydrogenFluoride>().globalQuantumNumbers();
      case Species::HydrogenChloride:
        return Isotopologue<Species::HydrogenChloride>().globalQuantumNumbers();
      case Species::HydrogenBromide:
        return Isotopologue<Species::HydrogenBromide>().globalQuantumNumbers();
      case Species::HydrogenIodide:
        return Isotopologue<Species::HydrogenIodide>().globalQuantumNumbers();
      case Species::ChlorineMonoxide:
        return Isotopologue<Species::ChlorineMonoxide>().globalQuantumNumbers();
      case Species::CarbonylSulfide:
        return Isotopologue<Species::CarbonylSulfide>().globalQuantumNumbers();
      case Species::Formaldehyde:
        return Isotopologue<Species::Formaldehyde>().globalQuantumNumbers();
      case Species::HypochlorousAcid:
        return Isotopologue<Species::HypochlorousAcid>().globalQuantumNumbers();
      case Species::Nitrogen:
        return Isotopologue<Species::Nitrogen>().globalQuantumNumbers();
      case Species::HydrogenCyanide:
        return Isotopologue<Species::HydrogenCyanide>().globalQuantumNumbers();
      case Species::MethylChloride:
        return Isotopologue<Species::MethylChloride>().globalQuantumNumbers();
      case Species::HydrogenPeroxide:
        return Isotopologue<Species::HydrogenPeroxide>().globalQuantumNumbers();
      case Species::Acetylene:
        return Isotopologue<Species::Acetylene>().globalQuantumNumbers();
      case Species::Ethane:
        return Isotopologue<Species::Ethane>().globalQuantumNumbers();
      case Species::Phosphine:
        return Isotopologue<Species::Phosphine>().globalQuantumNumbers();
      case Species::CarbonylFluoride:
        return Isotopologue<Species::CarbonylFluoride>().globalQuantumNumbers();
      case Species::SulfurHexafluoride:
        return Isotopologue<Species::SulfurHexafluoride>()
            .globalQuantumNumbers();
      case Species::HydrogenSulfide:
        return Isotopologue<Species::HydrogenSulfide>().globalQuantumNumbers();
      case Species::FormicAcid:
        return Isotopologue<Species::FormicAcid>().globalQuantumNumbers();
      case Species::Hydroperoxyl:
        return Isotopologue<Species::Hydroperoxyl>().globalQuantumNumbers();
      case Species::OxygenAtom:
        return Isotopologue<Species::OxygenAtom>().globalQuantumNumbers();
      case Species::ChlorineNitrate:
        return Isotopologue<Species::ChlorineNitrate>().globalQuantumNumbers();
      case Species::NitricOxideCation:
        return Isotopologue<Species::NitricOxideCation>()
            .globalQuantumNumbers();
      case Species::HypobromousAcid:
        return Isotopologue<Species::HypobromousAcid>().globalQuantumNumbers();
      case Species::Ethylene:
        return Isotopologue<Species::Ethylene>().globalQuantumNumbers();
      case Species::Methanol:
        return Isotopologue<Species::Methanol>().globalQuantumNumbers();
      case Species::MethylBromide:
        return Isotopologue<Species::MethylBromide>().globalQuantumNumbers();
      case Species::Acetonitrile:
        return Isotopologue<Species::Acetonitrile>().globalQuantumNumbers();
      case Species::Pfc14:
        return Isotopologue<Species::Pfc14>().globalQuantumNumbers();
      case Species::Diacetylene:
        return Isotopologue<Species::Diacetylene>().globalQuantumNumbers();
      case Species::Cyanoacetylene:
        return Isotopologue<Species::Cyanoacetylene>().globalQuantumNumbers();
      case Species::Hydrogen:
        return Isotopologue<Species::Hydrogen>().globalQuantumNumbers();
      case Species::CarbonMonosulfide:
        return Isotopologue<Species::CarbonMonosulfide>()
            .globalQuantumNumbers();
      case Species::SulfurTrioxide:
        return Isotopologue<Species::SulfurTrioxide>().globalQuantumNumbers();
      case Species::Cyanogen:
        return Isotopologue<Species::Cyanogen>().globalQuantumNumbers();
      case Species::Phosgene:
        return Isotopologue<Species::Phosgene>().globalQuantumNumbers();
      case Species::FINAL: { /*leave last*/
      }
    }
    return {};
  }
  std::vector<Quantum::Type> get_localquantumnumbers() const noexcept {
    switch (s) {
      case Species::Bath:
        return {};
      case Species::Water:
        return Isotopologue<Species::Water>().localQuantumNumbers();
      case Species::CarbonDioxide:
        return Isotopologue<Species::CarbonDioxide>().localQuantumNumbers();
      case Species::Ozone:
        return Isotopologue<Species::Ozone>().localQuantumNumbers();
      case Species::NitrogenOxide:
        return Isotopologue<Species::NitrogenOxide>().localQuantumNumbers();
      case Species::CarbonMonoxide:
        return Isotopologue<Species::CarbonMonoxide>().localQuantumNumbers();
      case Species::Methane:
        return Isotopologue<Species::Methane>().localQuantumNumbers();
      case Species::Oxygen:
        return Isotopologue<Species::Oxygen>().localQuantumNumbers();
      case Species::NitricOxide:
        return Isotopologue<Species::NitricOxide>().localQuantumNumbers();
      case Species::SulfurDioxide:
        return Isotopologue<Species::SulfurDioxide>().localQuantumNumbers();
      case Species::NitrogenDioxide:
        return Isotopologue<Species::NitrogenDioxide>().localQuantumNumbers();
      case Species::Ammonia:
        return Isotopologue<Species::Ammonia>().localQuantumNumbers();
      case Species::NitricAcid:
        return Isotopologue<Species::NitricAcid>().localQuantumNumbers();
      case Species::Hydroxyl:
        return Isotopologue<Species::Hydroxyl>().localQuantumNumbers();
      case Species::HydrogenFluoride:
        return Isotopologue<Species::HydrogenFluoride>().localQuantumNumbers();
      case Species::HydrogenChloride:
        return Isotopologue<Species::HydrogenChloride>().localQuantumNumbers();
      case Species::HydrogenBromide:
        return Isotopologue<Species::HydrogenBromide>().localQuantumNumbers();
      case Species::HydrogenIodide:
        return Isotopologue<Species::HydrogenIodide>().localQuantumNumbers();
      case Species::ChlorineMonoxide:
        return Isotopologue<Species::ChlorineMonoxide>().localQuantumNumbers();
      case Species::CarbonylSulfide:
        return Isotopologue<Species::CarbonylSulfide>().localQuantumNumbers();
      case Species::Formaldehyde:
        return Isotopologue<Species::Formaldehyde>().localQuantumNumbers();
      case Species::HypochlorousAcid:
        return Isotopologue<Species::HypochlorousAcid>().localQuantumNumbers();
      case Species::Nitrogen:
        return Isotopologue<Species::Nitrogen>().localQuantumNumbers();
      case Species::HydrogenCyanide:
        return Isotopologue<Species::HydrogenCyanide>().localQuantumNumbers();
      case Species::MethylChloride:
        return Isotopologue<Species::MethylChloride>().localQuantumNumbers();
      case Species::HydrogenPeroxide:
        return Isotopologue<Species::HydrogenPeroxide>().localQuantumNumbers();
      case Species::Acetylene:
        return Isotopologue<Species::Acetylene>().localQuantumNumbers();
      case Species::Ethane:
        return Isotopologue<Species::Ethane>().localQuantumNumbers();
      case Species::Phosphine:
        return Isotopologue<Species::Phosphine>().localQuantumNumbers();
      case Species::CarbonylFluoride:
        return Isotopologue<Species::CarbonylFluoride>().localQuantumNumbers();
      case Species::SulfurHexafluoride:
        return Isotopologue<Species::SulfurHexafluoride>()
            .localQuantumNumbers();
      case Species::HydrogenSulfide:
        return Isotopologue<Species::HydrogenSulfide>().localQuantumNumbers();
      case Species::FormicAcid:
        return Isotopologue<Species::FormicAcid>().localQuantumNumbers();
      case Species::Hydroperoxyl:
        return Isotopologue<Species::Hydroperoxyl>().localQuantumNumbers();
      case Species::OxygenAtom:
        return Isotopologue<Species::OxygenAtom>().localQuantumNumbers();
      case Species::ChlorineNitrate:
        return Isotopologue<Species::ChlorineNitrate>().localQuantumNumbers();
      case Species::NitricOxideCation:
        return Isotopologue<Species::NitricOxideCation>().localQuantumNumbers();
      case Species::HypobromousAcid:
        return Isotopologue<Species::HypobromousAcid>().localQuantumNumbers();
      case Species::Ethylene:
        return Isotopologue<Species::Ethylene>().localQuantumNumbers();
      case Species::Methanol:
        return Isotopologue<Species::Methanol>().localQuantumNumbers();
      case Species::MethylBromide:
        return Isotopologue<Species::MethylBromide>().localQuantumNumbers();
      case Species::Acetonitrile:
        return Isotopologue<Species::Acetonitrile>().localQuantumNumbers();
      case Species::Pfc14:
        return Isotopologue<Species::Pfc14>().localQuantumNumbers();
      case Species::Diacetylene:
        return Isotopologue<Species::Diacetylene>().localQuantumNumbers();
      case Species::Cyanoacetylene:
        return Isotopologue<Species::Cyanoacetylene>().localQuantumNumbers();
      case Species::Hydrogen:
        return Isotopologue<Species::Hydrogen>().localQuantumNumbers();
      case Species::CarbonMonosulfide:
        return Isotopologue<Species::CarbonMonosulfide>().localQuantumNumbers();
      case Species::SulfurTrioxide:
        return Isotopologue<Species::SulfurTrioxide>().localQuantumNumbers();
      case Species::Cyanogen:
        return Isotopologue<Species::Cyanogen>().localQuantumNumbers();
      case Species::Phosgene:
        return Isotopologue<Species::Phosgene>().localQuantumNumbers();
      case Species::FINAL: { /*leave last*/
      }
    }
    return {};
  }
};  // Isotope

AtomInfo getAtomInfo(Atom x) noexcept;
}  // namespace Species

#endif  // species_h
