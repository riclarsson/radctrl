#include "species.h"

namespace Species {
AtomInfo getAtomInfo(Atom x) noexcept {
  switch (x) {
    case Atom::H:
      return {"Hydrogen", 0.5};
    case Atom::D:
      return {"Deuterium", 1.0};
    case Atom::T:
      return {"Tritium", 0.5};
    case Atom::He4:
      return {"Helium", 0.0};
    case Atom::He3:
      return {"Helium", 0.5};
    case Atom::Li7:
      return {"Lithium", 1.5};
    case Atom::Li6:
      return {"Lithium", 1.0};
    case Atom::Be9:
      return {"Beryllium", 1.5};
    case Atom::B11:
      return {"Boron", 1.5};
    case Atom::B10:
      return {"Boron", 3.0};
    case Atom::C12:
      return {"Carbon", 0.0};
    case Atom::C13:
      return {"Carbon", 0.5};
    case Atom::N14:
      return {"Nitrogen", 1.0};
    case Atom::N15:
      return {"Nitrogen", 0.5};
    case Atom::O16:
      return {"Oxygen", 0.0};
    case Atom::O17:
      return {"Oxygen", 2.5};
    case Atom::O18:
      return {"Oxygen", 0.0};
    case Atom::F19:
      return {"Fluorine", 0.5};
    case Atom::Ne20:
      return {"Neon", 0.0};
    case Atom::Ne21:
      return {"Neon", 1.5};
    case Atom::Ne22:
      return {"Neon", 0.0};
    case Atom::Na23:
      return {"Sodium", 1.5};
    case Atom::Mg24:
      return {"Magnesium", 0.0};
    case Atom::Mg25:
      return {"Magnesium", 2.5};
    case Atom::Mg26:
      return {"Magnesium", 0.0};
    case Atom::Al27:
      return {"Aluminium", 2.5};
    case Atom::Si28:
      return {"Silicon", 0.0};
    case Atom::Si29:
      return {"Silicon", 0.5};
    case Atom::Si30:
      return {"Silicon", 0.0};
    case Atom::P31:
      return {"Phosphorus", 0.5};
    case Atom::S32:
      return {"Sulfur", 0.0};
    case Atom::S33:
      return {"Sulfur", 1.5};
    case Atom::S34:
      return {"Sulfur", 0.0};
    case Atom::Cl35:
      return {"Chlorine", 1.5};
    case Atom::Cl37:
      return {"Chlorine", 1.5};
    case Atom::Ar40:
      return {"Argon", 0.0};
    case Atom::Ar36:
      return {"Argon", 0.0};
    case Atom::Ar38:
      return {"Argon", 0.0};
    case Atom::K39:
      return {"Potassium", 1.5};
    case Atom::K40:
      return {"Potassium", 4.0};
    case Atom::K41:
      return {"Potassium", 1.5};
    case Atom::Ca40:
      return {"Calcium", 0.0};
    case Atom::Ca42:
      return {"Calcium", 0.0};
    case Atom::Ca43:
      return {"Calcium", 3.5};
    case Atom::Ca44:
      return {"Calcium", 0.0};
    case Atom::Ca46:
      return {"Calcium", 0.0};
    case Atom::Ca48:
      return {"Calcium", 0.0};
    case Atom::Sc45:
      return {"Scandium", 3.5};
    case Atom::Ti46:
      return {"Titanium", 0.0};
    case Atom::Ti47:
      return {"Titanium", 2.5};
    case Atom::Ti48:
      return {"Titanium", 0.0};
    case Atom::Ti49:
      return {"Titanium", 3.5};
    case Atom::Ti50:
      return {"Titanium", 0.0};
    case Atom::V51:
      return {"Vanadium", 3.5};
    case Atom::V50:
      return {"Vanadium", 6.0};
    case Atom::Cr52:
      return {"Chromium", 0.0};
    case Atom::Cr50:
      return {"Chromium", 0.0};
    case Atom::Cr53:
      return {"Chromium", 1.5};
    case Atom::Cr54:
      return {"Chromium", 0.5};
    case Atom::Mn55:
      return {"Manganese", 2.5};
    case Atom::Fe56:
      return {"Iron", 0.0};
    case Atom::Fe54:
      return {"Iron", 0.0};
    case Atom::Fe57:
      return {"Iron", 0.5};
    case Atom::Fe58:
      return {"Iron", 0.0};
    case Atom::Co59:
      return {"Cobalt", 3.5};
    case Atom::Ni58:
      return {"Nickel", 0.0};
    case Atom::Cu63:
      return {"Copper", 1.5};
    case Atom::Zn64:
      return {"Zinc", 0.0};
    case Atom::Ga69:
      return {"Gallium", 1.5};
    case Atom::Ge74:
      return {"Germanium", 0.0};
    case Atom::As75:
      return {"Arsenic", 1.5};
    case Atom::Se80:
      return {"Selenium", 0.0};
    case Atom::Br79:
      return {"Bromine", 1.5};
    case Atom::Br81:
      return {"Bromine", 1.5};
    case Atom::Kr84:
      return {"Krypton", 0.0};
    case Atom::Rb85:
      return {"Rubidium", 2.5};
    case Atom::Rb87:
      return {"Rubidium", 1.5};
    case Atom::Sr88:
      return {"Strontium", 0.0};
    case Atom::Y89:
      return {"Yttrium", 0.5};
    case Atom::Zr90:
      return {"Zirconium", 0.0};
    case Atom::Nb93:
      return {"Niobium", 4.5};
    case Atom::Mo98:
      return {"Molybdenum", 0.0};
    case Atom::Mo92:
      return {"Molybdenum", 0.0};
    case Atom::Mo94:
      return {"Molybdenum", 0.0};
    case Atom::Mo95:
      return {"Molybdenum", 2.5};
    case Atom::Mo96:
      return {"Molybdenum", 0.0};
    case Atom::Mo100:
      return {"Molybdenum", 0.0};
    case Atom::Tc99:
      return {"Technetium", 2.5};
    case Atom::Ru102:
      return {"Ruthenium", 0.0};
    case Atom::Ru99:
      return {"Ruthenium", 2.5};
    case Atom::Ru100:
      return {"Ruthenium", 0.0};
    case Atom::Ru101:
      return {"Ruthenium", 2.5};
    case Atom::Ru104:
      return {"Ruthenium", 0.0};
    case Atom::Rh103:
      return {"Rhodium", 0.5};
    case Atom::Pd106:
      return {"Palladium", 0.0};
    case Atom::Pd104:
      return {"Palladium", 0.0};
    case Atom::Pd105:
      return {"Palladium", 2.5};
    case Atom::Pd108:
      return {"Palladium", 0.0};
    case Atom::Ag107:
      return {"Silver", 0.5};
    case Atom::Ag109:
      return {"Silver", 0.5};
    case Atom::Cd114:
      return {"Cadmium", 0.0};
    case Atom::Cd110:
      return {"Cadmium", 0.0};
    case Atom::Cd111:
      return {"Cadmium", 0.5};
    case Atom::Cd112:
      return {"Cadmium", 0.0};
    case Atom::Cd113:
      return {"Cadmium", 0.5};
    case Atom::Cd116:
      return {"Cadmium", 0.0};
    case Atom::In115:
      return {"Indium", 4.5};
    case Atom::In113:
      return {"Indium", 4.5};
    case Atom::Sn120:
      return {"Tin", 0.0};
    case Atom::Sb121:
      return {"Antimony", 2.5};
    case Atom::Sb123:
      return {"Antimony", 3.5};
    case Atom::Te130:
      return {"Tellurium", 0.0};
    case Atom::I127:
      return {"Iodine", 2.5};
    case Atom::Xe132:
      return {"Xenon", 0.0};
    case Atom::Xe129:
      return {"Xenon", 0.5};
    case Atom::Xe131:
      return {"Xenon", 1.5};
    case Atom::Cs133:
      return {"Caesium", 3.5};
    case Atom::Ba138:
      return {"Barium", 0.0};
    case Atom::La139:
      return {"Lanthanum", 3.5};
    case Atom::Ce140:
      return {"Cerium", 0.0};
    case Atom::Pr141:
      return {"Praseodymium", 2.5};
    case Atom::Nd142:
      return {"Neodymium", 0.0};
    case Atom::Nd143:
      return {"Neodymium", 3.5};
    case Atom::Nd144:
      return {"Neodymium", 0.0};
    case Atom::Nd145:
      return {"Neodymium", 3.5};
    case Atom::Nd146:
      return {"Neodymium", 0.0};
    case Atom::Pm0:
      return {"Promethium", std::numeric_limits<double>::quiet_NaN()};
    case Atom::Sm144:
      return {"Samarium", 0.0};
    case Atom::Sm150:
      return {"Samarium", 0.0};
    case Atom::Sm152:
      return {"Samarium", 0.0};
    case Atom::Sm154:
      return {"Samarium", 0.0};
    case Atom::Eu151:
      return {"Europium", 2.5};
    case Atom::Eu153:
      return {"Europium", 2.5};
    case Atom::Gd154:
      return {"Gadolinium", 0.0};
    case Atom::Gd155:
      return {"Gadolinium", 1.5};
    case Atom::Gd156:
      return {"Gadolinium", 0.0};
    case Atom::Gd157:
      return {"Gadolinium", 0.0};
    case Atom::Gd158:
      return {"Gadolinium", 0.0};
    case Atom::Gd160:
      return {"Gadolinium", 0.0};
    case Atom::Tb159:
      return {"Terbium", 1.5};
    case Atom::Dy156:
      return {"Dysprosium", 0.0};
    case Atom::Dy158:
      return {"Dysprosium", 0.0};
    case Atom::Dy160:
      return {"Dysprosium", 0.0};
    case Atom::Dy161:
      return {"Dysprosium", 2.5};
    case Atom::Dy162:
      return {"Dysprosium", 0.0};
    case Atom::Dy163:
      return {"Dysprosium", 2.5};
    case Atom::Dy164:
      return {"Dysprosium", 0.0};
    case Atom::Ho165:
      return {"Holmium", 3.5};
    case Atom::Er162:
      return {"Erbium", 0.0};
    case Atom::Er164:
      return {"Erbium", 0.0};
    case Atom::Er166:
      return {"Erbium", 0.0};
    case Atom::Er167:
      return {"Erbium", 3.5};
    case Atom::Er168:
      return {"Erbium", 0.0};
    case Atom::Er170:
      return {"Erbium", 0.0};
    case Atom::Tm169:
      return {"Thulium", 0.5};
    case Atom::Yb168:
      return {"Ytterbium", 0.0};
    case Atom::Yb170:
      return {"Ytterbium", 0.0};
    case Atom::Yb171:
      return {"Ytterbium", 0.5};
    case Atom::Yb172:
      return {"Ytterbium", 0.0};
    case Atom::Yb173:
      return {"Ytterbium", 2.5};
    case Atom::Yb174:
      return {"Ytterbium", 0.0};
    case Atom::Yb176:
      return {"Ytterbium", 0.0};
    case Atom::Lu175:
      return {"Lutetium", 3.5};
    case Atom::Hf176:
      return {"Hafnium", 0.0};
    case Atom::Hf177:
      return {"Hafnium", 3.5};
    case Atom::Hf178:
      return {"Hafnium", 0.0};
    case Atom::Hf179:
      return {"Hafnium", 4.5};
    case Atom::Hf180:
      return {"Hafnium", 0.0};
    case Atom::Ta181:
      return {"Tantalum", 3.5};
    case Atom::W180:
      return {"Tungsten", 0.0};
    case Atom::W182:
      return {"Tungsten", 0.0};
    case Atom::W184:
      return {"Tungsten", 0.0};
    case Atom::W186:
      return {"Tungsten", 0.0};
    case Atom::Re185:
      return {"Rhenium", 2.5};
    case Atom::Os187:
      return {"Osmium", 0.5};
    case Atom::Os188:
      return {"Osmium", 0.0};
    case Atom::Os189:
      return {"Osmium", 1.5};
    case Atom::Os190:
      return {"Osmium", 0.0};
    case Atom::Os192:
      return {"Osmium", 0.0};
    case Atom::Ir191:
      return {"Iridium", 1.5};
    case Atom::Ir193:
      return {"Iridium", 1.5};
    case Atom::Pt192:
      return {"Platinum", 0.0};
    case Atom::Pt194:
      return {"Platinum", 0.0};
    case Atom::Pt195:
      return {"Platinum", 0.5};
    case Atom::Pt196:
      return {"Platinum", 0.0};
    case Atom::Pt198:
      return {"Platinum", 0.0};
    case Atom::Au197:
      return {"Gold", 1.5};
    case Atom::Hg196:
      return {"Mercury", 0.0};
    case Atom::Hg198:
      return {"Mercury", 0.0};
    case Atom::Hg199:
      return {"Mercury", 0.5};
    case Atom::Hg200:
      return {"Mercury", 0.0};
    case Atom::Hg201:
      return {"Mercury", 1.5};
    case Atom::Hg202:
      return {"Mercury", 0.0};
    case Atom::Hg204:
      return {"Mercury", 0.0};
    case Atom::Tl203:
      return {"Thallium", 0.5};
    case Atom::Tl205:
      return {"Thallium", 0.5};
    case Atom::Pb206:
      return {"Lead", 0.0};
    case Atom::Pb207:
      return {"Lead", 0.5};
    case Atom::Pb208:
      return {"Lead", 0.0};
    case Atom::Bi209:
      return {"Bismuth", 4.5};
    case Atom::Po0:
      return {"Polonium", std::numeric_limits<double>::quiet_NaN()};
    case Atom::At0:
      return {"Astatine", std::numeric_limits<double>::quiet_NaN()};
    case Atom::Rn0:
      return {"Radon", std::numeric_limits<double>::quiet_NaN()};
    case Atom::Fr0:
      return {"Francium", std::numeric_limits<double>::quiet_NaN()};
    case Atom::Ra0:
      return {"Radium", std::numeric_limits<double>::quiet_NaN()};
    case Atom::Ac0:
      return {"Actinium", std::numeric_limits<double>::quiet_NaN()};
    case Atom::Th232:
      return {"Thorium", 0.0};
    case Atom::Pa0:
      return {"Protactinium", std::numeric_limits<double>::quiet_NaN()};
    case Atom::U235:
      return {"Uranium", 3.5};
    case Atom::U238:
      return {"Uranium", 0.0};
    case Atom::Np0:
      return {"Neptunium", std::numeric_limits<double>::quiet_NaN()};
    case Atom::Pu0:
      return {"Plutonium", std::numeric_limits<double>::quiet_NaN()};
    case Atom::Am0:
      return {"Americium", std::numeric_limits<double>::quiet_NaN()};
    case Atom::Cm0:
      return {"Curium", std::numeric_limits<double>::quiet_NaN()};
    case Atom::Bk0:
      return {"Berkelium", std::numeric_limits<double>::quiet_NaN()};
    case Atom::Cf0:
      return {"Californium", std::numeric_limits<double>::quiet_NaN()};
    case Atom::Es0:
      return {"Einsteinium", std::numeric_limits<double>::quiet_NaN()};
    case Atom::Fm0:
      return {"Fermium", std::numeric_limits<double>::quiet_NaN()};
    case Atom::FINAL: { /* leave last */
    }
  }
  return {};
}

std::string toShortName(Species x) noexcept {
  switch (x) {
    case Species::Bath:
      return "AIR";
    case Species::Water:
      return "H2O";
    case Species::CarbonDioxide:
      return "CO2";
    case Species::Ozone:
      return "O3";
    case Species::NitrogenOxide:
      return "N2O";
    case Species::CarbonMonoxide:
      return "CO";
    case Species::Methane:
      return "CH4";
    case Species::Oxygen:
      return "O2";
    case Species::NitricOxide:
      return "NO";
    case Species::SulfurDioxide:
      return "SO2";
    case Species::NitrogenDioxide:
      return "NO2";
    case Species::Ammonia:
      return "NH3";
    case Species::NitricAcid:
      return "HNO3";
    case Species::Hydroxyl:
      return "OH";
    case Species::HydrogenFluoride:
      return "HF";
    case Species::HydrogenChloride:
      return "HCl";
    case Species::HydrogenBromide:
      return "HBr";
    case Species::HydrogenIodide:
      return "HI";
    case Species::ChlorineMonoxide:
      return "ClO";
    case Species::CarbonylSulfide:
      return "OCS";
    case Species::Formaldehyde:
      return "H2CO";
    case Species::HypochlorousAcid:
      return "HOCl";
    case Species::Nitrogen:
      return "N2";
    case Species::HydrogenCyanide:
      return "HCN";
    case Species::MethylChloride:
      return "CH3Cl";
    case Species::HydrogenPeroxide:
      return "H2O2";
    case Species::Acetylene:
      return "C2H2";
    case Species::Ethane:
      return "C2H6";
    case Species::Phosphine:
      return "PH3";
    case Species::CarbonylFluoride:
      return "COF2";
    case Species::SulfurHexafluoride:
      return "SF6";
    case Species::HydrogenSulfide:
      return "H2S";
    case Species::FormicAcid:
      return "HCOOH";
    case Species::Hydroperoxyl:
      return "HO2";
    case Species::OxygenAtom:
      return "O";
    case Species::ChlorineNitrate:
      return "ClONO2";
    case Species::NitricOxideCation:
      return "NO+";
    case Species::HypobromousAcid:
      return "HOBr";
    case Species::Ethylene:
      return "C2H4";
    case Species::Methanol:
      return "CH3OH";
    case Species::MethylBromide:
      return "CH3Br";
    case Species::Acetonitrile:
      return "CH3CN";
    case Species::Pfc14:
      return "CF4";
    case Species::Diacetylene:
      return "C4H2";
    case Species::Cyanoacetylene:
      return "HC3N";
    case Species::Hydrogen:
      return "H2";
    case Species::CarbonMonosulfide:
      return "CS";
    case Species::SulfurTrioxide:
      return "SO3";
    case Species::Cyanogen:
      return "C2N2";
    case Species::Phosgene:
      return "COCl2";
    case Species::FINAL: { /* Leave last */
    }
  }
  return {};
}

Species fromShortName(const std::string &x) noexcept {
  if (x == "H2O")
    return Species::Water;
  else if (x == "CO2")
    return Species::CarbonDioxide;
  else if (x == "O3")
    return Species::Ozone;
  else if (x == "N2O")
    return Species::NitrogenOxide;
  else if (x == "CO")
    return Species::CarbonMonoxide;
  else if (x == "CH4")
    return Species::Methane;
  else if (x == "O2")
    return Species::Oxygen;
  else if (x == "NO")
    return Species::NitricOxide;
  else if (x == "SO2")
    return Species::SulfurDioxide;
  else if (x == "NO2")
    return Species::NitrogenDioxide;
  else if (x == "NH3")
    return Species::Ammonia;
  else if (x == "HNO3")
    return Species::NitricAcid;
  else if (x == "OH")
    return Species::Hydroxyl;
  else if (x == "HF")
    return Species::HydrogenFluoride;
  else if (x == "HCl")
    return Species::HydrogenChloride;
  else if (x == "HBr")
    return Species::HydrogenBromide;
  else if (x == "HI")
    return Species::HydrogenIodide;
  else if (x == "ClO")
    return Species::ChlorineMonoxide;
  else if (x == "OCS")
    return Species::CarbonylSulfide;
  else if (x == "H2CO")
    return Species::Formaldehyde;
  else if (x == "HOCl")
    return Species::HypochlorousAcid;
  else if (x == "N2")
    return Species::Nitrogen;
  else if (x == "HCN")
    return Species::HydrogenCyanide;
  else if (x == "CH3Cl")
    return Species::MethylChloride;
  else if (x == "H2O2")
    return Species::HydrogenPeroxide;
  else if (x == "C2H2")
    return Species::Acetylene;
  else if (x == "C2H6")
    return Species::Ethane;
  else if (x == "PH3")
    return Species::Phosphine;
  else if (x == "COF2")
    return Species::CarbonylFluoride;
  else if (x == "SF6")
    return Species::SulfurHexafluoride;
  else if (x == "H2S")
    return Species::HydrogenSulfide;
  else if (x == "HCOOH")
    return Species::FormicAcid;
  else if (x == "HO2")
    return Species::Hydroperoxyl;
  else if (x == "O")
    return Species::OxygenAtom;
  else if (x == "ClONO2")
    return Species::ChlorineNitrate;
  else if (x == "NO+")
    return Species::NitricOxideCation;
  else if (x == "HOBr")
    return Species::HypobromousAcid;
  else if (x == "C2H4")
    return Species::Ethylene;
  else if (x == "CH3OH")
    return Species::Methanol;
  else if (x == "CH3Br")
    return Species::MethylBromide;
  else if (x == "CH3CN")
    return Species::Acetonitrile;
  else if (x == "CF4")
    return Species::Pfc14;
  else if (x == "C4H2")
    return Species::Diacetylene;
  else if (x == "HC3N")
    return Species::Cyanoacetylene;
  else if (x == "H2")
    return Species::Hydrogen;
  else if (x == "CS")
    return Species::CarbonMonosulfide;
  else if (x == "SO3")
    return Species::SulfurTrioxide;
  else if (x == "C2N2")
    return Species::Cyanogen;
  else if (x == "COCl2")
    return Species::Phosgene;
  else
    return Species::FINAL;
}
}  // namespace Species
