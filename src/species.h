#ifndef species_h
#define species_h

#include <vector>

#include "enums.h"
#include "quantum.h"

namespace Species {
/** named species */
ENUMCLASS(Species, unsigned char,
          Bath,
          Water,
          CarbonDioxide,
          Ozone,
          NitrogenOxide,
          CarbonMonoxide,
          Methane,
          Oxygen,
          NitricOxide,
          SulfurDioxide,
          NitrogenDioxide,
          Ammonia,
          NitricAcid,
          Hydroxyl,
          HydrogenFluoride,
          HydrogenChloride,
          HydrogenBromide,
          HydrogenIodide,
          ChlorineMonoxide,
          CarbonylSulfide,
          Formaldehyde,
          HypochlorousAcid,
          Nitrogen,
          HydrogenCyanide,
          MethylChloride,
          HydrogenPeroxide,
          Acetylene,
          Ethane,
          Phosphine,
          CarbonylFluoride,
          SulfurHexafluoride,
          HydrogenSulfide,
          FormicAcid,
          Hydroperoxyl,
          OxygenAtom,
          ChlorineNitrate,
          NitricOxideCation,
          HypobromousAcid,
          Ethylene,
          Methanol,
          MethylBromide,
          Acetonitrile,
          Pfc14,
          Diacetylene,
          Cyanoacetylene,
          Hydrogen,
          CarbonMonosulfide,
          SulfurTrioxide,
          Cyanogen,
          Phosgene
)  // Species

std::string toShortName(Species x) noexcept;
Species fromShortName(const std::string& x) noexcept;

/** named atoms */
ENUMCLASS(Atom, unsigned char,
          H, D, T, He4, He3, Li7, Li6, Be9, B11, B10, C12, C13, N14, N15, O16, O17, O18, F19, Ne20, Ne21, Ne22, Na23, Mg24, Mg25, Mg26, Al27, Si28, Si29, Si30, P31, S32, S33, S34, Cl35, Cl37, Ar40, Ar36, Ar38, K39, K40, K41, Ca40, Ca42, Ca43, Ca44, Ca46, Ca48, Sc45, Ti46, Ti47, Ti48, Ti49, Ti50, V51, V50, Cr52, Cr50, Cr53, Cr54, Mn55, Fe56, Fe54, Fe57, Fe58, Co59, Ni58, Cu63, Zn64, Ga69, Ge74, As75, Se80, Br79, Br81, Kr84, Rb85, Rb87, Sr88, Y89, Zr90, Nb93, Mo98, Mo92, Mo94, Mo95, Mo96, Mo100, Tc99, Ru102, Ru99, Ru100, Ru101, Ru104, Rh103, Pd106, Pd104, Pd105, Pd108, Ag107, Ag109, Cd114, Cd110, Cd111, Cd112, Cd113, Cd116, In115, In113, Sn120, Sb121, Sb123, Te130, I127, Xe132, Xe129, Xe131, Cs133, Ba138, La139, Ce140, Pr141, Nd142, Nd143, Nd144, Nd145, Nd146, Pm0, Sm144, Sm150, Sm152, Sm154, Eu151, Eu153, Gd154, Gd155, Gd156, Gd157, Gd158, Gd160, Tb159, Dy156, Dy158, Dy160, Dy161, Dy162, Dy163, Dy164, Ho165, Er162, Er164, Er166, Er167, Er168, Er170, Tm169, Yb168, Yb170, Yb171, Yb172, Yb173, Yb174, Yb176, Lu175, Hf176, Hf177, Hf178, Hf179, Hf180, Ta181, W180, W182, W184, W186, Re185, Os187, Os188, Os189, Os190, Os192, Ir191, Ir193, Pt192, Pt194, Pt195, Pt196, Pt198, Au197, Hg196, Hg198, Hg199, Hg200, Hg201, Hg202, Hg204, Tl203, Tl205, Pb206, Pb207, Pb208, Bi209, Po0, At0, Rn0, Fr0, Ra0, Ac0, Th232, Pa0, U235, U238, Np0, Pu0, Am0, Cm0, Bk0, Cf0, Es0, Fm0)

/** Information about atoms */
struct AtomInfo {std::string name; double nuclear_spin;};

/** Number of atoms for a species */
constexpr unsigned char getAtomCount(Species s) {
  switch(s) {
    case Species::Bath: return 0;
    case Species::Water: return 3;
    case Species::CarbonDioxide: return 3;
    case Species::Ozone: return 3;
    case Species::NitrogenOxide: return 3;
    case Species::CarbonMonoxide: return 2;
    case Species::Methane: return 5;
    case Species::Oxygen: return 2;
    case Species::NitricOxide: return 2;
    case Species::SulfurDioxide: return 3;
    case Species::NitrogenDioxide: return 3;
    case Species::Ammonia: return 4;
    case Species::NitricAcid: return 5;
    case Species::Hydroxyl: return 2;
    case Species::HydrogenFluoride: return 2;
    case Species::HydrogenChloride: return 2;
    case Species::HydrogenBromide: return 2;
    case Species::HydrogenIodide: return 2;
    case Species::ChlorineMonoxide: return 2;
    case Species::CarbonylSulfide: return 3;
    case Species::Formaldehyde: return 4;
    case Species::HypochlorousAcid: return 3;
    case Species::Nitrogen: return 2;
    case Species::HydrogenCyanide: return 3;
    case Species::MethylChloride: return 5;
    case Species::HydrogenPeroxide: return 4;
    case Species::Acetylene: return 4;
    case Species::Ethane: return 8;
    case Species::Phosphine: return 4;
    case Species::CarbonylFluoride: return 4;
    case Species::SulfurHexafluoride: return 7;
    case Species::HydrogenSulfide: return 3;
    case Species::FormicAcid: return 5;
    case Species::Hydroperoxyl: return 3;
    case Species::OxygenAtom: return 1;
    case Species::ChlorineNitrate: return 5;
    case Species::NitricOxideCation: return 2;
    case Species::HypobromousAcid: return 3;
    case Species::Ethylene: return 6;
    case Species::Methanol: return 6;
    case Species::MethylBromide: return 5;
    case Species::Acetonitrile: return 6;
    case Species::Pfc14: return 5;
    case Species::Diacetylene: return 6;
    case Species::Cyanoacetylene: return 5;
    case Species::Hydrogen: return 2;
    case Species::CarbonMonosulfide: return 2;
    case Species::SulfurTrioxide: return 4;
    case Species::Cyanogen: return 4;
    case Species::Phosgene: return 4;
    case Species::FINAL: {/*leave last*/}
  }
  return std::numeric_limits<unsigned char>::max();
}

/** Number of isotopes considered for a species */
constexpr unsigned char getIsotopologueCount(Species s) {
  switch(s) {
    case Species::Bath: return 0;
    case Species::Water: return 7;
    case Species::CarbonDioxide: return 12;
    case Species::Ozone: return 5;
    case Species::NitrogenOxide: return 5;
    case Species::CarbonMonoxide: return 6;
    case Species::Methane: return 4;
    case Species::Oxygen: return 3;
    case Species::NitricOxide: return 3;
    case Species::SulfurDioxide: return 2;
    case Species::NitrogenDioxide: return 2;
    case Species::Ammonia: return 2;
    case Species::NitricAcid: return 2;
    case Species::Hydroxyl: return 3;
    case Species::HydrogenFluoride: return 2;
    case Species::HydrogenChloride: return 4;
    case Species::HydrogenBromide: return 4;
    case Species::HydrogenIodide: return 2;
    case Species::ChlorineMonoxide: return 2;
    case Species::CarbonylSulfide: return 5;
    case Species::Formaldehyde: return 3;
    case Species::HypochlorousAcid: return 2;
    case Species::Nitrogen: return 2;
    case Species::HydrogenCyanide: return 3;
    case Species::MethylChloride: return 2;
    case Species::HydrogenPeroxide: return 1;
    case Species::Acetylene: return 3;
    case Species::Ethane: return 2;
    case Species::Phosphine: return 1;
    case Species::CarbonylFluoride: return 2;
    case Species::SulfurHexafluoride: return 1;
    case Species::HydrogenSulfide: return 3;
    case Species::FormicAcid: return 1;
    case Species::Hydroperoxyl: return 1;
    case Species::OxygenAtom: return 1;
    case Species::ChlorineNitrate: return 2;
    case Species::NitricOxideCation: return 1;
    case Species::HypobromousAcid: return 2;
    case Species::Ethylene: return 2;
    case Species::Methanol: return 1;
    case Species::MethylBromide: return 2;
    case Species::Acetonitrile: return 1;
    case Species::Pfc14: return 1;
    case Species::Diacetylene: return 1;
    case Species::Cyanoacetylene: return 1;
    case Species::Hydrogen: return 2;
    case Species::CarbonMonosulfide: return 4;
    case Species::SulfurTrioxide: return 1;
    case Species::Cyanogen: return 1;
    case Species::Phosgene: return 2;
    case Species::FINAL: {/*leave last*/}
  }
  return std::numeric_limits<unsigned char>::max();
}

/** Number of global quantum numbers considered for a species */
constexpr unsigned char getGlobalQuantumNumberCount(Species s) {
  switch(s) {
    case Species::Bath: return 0;
    case Species::Water: return 4;
    case Species::CarbonDioxide: return 7;
    case Species::Ozone: return 5;
    case Species::NitrogenOxide: return 6;
    case Species::CarbonMonoxide: return 2;
    case Species::Methane: return 11;
    case Species::Oxygen: return 4;
    case Species::NitricOxide: return 6;
    case Species::SulfurDioxide: return 4;
    case Species::NitrogenDioxide: return 5;
    case Species::Ammonia: return 12;
    case Species::NitricAcid: return 10;
    case Species::Hydroxyl: return 6;
    case Species::HydrogenFluoride: return 2;
    case Species::HydrogenChloride: return 2;
    case Species::HydrogenBromide: return 2;
    case Species::HydrogenIodide: return 2;
    case Species::ChlorineMonoxide: return 6;
    case Species::CarbonylSulfide: return 6;
    case Species::Formaldehyde: return 7;
    case Species::HypochlorousAcid: return 4;
    case Species::Nitrogen: return 2;
    case Species::HydrogenCyanide: return 6;
    case Species::MethylChloride: return 9;
    case Species::HydrogenPeroxide: return 8;
    case Species::Acetylene: return 12;
    case Species::Ethane: return 9;
    case Species::Phosphine: return 7;
    case Species::CarbonylFluoride: return 7;
    case Species::SulfurHexafluoride: return 0;
    case Species::HydrogenSulfide: return 4;
    case Species::FormicAcid: return 10;
    case Species::Hydroperoxyl: return 5;
    case Species::OxygenAtom: return 3;
    case Species::ChlorineNitrate: return 0;
    case Species::NitricOxideCation: return 2;
    case Species::HypobromousAcid: return 4;
    case Species::Ethylene: return 10;
    case Species::Methanol: return 11;
    case Species::MethylBromide: return 6;
    case Species::Acetonitrile: return 3;
    case Species::Pfc14: return 0;
    case Species::Diacetylene: return 17;
    case Species::Cyanoacetylene: return 13;
    case Species::Hydrogen: return 2;
    case Species::CarbonMonosulfide: return 2;
    case Species::SulfurTrioxide: return 1;
    case Species::Cyanogen: return 11;
    case Species::Phosgene: return 12;
    case Species::FINAL: {/*leave last*/}
  }
  return std::numeric_limits<unsigned char>::max();
}

/** Number of local quantum numbers considered for a species */
constexpr unsigned char getLocalQuantumNumberCount(Species s) {
  switch(s) {
    case Species::Bath: return 0;
    case Species::Water: return 3;
    case Species::CarbonDioxide: return 2;
    case Species::Ozone: return 4;
    case Species::NitrogenOxide: return 1;
    case Species::CarbonMonoxide: return 1;
    case Species::Methane: return 2;
    case Species::Oxygen: return 3;
    case Species::NitricOxide: return 3;
    case Species::SulfurDioxide: return 3;
    case Species::NitrogenDioxide: return 5;
    case Species::Ammonia: return 2;
    case Species::NitricAcid: return 4;
    case Species::Hydroxyl: return 4;
    case Species::HydrogenFluoride: return 1;
    case Species::HydrogenChloride: return 2;
    case Species::HydrogenBromide: return 2;
    case Species::HydrogenIodide: return 2;
    case Species::ChlorineMonoxide: return 3;
    case Species::CarbonylSulfide: return 1;
    case Species::Formaldehyde: return 3;
    case Species::HypochlorousAcid: return 3;
    case Species::Nitrogen: return 1;
    case Species::HydrogenCyanide: return 2;
    case Species::MethylChloride: return 3;
    case Species::HydrogenPeroxide: return 3;
    case Species::Acetylene: return 1;
    case Species::Ethane: return 2;
    case Species::Phosphine: return 2;
    case Species::CarbonylFluoride: return 3;
    case Species::SulfurHexafluoride: return 0;
    case Species::HydrogenSulfide: return 3;
    case Species::FormicAcid: return 3;
    case Species::Hydroperoxyl: return 5;
    case Species::OxygenAtom: return 2;
    case Species::ChlorineNitrate: return 0;
    case Species::NitricOxideCation: return 1;
    case Species::HypobromousAcid: return 3;
    case Species::Ethylene: return 3;
    case Species::Methanol: return 2;
    case Species::MethylBromide: return 2;
    case Species::Acetonitrile: return 2;
    case Species::Pfc14: return 0;
    case Species::Diacetylene: return 1;
    case Species::Cyanoacetylene: return 1;
    case Species::Hydrogen: return 1;
    case Species::CarbonMonosulfide: return 1;
    case Species::SulfurTrioxide: return 2;
    case Species::Cyanogen: return 1;
    case Species::Phosgene: return 3;
    case Species::FINAL: {/*leave last*/}
  }
  return std::numeric_limits<unsigned char>::max();
}

/** A struct representing an atom and a charge (defaults to 0 charge) */
struct ChargedAtom {
  Atom atom;
  char charge;
  constexpr ChargedAtom() noexcept : atom(Atom::FINAL), charge(0) {}
  constexpr ChargedAtom(Atom a, char c=0) noexcept : atom(a), charge(c) {}
  
  friend std::ostream& operator<<(std::ostream& os, ChargedAtom ca) {
    return os << ca.atom << ' ' << int(ca.charge);}
  friend std::istream& operator>>(std::istream& is, ChargedAtom& ca) {
    return is >> ca.atom >> ca.charge;}
  friend constexpr bool operator==(ChargedAtom a, ChargedAtom b) {return (a.atom == b.atom) and (a.charge == b.charge);}
};
constexpr ChargedAtom operator+(Atom a, int b) {return ChargedAtom(a, b);}

/** Struct containing all information needed about one isotope */
template <Species S>
struct IsotopeRecord {
  static constexpr unsigned char N = getAtomCount(S);
  std::array<ChargedAtom, N> atoms;
  double mass;
  long gi;
  constexpr IsotopeRecord() noexcept : atoms(), mass(0), gi(0) {}
  constexpr IsotopeRecord(std::array<ChargedAtom, N> a, double b, long c) noexcept : atoms(a), mass(b), gi(c) {}
};

/** Static data variable for looking up information about the species at runtime and at compile time */
template <Species S>
constexpr std::array<IsotopeRecord<S>, getIsotopologueCount(S)>
isotopeRecords() {
  using CA = ChargedAtom;
  using IR = IsotopeRecord<S>;
  using SA = std::array<CA, IR::N>;
  if constexpr (S == Species::Water) { // H2O
    return {
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::H}, CA{Atom::O16}}, 18.010565 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::H}, CA{Atom::O18}}, 20.014811 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::H}, CA{Atom::O17}}, 19.01478 /*grams/mol*/, 6 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::D}, CA{Atom::O16}}, 19.01674 /*grams/mol*/, 6 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::D}, CA{Atom::O18}}, 21.020985 /*grams/mol*/, 6 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::D}, CA{Atom::O17}}, 20.020956 /*grams/mol*/, 36 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::D}, CA{Atom::D}, CA{Atom::O16}}, 20.022915 /*grams/mol*/, 1 /*gi*/}};
  } else if constexpr (S == Species::CarbonDioxide) { // CO2
    return {
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::O16}, CA{Atom::O16}}, 43.98983 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C13}, CA{Atom::O16}, CA{Atom::O16}}, 44.993185 /*grams/mol*/, 2 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::O16}, CA{Atom::C12}, CA{Atom::O18}}, 45.994076 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::O16}, CA{Atom::C12}, CA{Atom::O17}}, 44.994045 /*grams/mol*/, 6 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::O16}, CA{Atom::C13}, CA{Atom::O18}}, 46.997431 /*grams/mol*/, 2 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::O16}, CA{Atom::C13}, CA{Atom::O17}}, 45.9974 /*grams/mol*/, 12 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::O18}, CA{Atom::O18}}, 47.998322 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::O17}, CA{Atom::C12}, CA{Atom::O18}}, 46.998291 /*grams/mol*/, 6 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::O17}, CA{Atom::O17}}, 45.998262 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C13}, CA{Atom::O18}, CA{Atom::O18}}, 49.001675 /*grams/mol*/, 2 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::O18}, CA{Atom::C13}, CA{Atom::O17}}, 48.001646 /*grams/mol*/, 12 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C13}, CA{Atom::O17}, CA{Atom::O17}}, 47.0016182378 /*grams/mol*/, 2 /*gi*/}};
  } else if constexpr (S == Species::Ozone) { // O3
    return {
      IR{/*atoms=*/SA{CA{Atom::O16}, CA{Atom::O16}, CA{Atom::O16}}, 47.984745 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::O16}, CA{Atom::O16}, CA{Atom::O18}}, 49.988991 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::O16}, CA{Atom::O18}, CA{Atom::O16}}, 49.988991 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::O16}, CA{Atom::O16}, CA{Atom::O17}}, 48.98896 /*grams/mol*/, 6 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::O16}, CA{Atom::O17}, CA{Atom::O16}}, 48.98896 /*grams/mol*/, 6 /*gi*/}};
  } else if constexpr (S == Species::NitrogenOxide) { // N2O
    return {
      IR{/*atoms=*/SA{CA{Atom::N14}, CA{Atom::N14}, CA{Atom::O16}}, 44.001062 /*grams/mol*/, 9 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::N14}, CA{Atom::N15}, CA{Atom::O16}}, 44.998096 /*grams/mol*/, 6 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::N15}, CA{Atom::N14}, CA{Atom::O16}}, 44.998096 /*grams/mol*/, 6 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::N14}, CA{Atom::N14}, CA{Atom::O18}}, 46.005308 /*grams/mol*/, 9 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::N14}, CA{Atom::N14}, CA{Atom::O17}}, 45.005278 /*grams/mol*/, 54 /*gi*/}};
  } else if constexpr (S == Species::CarbonMonoxide) { // CO
    return {
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::O16}}, 27.994915 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C13}, CA{Atom::O16}}, 28.99827 /*grams/mol*/, 2 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::O18}}, 29.999161 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::O17}}, 28.99913 /*grams/mol*/, 6 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C13}, CA{Atom::O18}}, 31.002516 /*grams/mol*/, 2 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C13}, CA{Atom::O17}}, 30.002485 /*grams/mol*/, 12 /*gi*/}};
  } else if constexpr (S == Species::Methane) { // CH4
    return {
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}}, 16.0313 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C13}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}}, 17.034655 /*grams/mol*/, 2 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}, CA{Atom::D}}, 17.037475 /*grams/mol*/, 3 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C13}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}, CA{Atom::D}}, 18.04083 /*grams/mol*/, 6 /*gi*/}};
  } else if constexpr (S == Species::Oxygen) { // O2
    return {
      IR{/*atoms=*/SA{CA{Atom::O16}, CA{Atom::O16}}, 31.98983 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::O16}, CA{Atom::O18}}, 33.994076 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::O16}, CA{Atom::O17}}, 32.994045 /*grams/mol*/, 6 /*gi*/}};
  } else if constexpr (S == Species::NitricOxide) { // NO
    return {
      IR{/*atoms=*/SA{CA{Atom::N14}, CA{Atom::O16}}, 29.997989 /*grams/mol*/, 3 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::N15}, CA{Atom::O16}}, 30.995023 /*grams/mol*/, 2 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::N14}, CA{Atom::O18}}, 32.002234 /*grams/mol*/, 3 /*gi*/}};
  } else if constexpr (S == Species::SulfurDioxide) { // SO2
    return {
      IR{/*atoms=*/SA{CA{Atom::S32}, CA{Atom::O16}, CA{Atom::O16}}, 63.961901 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::S34}, CA{Atom::O16}, CA{Atom::O16}}, 65.957695 /*grams/mol*/, 1 /*gi*/}};
  } else if constexpr (S == Species::NitrogenDioxide) { // NO2
    return {
      IR{/*atoms=*/SA{CA{Atom::N14}, CA{Atom::O16}, CA{Atom::O16}}, 45.992904 /*grams/mol*/, 3 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::N15}, CA{Atom::O16}, CA{Atom::O16}}, 46.989938 /*grams/mol*/, 2 /*gi*/}};
  } else if constexpr (S == Species::Ammonia) { // NH3
    return {
      IR{/*atoms=*/SA{CA{Atom::N14}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}}, 17.026549 /*grams/mol*/, 3 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::N15}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}}, 18.023583 /*grams/mol*/, 2 /*gi*/}};
  } else if constexpr (S == Species::NitricAcid) { // HNO3
    return {
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::N14}, CA{Atom::O16}, CA{Atom::O16}, CA{Atom::O16}}, 62.995644 /*grams/mol*/, 6 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::N15}, CA{Atom::O16}, CA{Atom::O16}, CA{Atom::O16}}, 63.99268 /*grams/mol*/, 4 /*gi*/}};
  } else if constexpr (S == Species::Hydroxyl) { // OH
    return {
      IR{/*atoms=*/SA{CA{Atom::O16}, CA{Atom::H}}, 17.00274 /*grams/mol*/, 2 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::O18}, CA{Atom::H}}, 19.006986 /*grams/mol*/, 2 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::O16}, CA{Atom::D}}, 18.008915 /*grams/mol*/, 3 /*gi*/}};
  } else if constexpr (S == Species::HydrogenFluoride) { // HF
    return {
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::F19}}, 20.006229 /*grams/mol*/, 4 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::D}, CA{Atom::F19}}, 21.012404 /*grams/mol*/, 6 /*gi*/}};
  } else if constexpr (S == Species::HydrogenChloride) { // HCl
    return {
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::Cl35}}, 35.976678 /*grams/mol*/, 8 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::Cl37}}, 37.973729 /*grams/mol*/, 8 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::D}, CA{Atom::Cl35}}, 36.982853 /*grams/mol*/, 12 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::D}, CA{Atom::Cl37}}, 38.979904 /*grams/mol*/, 12 /*gi*/}};
  } else if constexpr (S == Species::HydrogenBromide) { // HBr
    return {
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::Br79}}, 79.92616 /*grams/mol*/, 8 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::Br81}}, 81.924115 /*grams/mol*/, 8 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::D}, CA{Atom::Br79}}, 80.932336 /*grams/mol*/, 12 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::D}, CA{Atom::Br81}}, 82.930289 /*grams/mol*/, 12 /*gi*/}};
  } else if constexpr (S == Species::HydrogenIodide) { // HI
    return {
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::I127}}, 127.912297 /*grams/mol*/, 12 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::D}, CA{Atom::I127}}, 128.918472 /*grams/mol*/, 18 /*gi*/}};
  } else if constexpr (S == Species::ChlorineMonoxide) { // ClO
    return {
      IR{/*atoms=*/SA{CA{Atom::Cl35}, CA{Atom::O16}}, 50.963768 /*grams/mol*/, 4 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::Cl37}, CA{Atom::O16}}, 52.960819 /*grams/mol*/, 4 /*gi*/}};
  } else if constexpr (S == Species::CarbonylSulfide) { // OCS
    return {
      IR{/*atoms=*/SA{CA{Atom::O16}, CA{Atom::C12}, CA{Atom::S32}}, 59.966986 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::O16}, CA{Atom::C12}, CA{Atom::S34}}, 61.96278 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::O16}, CA{Atom::C13}, CA{Atom::S32}}, 60.970341 /*grams/mol*/, 2 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::O16}, CA{Atom::C12}, CA{Atom::S33}}, 60.966371 /*grams/mol*/, 4 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::O18}, CA{Atom::C12}, CA{Atom::S32}}, 61.971231 /*grams/mol*/, 1 /*gi*/}};
  } else if constexpr (S == Species::Formaldehyde) { // H2CO
    return {
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::H}, CA{Atom::C12}, CA{Atom::O16}}, 30.010565 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::H}, CA{Atom::C13}, CA{Atom::O16}}, 31.01392 /*grams/mol*/, 2 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::H}, CA{Atom::C12}, CA{Atom::O18}}, 32.014811 /*grams/mol*/, 1 /*gi*/}};
  } else if constexpr (S == Species::HypochlorousAcid) { // HOCl
    return {
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::O16}, CA{Atom::Cl35}}, 51.971593 /*grams/mol*/, 8 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::O16}, CA{Atom::Cl37}}, 53.968644 /*grams/mol*/, 8 /*gi*/}};
  } else if constexpr (S == Species::Nitrogen) { // N2
    return {
      IR{/*atoms=*/SA{CA{Atom::N14}, CA{Atom::N14}}, 28.006148 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::N14}, CA{Atom::N15}}, 29.003182 /*grams/mol*/, 6 /*gi*/}};
  } else if constexpr (S == Species::HydrogenCyanide) { // HCN
    return {
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::C12}, CA{Atom::N14}}, 27.010899 /*grams/mol*/, 6 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::C13}, CA{Atom::N14}}, 28.014254 /*grams/mol*/, 12 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::C12}, CA{Atom::N15}}, 28.007933 /*grams/mol*/, 4 /*gi*/}};
  } else if constexpr (S == Species::MethylChloride) { // CH3Cl
    return {
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}, CA{Atom::Cl35}}, 49.992328 /*grams/mol*/, 4 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}, CA{Atom::Cl37}}, 51.989379 /*grams/mol*/, 4 /*gi*/}};
  } else if constexpr (S == Species::HydrogenPeroxide) { // H2O2
    return {
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::H}, CA{Atom::O16}, CA{Atom::O16}}, 34.00548 /*grams/mol*/, 1 /*gi*/}};
  } else if constexpr (S == Species::Acetylene) { // C2H2
    return {
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::C12}, CA{Atom::H}, CA{Atom::H}}, 26.01565 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::C12}, CA{Atom::C13}, CA{Atom::H}}, 27.019005 /*grams/mol*/, 8 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::C12}, CA{Atom::C12}, CA{Atom::D}}, 27.021825 /*grams/mol*/, 6 /*gi*/}};
  } else if constexpr (S == Species::Ethane) { // C2H6
    return {
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::C12}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}}, 30.04695 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}, CA{Atom::C13}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}}, 31.050305 /*grams/mol*/, 2 /*gi*/}};
  } else if constexpr (S == Species::Phosphine) { // PH3
    return {
      IR{/*atoms=*/SA{CA{Atom::P31}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}}, 33.997238 /*grams/mol*/, 2 /*gi*/}};
  } else if constexpr (S == Species::CarbonylFluoride) { // COF2
    return {
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::O16}, CA{Atom::F19}, CA{Atom::F19}}, 65.991722 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C13}, CA{Atom::O16}, CA{Atom::F19}, CA{Atom::F19}}, 66.995083 /*grams/mol*/, 2 /*gi*/}};
  } else if constexpr (S == Species::SulfurHexafluoride) { // SF6
    return {
      IR{/*atoms=*/SA{CA{Atom::S32}, CA{Atom::F19}, CA{Atom::F19}, CA{Atom::F19}, CA{Atom::F19}, CA{Atom::F19}, CA{Atom::F19}}, 145.962492 /*grams/mol*/, 1 /*gi*/}};
  } else if constexpr (S == Species::HydrogenSulfide) { // H2S
    return {
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::H}, CA{Atom::S32}}, 33.987721 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::H}, CA{Atom::S34}}, 35.983515 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::H}, CA{Atom::S33}}, 34.987105 /*grams/mol*/, 4 /*gi*/}};
  } else if constexpr (S == Species::FormicAcid) { // HCOOH
    return {
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::C12}, CA{Atom::O16}, CA{Atom::O16}, CA{Atom::H}}, 46.00548 /*grams/mol*/, 4 /*gi*/}};
  } else if constexpr (S == Species::Hydroperoxyl) { // HO2
    return {
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::O16}, CA{Atom::O16}}, 32.997655 /*grams/mol*/, 2 /*gi*/}};
  } else if constexpr (S == Species::OxygenAtom) { // O
    return {
      IR{/*atoms=*/SA{CA{Atom::O16}}, 15.994915 /*grams/mol*/, 1 /*gi*/}};
  } else if constexpr (S == Species::ChlorineNitrate) { // ClONO2
    return {
      IR{/*atoms=*/SA{CA{Atom::Cl35}, CA{Atom::O16}, CA{Atom::N14}, CA{Atom::O16}, CA{Atom::O16}}, 96.956672 /*grams/mol*/, 12 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::Cl37}, CA{Atom::O16}, CA{Atom::N14}, CA{Atom::O16}, CA{Atom::O16}}, 98.953723 /*grams/mol*/, 12 /*gi*/}};
  } else if constexpr (S == Species::NitricOxideCation) { // NO+
    return {
      IR{/*atoms=*/SA{CA{Atom::N14}, CA{Atom::O16, +1}}, 29.997989 /*grams/mol*/, 3 /*gi*/}};
  } else if constexpr (S == Species::HypobromousAcid) { // HOBr
    return {
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::O16}, CA{Atom::Br79}}, 95.921076 /*grams/mol*/, 8 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::O16}, CA{Atom::Br81}}, 97.919027 /*grams/mol*/, 8 /*gi*/}};
  } else if constexpr (S == Species::Ethylene) { // C2H4
    return {
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::C12}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}}, 28.0313 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::H}, CA{Atom::H}, CA{Atom::C13}, CA{Atom::H}, CA{Atom::H}}, 29.034655 /*grams/mol*/, 2 /*gi*/}};
  } else if constexpr (S == Species::Methanol) { // CH3OH
    return {
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}, CA{Atom::O16}, CA{Atom::H}}, 32.026215 /*grams/mol*/, 2 /*gi*/}};
  } else if constexpr (S == Species::MethylBromide) { // CH3Br
    return {
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}, CA{Atom::Br79}}, 93.941811 /*grams/mol*/, 4 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}, CA{Atom::Br81}}, 95.939764 /*grams/mol*/, 4 /*gi*/}};
  } else if constexpr (S == Species::Acetonitrile) { // CH3CN
    return {
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::H}, CA{Atom::H}, CA{Atom::H}, CA{Atom::C12}, CA{Atom::N14}}, 41.026549 /*grams/mol*/, 3 /*gi*/}};
  } else if constexpr (S == Species::Pfc14) { // CF4
    return {
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::F19}, CA{Atom::F19}, CA{Atom::F19}, CA{Atom::F19}}, 87.993616 /*grams/mol*/, 1 /*gi*/}};
  } else if constexpr (S == Species::Diacetylene) { // C4H2
    return {
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::C12}, CA{Atom::C12}, CA{Atom::C12}, CA{Atom::H}, CA{Atom::H}}, 50.01565 /*grams/mol*/, 1 /*gi*/}};
  } else if constexpr (S == Species::Cyanoacetylene) { // HC3N
    return {
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::C12}, CA{Atom::C12}, CA{Atom::C12}, CA{Atom::N14}}, 51.010899 /*grams/mol*/, 6 /*gi*/}};
  } else if constexpr (S == Species::Hydrogen) { // H2
    return {
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::H}}, 2.01565 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::H}, CA{Atom::D}}, 3.021825 /*grams/mol*/, 6 /*gi*/}};
  } else if constexpr (S == Species::CarbonMonosulfide) { // CS
    return {
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::S32}}, 43.971036 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::S34}}, 45.966787 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C13}, CA{Atom::S32}}, 44.974368 /*grams/mol*/, 2 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::S33}}, 44.970399 /*grams/mol*/, 4 /*gi*/}};
  } else if constexpr (S == Species::SulfurTrioxide) { // SO3
    return {
      IR{/*atoms=*/SA{CA{Atom::S32}, CA{Atom::O16}, CA{Atom::O16}, CA{Atom::O16}}, 79.95682 /*grams/mol*/, 1 /*gi*/}};
  } else if constexpr (S == Species::Cyanogen) { // C2N2
    return {
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::C12}, CA{Atom::N14}, CA{Atom::N14}}, 52.006148 /*grams/mol*/, 1 /*gi*/}};
  } else if constexpr (S == Species::Phosgene) { // COCl2
    return {
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::O16}, CA{Atom::Cl35}, CA{Atom::Cl35}}, 97.9326199796 /*grams/mol*/, 1 /*gi*/},
      IR{/*atoms=*/SA{CA{Atom::C12}, CA{Atom::O16}, CA{Atom::Cl35}, CA{Atom::Cl37}}, 99.9296698896 /*grams/mol*/, 16 /*gi*/}};
  } else {
    return {};
  }
}


template <Species S>
constexpr std::array<Quantum::Type, getGlobalQuantumNumberCount(S)> globalQuantumNumberTypesRecord()
{
  if constexpr (S == Species::Water) { // H2O
    return {
      Quantum::Type::v1,
      Quantum::Type::v2,
      Quantum::Type::v3,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::CarbonDioxide) { // CO2
    return {
      Quantum::Type::v1,
      Quantum::Type::v2,
      Quantum::Type::v3,
      Quantum::Type::l2,
      Quantum::Type::r,
      Quantum::Type::ElecStateLabel,
      Quantum::Type::kronigParity};
  } else if constexpr (S == Species::Ozone) { // O3
    return {
      Quantum::Type::v1,
      Quantum::Type::v2,
      Quantum::Type::v3,
      Quantum::Type::r,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::NitrogenOxide) { // N2O
    return {
      Quantum::Type::v1,
      Quantum::Type::v2,
      Quantum::Type::v3,
      Quantum::Type::l2,
      Quantum::Type::ElecStateLabel,
      Quantum::Type::kronigParity};
  } else if constexpr (S == Species::CarbonMonoxide) { // CO
    return {
      Quantum::Type::v,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Methane) { // CH4
    return {
      Quantum::Type::v1,
      Quantum::Type::v2,
      Quantum::Type::v3,
      Quantum::Type::v4,
      Quantum::Type::v5,
      Quantum::Type::v6,
      Quantum::Type::rovibSym,
      Quantum::Type::n,
      Quantum::Type::alpha,
      Quantum::Type::ElecStateLabel,
      Quantum::Type::vibSym};
  } else if constexpr (S == Species::Oxygen) { // O2
    return {
      Quantum::Type::S,
      Quantum::Type::v,
      Quantum::Type::Lambda,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::NitricOxide) { // NO
    return {
      Quantum::Type::S,
      Quantum::Type::v,
      Quantum::Type::Lambda,
      Quantum::Type::ElecStateLabel,
      Quantum::Type::kronigParity,
      Quantum::Type::parity};
  } else if constexpr (S == Species::SulfurDioxide) { // SO2
    return {
      Quantum::Type::v1,
      Quantum::Type::v2,
      Quantum::Type::v3,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::NitrogenDioxide) { // NO2
    return {
      Quantum::Type::S,
      Quantum::Type::v1,
      Quantum::Type::v2,
      Quantum::Type::v3,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Ammonia) { // NH3
    return {
      Quantum::Type::v1,
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
  } else if constexpr (S == Species::NitricAcid) { // HNO3
    return {
      Quantum::Type::v1,
      Quantum::Type::v2,
      Quantum::Type::v3,
      Quantum::Type::v4,
      Quantum::Type::v5,
      Quantum::Type::v6,
      Quantum::Type::v7,
      Quantum::Type::v8,
      Quantum::Type::v9,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Hydroxyl) { // OH
    return {
      Quantum::Type::S,
      Quantum::Type::v,
      Quantum::Type::Lambda,
      Quantum::Type::ElecStateLabel,
      Quantum::Type::kronigParity,
      Quantum::Type::parity};
  } else if constexpr (S == Species::HydrogenFluoride) { // HF
    return {
      Quantum::Type::v,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::HydrogenChloride) { // HCl
    return {
      Quantum::Type::v,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::HydrogenBromide) { // HBr
    return {
      Quantum::Type::v,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::HydrogenIodide) { // HI
    return {
      Quantum::Type::v,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::ChlorineMonoxide) { // ClO
    return {
      Quantum::Type::S,
      Quantum::Type::v,
      Quantum::Type::Lambda,
      Quantum::Type::ElecStateLabel,
      Quantum::Type::kronigParity,
      Quantum::Type::parity};
  } else if constexpr (S == Species::CarbonylSulfide) { // OCS
    return {
      Quantum::Type::v1,
      Quantum::Type::v2,
      Quantum::Type::v3,
      Quantum::Type::l2,
      Quantum::Type::ElecStateLabel,
      Quantum::Type::kronigParity};
  } else if constexpr (S == Species::Formaldehyde) { // H2CO
    return {
      Quantum::Type::v1,
      Quantum::Type::v2,
      Quantum::Type::v3,
      Quantum::Type::v4,
      Quantum::Type::v5,
      Quantum::Type::v6,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::HypochlorousAcid) { // HOCl
    return {
      Quantum::Type::v1,
      Quantum::Type::v2,
      Quantum::Type::v3,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Nitrogen) { // N2
    return {
      Quantum::Type::v,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::HydrogenCyanide) { // HCN
    return {
      Quantum::Type::v1,
      Quantum::Type::v2,
      Quantum::Type::v3,
      Quantum::Type::l2,
      Quantum::Type::ElecStateLabel,
      Quantum::Type::kronigParity};
  } else if constexpr (S == Species::MethylChloride) { // CH3Cl
    return {
      Quantum::Type::v1,
      Quantum::Type::v2,
      Quantum::Type::v3,
      Quantum::Type::v4,
      Quantum::Type::v5,
      Quantum::Type::v6,
      Quantum::Type::l,
      Quantum::Type::rovibSym,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::HydrogenPeroxide) { // H2O2
    return {
      Quantum::Type::v1,
      Quantum::Type::v2,
      Quantum::Type::v3,
      Quantum::Type::v5,
      Quantum::Type::v6,
      Quantum::Type::n,
      Quantum::Type::tau,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Acetylene) { // C2H2
    return {
      Quantum::Type::v1,
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
  } else if constexpr (S == Species::Ethane) { // C2H6
    return {
      Quantum::Type::v4,
      Quantum::Type::v6,
      Quantum::Type::v7,
      Quantum::Type::v8,
      Quantum::Type::v9,
      Quantum::Type::v12,
      Quantum::Type::l,
      Quantum::Type::rovibSym,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Phosphine) { // PH3
    return {
      Quantum::Type::v1,
      Quantum::Type::v2,
      Quantum::Type::v3,
      Quantum::Type::v4,
      Quantum::Type::l,
      Quantum::Type::ElecStateLabel,
      Quantum::Type::vibSym};
  } else if constexpr (S == Species::CarbonylFluoride) { // COF2
    return {
      Quantum::Type::v1,
      Quantum::Type::v2,
      Quantum::Type::v3,
      Quantum::Type::v4,
      Quantum::Type::v5,
      Quantum::Type::v6,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::SulfurHexafluoride) { // SF6
    return {};
  } else if constexpr (S == Species::HydrogenSulfide) { // H2S
    return {
      Quantum::Type::v1,
      Quantum::Type::v2,
      Quantum::Type::v3,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::FormicAcid) { // HCOOH
    return {
      Quantum::Type::v1,
      Quantum::Type::v2,
      Quantum::Type::v3,
      Quantum::Type::v4,
      Quantum::Type::v5,
      Quantum::Type::v6,
      Quantum::Type::v7,
      Quantum::Type::v8,
      Quantum::Type::v9,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Hydroperoxyl) { // HO2
    return {
      Quantum::Type::S,
      Quantum::Type::v1,
      Quantum::Type::v2,
      Quantum::Type::v3,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::OxygenAtom) { // O
    return {
      Quantum::Type::S,
      Quantum::Type::config,
      Quantum::Type::term};
  } else if constexpr (S == Species::ChlorineNitrate) { // ClONO2
    return {};
  } else if constexpr (S == Species::NitricOxideCation) { // NO+
    return {
      Quantum::Type::v,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::HypobromousAcid) { // HOBr
    return {
      Quantum::Type::v1,
      Quantum::Type::v2,
      Quantum::Type::v3,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Ethylene) { // C2H4
    return {
      Quantum::Type::v2,
      Quantum::Type::v4,
      Quantum::Type::v6,
      Quantum::Type::v7,
      Quantum::Type::v8,
      Quantum::Type::v9,
      Quantum::Type::v10,
      Quantum::Type::v11,
      Quantum::Type::v12,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Methanol) { // CH3OH
    return {
      Quantum::Type::v1,
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
  } else if constexpr (S == Species::MethylBromide) { // CH3Br
    return {
      Quantum::Type::v2,
      Quantum::Type::v5,
      Quantum::Type::v6,
      Quantum::Type::l,
      Quantum::Type::rovibSym,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Acetonitrile) { // CH3CN
    return {
      Quantum::Type::v4,
      Quantum::Type::rovibSym,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Pfc14) { // CF4
    return {};
  } else if constexpr (S == Species::Diacetylene) { // C4H2
    return {
      Quantum::Type::v1,
      Quantum::Type::v2,
      Quantum::Type::v3,
      Quantum::Type::v4,
      Quantum::Type::v5,
      Quantum::Type::v6,
      Quantum::Type::v7,
      Quantum::Type::v8,
      Quantum::Type::v9,
      Quantum::Type::l6,
      Quantum::Type::l7,
      Quantum::Type::l8,
      Quantum::Type::l9,
      Quantum::Type::ElecStateLabel,
      Quantum::Type::kronigParity,
      Quantum::Type::parity,
      Quantum::Type::vibInv};
  } else if constexpr (S == Species::Cyanoacetylene) { // HC3N
    return {
      Quantum::Type::v1,
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
  } else if constexpr (S == Species::Hydrogen) { // H2
    return {
      Quantum::Type::v,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::CarbonMonosulfide) { // CS
    return {
      Quantum::Type::v,
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::SulfurTrioxide) { // SO3
    return {
      Quantum::Type::ElecStateLabel};
  } else if constexpr (S == Species::Cyanogen) { // C2N2
    return {
      Quantum::Type::v1,
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
  } else if constexpr (S == Species::Phosgene) { // COCl2
    return {
      Quantum::Type::v1,
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
constexpr std::array<Quantum::Type, getLocalQuantumNumberCount(S)> localQuantumNumberTypesRecord()
{
  if constexpr (S == Species::Water) { // H2O
    return {
      Quantum::Type::J,
      Quantum::Type::Ka,
      Quantum::Type::Kc};
  } else if constexpr (S == Species::CarbonDioxide) { // CO2
    return {
      Quantum::Type::J,
      Quantum::Type::F};
  } else if constexpr (S == Species::Ozone) { // O3
    return {
      Quantum::Type::J,
      Quantum::Type::F,
      Quantum::Type::Ka,
      Quantum::Type::Kc};
  } else if constexpr (S == Species::NitrogenOxide) { // N2O
    return {
      Quantum::Type::J};
  } else if constexpr (S == Species::CarbonMonoxide) { // CO
    return {
      Quantum::Type::J};
  } else if constexpr (S == Species::Methane) { // CH4
    return {
      Quantum::Type::J,
      Quantum::Type::K};
  } else if constexpr (S == Species::Oxygen) { // O2
    return {
      Quantum::Type::J,
      Quantum::Type::N,
      Quantum::Type::F};
  } else if constexpr (S == Species::NitricOxide) { // NO
    return {
      Quantum::Type::J,
      Quantum::Type::F,
      Quantum::Type::Omega};
  } else if constexpr (S == Species::SulfurDioxide) { // SO2
    return {
      Quantum::Type::J,
      Quantum::Type::Ka,
      Quantum::Type::Kc};
  } else if constexpr (S == Species::NitrogenDioxide) { // NO2
    return {
      Quantum::Type::J,
      Quantum::Type::N,
      Quantum::Type::F,
      Quantum::Type::Ka,
      Quantum::Type::Kc};
  } else if constexpr (S == Species::Ammonia) { // NH3
    return {
      Quantum::Type::J,
      Quantum::Type::K};
  } else if constexpr (S == Species::NitricAcid) { // HNO3
    return {
      Quantum::Type::J,
      Quantum::Type::F,
      Quantum::Type::Ka,
      Quantum::Type::Kc};
  } else if constexpr (S == Species::Hydroxyl) { // OH
    return {
      Quantum::Type::J,
      Quantum::Type::N,
      Quantum::Type::F,
      Quantum::Type::Omega};
  } else if constexpr (S == Species::HydrogenFluoride) { // HF
    return {
      Quantum::Type::J};
  } else if constexpr (S == Species::HydrogenChloride) { // HCl
    return {
      Quantum::Type::J,
      Quantum::Type::F};
  } else if constexpr (S == Species::HydrogenBromide) { // HBr
    return {
      Quantum::Type::J,
      Quantum::Type::F};
  } else if constexpr (S == Species::HydrogenIodide) { // HI
    return {
      Quantum::Type::J,
      Quantum::Type::F};
  } else if constexpr (S == Species::ChlorineMonoxide) { // ClO
    return {
      Quantum::Type::J,
      Quantum::Type::F,
      Quantum::Type::Omega};
  } else if constexpr (S == Species::CarbonylSulfide) { // OCS
    return {
      Quantum::Type::J};
  } else if constexpr (S == Species::Formaldehyde) { // H2CO
    return {
      Quantum::Type::J,
      Quantum::Type::Ka,
      Quantum::Type::Kc};
  } else if constexpr (S == Species::HypochlorousAcid) { // HOCl
    return {
      Quantum::Type::J,
      Quantum::Type::Ka,
      Quantum::Type::Kc};
  } else if constexpr (S == Species::Nitrogen) { // N2
    return {
      Quantum::Type::J};
  } else if constexpr (S == Species::HydrogenCyanide) { // HCN
    return {
      Quantum::Type::J,
      Quantum::Type::F};
  } else if constexpr (S == Species::MethylChloride) { // CH3Cl
    return {
      Quantum::Type::J,
      Quantum::Type::F,
      Quantum::Type::K};
  } else if constexpr (S == Species::HydrogenPeroxide) { // H2O2
    return {
      Quantum::Type::J,
      Quantum::Type::Ka,
      Quantum::Type::Kc};
  } else if constexpr (S == Species::Acetylene) { // C2H2
    return {
      Quantum::Type::J};
  } else if constexpr (S == Species::Ethane) { // C2H6
    return {
      Quantum::Type::J,
      Quantum::Type::K};
  } else if constexpr (S == Species::Phosphine) { // PH3
    return {
      Quantum::Type::J,
      Quantum::Type::K};
  } else if constexpr (S == Species::CarbonylFluoride) { // COF2
    return {
      Quantum::Type::J,
      Quantum::Type::Ka,
      Quantum::Type::Kc};
  } else if constexpr (S == Species::SulfurHexafluoride) { // SF6
    return {};
  } else if constexpr (S == Species::HydrogenSulfide) { // H2S
    return {
      Quantum::Type::J,
      Quantum::Type::Ka,
      Quantum::Type::Kc};
  } else if constexpr (S == Species::FormicAcid) { // HCOOH
    return {
      Quantum::Type::J,
      Quantum::Type::Ka,
      Quantum::Type::Kc};
  } else if constexpr (S == Species::Hydroperoxyl) { // HO2
    return {
      Quantum::Type::J,
      Quantum::Type::N,
      Quantum::Type::F,
      Quantum::Type::Ka,
      Quantum::Type::Kc};
  } else if constexpr (S == Species::OxygenAtom) { // O
    return {
      Quantum::Type::J,
      Quantum::Type::L};
  } else if constexpr (S == Species::ChlorineNitrate) { // ClONO2
    return {};
  } else if constexpr (S == Species::NitricOxideCation) { // NO+
    return {
      Quantum::Type::J};
  } else if constexpr (S == Species::HypobromousAcid) { // HOBr
    return {
      Quantum::Type::J,
      Quantum::Type::Ka,
      Quantum::Type::Kc};
  } else if constexpr (S == Species::Ethylene) { // C2H4
    return {
      Quantum::Type::J,
      Quantum::Type::Ka,
      Quantum::Type::Kc};
  } else if constexpr (S == Species::Methanol) { // CH3OH
    return {
      Quantum::Type::J,
      Quantum::Type::K};
  } else if constexpr (S == Species::MethylBromide) { // CH3Br
    return {
      Quantum::Type::J,
      Quantum::Type::K};
  } else if constexpr (S == Species::Acetonitrile) { // CH3CN
    return {
      Quantum::Type::J,
      Quantum::Type::K};
  } else if constexpr (S == Species::Pfc14) { // CF4
    return {};
  } else if constexpr (S == Species::Diacetylene) { // C4H2
    return {
      Quantum::Type::J};
  } else if constexpr (S == Species::Cyanoacetylene) { // HC3N
    return {
      Quantum::Type::J};
  } else if constexpr (S == Species::Hydrogen) { // H2
    return {
      Quantum::Type::J};
  } else if constexpr (S == Species::CarbonMonosulfide) { // CS
    return {
      Quantum::Type::J};
  } else if constexpr (S == Species::SulfurTrioxide) { // SO3
    return {
      Quantum::Type::Ka,
      Quantum::Type::Kc};
  } else if constexpr (S == Species::Cyanogen) { // C2N2
    return {
      Quantum::Type::J};
  } else if constexpr (S == Species::Phosgene) { // COCl2
    return {
      Quantum::Type::J,
      Quantum::Type::Ka,
      Quantum::Type::Kc};
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
  constexpr IsotopologueRecord() noexcept : isot(isotopeRecords<S>()), global(globalQuantumNumberTypesRecord<S>()), local(localQuantumNumberTypesRecord<S>()) {}
};

template <size_t N>
constexpr bool check_quantumnumbers(const std::array<Quantum::Type, N>& data) {
  if constexpr (N > 1) {
    for (size_t i=0; i<N-1; i++) {
      if (long(data[i]) >= long(data[i+1])){
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
  static_assert(check_quantumnumbers(data.local), "Error, quantum numbers must be in increasing order (you might be missing a number)");
  static_assert(check_quantumnumbers(data.global), "Error, quantum numbers must be in increasing order (you might be missing a number)");
public:
  constexpr Isotopologue() noexcept {}
  
  constexpr double Mass(long i) const noexcept {return data.isot[i].mass;}
  constexpr long G(long i) const noexcept {return data.isot[i].gi;}
  std::vector<ChargedAtom> Atoms(long i) const noexcept {return {data.isot[i].atoms.cbegin(), data.isot[i].atoms.cend()};}
  std::vector<Quantum::Type> globalQuantumNumbers() const noexcept {return {data.global.cbegin(), data.global.cend()};}
  std::vector<Quantum::Type> localQuantumNumbers() const noexcept {return {data.local.cbegin(), data.local.cend()};}
  
  constexpr unsigned char findNum(std::array<ChargedAtom, NA> atoms) const noexcept { 
    // For all defined isotopes
    for (unsigned char i=0; i<NI; i++) {
      
      // a list saying we haven't seen the atom so far
      std::array<bool, NA> found{0};
      for (unsigned char j=0; j<NA; j++) {
        found[j]=false;
      }
      
      for (unsigned char j=0; j<NA; j++) {
        if (found[j]) continue;  // Guard against finding the same atom twice
        if (atoms[j] == data.isot[i].atoms[j]) found[j] = true;
      }
      
      // Return if "all_of" (which is not constexpr before C++20)
      bool all = true;
      for (bool here: found) {all = all and here;} if (all) return i;
    }
    
    // Placeholder for out of range
    return NA;
  }
  
  unsigned char findNum(const std::vector<ChargedAtom>& atoms) const noexcept {
    std::array<ChargedAtom, NA> a;
    for (unsigned char i=0; i<NA; i++)
      a[i] = atoms[i];
    return findNum(a);
  }
  
  template <typename ... A> constexpr unsigned char findNum(A ... atoms) const noexcept {
    // FIXME: Make consteval when possible
    static_assert(sizeof...(A) == NA, "Wrong number of atoms");
    return findNum(std::array<ChargedAtom, NA>{ChargedAtom(atoms)...});
  }
  
  std::string Name() const noexcept {return toString(S);}
  std::string ShortName() const noexcept {return toShortName(S);}
  
  bool set_local(std::vector<Quantum::Number>& v, const Quantum::Number& n, Quantum::Type t) noexcept {
    for (size_t i=0; i<data.local.size(); i++) {
      if (data.local[i] == t) {
        v[i] = n;
        return true;
      }
    }
    return false;
  }
  
  bool set_global(std::vector<Quantum::Number>& v, const Quantum::Number& n, Quantum::Type t) noexcept {
    for (size_t i=0; i<data.global.size(); i++) {
      if (data.global[i] == t) {
        v[i] = n;
        return true;
      }
    }
    return false;
  }
};

class Isotope {
  Species s;
  char num;

public:
  constexpr Isotope(Species S, long n) noexcept : s(S), num(n) {}
  Isotope(Species S, const std::vector<ChargedAtom>& ca) : s(S), num(0) {
    switch(s) {
      case Species::Bath: num = 0; break;
      case Species::Water: num = Isotopologue<Species::Water>().findNum(ca); break;
      case Species::CarbonDioxide: num = Isotopologue<Species::CarbonDioxide>().findNum(ca); break;
      case Species::Ozone: num = Isotopologue<Species::Ozone>().findNum(ca); break;
      case Species::NitrogenOxide: num = Isotopologue<Species::NitrogenOxide>().findNum(ca); break;
      case Species::CarbonMonoxide: num = Isotopologue<Species::CarbonMonoxide>().findNum(ca); break;
      case Species::Methane: num = Isotopologue<Species::Methane>().findNum(ca); break;
      case Species::Oxygen: num = Isotopologue<Species::Oxygen>().findNum(ca); break;
      case Species::NitricOxide: num = Isotopologue<Species::NitricOxide>().findNum(ca); break;
      case Species::SulfurDioxide: num = Isotopologue<Species::SulfurDioxide>().findNum(ca); break;
      case Species::NitrogenDioxide: num = Isotopologue<Species::NitrogenDioxide>().findNum(ca); break;
      case Species::Ammonia: num = Isotopologue<Species::Ammonia>().findNum(ca); break;
      case Species::NitricAcid: num = Isotopologue<Species::NitricAcid>().findNum(ca); break;
      case Species::Hydroxyl: num = Isotopologue<Species::Hydroxyl>().findNum(ca); break;
      case Species::HydrogenFluoride: num = Isotopologue<Species::HydrogenFluoride>().findNum(ca); break;
      case Species::HydrogenChloride: num = Isotopologue<Species::HydrogenChloride>().findNum(ca); break;
      case Species::HydrogenBromide: num = Isotopologue<Species::HydrogenBromide>().findNum(ca); break;
      case Species::HydrogenIodide: num = Isotopologue<Species::HydrogenIodide>().findNum(ca); break;
      case Species::ChlorineMonoxide: num = Isotopologue<Species::ChlorineMonoxide>().findNum(ca); break;
      case Species::CarbonylSulfide: num = Isotopologue<Species::CarbonylSulfide>().findNum(ca); break;
      case Species::Formaldehyde: num = Isotopologue<Species::Formaldehyde>().findNum(ca); break;
      case Species::HypochlorousAcid: num = Isotopologue<Species::HypochlorousAcid>().findNum(ca); break;
      case Species::Nitrogen: num = Isotopologue<Species::Nitrogen>().findNum(ca); break;
      case Species::HydrogenCyanide: num = Isotopologue<Species::HydrogenCyanide>().findNum(ca); break;
      case Species::MethylChloride: num = Isotopologue<Species::MethylChloride>().findNum(ca); break;
      case Species::HydrogenPeroxide: num = Isotopologue<Species::HydrogenPeroxide>().findNum(ca); break;
      case Species::Acetylene: num = Isotopologue<Species::Acetylene>().findNum(ca); break;
      case Species::Ethane: num = Isotopologue<Species::Ethane>().findNum(ca); break;
      case Species::Phosphine: num = Isotopologue<Species::Phosphine>().findNum(ca); break;
      case Species::CarbonylFluoride: num = Isotopologue<Species::CarbonylFluoride>().findNum(ca); break;
      case Species::SulfurHexafluoride: num = Isotopologue<Species::SulfurHexafluoride>().findNum(ca); break;
      case Species::HydrogenSulfide: num = Isotopologue<Species::HydrogenSulfide>().findNum(ca); break;
      case Species::FormicAcid: num = Isotopologue<Species::FormicAcid>().findNum(ca); break;
      case Species::Hydroperoxyl: num = Isotopologue<Species::Hydroperoxyl>().findNum(ca); break;
      case Species::OxygenAtom: num = Isotopologue<Species::OxygenAtom>().findNum(ca); break;
      case Species::ChlorineNitrate: num = Isotopologue<Species::ChlorineNitrate>().findNum(ca); break;
      case Species::NitricOxideCation: num = Isotopologue<Species::NitricOxideCation>().findNum(ca); break;
      case Species::HypobromousAcid: num = Isotopologue<Species::HypobromousAcid>().findNum(ca); break;
      case Species::Ethylene: num = Isotopologue<Species::Ethylene>().findNum(ca); break;
      case Species::Methanol: num = Isotopologue<Species::Methanol>().findNum(ca); break;
      case Species::MethylBromide: num = Isotopologue<Species::MethylBromide>().findNum(ca); break;
      case Species::Acetonitrile: num = Isotopologue<Species::Acetonitrile>().findNum(ca); break;
      case Species::Pfc14: num = Isotopologue<Species::Pfc14>().findNum(ca); break;
      case Species::Diacetylene: num = Isotopologue<Species::Diacetylene>().findNum(ca); break;
      case Species::Cyanoacetylene: num = Isotopologue<Species::Cyanoacetylene>().findNum(ca); break;
      case Species::Hydrogen: num = Isotopologue<Species::Hydrogen>().findNum(ca); break;
      case Species::CarbonMonosulfide: num = Isotopologue<Species::CarbonMonosulfide>().findNum(ca); break;
      case Species::SulfurTrioxide: num = Isotopologue<Species::SulfurTrioxide>().findNum(ca); break;
      case Species::Cyanogen: num = Isotopologue<Species::Cyanogen>().findNum(ca); break;
      case Species::Phosgene: num = Isotopologue<Species::Phosgene>().findNum(ca); break;
      case Species::FINAL: num = 0;
    }
    
    if (num == getIsotopologueCount(S))
      throw std::runtime_error("Cannot find atoms");
  }
  
  friend std::ostream& operator<<(std::ostream& os, Isotope x) {return os << x.s << '-' << int(x.num);}
  friend bool operator==(Isotope x, Isotope y) {return (x.s == y.s) and (x.num == y.num);}
  
  constexpr Species Spec() const noexcept {return s;}
  
  constexpr double mass() const noexcept {
    switch(s) {
      case Species::Bath: return std::numeric_limits<double>::quiet_NaN();
      case Species::Water: return Isotopologue<Species::Water>().Mass(long(num));
      case Species::CarbonDioxide: return Isotopologue<Species::CarbonDioxide>().Mass(long(num));
      case Species::Ozone: return Isotopologue<Species::Ozone>().Mass(long(num));
      case Species::NitrogenOxide: return Isotopologue<Species::NitrogenOxide>().Mass(long(num));
      case Species::CarbonMonoxide: return Isotopologue<Species::CarbonMonoxide>().Mass(long(num));
      case Species::Methane: return Isotopologue<Species::Methane>().Mass(long(num));
      case Species::Oxygen: return Isotopologue<Species::Oxygen>().Mass(long(num));
      case Species::NitricOxide: return Isotopologue<Species::NitricOxide>().Mass(long(num));
      case Species::SulfurDioxide: return Isotopologue<Species::SulfurDioxide>().Mass(long(num));
      case Species::NitrogenDioxide: return Isotopologue<Species::NitrogenDioxide>().Mass(long(num));
      case Species::Ammonia: return Isotopologue<Species::Ammonia>().Mass(long(num));
      case Species::NitricAcid: return Isotopologue<Species::NitricAcid>().Mass(long(num));
      case Species::Hydroxyl: return Isotopologue<Species::Hydroxyl>().Mass(long(num));
      case Species::HydrogenFluoride: return Isotopologue<Species::HydrogenFluoride>().Mass(long(num));
      case Species::HydrogenChloride: return Isotopologue<Species::HydrogenChloride>().Mass(long(num));
      case Species::HydrogenBromide: return Isotopologue<Species::HydrogenBromide>().Mass(long(num));
      case Species::HydrogenIodide: return Isotopologue<Species::HydrogenIodide>().Mass(long(num));
      case Species::ChlorineMonoxide: return Isotopologue<Species::ChlorineMonoxide>().Mass(long(num));
      case Species::CarbonylSulfide: return Isotopologue<Species::CarbonylSulfide>().Mass(long(num));
      case Species::Formaldehyde: return Isotopologue<Species::Formaldehyde>().Mass(long(num));
      case Species::HypochlorousAcid: return Isotopologue<Species::HypochlorousAcid>().Mass(long(num));
      case Species::Nitrogen: return Isotopologue<Species::Nitrogen>().Mass(long(num));
      case Species::HydrogenCyanide: return Isotopologue<Species::HydrogenCyanide>().Mass(long(num));
      case Species::MethylChloride: return Isotopologue<Species::MethylChloride>().Mass(long(num));
      case Species::HydrogenPeroxide: return Isotopologue<Species::HydrogenPeroxide>().Mass(long(num));
      case Species::Acetylene: return Isotopologue<Species::Acetylene>().Mass(long(num));
      case Species::Ethane: return Isotopologue<Species::Ethane>().Mass(long(num));
      case Species::Phosphine: return Isotopologue<Species::Phosphine>().Mass(long(num));
      case Species::CarbonylFluoride: return Isotopologue<Species::CarbonylFluoride>().Mass(long(num));
      case Species::SulfurHexafluoride: return Isotopologue<Species::SulfurHexafluoride>().Mass(long(num));
      case Species::HydrogenSulfide: return Isotopologue<Species::HydrogenSulfide>().Mass(long(num));
      case Species::FormicAcid: return Isotopologue<Species::FormicAcid>().Mass(long(num));
      case Species::Hydroperoxyl: return Isotopologue<Species::Hydroperoxyl>().Mass(long(num));
      case Species::OxygenAtom: return Isotopologue<Species::OxygenAtom>().Mass(long(num));
      case Species::ChlorineNitrate: return Isotopologue<Species::ChlorineNitrate>().Mass(long(num));
      case Species::NitricOxideCation: return Isotopologue<Species::NitricOxideCation>().Mass(long(num));
      case Species::HypobromousAcid: return Isotopologue<Species::HypobromousAcid>().Mass(long(num));
      case Species::Ethylene: return Isotopologue<Species::Ethylene>().Mass(long(num));
      case Species::Methanol: return Isotopologue<Species::Methanol>().Mass(long(num));
      case Species::MethylBromide: return Isotopologue<Species::MethylBromide>().Mass(long(num));
      case Species::Acetonitrile: return Isotopologue<Species::Acetonitrile>().Mass(long(num));
      case Species::Pfc14: return Isotopologue<Species::Pfc14>().Mass(long(num));
      case Species::Diacetylene: return Isotopologue<Species::Diacetylene>().Mass(long(num));
      case Species::Cyanoacetylene: return Isotopologue<Species::Cyanoacetylene>().Mass(long(num));
      case Species::Hydrogen: return Isotopologue<Species::Hydrogen>().Mass(long(num));
      case Species::CarbonMonosulfide: return Isotopologue<Species::CarbonMonosulfide>().Mass(long(num));
      case Species::SulfurTrioxide: return Isotopologue<Species::SulfurTrioxide>().Mass(long(num));
      case Species::Cyanogen: return Isotopologue<Species::Cyanogen>().Mass(long(num));
      case Species::Phosgene: return Isotopologue<Species::Phosgene>().Mass(long(num));
      case Species::FINAL: {/*leave last*/}
    }
    return {};
  }
  
  constexpr double gi() const noexcept {
    switch(s) {
      case Species::Bath: return std::numeric_limits<double>::quiet_NaN();
      case Species::Water: return Isotopologue<Species::Water>().G(long(num));
      case Species::CarbonDioxide: return Isotopologue<Species::CarbonDioxide>().G(long(num));
      case Species::Ozone: return Isotopologue<Species::Ozone>().G(long(num));
      case Species::NitrogenOxide: return Isotopologue<Species::NitrogenOxide>().G(long(num));
      case Species::CarbonMonoxide: return Isotopologue<Species::CarbonMonoxide>().G(long(num));
      case Species::Methane: return Isotopologue<Species::Methane>().G(long(num));
      case Species::Oxygen: return Isotopologue<Species::Oxygen>().G(long(num));
      case Species::NitricOxide: return Isotopologue<Species::NitricOxide>().G(long(num));
      case Species::SulfurDioxide: return Isotopologue<Species::SulfurDioxide>().G(long(num));
      case Species::NitrogenDioxide: return Isotopologue<Species::NitrogenDioxide>().G(long(num));
      case Species::Ammonia: return Isotopologue<Species::Ammonia>().G(long(num));
      case Species::NitricAcid: return Isotopologue<Species::NitricAcid>().G(long(num));
      case Species::Hydroxyl: return Isotopologue<Species::Hydroxyl>().G(long(num));
      case Species::HydrogenFluoride: return Isotopologue<Species::HydrogenFluoride>().G(long(num));
      case Species::HydrogenChloride: return Isotopologue<Species::HydrogenChloride>().G(long(num));
      case Species::HydrogenBromide: return Isotopologue<Species::HydrogenBromide>().G(long(num));
      case Species::HydrogenIodide: return Isotopologue<Species::HydrogenIodide>().G(long(num));
      case Species::ChlorineMonoxide: return Isotopologue<Species::ChlorineMonoxide>().G(long(num));
      case Species::CarbonylSulfide: return Isotopologue<Species::CarbonylSulfide>().G(long(num));
      case Species::Formaldehyde: return Isotopologue<Species::Formaldehyde>().G(long(num));
      case Species::HypochlorousAcid: return Isotopologue<Species::HypochlorousAcid>().G(long(num));
      case Species::Nitrogen: return Isotopologue<Species::Nitrogen>().G(long(num));
      case Species::HydrogenCyanide: return Isotopologue<Species::HydrogenCyanide>().G(long(num));
      case Species::MethylChloride: return Isotopologue<Species::MethylChloride>().G(long(num));
      case Species::HydrogenPeroxide: return Isotopologue<Species::HydrogenPeroxide>().G(long(num));
      case Species::Acetylene: return Isotopologue<Species::Acetylene>().G(long(num));
      case Species::Ethane: return Isotopologue<Species::Ethane>().G(long(num));
      case Species::Phosphine: return Isotopologue<Species::Phosphine>().G(long(num));
      case Species::CarbonylFluoride: return Isotopologue<Species::CarbonylFluoride>().G(long(num));
      case Species::SulfurHexafluoride: return Isotopologue<Species::SulfurHexafluoride>().G(long(num));
      case Species::HydrogenSulfide: return Isotopologue<Species::HydrogenSulfide>().G(long(num));
      case Species::FormicAcid: return Isotopologue<Species::FormicAcid>().G(long(num));
      case Species::Hydroperoxyl: return Isotopologue<Species::Hydroperoxyl>().G(long(num));
      case Species::OxygenAtom: return Isotopologue<Species::OxygenAtom>().G(long(num));
      case Species::ChlorineNitrate: return Isotopologue<Species::ChlorineNitrate>().G(long(num));
      case Species::NitricOxideCation: return Isotopologue<Species::NitricOxideCation>().G(long(num));
      case Species::HypobromousAcid: return Isotopologue<Species::HypobromousAcid>().G(long(num));
      case Species::Ethylene: return Isotopologue<Species::Ethylene>().G(long(num));
      case Species::Methanol: return Isotopologue<Species::Methanol>().G(long(num));
      case Species::MethylBromide: return Isotopologue<Species::MethylBromide>().G(long(num));
      case Species::Acetonitrile: return Isotopologue<Species::Acetonitrile>().G(long(num));
      case Species::Pfc14: return Isotopologue<Species::Pfc14>().G(long(num));
      case Species::Diacetylene: return Isotopologue<Species::Diacetylene>().G(long(num));
      case Species::Cyanoacetylene: return Isotopologue<Species::Cyanoacetylene>().G(long(num));
      case Species::Hydrogen: return Isotopologue<Species::Hydrogen>().G(long(num));
      case Species::CarbonMonosulfide: return Isotopologue<Species::CarbonMonosulfide>().G(long(num));
      case Species::SulfurTrioxide: return Isotopologue<Species::SulfurTrioxide>().G(long(num));
      case Species::Cyanogen: return Isotopologue<Species::Cyanogen>().G(long(num));
      case Species::Phosgene: return Isotopologue<Species::Phosgene>().G(long(num));
      case Species::FINAL: {/*leave last*/}
    }
    return {};
  }
  
  std::vector<ChargedAtom> atoms() const noexcept {
    switch(s) {
      case Species::Bath: {};
      case Species::Water: return Isotopologue<Species::Water>().Atoms(long(num));
      case Species::CarbonDioxide: return Isotopologue<Species::CarbonDioxide>().Atoms(long(num));
      case Species::Ozone: return Isotopologue<Species::Ozone>().Atoms(long(num));
      case Species::NitrogenOxide: return Isotopologue<Species::NitrogenOxide>().Atoms(long(num));
      case Species::CarbonMonoxide: return Isotopologue<Species::CarbonMonoxide>().Atoms(long(num));
      case Species::Methane: return Isotopologue<Species::Methane>().Atoms(long(num));
      case Species::Oxygen: return Isotopologue<Species::Oxygen>().Atoms(long(num));
      case Species::NitricOxide: return Isotopologue<Species::NitricOxide>().Atoms(long(num));
      case Species::SulfurDioxide: return Isotopologue<Species::SulfurDioxide>().Atoms(long(num));
      case Species::NitrogenDioxide: return Isotopologue<Species::NitrogenDioxide>().Atoms(long(num));
      case Species::Ammonia: return Isotopologue<Species::Ammonia>().Atoms(long(num));
      case Species::NitricAcid: return Isotopologue<Species::NitricAcid>().Atoms(long(num));
      case Species::Hydroxyl: return Isotopologue<Species::Hydroxyl>().Atoms(long(num));
      case Species::HydrogenFluoride: return Isotopologue<Species::HydrogenFluoride>().Atoms(long(num));
      case Species::HydrogenChloride: return Isotopologue<Species::HydrogenChloride>().Atoms(long(num));
      case Species::HydrogenBromide: return Isotopologue<Species::HydrogenBromide>().Atoms(long(num));
      case Species::HydrogenIodide: return Isotopologue<Species::HydrogenIodide>().Atoms(long(num));
      case Species::ChlorineMonoxide: return Isotopologue<Species::ChlorineMonoxide>().Atoms(long(num));
      case Species::CarbonylSulfide: return Isotopologue<Species::CarbonylSulfide>().Atoms(long(num));
      case Species::Formaldehyde: return Isotopologue<Species::Formaldehyde>().Atoms(long(num));
      case Species::HypochlorousAcid: return Isotopologue<Species::HypochlorousAcid>().Atoms(long(num));
      case Species::Nitrogen: return Isotopologue<Species::Nitrogen>().Atoms(long(num));
      case Species::HydrogenCyanide: return Isotopologue<Species::HydrogenCyanide>().Atoms(long(num));
      case Species::MethylChloride: return Isotopologue<Species::MethylChloride>().Atoms(long(num));
      case Species::HydrogenPeroxide: return Isotopologue<Species::HydrogenPeroxide>().Atoms(long(num));
      case Species::Acetylene: return Isotopologue<Species::Acetylene>().Atoms(long(num));
      case Species::Ethane: return Isotopologue<Species::Ethane>().Atoms(long(num));
      case Species::Phosphine: return Isotopologue<Species::Phosphine>().Atoms(long(num));
      case Species::CarbonylFluoride: return Isotopologue<Species::CarbonylFluoride>().Atoms(long(num));
      case Species::SulfurHexafluoride: return Isotopologue<Species::SulfurHexafluoride>().Atoms(long(num));
      case Species::HydrogenSulfide: return Isotopologue<Species::HydrogenSulfide>().Atoms(long(num));
      case Species::FormicAcid: return Isotopologue<Species::FormicAcid>().Atoms(long(num));
      case Species::Hydroperoxyl: return Isotopologue<Species::Hydroperoxyl>().Atoms(long(num));
      case Species::OxygenAtom: return Isotopologue<Species::OxygenAtom>().Atoms(long(num));
      case Species::ChlorineNitrate: return Isotopologue<Species::ChlorineNitrate>().Atoms(long(num));
      case Species::NitricOxideCation: return Isotopologue<Species::NitricOxideCation>().Atoms(long(num));
      case Species::HypobromousAcid: return Isotopologue<Species::HypobromousAcid>().Atoms(long(num));
      case Species::Ethylene: return Isotopologue<Species::Ethylene>().Atoms(long(num));
      case Species::Methanol: return Isotopologue<Species::Methanol>().Atoms(long(num));
      case Species::MethylBromide: return Isotopologue<Species::MethylBromide>().Atoms(long(num));
      case Species::Acetonitrile: return Isotopologue<Species::Acetonitrile>().Atoms(long(num));
      case Species::Pfc14: return Isotopologue<Species::Pfc14>().Atoms(long(num));
      case Species::Diacetylene: return Isotopologue<Species::Diacetylene>().Atoms(long(num));
      case Species::Cyanoacetylene: return Isotopologue<Species::Cyanoacetylene>().Atoms(long(num));
      case Species::Hydrogen: return Isotopologue<Species::Hydrogen>().Atoms(long(num));
      case Species::CarbonMonosulfide: return Isotopologue<Species::CarbonMonosulfide>().Atoms(long(num));
      case Species::SulfurTrioxide: return Isotopologue<Species::SulfurTrioxide>().Atoms(long(num));
      case Species::Cyanogen: return Isotopologue<Species::Cyanogen>().Atoms(long(num));
      case Species::Phosgene: return Isotopologue<Species::Phosgene>().Atoms(long(num));
      case Species::FINAL: {/*leave last*/}
    }
    return {};
  }
  
  constexpr unsigned char localQuantumNumberCount() const noexcept {return getLocalQuantumNumberCount(s);}
  
  constexpr unsigned char globalQuantumNumberCount() const noexcept {return getGlobalQuantumNumberCount(s);}
  
  bool set_local(std::vector<Quantum::Number>& v, const Quantum::Number& n, Quantum::Type t) const noexcept {
    switch(s) {
      case Species::Bath: return false;
      case Species::Water: return Isotopologue<Species::Water>().set_local(v, n, t);
      case Species::CarbonDioxide: return Isotopologue<Species::CarbonDioxide>().set_local(v, n, t);
      case Species::Ozone: return Isotopologue<Species::Ozone>().set_local(v, n, t);
      case Species::NitrogenOxide: return Isotopologue<Species::NitrogenOxide>().set_local(v, n, t);
      case Species::CarbonMonoxide: return Isotopologue<Species::CarbonMonoxide>().set_local(v, n, t);
      case Species::Methane: return Isotopologue<Species::Methane>().set_local(v, n, t);
      case Species::Oxygen: return Isotopologue<Species::Oxygen>().set_local(v, n, t);
      case Species::NitricOxide: return Isotopologue<Species::NitricOxide>().set_local(v, n, t);
      case Species::SulfurDioxide: return Isotopologue<Species::SulfurDioxide>().set_local(v, n, t);
      case Species::NitrogenDioxide: return Isotopologue<Species::NitrogenDioxide>().set_local(v, n, t);
      case Species::Ammonia: return Isotopologue<Species::Ammonia>().set_local(v, n, t);
      case Species::NitricAcid: return Isotopologue<Species::NitricAcid>().set_local(v, n, t);
      case Species::Hydroxyl: return Isotopologue<Species::Hydroxyl>().set_local(v, n, t);
      case Species::HydrogenFluoride: return Isotopologue<Species::HydrogenFluoride>().set_local(v, n, t);
      case Species::HydrogenChloride: return Isotopologue<Species::HydrogenChloride>().set_local(v, n, t);
      case Species::HydrogenBromide: return Isotopologue<Species::HydrogenBromide>().set_local(v, n, t);
      case Species::HydrogenIodide: return Isotopologue<Species::HydrogenIodide>().set_local(v, n, t);
      case Species::ChlorineMonoxide: return Isotopologue<Species::ChlorineMonoxide>().set_local(v, n, t);
      case Species::CarbonylSulfide: return Isotopologue<Species::CarbonylSulfide>().set_local(v, n, t);
      case Species::Formaldehyde: return Isotopologue<Species::Formaldehyde>().set_local(v, n, t);
      case Species::HypochlorousAcid: return Isotopologue<Species::HypochlorousAcid>().set_local(v, n, t);
      case Species::Nitrogen: return Isotopologue<Species::Nitrogen>().set_local(v, n, t);
      case Species::HydrogenCyanide: return Isotopologue<Species::HydrogenCyanide>().set_local(v, n, t);
      case Species::MethylChloride: return Isotopologue<Species::MethylChloride>().set_local(v, n, t);
      case Species::HydrogenPeroxide: return Isotopologue<Species::HydrogenPeroxide>().set_local(v, n, t);
      case Species::Acetylene: return Isotopologue<Species::Acetylene>().set_local(v, n, t);
      case Species::Ethane: return Isotopologue<Species::Ethane>().set_local(v, n, t);
      case Species::Phosphine: return Isotopologue<Species::Phosphine>().set_local(v, n, t);
      case Species::CarbonylFluoride: return Isotopologue<Species::CarbonylFluoride>().set_local(v, n, t);
      case Species::SulfurHexafluoride: return Isotopologue<Species::SulfurHexafluoride>().set_local(v, n, t);
      case Species::HydrogenSulfide: return Isotopologue<Species::HydrogenSulfide>().set_local(v, n, t);
      case Species::FormicAcid: return Isotopologue<Species::FormicAcid>().set_local(v, n, t);
      case Species::Hydroperoxyl: return Isotopologue<Species::Hydroperoxyl>().set_local(v, n, t);
      case Species::OxygenAtom: return Isotopologue<Species::OxygenAtom>().set_local(v, n, t);
      case Species::ChlorineNitrate: return Isotopologue<Species::ChlorineNitrate>().set_local(v, n, t);
      case Species::NitricOxideCation: return Isotopologue<Species::NitricOxideCation>().set_local(v, n, t);
      case Species::HypobromousAcid: return Isotopologue<Species::HypobromousAcid>().set_local(v, n, t);
      case Species::Ethylene: return Isotopologue<Species::Ethylene>().set_local(v, n, t);
      case Species::Methanol: return Isotopologue<Species::Methanol>().set_local(v, n, t);
      case Species::MethylBromide: return Isotopologue<Species::MethylBromide>().set_local(v, n, t);
      case Species::Acetonitrile: return Isotopologue<Species::Acetonitrile>().set_local(v, n, t);
      case Species::Pfc14: return Isotopologue<Species::Pfc14>().set_local(v, n, t);
      case Species::Diacetylene: return Isotopologue<Species::Diacetylene>().set_local(v, n, t);
      case Species::Cyanoacetylene: return Isotopologue<Species::Cyanoacetylene>().set_local(v, n, t);
      case Species::Hydrogen: return Isotopologue<Species::Hydrogen>().set_local(v, n, t);
      case Species::CarbonMonosulfide: return Isotopologue<Species::CarbonMonosulfide>().set_local(v, n, t);
      case Species::SulfurTrioxide: return Isotopologue<Species::SulfurTrioxide>().set_local(v, n, t);
      case Species::Cyanogen: return Isotopologue<Species::Cyanogen>().set_local(v, n, t);
      case Species::Phosgene: return Isotopologue<Species::Phosgene>().set_local(v, n, t);
      case Species::FINAL: {/*leave last*/}
    }
    return {};
  }
  
  bool set_global(std::vector<Quantum::Number>& v, const Quantum::Number& n, Quantum::Type t) const noexcept {
    switch(s) {
      case Species::Bath: return false;
      case Species::Water: return Isotopologue<Species::Water>().set_global(v, n, t);
      case Species::CarbonDioxide: return Isotopologue<Species::CarbonDioxide>().set_global(v, n, t);
      case Species::Ozone: return Isotopologue<Species::Ozone>().set_global(v, n, t);
      case Species::NitrogenOxide: return Isotopologue<Species::NitrogenOxide>().set_global(v, n, t);
      case Species::CarbonMonoxide: return Isotopologue<Species::CarbonMonoxide>().set_global(v, n, t);
      case Species::Methane: return Isotopologue<Species::Methane>().set_global(v, n, t);
      case Species::Oxygen: return Isotopologue<Species::Oxygen>().set_global(v, n, t);
      case Species::NitricOxide: return Isotopologue<Species::NitricOxide>().set_global(v, n, t);
      case Species::SulfurDioxide: return Isotopologue<Species::SulfurDioxide>().set_global(v, n, t);
      case Species::NitrogenDioxide: return Isotopologue<Species::NitrogenDioxide>().set_global(v, n, t);
      case Species::Ammonia: return Isotopologue<Species::Ammonia>().set_global(v, n, t);
      case Species::NitricAcid: return Isotopologue<Species::NitricAcid>().set_global(v, n, t);
      case Species::Hydroxyl: return Isotopologue<Species::Hydroxyl>().set_global(v, n, t);
      case Species::HydrogenFluoride: return Isotopologue<Species::HydrogenFluoride>().set_global(v, n, t);
      case Species::HydrogenChloride: return Isotopologue<Species::HydrogenChloride>().set_global(v, n, t);
      case Species::HydrogenBromide: return Isotopologue<Species::HydrogenBromide>().set_global(v, n, t);
      case Species::HydrogenIodide: return Isotopologue<Species::HydrogenIodide>().set_global(v, n, t);
      case Species::ChlorineMonoxide: return Isotopologue<Species::ChlorineMonoxide>().set_global(v, n, t);
      case Species::CarbonylSulfide: return Isotopologue<Species::CarbonylSulfide>().set_global(v, n, t);
      case Species::Formaldehyde: return Isotopologue<Species::Formaldehyde>().set_global(v, n, t);
      case Species::HypochlorousAcid: return Isotopologue<Species::HypochlorousAcid>().set_global(v, n, t);
      case Species::Nitrogen: return Isotopologue<Species::Nitrogen>().set_global(v, n, t);
      case Species::HydrogenCyanide: return Isotopologue<Species::HydrogenCyanide>().set_global(v, n, t);
      case Species::MethylChloride: return Isotopologue<Species::MethylChloride>().set_global(v, n, t);
      case Species::HydrogenPeroxide: return Isotopologue<Species::HydrogenPeroxide>().set_global(v, n, t);
      case Species::Acetylene: return Isotopologue<Species::Acetylene>().set_global(v, n, t);
      case Species::Ethane: return Isotopologue<Species::Ethane>().set_global(v, n, t);
      case Species::Phosphine: return Isotopologue<Species::Phosphine>().set_global(v, n, t);
      case Species::CarbonylFluoride: return Isotopologue<Species::CarbonylFluoride>().set_global(v, n, t);
      case Species::SulfurHexafluoride: return Isotopologue<Species::SulfurHexafluoride>().set_global(v, n, t);
      case Species::HydrogenSulfide: return Isotopologue<Species::HydrogenSulfide>().set_global(v, n, t);
      case Species::FormicAcid: return Isotopologue<Species::FormicAcid>().set_global(v, n, t);
      case Species::Hydroperoxyl: return Isotopologue<Species::Hydroperoxyl>().set_global(v, n, t);
      case Species::OxygenAtom: return Isotopologue<Species::OxygenAtom>().set_global(v, n, t);
      case Species::ChlorineNitrate: return Isotopologue<Species::ChlorineNitrate>().set_global(v, n, t);
      case Species::NitricOxideCation: return Isotopologue<Species::NitricOxideCation>().set_global(v, n, t);
      case Species::HypobromousAcid: return Isotopologue<Species::HypobromousAcid>().set_global(v, n, t);
      case Species::Ethylene: return Isotopologue<Species::Ethylene>().set_global(v, n, t);
      case Species::Methanol: return Isotopologue<Species::Methanol>().set_global(v, n, t);
      case Species::MethylBromide: return Isotopologue<Species::MethylBromide>().set_global(v, n, t);
      case Species::Acetonitrile: return Isotopologue<Species::Acetonitrile>().set_global(v, n, t);
      case Species::Pfc14: return Isotopologue<Species::Pfc14>().set_global(v, n, t);
      case Species::Diacetylene: return Isotopologue<Species::Diacetylene>().set_global(v, n, t);
      case Species::Cyanoacetylene: return Isotopologue<Species::Cyanoacetylene>().set_global(v, n, t);
      case Species::Hydrogen: return Isotopologue<Species::Hydrogen>().set_global(v, n, t);
      case Species::CarbonMonosulfide: return Isotopologue<Species::CarbonMonosulfide>().set_global(v, n, t);
      case Species::SulfurTrioxide: return Isotopologue<Species::SulfurTrioxide>().set_global(v, n, t);
      case Species::Cyanogen: return Isotopologue<Species::Cyanogen>().set_global(v, n, t);
      case Species::Phosgene: return Isotopologue<Species::Phosgene>().set_global(v, n, t);
      case Species::FINAL: {/*leave last*/}
    }
    return {};
  }
  
  std::vector<Quantum::Type> get_globalquantumnumbers() const noexcept {
    switch(s) {
      case Species::Bath: return {};
      case Species::Water: return Isotopologue<Species::Water>().globalQuantumNumbers();
      case Species::CarbonDioxide: return Isotopologue<Species::CarbonDioxide>().globalQuantumNumbers();
      case Species::Ozone: return Isotopologue<Species::Ozone>().globalQuantumNumbers();
      case Species::NitrogenOxide: return Isotopologue<Species::NitrogenOxide>().globalQuantumNumbers();
      case Species::CarbonMonoxide: return Isotopologue<Species::CarbonMonoxide>().globalQuantumNumbers();
      case Species::Methane: return Isotopologue<Species::Methane>().globalQuantumNumbers();
      case Species::Oxygen: return Isotopologue<Species::Oxygen>().globalQuantumNumbers();
      case Species::NitricOxide: return Isotopologue<Species::NitricOxide>().globalQuantumNumbers();
      case Species::SulfurDioxide: return Isotopologue<Species::SulfurDioxide>().globalQuantumNumbers();
      case Species::NitrogenDioxide: return Isotopologue<Species::NitrogenDioxide>().globalQuantumNumbers();
      case Species::Ammonia: return Isotopologue<Species::Ammonia>().globalQuantumNumbers();
      case Species::NitricAcid: return Isotopologue<Species::NitricAcid>().globalQuantumNumbers();
      case Species::Hydroxyl: return Isotopologue<Species::Hydroxyl>().globalQuantumNumbers();
      case Species::HydrogenFluoride: return Isotopologue<Species::HydrogenFluoride>().globalQuantumNumbers();
      case Species::HydrogenChloride: return Isotopologue<Species::HydrogenChloride>().globalQuantumNumbers();
      case Species::HydrogenBromide: return Isotopologue<Species::HydrogenBromide>().globalQuantumNumbers();
      case Species::HydrogenIodide: return Isotopologue<Species::HydrogenIodide>().globalQuantumNumbers();
      case Species::ChlorineMonoxide: return Isotopologue<Species::ChlorineMonoxide>().globalQuantumNumbers();
      case Species::CarbonylSulfide: return Isotopologue<Species::CarbonylSulfide>().globalQuantumNumbers();
      case Species::Formaldehyde: return Isotopologue<Species::Formaldehyde>().globalQuantumNumbers();
      case Species::HypochlorousAcid: return Isotopologue<Species::HypochlorousAcid>().globalQuantumNumbers();
      case Species::Nitrogen: return Isotopologue<Species::Nitrogen>().globalQuantumNumbers();
      case Species::HydrogenCyanide: return Isotopologue<Species::HydrogenCyanide>().globalQuantumNumbers();
      case Species::MethylChloride: return Isotopologue<Species::MethylChloride>().globalQuantumNumbers();
      case Species::HydrogenPeroxide: return Isotopologue<Species::HydrogenPeroxide>().globalQuantumNumbers();
      case Species::Acetylene: return Isotopologue<Species::Acetylene>().globalQuantumNumbers();
      case Species::Ethane: return Isotopologue<Species::Ethane>().globalQuantumNumbers();
      case Species::Phosphine: return Isotopologue<Species::Phosphine>().globalQuantumNumbers();
      case Species::CarbonylFluoride: return Isotopologue<Species::CarbonylFluoride>().globalQuantumNumbers();
      case Species::SulfurHexafluoride: return Isotopologue<Species::SulfurHexafluoride>().globalQuantumNumbers();
      case Species::HydrogenSulfide: return Isotopologue<Species::HydrogenSulfide>().globalQuantumNumbers();
      case Species::FormicAcid: return Isotopologue<Species::FormicAcid>().globalQuantumNumbers();
      case Species::Hydroperoxyl: return Isotopologue<Species::Hydroperoxyl>().globalQuantumNumbers();
      case Species::OxygenAtom: return Isotopologue<Species::OxygenAtom>().globalQuantumNumbers();
      case Species::ChlorineNitrate: return Isotopologue<Species::ChlorineNitrate>().globalQuantumNumbers();
      case Species::NitricOxideCation: return Isotopologue<Species::NitricOxideCation>().globalQuantumNumbers();
      case Species::HypobromousAcid: return Isotopologue<Species::HypobromousAcid>().globalQuantumNumbers();
      case Species::Ethylene: return Isotopologue<Species::Ethylene>().globalQuantumNumbers();
      case Species::Methanol: return Isotopologue<Species::Methanol>().globalQuantumNumbers();
      case Species::MethylBromide: return Isotopologue<Species::MethylBromide>().globalQuantumNumbers();
      case Species::Acetonitrile: return Isotopologue<Species::Acetonitrile>().globalQuantumNumbers();
      case Species::Pfc14: return Isotopologue<Species::Pfc14>().globalQuantumNumbers();
      case Species::Diacetylene: return Isotopologue<Species::Diacetylene>().globalQuantumNumbers();
      case Species::Cyanoacetylene: return Isotopologue<Species::Cyanoacetylene>().globalQuantumNumbers();
      case Species::Hydrogen: return Isotopologue<Species::Hydrogen>().globalQuantumNumbers();
      case Species::CarbonMonosulfide: return Isotopologue<Species::CarbonMonosulfide>().globalQuantumNumbers();
      case Species::SulfurTrioxide: return Isotopologue<Species::SulfurTrioxide>().globalQuantumNumbers();
      case Species::Cyanogen: return Isotopologue<Species::Cyanogen>().globalQuantumNumbers();
      case Species::Phosgene: return Isotopologue<Species::Phosgene>().globalQuantumNumbers();
      case Species::FINAL: {/*leave last*/}
    }
    return {};
  }
  std::vector<Quantum::Type> get_localquantumnumbers() const noexcept {
    switch(s) {
      case Species::Bath: return {};
      case Species::Water: return Isotopologue<Species::Water>().localQuantumNumbers();
      case Species::CarbonDioxide: return Isotopologue<Species::CarbonDioxide>().localQuantumNumbers();
      case Species::Ozone: return Isotopologue<Species::Ozone>().localQuantumNumbers();
      case Species::NitrogenOxide: return Isotopologue<Species::NitrogenOxide>().localQuantumNumbers();
      case Species::CarbonMonoxide: return Isotopologue<Species::CarbonMonoxide>().localQuantumNumbers();
      case Species::Methane: return Isotopologue<Species::Methane>().localQuantumNumbers();
      case Species::Oxygen: return Isotopologue<Species::Oxygen>().localQuantumNumbers();
      case Species::NitricOxide: return Isotopologue<Species::NitricOxide>().localQuantumNumbers();
      case Species::SulfurDioxide: return Isotopologue<Species::SulfurDioxide>().localQuantumNumbers();
      case Species::NitrogenDioxide: return Isotopologue<Species::NitrogenDioxide>().localQuantumNumbers();
      case Species::Ammonia: return Isotopologue<Species::Ammonia>().localQuantumNumbers();
      case Species::NitricAcid: return Isotopologue<Species::NitricAcid>().localQuantumNumbers();
      case Species::Hydroxyl: return Isotopologue<Species::Hydroxyl>().localQuantumNumbers();
      case Species::HydrogenFluoride: return Isotopologue<Species::HydrogenFluoride>().localQuantumNumbers();
      case Species::HydrogenChloride: return Isotopologue<Species::HydrogenChloride>().localQuantumNumbers();
      case Species::HydrogenBromide: return Isotopologue<Species::HydrogenBromide>().localQuantumNumbers();
      case Species::HydrogenIodide: return Isotopologue<Species::HydrogenIodide>().localQuantumNumbers();
      case Species::ChlorineMonoxide: return Isotopologue<Species::ChlorineMonoxide>().localQuantumNumbers();
      case Species::CarbonylSulfide: return Isotopologue<Species::CarbonylSulfide>().localQuantumNumbers();
      case Species::Formaldehyde: return Isotopologue<Species::Formaldehyde>().localQuantumNumbers();
      case Species::HypochlorousAcid: return Isotopologue<Species::HypochlorousAcid>().localQuantumNumbers();
      case Species::Nitrogen: return Isotopologue<Species::Nitrogen>().localQuantumNumbers();
      case Species::HydrogenCyanide: return Isotopologue<Species::HydrogenCyanide>().localQuantumNumbers();
      case Species::MethylChloride: return Isotopologue<Species::MethylChloride>().localQuantumNumbers();
      case Species::HydrogenPeroxide: return Isotopologue<Species::HydrogenPeroxide>().localQuantumNumbers();
      case Species::Acetylene: return Isotopologue<Species::Acetylene>().localQuantumNumbers();
      case Species::Ethane: return Isotopologue<Species::Ethane>().localQuantumNumbers();
      case Species::Phosphine: return Isotopologue<Species::Phosphine>().localQuantumNumbers();
      case Species::CarbonylFluoride: return Isotopologue<Species::CarbonylFluoride>().localQuantumNumbers();
      case Species::SulfurHexafluoride: return Isotopologue<Species::SulfurHexafluoride>().localQuantumNumbers();
      case Species::HydrogenSulfide: return Isotopologue<Species::HydrogenSulfide>().localQuantumNumbers();
      case Species::FormicAcid: return Isotopologue<Species::FormicAcid>().localQuantumNumbers();
      case Species::Hydroperoxyl: return Isotopologue<Species::Hydroperoxyl>().localQuantumNumbers();
      case Species::OxygenAtom: return Isotopologue<Species::OxygenAtom>().localQuantumNumbers();
      case Species::ChlorineNitrate: return Isotopologue<Species::ChlorineNitrate>().localQuantumNumbers();
      case Species::NitricOxideCation: return Isotopologue<Species::NitricOxideCation>().localQuantumNumbers();
      case Species::HypobromousAcid: return Isotopologue<Species::HypobromousAcid>().localQuantumNumbers();
      case Species::Ethylene: return Isotopologue<Species::Ethylene>().localQuantumNumbers();
      case Species::Methanol: return Isotopologue<Species::Methanol>().localQuantumNumbers();
      case Species::MethylBromide: return Isotopologue<Species::MethylBromide>().localQuantumNumbers();
      case Species::Acetonitrile: return Isotopologue<Species::Acetonitrile>().localQuantumNumbers();
      case Species::Pfc14: return Isotopologue<Species::Pfc14>().localQuantumNumbers();
      case Species::Diacetylene: return Isotopologue<Species::Diacetylene>().localQuantumNumbers();
      case Species::Cyanoacetylene: return Isotopologue<Species::Cyanoacetylene>().localQuantumNumbers();
      case Species::Hydrogen: return Isotopologue<Species::Hydrogen>().localQuantumNumbers();
      case Species::CarbonMonosulfide: return Isotopologue<Species::CarbonMonosulfide>().localQuantumNumbers();
      case Species::SulfurTrioxide: return Isotopologue<Species::SulfurTrioxide>().localQuantumNumbers();
      case Species::Cyanogen: return Isotopologue<Species::Cyanogen>().localQuantumNumbers();
      case Species::Phosgene: return Isotopologue<Species::Phosgene>().localQuantumNumbers();
      case Species::FINAL: {/*leave last*/}
    }
    return {};
  }
};  // Isotope

AtomInfo getAtomInfo(Atom x) noexcept;
}  // Species

#endif  // species_h
