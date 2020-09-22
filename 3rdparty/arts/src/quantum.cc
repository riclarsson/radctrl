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

#include "quantum.h"
#include <stdexcept>
#include "absorption.h"
#include "global_data.h"
#include "special_interp.h"

bool QuantumNumbers::Compare(const QuantumNumbers& qn) const {
  const QuantumContainer& qnumbers2 = qn.GetNumbers();

  bool match = true;

  Index qnri = 0;

  // Compare all quantum numbers in mqnumbers and qnumbers2
  while (match && qnri != Index(QuantumNumberType::FINAL_ENTRY)) {
    // If one of the two numbers is undefined, it is considered as
    // a match.
    if (!mqnumbers[qnri].isUndefined() && !qnumbers2[qnri].isUndefined() &&
        mqnumbers[qnri] != qnumbers2[qnri])
      match = false;

    qnri++;
  }

  return match;
}

// Tests if this is in other upper
bool QuantumIdentifier::InUpper(const QuantumIdentifier& other) const {
  if (mspecies not_eq other.mspecies) return false;
  if (miso not_eq other.miso) return false;

  if (mqtype == QuantumIdentifier::NONE or
      other.mqtype == QuantumIdentifier::NONE)
    return false;
  else if (mqtype == QuantumIdentifier::ALL or
           other.mqtype == QuantumIdentifier::ALL)
    return true;
  else if (mqtype not_eq QuantumIdentifier::ENERGY_LEVEL or
           other.mqtype not_eq QuantumIdentifier::TRANSITION)
    throw runtime_error(
        "One of your inputs is bad.  You are using function comparing energy levels to the upper state of lines, but the types mismatch");

  Index qnri = 0;
  while (qnri not_eq Index(QuantumNumberType::FINAL_ENTRY)) {
    if (other.mqm[TRANSITION_UPPER_INDEX][qnri].isUndefined()) {
      if (not mqm[ENERGY_LEVEL_INDEX][qnri].isUndefined()) return false;
    } else {
      if (other.mqm[TRANSITION_UPPER_INDEX][qnri] not_eq
          mqm[ENERGY_LEVEL_INDEX][qnri])
        return false;
    }
    qnri++;
  }

  return true;
}

// Tests that if this is in other lower
bool QuantumIdentifier::InLower(const QuantumIdentifier& other) const {
  if (mspecies not_eq other.mspecies) return false;
  if (miso not_eq other.miso) return false;

  if (mqtype == QuantumIdentifier::NONE or
      other.mqtype == QuantumIdentifier::NONE)
    return false;
  else if (mqtype == QuantumIdentifier::ALL or
           other.mqtype == QuantumIdentifier::ALL)
    return true;
  else if (mqtype not_eq QuantumIdentifier::ENERGY_LEVEL or
           other.mqtype not_eq QuantumIdentifier::TRANSITION)
    throw runtime_error(
        "One of your inputs is bad.  You are using function comparing energy levels to the lower state of lines, but the types mismatch");

  Index qnri = 0;
  while (qnri not_eq Index(QuantumNumberType::FINAL_ENTRY)) {
    if (other.mqm[TRANSITION_LOWER_INDEX][qnri].isUndefined()) {
      if (not mqm[ENERGY_LEVEL_INDEX][qnri].isUndefined()) return false;
    } else {
      if (other.mqm[TRANSITION_LOWER_INDEX][qnri] not_eq
          mqm[ENERGY_LEVEL_INDEX][qnri])
        return false;
    }
    qnri++;
  }

  return true;
}

// Tests that all of this is in other
bool QuantumIdentifier::In(const QuantumIdentifier& other) const {
  if (mspecies not_eq other.mspecies) return false;
  if (miso not_eq other.miso) return false;

  if (mqtype == QuantumIdentifier::NONE or
      other.mqtype == QuantumIdentifier::NONE)
    return false;
  else if (mqtype == QuantumIdentifier::ALL or
           other.mqtype == QuantumIdentifier::ALL) {
  } else if (mqtype not_eq other.mqtype)
    throw std::runtime_error(
        "Can never compare different types of identifiers with QID.In(QID), one of your inputs is of wrong QuantumIdentifier type");
  else if (mqtype == QuantumIdentifier::TRANSITION) {
    auto& other_low = other.mqm[TRANSITION_LOWER_INDEX];
    auto& other_upp = other.mqm[TRANSITION_UPPER_INDEX];
    auto& this_low = mqm[TRANSITION_LOWER_INDEX];
    auto& this_upp = mqm[TRANSITION_UPPER_INDEX];

    for (Index i = 0; i < Index(QuantumNumberType::FINAL_ENTRY); i++) {
      if (other_low[i].isUndefined()) {
      } else if (this_low[i].isUndefined())
        return false;
      else if (this_low[i] not_eq other_low[i])
        return false;

      if (other_upp[i].isUndefined()) {
      } else if (this_upp[i].isUndefined())
        return false;
      else if (this_upp[i] not_eq other_upp[i])
        return false;
    }
  } else {
    auto& other_qn = other.mqm[ENERGY_LEVEL_INDEX];
    auto& this_qn = mqm[ENERGY_LEVEL_INDEX];

    for (Index i = 0; i < Index(QuantumNumberType::FINAL_ENTRY); i++) {
      if (other_qn[i].isUndefined()) {
      } else if (this_qn[i].isUndefined())
        return false;
      else if (this_qn[i] not_eq other_qn[i])
        return false;
    }
  }

  return true;
}

bool IsValidQuantumNumberName(String name) {
  return QuantumNumberType::FINAL_ENTRY not_eq string2quantumnumbertype(name);
}

void ThrowIfQuantumNumberNameInvalid(String name) {
  if (!IsValidQuantumNumberName(name)) {
    ostringstream os;
    os << "Invalid quantum number: " << name;
    throw std::runtime_error(os.str());
  }
}

std::istream& operator>>(std::istream& is, QuantumNumbers& qn) {
  String name;
  Rational r;

  is >> name >> r;

  qn.Set(name, r);
  
  return is;
}

std::ostream& operator<<(std::ostream& os, const QuantumNumbers& qn) {
  bool first = true;
  for (Index i=0; i<Index(QuantumNumberType::FINAL_ENTRY); i++) {
    if (qn[i].isDefined()) {
      if (first) {
        os << quantumnumbertype2string(QuantumNumberType(i)) << ' ' << qn[i];
        first = false;
      } else {
        os << ' ' << quantumnumbertype2string(QuantumNumberType(i)) << ' ' << qn[i];
      }
    }
  }

  return os;
}

String QuantumIdentifier::TypeStr() const {
  String t;
  switch (mqtype) {
    case QuantumIdentifier::TRANSITION:
      t = "TR";
      break;
    case QuantumIdentifier::ENERGY_LEVEL:
      t = "EN";
      break;
    case QuantumIdentifier::ALL:
      t = "ALL";
      break;
    case QuantumIdentifier::NONE:
      t = "NONE";
      break;
    default:
      assert(0);
      break;
  }
  return t;
}

String QuantumIdentifier::SpeciesName() const {
  return species_name_from_species_index(mspecies);
}

void QuantumIdentifier::SetTransition(const QuantumNumbers& upper,
                                      const QuantumNumbers& lower) {
  mqtype = QuantumIdentifier::TRANSITION;
  mqm[TRANSITION_UPPER_INDEX] = upper;
  mqm[TRANSITION_LOWER_INDEX] = lower;
}

void QuantumIdentifier::SetEnergyLevel(const QuantumNumbers& q) {
  mqtype = QuantumIdentifier::ENERGY_LEVEL;
  mqm[ENERGY_LEVEL_INDEX] = q;
}

void QuantumIdentifier::SetFromString(String str) {
  // Global species lookup data:
  using global_data::species_data;

  // We need a species index sorted by Arts identifier. Keep this in a
  // static variable, so that we have to do this only once.  The ARTS
  // species index is ArtsMap[<Arts String>].
  static map<String, SpecIsoMap> ArtsMap;

  // Remember if this stuff has already been initialized:
  static bool hinit = false;

  if (!hinit) {
    for (Index i = 0; i < species_data.nelem(); ++i) {
      const SpeciesRecord& sr = species_data[i];

      for (Index j = 0; j < sr.Isotopologue().nelem(); ++j) {
        SpecIsoMap indicies(i, j);
        String buf = sr.Name() + "-" + sr.Isotopologue()[j].Name();

        ArtsMap[buf] = indicies;
      }
    }
    hinit = true;
  }

  std::istringstream is(str);
  String token;

  is >> token;

  // ok, now for the cool index map:
  // is this arts identifier valid?
  const map<String, SpecIsoMap>::const_iterator i = ArtsMap.find(token);
  if (i == ArtsMap.end()) {
    ostringstream os;
    os << "ARTS Tag: " << token << " is unknown.";
    throw runtime_error(os.str());
  }

  SpecIsoMap id = i->second;
  Species(id.Speciesindex());
  Isotopologue(id.Isotopologueindex());

  is >> token;
  if (token == "TR") {
    mqtype = QuantumIdentifier::TRANSITION;
    is >> token;
    if (token != "UP") {
      std::ostringstream os;
      os << "Expected 'UP', but got: " << token;
      throw std::runtime_error(os.str());
    }

    is >> token;
    Rational r;
    while (is) {
      ThrowIfQuantumNumberNameInvalid(token);
      is >> r;
      QuantumMatch()[QuantumIdentifier::TRANSITION_UPPER_INDEX].Set(token, r);
      is >> token;
      if (token == "LO") break;
    }

    if (!is) {
      std::ostringstream os;
      os << "Premature end of data, expected 'LO'.";
      throw std::runtime_error(os.str());
    }
    is >> token;
    while (is) {
      ThrowIfQuantumNumberNameInvalid(token);
      is >> r;
      QuantumMatch()[QuantumIdentifier::TRANSITION_LOWER_INDEX].Set(token, r);
      is >> token;
    }
  } else if (token == "EN") {
    mqtype = QuantumIdentifier::ENERGY_LEVEL;

    is >> token;
    Rational r;
    while (is) {
      ThrowIfQuantumNumberNameInvalid(token);
      is >> r;
      QuantumMatch()[QuantumIdentifier::TRANSITION_UPPER_INDEX].Set(token, r);
      is >> token;
    }
  } else if (token == "ALL") {
    mqtype = QuantumIdentifier::ALL;
  } else if (token == "NONE") {
    mqtype = QuantumIdentifier::NONE;
  } else {
    std::ostringstream os;
    os << "Error parsing QuantumIdentifier. Expected TR or EN, but got: "
       << token << "\n"
       << "QI: " << str;
    throw std::runtime_error(os.str());
  }
}

void QuantumIdentifier::SetFromStringForCO2Band(String upper,
                                                String lower,
                                                String iso) {
  assert(upper.nelem() == 5);
  assert(lower.nelem() == 5);
  assert(iso.nelem() == 3);

  std::ostringstream os;

  os << "CO2-" << iso << " TR "
     << "UP "
     << "v1 " << upper[0] << " v2 " << upper[1] << " l2 " << upper[2] << " v3 "
     << upper[3] << " r " << upper[4] << " "
     << "LO "
     << "v1 " << lower[0] << " v2 " << lower[1] << " l2 " << lower[2] << " v3 "
     << lower[3] << " r " << lower[4];

  SetFromString(os.str());
}

std::ostream& operator<<(std::ostream& os, const QuantumIdentifier& qi) {
  using global_data::species_data;

  if (qi.Species() < 0 || qi.Isotopologue() < 0)
    return os;

  const SpeciesRecord& spr = species_data[qi.Species()];

  os << spr.Name() << "-";
  if (qi.Isotopologue() == spr.Isotopologue().nelem())
    os << "*";
  else
    os << spr.Isotopologue()[qi.Isotopologue()].Name();
  os << " ";

  if (qi.Type() == QuantumIdentifier::TRANSITION)
    os << "TR UP "
       << qi.QuantumMatch()[QuantumIdentifier::TRANSITION_UPPER_INDEX] << " LO "
       << qi.QuantumMatch()[QuantumIdentifier::TRANSITION_LOWER_INDEX];
  else if (qi.Type() == QuantumIdentifier::ENERGY_LEVEL)
    os << "EN " << qi.QuantumMatch()[QuantumIdentifier::ENERGY_LEVEL_INDEX];
  else if (qi.Type() == QuantumIdentifier::ALL)
    os << "ALL";
  else if (qi.Type() == QuantumIdentifier::NONE)
    os << "NONE";
  else
    assert(0);

  return os;
}

bool QuantumIdentifier::any_quantumnumbers() const {
  Index qni = 0;
  switch (mqtype) {
    case QuantumIdentifier::ENERGY_LEVEL:
    case QuantumIdentifier::TRANSITION:
      for (const auto& qns : mqm) do {
          if (not qns[qni].isUndefined()) return true;
          qni++;
        } while (qni not_eq Index(QuantumNumberType::FINAL_ENTRY));
      break;
    case QuantumIdentifier::ALL:
    case QuantumIdentifier::NONE:
      break;
  }
  return false;
}

std::ostream& operator<<(std::ostream& os, QuantumNumberType t)
{
  return os << quantumnumbertype2string(t);
}

Rational interpret_stringdata(const QuantumNumberType key, const String& val) {
  if (key == QuantumNumberType::parity) {
    if (val == "+")
      return 1_rat;
    else if (val == "-")
      return -1_rat;
  } else if (key == QuantumNumberType::ElectronState) {
    if (val == "X")
      return Rational(int('X'));
  } else if (key == QuantumNumberType::kronigParity) {
    if (val == "f")
      return Rational(int('f'));
    else if (val == "e")
      return Rational(int('e'));
  } else {
    return Rational(val);
  }
  
  return RATIONAL_UNDEFINED;
}

void update_id(QuantumIdentifier& qid, const std::vector<std::array<String, 2> >& upper_list, const std::vector<std::array<String, 2> >& lower_list)
{
  for (auto& keyval: upper_list) {
    auto key = string2quantumnumbertype(keyval[0]);
    if (key == QuantumNumberType::FINAL_ENTRY) {
      std::ostringstream os;
      os << "The key \"" << keyval[0] << "\" is an invalid input as a quantum number key";
      std::cout << "WARNING: " << os.str() << '\n';
    } else {
      auto val = interpret_stringdata(key, keyval[1]);
      if (val != RATIONAL_UNDEFINED) {
        qid.UpperQuantumNumber(key) = val;
      } else {
        std::ostringstream os;
        os << "The key \"" << keyval[0] << "\" and value \"" << keyval[1] << "\" are invalid input as a quantum number key and value pair";
        std::cout << "WARNING: " << os.str() << '\n';
      }
    }
  }
  
  for (auto& keyval: lower_list) {
    auto key = string2quantumnumbertype(keyval[0]);
    if (key == QuantumNumberType::FINAL_ENTRY) {
      std::ostringstream os;
      os << "The key \"" << keyval[0] << "\" is an invalid input as a quantum number key";
      std::cout << "WARNING: " << os.str() << '\n';
    } else {
      auto val = interpret_stringdata(key, keyval[1]);
      if (val != RATIONAL_UNDEFINED) {
        qid.LowerQuantumNumber(key) = val;
      } else {
        std::ostringstream os;
        os << "The key \"" << keyval[0] << "\" and value \"" << keyval[1] << "\" are invalid input as a quantum number key and value pair";
        std::cout << "WARNING: " << os.str() << '\n';
      }
    }
  }
}
