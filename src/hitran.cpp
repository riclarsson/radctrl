#include "hitran.h"

namespace HITRAN {
constexpr long toNumber(Species::Species x) noexcept {
  switch (x) {
    case Species::Species::Water:
      return 1;
    case Species::Species::CarbonDioxide:
      return 2;
    case Species::Species::Ozone:
      return 3;
    case Species::Species::NitrogenOxide:
      return 4;
    case Species::Species::CarbonMonoxide:
      return 5;
    case Species::Species::Methane:
      return 6;
    case Species::Species::Oxygen:
      return 7;
    case Species::Species::NitricOxide:
      return 8;
    case Species::Species::SulfurDioxide:
      return 9;
    case Species::Species::NitrogenDioxide:
      return 10;
    case Species::Species::Ammonia:
      return 11;
    case Species::Species::NitricAcid:
      return 12;
    case Species::Species::Hydroxyl:
      return 13;
    case Species::Species::HydrogenFluoride:
      return 14;
    case Species::Species::HydrogenChloride:
      return 15;
    case Species::Species::HydrogenBromide:
      return 16;
    case Species::Species::HydrogenIodide:
      return 17;
    case Species::Species::ChlorineMonoxide:
      return 18;
    case Species::Species::CarbonylSulfide:
      return 19;
    case Species::Species::Formaldehyde:
      return 20;
    case Species::Species::HypochlorousAcid:
      return 21;
    case Species::Species::Nitrogen:
      return 22;
    case Species::Species::HydrogenCyanide:
      return 23;
    case Species::Species::MethylChloride:
      return 24;
    case Species::Species::HydrogenPeroxide:
      return 25;
    case Species::Species::Acetylene:
      return 26;
    case Species::Species::Ethane:
      return 27;
    case Species::Species::Phosphine:
      return 28;
    case Species::Species::CarbonylFluoride:
      return 29;
    case Species::Species::SulfurHexafluoride:
      return 30;
    case Species::Species::HydrogenSulfide:
      return 31;
    case Species::Species::FormicAcid:
      return 32;
    case Species::Species::Hydroperoxyl:
      return 33;
    case Species::Species::OxygenAtom:
      return 34;
    case Species::Species::ChlorineNitrate:
      return 35;
    case Species::Species::NitricOxideCation:
      return 36;
    case Species::Species::HypobromousAcid:
      return 37;
    case Species::Species::Ethylene:
      return 38;
    case Species::Species::Methanol:
      return 39;
    case Species::Species::MethylBromide:
      return 40;
    case Species::Species::Acetonitrile:
      return 41;
    case Species::Species::Pfc14:
      return 42;
    case Species::Species::Diacetylene:
      return 43;
    case Species::Species::Cyanoacetylene:
      return 44;
    case Species::Species::Hydrogen:
      return 45;
    case Species::Species::CarbonMonosulfide:
      return 46;
    case Species::Species::SulfurTrioxide:
      return 47;
    case Species::Species::Cyanogen:
      return 48;
    case Species::Species::Phosgene:
      return 49;
    default: { /* you have some work to do if you see this! */
    }
  }
  return -1;
}

std::vector<IsotopologueData> getIsotopologueData(long x) noexcept {
  switch (x) {
    case 1:
      return std::vector<IsotopologueData>{
          {Species::Species::Water, 161, 0.997317, 174.58,
           Species::Isotopologue<Species::Species::Water>().findNum(
               Species::Atom::H, Species::Atom::H, Species::Atom::O16)},
          {Species::Species::Water, 181, 0.002, 176.05,
           Species::Isotopologue<Species::Species::Water>().findNum(
               Species::Atom::H, Species::Atom::H, Species::Atom::O18)},
          {Species::Species::Water, 171, 0.000371884, 1052.14,
           Species::Isotopologue<Species::Species::Water>().findNum(
               Species::Atom::H, Species::Atom::H, Species::Atom::O17)},
          {Species::Species::Water, 162, 0.000310693, 864.74,
           Species::Isotopologue<Species::Species::Water>().findNum(
               Species::Atom::H, Species::Atom::D, Species::Atom::O16)},
          {Species::Species::Water, 182, 6.23003e-07, 875.57,
           Species::Isotopologue<Species::Species::Water>().findNum(
               Species::Atom::H, Species::Atom::D, Species::Atom::O18)},
          {Species::Species::Water, 172, 1.15853e-07, 5226.79,
           Species::Isotopologue<Species::Species::Water>().findNum(
               Species::Atom::H, Species::Atom::D, Species::Atom::O17)},
          {Species::Species::Water, 262, 2.4197e-08, 1027.8,
           Species::Isotopologue<Species::Species::Water>().findNum(
               Species::Atom::D, Species::Atom::D, Species::Atom::O16)}};
    case 2:
      return std::vector<IsotopologueData>{
          {Species::Species::CarbonDioxide, 626, 0.984204, 286.09,
           Species::Isotopologue<Species::Species::CarbonDioxide>().findNum(
               Species::Atom::C12, Species::Atom::O16, Species::Atom::O16)},
          {Species::Species::CarbonDioxide, 636, 0.011057, 576.64,
           Species::Isotopologue<Species::Species::CarbonDioxide>().findNum(
               Species::Atom::C13, Species::Atom::O16, Species::Atom::O16)},
          {Species::Species::CarbonDioxide, 628, 0.003947, 607.81,
           Species::Isotopologue<Species::Species::CarbonDioxide>().findNum(
               Species::Atom::O16, Species::Atom::C12, Species::Atom::O18)},
          {Species::Species::CarbonDioxide, 627, 0.000733989, 3542.61,
           Species::Isotopologue<Species::Species::CarbonDioxide>().findNum(
               Species::Atom::O16, Species::Atom::C12, Species::Atom::O17)},
          {Species::Species::CarbonDioxide, 638, 4.43446e-05, 1225.46,
           Species::Isotopologue<Species::Species::CarbonDioxide>().findNum(
               Species::Atom::O16, Species::Atom::C13, Species::Atom::O18)},
          {Species::Species::CarbonDioxide, 637, 8.24623e-06, 7141.32,
           Species::Isotopologue<Species::Species::CarbonDioxide>().findNum(
               Species::Atom::O16, Species::Atom::C13, Species::Atom::O17)},
          {Species::Species::CarbonDioxide, 828, 3.95734e-06, 323.42,
           Species::Isotopologue<Species::Species::CarbonDioxide>().findNum(
               Species::Atom::C12, Species::Atom::O18, Species::Atom::O18)},
          {Species::Species::CarbonDioxide, 827, 1.4718e-06, 3766.58,
           Species::Isotopologue<Species::Species::CarbonDioxide>().findNum(
               Species::Atom::O17, Species::Atom::C12, Species::Atom::O18)},
          {Species::Species::CarbonDioxide, 727, 1.36847e-07, 10971.57,
           Species::Isotopologue<Species::Species::CarbonDioxide>().findNum(
               Species::Atom::C12, Species::Atom::O17, Species::Atom::O17)},
          {Species::Species::CarbonDioxide, 838, 4.446e-08, 652.24,
           Species::Isotopologue<Species::Species::CarbonDioxide>().findNum(
               Species::Atom::C13, Species::Atom::O18, Species::Atom::O18)},
          {Species::Species::CarbonDioxide, 837, 1.65354e-08, 7595.04,
           Species::Isotopologue<Species::Species::CarbonDioxide>().findNum(
               Species::Atom::O18, Species::Atom::C13, Species::Atom::O17)},
          {Species::Species::CarbonDioxide, 737, 1.5375e-09, 22120.47,
           Species::Isotopologue<Species::Species::CarbonDioxide>().findNum(
               Species::Atom::C13, Species::Atom::O17, Species::Atom::O17)}};
    case 3:
      return std::vector<IsotopologueData>{
          {Species::Species::Ozone, 666, 0.992901, 3483.71,
           Species::Isotopologue<Species::Species::Ozone>().findNum(
               Species::Atom::O16, Species::Atom::O16, Species::Atom::O16)},
          {Species::Species::Ozone, 668, 0.003982, 7465.68,
           Species::Isotopologue<Species::Species::Ozone>().findNum(
               Species::Atom::O16, Species::Atom::O16, Species::Atom::O18)},
          {Species::Species::Ozone, 686, 0.001991, 3647.08,
           Species::Isotopologue<Species::Species::Ozone>().findNum(
               Species::Atom::O16, Species::Atom::O18, Species::Atom::O16)},
          {Species::Species::Ozone, 667, 0.000740475, 43330.85,
           Species::Isotopologue<Species::Species::Ozone>().findNum(
               Species::Atom::O16, Species::Atom::O16, Species::Atom::O17)},
          {Species::Species::Ozone, 676, 0.000370237, 21404.96,
           Species::Isotopologue<Species::Species::Ozone>().findNum(
               Species::Atom::O16, Species::Atom::O17, Species::Atom::O16)}};
    case 4:
      return std::vector<IsotopologueData>{
          {Species::Species::NitrogenOxide, 446, 0.990333, 4984.9,
           Species::Isotopologue<Species::Species::NitrogenOxide>().findNum(
               Species::Atom::N14, Species::Atom::N14, Species::Atom::O16)},
          {Species::Species::NitrogenOxide, 456, 0.003641, 3362.01,
           Species::Isotopologue<Species::Species::NitrogenOxide>().findNum(
               Species::Atom::N14, Species::Atom::N15, Species::Atom::O16)},
          {Species::Species::NitrogenOxide, 546, 0.003641, 3458.58,
           Species::Isotopologue<Species::Species::NitrogenOxide>().findNum(
               Species::Atom::N15, Species::Atom::N14, Species::Atom::O16)},
          {Species::Species::NitrogenOxide, 448, 0.001986, 5314.74,
           Species::Isotopologue<Species::Species::NitrogenOxide>().findNum(
               Species::Atom::N14, Species::Atom::N14, Species::Atom::O18)},
          {Species::Species::NitrogenOxide, 447, 0.00036928, 30971.79,
           Species::Isotopologue<Species::Species::NitrogenOxide>().findNum(
               Species::Atom::N14, Species::Atom::N14, Species::Atom::O17)}};
    case 5:
      return std::vector<IsotopologueData>{
          {Species::Species::CarbonMonoxide, 26, 0.986544, 107.42,
           Species::Isotopologue<Species::Species::CarbonMonoxide>().findNum(
               Species::Atom::C12, Species::Atom::O16)},
          {Species::Species::CarbonMonoxide, 36, 0.011084, 224.69,
           Species::Isotopologue<Species::Species::CarbonMonoxide>().findNum(
               Species::Atom::C13, Species::Atom::O16)},
          {Species::Species::CarbonMonoxide, 28, 0.001978, 112.77,
           Species::Isotopologue<Species::Species::CarbonMonoxide>().findNum(
               Species::Atom::C12, Species::Atom::O18)},
          {Species::Species::CarbonMonoxide, 27, 0.000367867, 661.17,
           Species::Isotopologue<Species::Species::CarbonMonoxide>().findNum(
               Species::Atom::C12, Species::Atom::O17)},
          {Species::Species::CarbonMonoxide, 38, 2.2225e-05, 236.44,
           Species::Isotopologue<Species::Species::CarbonMonoxide>().findNum(
               Species::Atom::C13, Species::Atom::O18)},
          {Species::Species::CarbonMonoxide, 37, 4.13292e-06, 1384.66,
           Species::Isotopologue<Species::Species::CarbonMonoxide>().findNum(
               Species::Atom::C13, Species::Atom::O17)}};
    case 6:
      return std::vector<IsotopologueData>{
          {Species::Species::Methane, 211, 0.988274, 590.48,
           Species::Isotopologue<Species::Species::Methane>().findNum(
               Species::Atom::C12, Species::Atom::H, Species::Atom::H,
               Species::Atom::H, Species::Atom::H)},
          {Species::Species::Methane, 311, 0.011103, 1180.82,
           Species::Isotopologue<Species::Species::Methane>().findNum(
               Species::Atom::C13, Species::Atom::H, Species::Atom::H,
               Species::Atom::H, Species::Atom::H)},
          {Species::Species::Methane, 212, 0.000615751, 4794.73,
           Species::Isotopologue<Species::Species::Methane>().findNum(
               Species::Atom::C12, Species::Atom::H, Species::Atom::H,
               Species::Atom::H, Species::Atom::D)},
          {Species::Species::Methane, 312, 6.91785e-06, 9599.16,
           Species::Isotopologue<Species::Species::Methane>().findNum(
               Species::Atom::C13, Species::Atom::H, Species::Atom::H,
               Species::Atom::H, Species::Atom::D)}};
    case 7:
      return std::vector<IsotopologueData>{
          {Species::Species::Oxygen, 66, 0.995262, 215.73,
           Species::Isotopologue<Species::Species::Oxygen>().findNum(
               Species::Atom::O16, Species::Atom::O16)},
          {Species::Species::Oxygen, 68, 0.003991, 455.23,
           Species::Isotopologue<Species::Species::Oxygen>().findNum(
               Species::Atom::O16, Species::Atom::O18)},
          {Species::Species::Oxygen, 67, 0.000742235, 2658.12,
           Species::Isotopologue<Species::Species::Oxygen>().findNum(
               Species::Atom::O16, Species::Atom::O17)}};
    case 8:
      return std::vector<IsotopologueData>{
          {Species::Species::NitricOxide, 46, 0.993974, 1142.13,
           Species::Isotopologue<Species::Species::NitricOxide>().findNum(
               Species::Atom::N14, Species::Atom::O16)},
          {Species::Species::NitricOxide, 56, 0.003654, 789.26,
           Species::Isotopologue<Species::Species::NitricOxide>().findNum(
               Species::Atom::N15, Species::Atom::O16)},
          {Species::Species::NitricOxide, 48, 0.001993, 1204.44,
           Species::Isotopologue<Species::Species::NitricOxide>().findNum(
               Species::Atom::N14, Species::Atom::O18)}};
    case 9:
      return std::vector<IsotopologueData>{
          {Species::Species::SulfurDioxide, 626, 0.945678, 6340.3,
           Species::Isotopologue<Species::Species::SulfurDioxide>().findNum(
               Species::Atom::S32, Species::Atom::O16, Species::Atom::O16)},
          {Species::Species::SulfurDioxide, 646, 0.04195, 6368.98,
           Species::Isotopologue<Species::Species::SulfurDioxide>().findNum(
               Species::Atom::S34, Species::Atom::O16, Species::Atom::O16)}};
    case 10:
      return std::vector<IsotopologueData>{
          {Species::Species::NitrogenDioxide, 646, 0.991616, 13577.48,
           Species::Isotopologue<Species::Species::NitrogenDioxide>().findNum(
               Species::Atom::N14, Species::Atom::O16, Species::Atom::O16)},
          {Species::Species::NitrogenDioxide, 656, 0.003646, 9324.7,
           Species::Isotopologue<Species::Species::NitrogenDioxide>().findNum(
               Species::Atom::N15, Species::Atom::O16, Species::Atom::O16)}};
    case 11:
      return std::vector<IsotopologueData>{
          {Species::Species::Ammonia, 4111, 0.995872, 1725.22,
           Species::Isotopologue<Species::Species::Ammonia>().findNum(
               Species::Atom::N14, Species::Atom::H, Species::Atom::H,
               Species::Atom::H)},
          {Species::Species::Ammonia, 5111, 0.003661, 1153.3,
           Species::Isotopologue<Species::Species::Ammonia>().findNum(
               Species::Atom::N15, Species::Atom::H, Species::Atom::H,
               Species::Atom::H)}};
    case 12:
      return std::vector<IsotopologueData>{
          {Species::Species::NitricAcid, 146, 0.98911, 214000.0,
           Species::Isotopologue<Species::Species::NitricAcid>().findNum(
               Species::Atom::H, Species::Atom::N14, Species::Atom::O16,
               Species::Atom::O16, Species::Atom::O16)},
          {Species::Species::NitricAcid, 156, 0.003636, 143000.0,
           Species::Isotopologue<Species::Species::NitricAcid>().findNum(
               Species::Atom::H, Species::Atom::N15, Species::Atom::O16,
               Species::Atom::O16, Species::Atom::O16)}};
    case 13:
      return std::vector<IsotopologueData>{
          {Species::Species::Hydroxyl, 61, 0.997473, 80.35,
           Species::Isotopologue<Species::Species::Hydroxyl>().findNum(
               Species::Atom::O16, Species::Atom::H)},
          {Species::Species::Hydroxyl, 81, 0.002, 80.88,
           Species::Isotopologue<Species::Species::Hydroxyl>().findNum(
               Species::Atom::O18, Species::Atom::H)},
          {Species::Species::Hydroxyl, 62, 0.000155371, 209.32,
           Species::Isotopologue<Species::Species::Hydroxyl>().findNum(
               Species::Atom::O16, Species::Atom::D)}};
    case 14:
      return std::vector<IsotopologueData>{
          {Species::Species::HydrogenFluoride, 19, 0.999844, 41.47,
           Species::Isotopologue<Species::Species::HydrogenFluoride>().findNum(
               Species::Atom::H, Species::Atom::F19)},
          {Species::Species::HydrogenFluoride, 29, 0.000155741, 115.91,
           Species::Isotopologue<Species::Species::HydrogenFluoride>().findNum(
               Species::Atom::D, Species::Atom::F19)}};
    case 15:
      return std::vector<IsotopologueData>{
          {Species::Species::HydrogenChloride, 15, 0.757587, 160.65,
           Species::Isotopologue<Species::Species::HydrogenChloride>().findNum(
               Species::Atom::H, Species::Atom::Cl35)},
          {Species::Species::HydrogenChloride, 17, 0.242257, 160.89,
           Species::Isotopologue<Species::Species::HydrogenChloride>().findNum(
               Species::Atom::H, Species::Atom::Cl37)},
          {Species::Species::HydrogenChloride, 25, 0.000118005, 462.78,
           Species::Isotopologue<Species::Species::HydrogenChloride>().findNum(
               Species::Atom::D, Species::Atom::Cl35)},
          {Species::Species::HydrogenChloride, 27, 3.7735e-05, 464.13,
           Species::Isotopologue<Species::Species::HydrogenChloride>().findNum(
               Species::Atom::D, Species::Atom::Cl37)}};
    case 16:
      return std::vector<IsotopologueData>{
          {Species::Species::HydrogenBromide, 19, 0.506781, 200.17,
           Species::Isotopologue<Species::Species::HydrogenBromide>().findNum(
               Species::Atom::H, Species::Atom::Br79)},
          {Species::Species::HydrogenBromide, 11, 0.493063, 200.23,
           Species::Isotopologue<Species::Species::HydrogenBromide>().findNum(
               Species::Atom::H, Species::Atom::Br81)},
          {Species::Species::HydrogenBromide, 29, 7.89384e-05, 586.4,
           Species::Isotopologue<Species::Species::HydrogenBromide>().findNum(
               Species::Atom::D, Species::Atom::Br79)},
          {Species::Species::HydrogenBromide, 21, 7.68016e-05, 586.76,
           Species::Isotopologue<Species::Species::HydrogenBromide>().findNum(
               Species::Atom::D, Species::Atom::Br81)}};
    case 17:
      return std::vector<IsotopologueData>{
          {Species::Species::HydrogenIodide, 17, 0.999844, 388.99,
           Species::Isotopologue<Species::Species::HydrogenIodide>().findNum(
               Species::Atom::H, Species::Atom::I127)},
          {Species::Species::HydrogenIodide, 27, 0.000155741, 1147.06,
           Species::Isotopologue<Species::Species::HydrogenIodide>().findNum(
               Species::Atom::D, Species::Atom::I127)}};
    case 18:
      return std::vector<IsotopologueData>{
          {Species::Species::ChlorineMonoxide, 56, 0.755908, 3274.61,
           Species::Isotopologue<Species::Species::ChlorineMonoxide>().findNum(
               Species::Atom::Cl35, Species::Atom::O16)},
          {Species::Species::ChlorineMonoxide, 76, 0.24172, 3332.29,
           Species::Isotopologue<Species::Species::ChlorineMonoxide>().findNum(
               Species::Atom::Cl37, Species::Atom::O16)}};
    case 19:
      return std::vector<IsotopologueData>{
          {Species::Species::CarbonylSulfide, 622, 0.937395, 1221.01,
           Species::Isotopologue<Species::Species::CarbonylSulfide>().findNum(
               Species::Atom::O16, Species::Atom::C12, Species::Atom::S32)},
          {Species::Species::CarbonylSulfide, 624, 0.041583, 1253.48,
           Species::Isotopologue<Species::Species::CarbonylSulfide>().findNum(
               Species::Atom::O16, Species::Atom::C12, Species::Atom::S34)},
          {Species::Species::CarbonylSulfide, 632, 0.010531, 2484.15,
           Species::Isotopologue<Species::Species::CarbonylSulfide>().findNum(
               Species::Atom::O16, Species::Atom::C13, Species::Atom::S32)},
          {Species::Species::CarbonylSulfide, 623, 0.007399, 4950.11,
           Species::Isotopologue<Species::Species::CarbonylSulfide>().findNum(
               Species::Atom::O16, Species::Atom::C12, Species::Atom::S33)},
          {Species::Species::CarbonylSulfide, 822, 0.00188, 1313.78,
           Species::Isotopologue<Species::Species::CarbonylSulfide>().findNum(
               Species::Atom::O18, Species::Atom::C12, Species::Atom::S32)}};
    case 20:
      return std::vector<IsotopologueData>{
          {Species::Species::Formaldehyde, 126, 0.986237, 2844.53,
           Species::Isotopologue<Species::Species::Formaldehyde>().findNum(
               Species::Atom::H, Species::Atom::H, Species::Atom::C12,
               Species::Atom::O16)},
          {Species::Species::Formaldehyde, 136, 0.01108, 5837.69,
           Species::Isotopologue<Species::Species::Formaldehyde>().findNum(
               Species::Atom::H, Species::Atom::H, Species::Atom::C13,
               Species::Atom::O16)},
          {Species::Species::Formaldehyde, 128, 0.001978, 2986.44,
           Species::Isotopologue<Species::Species::Formaldehyde>().findNum(
               Species::Atom::H, Species::Atom::H, Species::Atom::C12,
               Species::Atom::O18)}};
    case 21:
      return std::vector<IsotopologueData>{
          {Species::Species::HypochlorousAcid, 165, 0.75579, 19274.79,
           Species::Isotopologue<Species::Species::HypochlorousAcid>().findNum(
               Species::Atom::H, Species::Atom::O16, Species::Atom::Cl35)},
          {Species::Species::HypochlorousAcid, 167, 0.241683, 19616.2,
           Species::Isotopologue<Species::Species::HypochlorousAcid>().findNum(
               Species::Atom::H, Species::Atom::O16, Species::Atom::Cl37)}};
    case 22:
      return std::vector<IsotopologueData>{
          {Species::Species::Nitrogen, 44, 0.992687, 467.1,
           Species::Isotopologue<Species::Species::Nitrogen>().findNum(
               Species::Atom::N14, Species::Atom::N14)},
          {Species::Species::Nitrogen, 45, 0.007478, 644.1,
           Species::Isotopologue<Species::Species::Nitrogen>().findNum(
               Species::Atom::N14, Species::Atom::N15)}};
    case 23:
      return std::vector<IsotopologueData>{
          {Species::Species::HydrogenCyanide, 124, 0.985114, 892.2,
           Species::Isotopologue<Species::Species::HydrogenCyanide>().findNum(
               Species::Atom::H, Species::Atom::C12, Species::Atom::N14)},
          {Species::Species::HydrogenCyanide, 134, 0.011068, 1830.97,
           Species::Isotopologue<Species::Species::HydrogenCyanide>().findNum(
               Species::Atom::H, Species::Atom::C13, Species::Atom::N14)},
          {Species::Species::HydrogenCyanide, 125, 0.003622, 615.28,
           Species::Isotopologue<Species::Species::HydrogenCyanide>().findNum(
               Species::Atom::H, Species::Atom::C12, Species::Atom::N15)}};
    case 24:
      return std::vector<IsotopologueData>{
          {Species::Species::MethylChloride, 215, 0.748937, 57916.12,
           Species::Isotopologue<Species::Species::MethylChloride>().findNum(
               Species::Atom::C12, Species::Atom::H, Species::Atom::H,
               Species::Atom::H, Species::Atom::Cl35)},
          {Species::Species::MethylChloride, 217, 0.239491, 58833.9,
           Species::Isotopologue<Species::Species::MethylChloride>().findNum(
               Species::Atom::C12, Species::Atom::H, Species::Atom::H,
               Species::Atom::H, Species::Atom::Cl37)}};
    case 25:
      return std::vector<IsotopologueData>{
          {Species::Species::HydrogenPeroxide, 1661, 0.994952, 9847.99,
           Species::Isotopologue<Species::Species::HydrogenPeroxide>().findNum(
               Species::Atom::H, Species::Atom::H, Species::Atom::O16,
               Species::Atom::O16)}};
    case 26:
      return std::vector<IsotopologueData>{
          {Species::Species::Acetylene, 1221, 0.977599, 412.45,
           Species::Isotopologue<Species::Species::Acetylene>().findNum(
               Species::Atom::C12, Species::Atom::C12, Species::Atom::H,
               Species::Atom::H)},
          {Species::Species::Acetylene, 1231, 0.021966, 1656.18,
           Species::Isotopologue<Species::Species::Acetylene>().findNum(
               Species::Atom::H, Species::Atom::C12, Species::Atom::C13,
               Species::Atom::H)},
          {Species::Species::Acetylene, 1222, 0.00030455, 1581.84,
           Species::Isotopologue<Species::Species::Acetylene>().findNum(
               Species::Atom::H, Species::Atom::C12, Species::Atom::C12,
               Species::Atom::D)}};
    case 27:
      return std::vector<IsotopologueData>{
          {Species::Species::Ethane, 1221, 0.97699, 70882.52,
           Species::Isotopologue<Species::Species::Ethane>().findNum(
               Species::Atom::C12, Species::Atom::C12, Species::Atom::H,
               Species::Atom::H, Species::Atom::H, Species::Atom::H,
               Species::Atom::H, Species::Atom::H)},
          {Species::Species::Ethane, 1231, 0.021953, 36191.8,
           Species::Isotopologue<Species::Species::Ethane>().findNum(
               Species::Atom::C12, Species::Atom::H, Species::Atom::H,
               Species::Atom::H, Species::Atom::C13, Species::Atom::H,
               Species::Atom::H, Species::Atom::H)}};
    case 28:
      return std::vector<IsotopologueData>{
          {Species::Species::Phosphine, 1111, 0.999533, 3249.44,
           Species::Isotopologue<Species::Species::Phosphine>().findNum(
               Species::Atom::P31, Species::Atom::H, Species::Atom::H,
               Species::Atom::H)}};
    case 29:
      return std::vector<IsotopologueData>{
          {Species::Species::CarbonylFluoride, 269, 0.986544, 70028.43,
           Species::Isotopologue<Species::Species::CarbonylFluoride>().findNum(
               Species::Atom::C12, Species::Atom::O16, Species::Atom::F19,
               Species::Atom::F19)},
          {Species::Species::CarbonylFluoride, 369, 0.011083, 140000.0,
           Species::Isotopologue<Species::Species::CarbonylFluoride>().findNum(
               Species::Atom::C13, Species::Atom::O16, Species::Atom::F19,
               Species::Atom::F19)}};
    case 30:
      return std::vector<IsotopologueData>{
          {Species::Species::SulfurHexafluoride, 29, 0.95018, 1620000.0,
           Species::Isotopologue<Species::Species::SulfurHexafluoride>()
               .findNum(Species::Atom::S32, Species::Atom::F19,
                        Species::Atom::F19, Species::Atom::F19,
                        Species::Atom::F19, Species::Atom::F19,
                        Species::Atom::F19)}};
    case 31:
      return std::vector<IsotopologueData>{
          {Species::Species::HydrogenSulfide, 121, 0.949884, 505.79,
           Species::Isotopologue<Species::Species::HydrogenSulfide>().findNum(
               Species::Atom::H, Species::Atom::H, Species::Atom::S32)},
          {Species::Species::HydrogenSulfide, 141, 0.042137, 504.35,
           Species::Isotopologue<Species::Species::HydrogenSulfide>().findNum(
               Species::Atom::H, Species::Atom::H, Species::Atom::S34)},
          {Species::Species::HydrogenSulfide, 131, 0.007498, 2014.94,
           Species::Isotopologue<Species::Species::HydrogenSulfide>().findNum(
               Species::Atom::H, Species::Atom::H, Species::Atom::S33)}};
    case 32:
      return std::vector<IsotopologueData>{
          {Species::Species::FormicAcid, 126, 0.983898, 39132.76,
           Species::Isotopologue<Species::Species::FormicAcid>().findNum(
               Species::Atom::H, Species::Atom::C12, Species::Atom::O16,
               Species::Atom::O16, Species::Atom::H)}};
    case 33:
      return std::vector<IsotopologueData>{
          {Species::Species::Hydroperoxyl, 166, 0.995107, 4300.39,
           Species::Isotopologue<Species::Species::Hydroperoxyl>().findNum(
               Species::Atom::H, Species::Atom::O16, Species::Atom::O16)}};
    case 34:
      return std::vector<IsotopologueData>{
          {Species::Species::OxygenAtom, 6, 0.997628, 6.72,
           Species::Isotopologue<Species::Species::OxygenAtom>().findNum(
               Species::Atom::O16)}};
    case 35:
      return std::vector<IsotopologueData>{
          {Species::Species::ChlorineNitrate, 5646, 0.74957, 4790000.0,
           Species::Isotopologue<Species::Species::ChlorineNitrate>().findNum(
               Species::Atom::Cl35, Species::Atom::O16, Species::Atom::N14,
               Species::Atom::O16, Species::Atom::O16)},
          {Species::Species::ChlorineNitrate, 7646, 0.239694, 4910000.0,
           Species::Isotopologue<Species::Species::ChlorineNitrate>().findNum(
               Species::Atom::Cl37, Species::Atom::O16, Species::Atom::N14,
               Species::Atom::O16, Species::Atom::O16)}};
    case 36:
      return std::vector<IsotopologueData>{
          {Species::Species::NitricOxideCation, 46, 0.993974, 311.69,
           Species::Isotopologue<Species::Species::NitricOxideCation>().findNum(
               Species::Atom::N14, Species::Atom::O16 + 1)}};
    case 37:
      return std::vector<IsotopologueData>{
          {Species::Species::HypobromousAcid, 169, 0.505579, 28339.38,
           Species::Isotopologue<Species::Species::HypobromousAcid>().findNum(
               Species::Atom::H, Species::Atom::O16, Species::Atom::Br79)},
          {Species::Species::HypobromousAcid, 161, 0.491894, 28237.98,
           Species::Isotopologue<Species::Species::HypobromousAcid>().findNum(
               Species::Atom::H, Species::Atom::O16, Species::Atom::Br81)}};
    case 38:
      return std::vector<IsotopologueData>{
          {Species::Species::Ethylene, 221, 0.977294, 11041.54,
           Species::Isotopologue<Species::Species::Ethylene>().findNum(
               Species::Atom::C12, Species::Atom::C12, Species::Atom::H,
               Species::Atom::H, Species::Atom::H, Species::Atom::H)},
          {Species::Species::Ethylene, 231, 0.021959, 45196.89,
           Species::Isotopologue<Species::Species::Ethylene>().findNum(
               Species::Atom::C12, Species::Atom::H, Species::Atom::H,
               Species::Atom::C13, Species::Atom::H, Species::Atom::H)}};
    case 39:
      return std::vector<IsotopologueData>{
          {Species::Species::Methanol, 2161, 0.98593, 70569.92,
           Species::Isotopologue<Species::Species::Methanol>().findNum(
               Species::Atom::C12, Species::Atom::H, Species::Atom::H,
               Species::Atom::H, Species::Atom::O16, Species::Atom::H)}};
    case 40:
      return std::vector<IsotopologueData>{
          {Species::Species::MethylBromide, 219, 0.500995, 83051.98,
           Species::Isotopologue<Species::Species::MethylBromide>().findNum(
               Species::Atom::C12, Species::Atom::H, Species::Atom::H,
               Species::Atom::H, Species::Atom::Br79)},
          {Species::Species::MethylBromide, 211, 0.487433, 83395.21,
           Species::Isotopologue<Species::Species::MethylBromide>().findNum(
               Species::Atom::C12, Species::Atom::H, Species::Atom::H,
               Species::Atom::H, Species::Atom::Br81)}};
    case 41:
      return std::vector<IsotopologueData>{
          {Species::Species::Acetonitrile, 2124, 0.973866, 88672.19,
           Species::Isotopologue<Species::Species::Acetonitrile>().findNum(
               Species::Atom::C12, Species::Atom::H, Species::Atom::H,
               Species::Atom::H, Species::Atom::C12, Species::Atom::N14)}};
    case 42:
      return std::vector<IsotopologueData>{
          {Species::Species::Pfc14, 29, 0.98889, 121000.0,
           Species::Isotopologue<Species::Species::Pfc14>().findNum(
               Species::Atom::C12, Species::Atom::F19, Species::Atom::F19,
               Species::Atom::F19, Species::Atom::F19)}};
    case 43:
      return std::vector<IsotopologueData>{
          {Species::Species::Diacetylene, 2211, 0.955998, 9818.97,
           Species::Isotopologue<Species::Species::Diacetylene>().findNum(
               Species::Atom::C12, Species::Atom::C12, Species::Atom::C12,
               Species::Atom::C12, Species::Atom::H, Species::Atom::H)}};
    case 44:
      return std::vector<IsotopologueData>{
          {Species::Species::Cyanoacetylene, 1224, 0.963346, 24786.84,
           Species::Isotopologue<Species::Species::Cyanoacetylene>().findNum(
               Species::Atom::H, Species::Atom::C12, Species::Atom::C12,
               Species::Atom::C12, Species::Atom::N14)}};
    case 45:
      return std::vector<IsotopologueData>{
          {Species::Species::Hydrogen, 11, 0.999688, 7.67,
           Species::Isotopologue<Species::Species::Hydrogen>().findNum(
               Species::Atom::H, Species::Atom::H)},
          {Species::Species::Hydrogen, 12, 0.000311432, 29.87,
           Species::Isotopologue<Species::Species::Hydrogen>().findNum(
               Species::Atom::H, Species::Atom::D)}};
    case 46:
      return std::vector<IsotopologueData>{
          {Species::Species::CarbonMonosulfide, 22, 0.939624, 253.62,
           Species::Isotopologue<Species::Species::CarbonMonosulfide>().findNum(
               Species::Atom::C12, Species::Atom::S32)},
          {Species::Species::CarbonMonosulfide, 24, 0.041682, 257.77,
           Species::Isotopologue<Species::Species::CarbonMonosulfide>().findNum(
               Species::Atom::C12, Species::Atom::S34)},
          {Species::Species::CarbonMonosulfide, 32, 0.010556, 537.5,
           Species::Isotopologue<Species::Species::CarbonMonosulfide>().findNum(
               Species::Atom::C13, Species::Atom::S32)},
          {Species::Species::CarbonMonosulfide, 23, 0.007417, 1022.97,
           Species::Isotopologue<Species::Species::CarbonMonosulfide>().findNum(
               Species::Atom::C12, Species::Atom::S33)}};
    case 47:
      return std::vector<IsotopologueData>{
          {Species::Species::SulfurTrioxide, 26, 0.9434, 7783.3,
           Species::Isotopologue<Species::Species::SulfurTrioxide>().findNum(
               Species::Atom::S32, Species::Atom::O16, Species::Atom::O16,
               Species::Atom::O16)}};
    case 48:
      return std::vector<IsotopologueData>{
          {Species::Species::Cyanogen, 4224, 0.970752, 15582.44,
           Species::Isotopologue<Species::Species::Cyanogen>().findNum(
               Species::Atom::C12, Species::Atom::C12, Species::Atom::N14,
               Species::Atom::N14)}};
    case 49:
      return std::vector<IsotopologueData>{
          {Species::Species::Phosgene, 2655, 0.566392, 1480000.0,
           Species::Isotopologue<Species::Species::Phosgene>().findNum(
               Species::Atom::C12, Species::Atom::O16, Species::Atom::Cl35,
               Species::Atom::Cl35)},
          {Species::Species::Phosgene, 2657, 0.362235, 3040000.0,
           Species::Isotopologue<Species::Species::Phosgene>().findNum(
               Species::Atom::C12, Species::Atom::O16, Species::Atom::Cl35,
               Species::Atom::Cl37)}};
    default: { /* you have some work to do if you see this! */
    }
  }
  return std::vector<IsotopologueData>{};
}

std::vector<IsotopologueData> getIsotopologueData(Species::Species x) {
  const std::vector<IsotopologueData> out = getIsotopologueData(toNumber(x));
  if (out.size() == 0)
    throw std::runtime_error("Bad species for HITRAN data...");
  else
    return out;
}

ParForm parse_parform(const std::string &line,
                      const unsigned long offset) noexcept {
  ParForm out;

  char m1, m2;
  char c;
  sscanf(line.c_str() + offset,
         "%c%c"
         "%c"
         "%12lf"
         "%10lf"
         "%10lf"
         "%5lf"
         "%5lf"
         "%10lf"
         "%4lf"
         "%8lf"
         "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
         "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
         "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
         "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
         "%c%c%c%c%c%c"
         "%c%c%c%c%c%c%c%c%c%c%c%c"
         "%c"
         "%7lf"
         "%7lf",
         &m1, &m2, &c, &out.v, &out.S, &out.A, &out.gamma_air, &out.gamma_self,
         &out.Elow, &out.n_air, &out.delta_air, &out.Vlow[0], &out.Vlow[1],
         &out.Vlow[2], &out.Vlow[3], &out.Vlow[4], &out.Vlow[5], &out.Vlow[6],
         &out.Vlow[7], &out.Vlow[8], &out.Vlow[9], &out.Vlow[10], &out.Vlow[11],
         &out.Vlow[12], &out.Vlow[13], &out.Vlow[14], &out.Vup[0], &out.Vup[1],
         &out.Vup[2], &out.Vup[3], &out.Vup[4], &out.Vup[5], &out.Vup[6],
         &out.Vup[7], &out.Vup[8], &out.Vup[9], &out.Vup[10], &out.Vup[11],
         &out.Vup[12], &out.Vup[13], &out.Vup[14], &out.Qlow[0], &out.Qlow[1],
         &out.Qlow[2], &out.Qlow[3], &out.Qlow[4], &out.Qlow[5], &out.Qlow[6],
         &out.Qlow[7], &out.Qlow[8], &out.Qlow[9], &out.Qlow[10], &out.Qlow[11],
         &out.Qlow[12], &out.Qlow[13], &out.Qlow[14], &out.Qup[0], &out.Qup[1],
         &out.Qup[2], &out.Qup[3], &out.Qup[4], &out.Qup[5], &out.Qup[6],
         &out.Qup[7], &out.Qup[8], &out.Qup[9], &out.Qup[10], &out.Qup[11],
         &out.Qup[12], &out.Qup[13], &out.Qup[14], &out.Ierr[0], &out.Ierr[1],
         &out.Ierr[2], &out.Ierr[3], &out.Ierr[4], &out.Ierr[5], &out.Iref[0],
         &out.Iref[1], &out.Iref[2], &out.Iref[3], &out.Iref[4], &out.Iref[5],
         &out.Iref[6], &out.Iref[7], &out.Iref[8], &out.Iref[9], &out.Iref[10],
         &out.Iref[11], &out.star, &out.gupp, &out.glow);

  if (c == '0')
    out.I = 10;
  else if (c == 'A')
    out.I = 11;
  else if (c == 'B')
    out.I = 12;
  else {
    std::stringstream str;
    str << c;
    str >> out.I;
  }
  std::stringstream str;
  str << m1 << m2;
  str >> out.M;

  return out;
}

std::pair<unsigned long, std::vector<std::array<std::string, 2>>>
split_quantum_numbers_from_hitran_online(const std::string &qns,
                                         const unsigned long offset) noexcept {
  auto start = qns.cbegin() + offset;  // start point
  auto first_tab =
      std::find(start, qns.cend(), '\t');  // separator of upper/lower
  auto first_newline =
      std::find(start, qns.cend(), '\n');         // separator of lines
  auto end = std::min(first_tab, first_newline);  // end point

  std::vector<std::array<std::string, 2>> out{0};
  while (true) {
    auto semicolon = std::find(start, end, ';');
    auto equalsign = std::find(start, semicolon, '=');
    auto hashsign = std::find(start, semicolon, '#');
    if (semicolon == equalsign)
      break;  // This means we are at the end of the line
    out.push_back(
        {std::string{start, std::min(equalsign, hashsign)},
         std::string{equalsign + 1, semicolon}});  // key + value (minus '=')
    start += (semicolon - start) + 1;  // move start to after the semicolon
  }

  return std::make_pair(end - qns.cbegin() + 1, out);
}

std::array<Quantum::Number, size_t(QuantumTypes::FINAL)> process_qns_strings(
    const std::vector<std::array<std::string, 2>> &qnspairs) noexcept {
  std::array<Quantum::Number, size_t(QuantumTypes::FINAL)> out;
  for (auto &keyval : qnspairs) {
    out[(unsigned char)toQuantumTypes(keyval[0])] = Quantum::Number(
        keyval[1]);  // FIXME:  If there are too many F#, it might fail...
  }
  return out;
}

std::array<std::array<Quantum::Number, size_t(QuantumTypes::FINAL)>, 2>
parse_quantum(const std::string &line, const unsigned long offset) noexcept {
  const auto upper = split_quantum_numbers_from_hitran_online(line, offset);
  const auto lower =
      split_quantum_numbers_from_hitran_online(line, upper.first);
  return {process_qns_strings(upper.second), process_qns_strings(lower.second)};
}

}  // namespace HITRAN
