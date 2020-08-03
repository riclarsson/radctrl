#ifndef hitran_h
#define hitran_h

#include "enums.h"
#include "quantum.h"
#include "units.h"
#include "species.h"

namespace HITRAN {
// HITRAN Isotopologue Data
struct IsotopologueData {
  Species::Species spec;
  long afgl;
  double abundance;
  double Q296;
  unsigned char isotpos;
};  // IsotopologueData

// HITRAN data on 2020-07-13 (do not remove)
std::vector<IsotopologueData> getIsotopologueData(Species::Species x);

/* struct containing par-format results */
struct ParForm {
  Frequency<FrequencyType::Kayser> v;
  LineStrength<FrequencyType::Kayser, AreaType::cm2> S;
  double A;
  PressureBroadening<FrequencyType::Kayser, PressureType::Atm> gamma_air;
  PressureBroadening<FrequencyType::Kayser, PressureType::Atm> gamma_self;
  Energy<EnergyType::invcm> Elow;
  double n_air;
  PressureBroadening<FrequencyType::Kayser, PressureType::Atm> delta_air;
  double glow;
  double gupp;
  long M;
  long I;
  char star;
  std::array<char, 6> Ierr;
  std::array<char, 12> Iref;
  std::array<char, 15> Vup;
  std::array<char, 15> Vlow;
  std::array<char, 15> Qup;
  std::array<char, 15> Qlow;
};

std::vector<IsotopologueData> getIsotopologueData(long x) noexcept;

ParForm parse_parform(const std::string& line, unsigned long offset=0) noexcept;

ENUMCLASS(QuantumTypes, unsigned char,
          J, N, S, F, K, Ka, Kc, N1, L,
          v, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12,
          l, l2, l3, l4, l5, l6, l7, l8, l9, l10, l11, l12,
          Lambda, Omega, rovibSym, n, alpha, tau, r, config, term, 
          ElecStateLabel, kronigParity, parity, vibSym, vibInv, rotSym, vibRefl)

using QNS = std::array<Quantum::Number, size_t(QuantumTypes::FINAL)>;
std::array<QNS, 2> parse_quantum(const std::string& line, const unsigned long offset=0) noexcept;

constexpr Quantum::Type toQuantumType(QuantumTypes x) {
  switch(x) {
    case QuantumTypes::J: return Quantum::Type::J;
    case QuantumTypes::N: return Quantum::Type::N;
    case QuantumTypes::S: return Quantum::Type::S;
    case QuantumTypes::F: return Quantum::Type::F;
    case QuantumTypes::K: return Quantum::Type::K;
    case QuantumTypes::Ka: return Quantum::Type::Ka;
    case QuantumTypes::Kc: return Quantum::Type::Kc;
    case QuantumTypes::N1: return Quantum::Type::N1;
    case QuantumTypes::L: return Quantum::Type::L;
    case QuantumTypes::v: return Quantum::Type::v;
    case QuantumTypes::v1: return Quantum::Type::v1;
    case QuantumTypes::v2: return Quantum::Type::v2;
    case QuantumTypes::v3: return Quantum::Type::v3;
    case QuantumTypes::v4: return Quantum::Type::v4;
    case QuantumTypes::v5: return Quantum::Type::v5;
    case QuantumTypes::v6: return Quantum::Type::v6;
    case QuantumTypes::v7: return Quantum::Type::v7;
    case QuantumTypes::v8: return Quantum::Type::v8;
    case QuantumTypes::v9: return Quantum::Type::v9;
    case QuantumTypes::v10: return Quantum::Type::v10;
    case QuantumTypes::v11: return Quantum::Type::v11;
    case QuantumTypes::v12: return Quantum::Type::v12;
    case QuantumTypes::l: return Quantum::Type::l;
    case QuantumTypes::l2: return Quantum::Type::l2;
    case QuantumTypes::l3: return Quantum::Type::l3;
    case QuantumTypes::l4: return Quantum::Type::l4;
    case QuantumTypes::l5: return Quantum::Type::l5;
    case QuantumTypes::l6: return Quantum::Type::l6;
    case QuantumTypes::l7: return Quantum::Type::l7;
    case QuantumTypes::l8: return Quantum::Type::l8;
    case QuantumTypes::l9: return Quantum::Type::l9;
    case QuantumTypes::l10: return Quantum::Type::l10;
    case QuantumTypes::l11: return Quantum::Type::l11;
    case QuantumTypes::l12: return Quantum::Type::l12;
    case QuantumTypes::Lambda: return Quantum::Type::Lambda;
    case QuantumTypes::Omega: return Quantum::Type::Omega;
    case QuantumTypes::rovibSym: return Quantum::Type::rovibSym;
    case QuantumTypes::n: return Quantum::Type::n;
    case QuantumTypes::alpha: return Quantum::Type::alpha;
    case QuantumTypes::tau: return Quantum::Type::tau;
    case QuantumTypes::r: return Quantum::Type::r;
    case QuantumTypes::config: return Quantum::Type::config;
    case QuantumTypes::term: return Quantum::Type::term;
    case QuantumTypes::ElecStateLabel: return Quantum::Type::ElecStateLabel;
    case QuantumTypes::kronigParity: return Quantum::Type::kronigParity;
    case QuantumTypes::parity: return Quantum::Type::parity;
    case QuantumTypes::vibSym: return Quantum::Type::vibSym;
    case QuantumTypes::vibInv: return Quantum::Type::vibInv;
    case QuantumTypes::rotSym: return Quantum::Type::rotSym;
    case QuantumTypes::vibRefl: return Quantum::Type::vibRefl;
    case QuantumTypes::FINAL: return Quantum::Type::FINAL;
  }
  return {};
}

}  // HITRAN

#endif  // hitran_h
