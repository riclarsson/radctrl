/* Copyright (C) 2019
 Richard Larsson <larsson@mps.mpg.de>

 
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

/** Contains the absorption namespace
 * @file   absorptionlines.cc
 * @author Richard Larsson
 * @date   2019-09-07
 * 
 * @brief  Contains the absorption lines implementation
 * 
 * This namespace contains classes to deal with absorption lines
 **/

#include "absorptionlines.h"

#include "absorption.h"
#include "constants.h"
#include "file.h"
#include "global_data.h"
#include "quantum_parser_hitran.h"

Rational Absorption::Lines::LowerQuantumNumber(size_t k, QuantumNumberType qnt) const noexcept {
  for(size_t i=0; i<mlocalquanta.size(); i++)
    if(mlocalquanta[i] == qnt)
      return mlines[k].LowerQuantumNumber(i);
  return mquantumidentity.LowerQuantumNumber(qnt);
}

Rational Absorption::Lines::UpperQuantumNumber(size_t k, QuantumNumberType qnt) const noexcept {
  for(size_t i=0; i<mlocalquanta.size(); i++)
    if(mlocalquanta[i] == qnt)
      return mlines[k].UpperQuantumNumber(i);
  return mquantumidentity.UpperQuantumNumber(qnt);
}

Rational& Absorption::Lines::LowerQuantumNumber(size_t k, QuantumNumberType qnt) noexcept {
  for(size_t i=0; i<mlocalquanta.size(); i++)
    if(mlocalquanta[i] == qnt)
      return mlines[k].LowerQuantumNumber(i);  
  return mquantumidentity.LowerQuantumNumber(qnt);
}

Rational& Absorption::Lines::UpperQuantumNumber(size_t k, QuantumNumberType qnt) noexcept {
  for(size_t i=0; i<mlocalquanta.size(); i++)
    if(mlocalquanta[i] == qnt)
      return mlines[k].UpperQuantumNumber(i);
  return mquantumidentity.UpperQuantumNumber(qnt);
}

LineShape::Output Absorption::Lines::ShapeParameters(size_t k, Numeric T, Numeric P, const Vector& vmrs) const noexcept {
  auto x = mlines[k].LineShape().GetParams(T, mT0, P, vmrs);
  
  if (not DoLineMixing(P)) x.Y = x.G = x.DV = 0;
  
  return x;
}

LineShape::Output Absorption::Lines::ShapeParameters_dT(size_t k, Numeric T, Numeric P, const Vector& vmrs) const noexcept {
  auto x = mlines[k].LineShape().GetTemperatureDerivs(T, mT0, P, vmrs);
  
  if (not DoLineMixing(P)) x.Y = x.G = x.DV = 0;
  
  return x;
}

Index Absorption::Lines::LineShapePos(const Index& spec) const noexcept {
  // Is always first if this is self and self broadening exists
  if(mselfbroadening and spec == mquantumidentity.Species())
    return 0;
  
  // First and last might be artificial so they should not be checked
  for(Index i=Index(mselfbroadening); i<Index(mbroadeningspecies.size())-Index(mbathbroadening); i++) {
    if(spec == mbroadeningspecies[i].Species())
      return Index(i);
  }
  
  // At this point, the ID is not explicitly among the broadeners, but bath broadening means its VMR still might matter
  if(mbathbroadening)
    return Index(mbroadeningspecies.size())-Index(mbathbroadening);
  else
    return -1;
}

LineShape::Output Absorption::Lines::ShapeParameters_dVMR(size_t k, Numeric T, Numeric P, const QuantumIdentifier& vmr_qid) const noexcept {
  const auto self = vmr_qid.Species() == mquantumidentity.Species();
  const auto& ls = mlines[k].LineShape();
  
  if (mselfbroadening and self) {
    auto x = ls.GetVMRDerivs(T, mT0, P, 0);
    
    if (mbathbroadening)
      x = LineShape::differenceOutput(x, ls.GetVMRDerivs(
          T, mT0, P, ls.nelem() - 1));
    
    if (not DoLineMixing(P)) x.Y = x.G = x.DV = 0;
    
    return x;
  } else if (mbathbroadening and self)
    return {0, 0, 0, 0, 0, 0, 0, 0, 0};
  else {
    auto x = ls.GetVMRDerivs(T, mT0, P, LineShapePos(vmr_qid));
    
    if (mbathbroadening)
      x = LineShape::differenceOutput(x, ls.GetVMRDerivs(
          T, mT0, P, ls.nelem() - 1));
    
    if (not DoLineMixing(P)) x.Y = x.G = x.DV = 0;
    
    return x;
  }
}

Numeric Absorption::Lines::ShapeParameter_dInternal(size_t k, Numeric T, Numeric P, const Vector& vmrs, const RetrievalQuantity& derivative) const noexcept {
  const auto self = derivative.Mode() == LineShape::self_broadening;
  const auto bath = derivative.Mode() == LineShape::bath_broadening;
  const auto& ls = mlines[k].LineShape();
  
  if(derivative.QuantumIdentity().Species() not_eq Species() or
      derivative.QuantumIdentity().Isotopologue() not_eq Isotopologue())
    return 0;
  else if(self and mselfbroadening)
    return ls.GetInternalDeriv(
      T, mT0, P, 0, vmrs, derivative.PropMatType());
  else if(self)
    return ls.GetInternalDeriv(
      T, mT0, P, LineShapePos(SpeciesTag(derivative.Mode()).Species()), vmrs, derivative.PropMatType());
  else if(bath and mbathbroadening)
    return ls.GetInternalDeriv(
      T, mT0, P, ls.nelem() - 1, vmrs, derivative.PropMatType());
  else if(bath)
    return 0;
  else
    return ls.GetInternalDeriv(
      T, mT0, P, LineShapePos(SpeciesTag(derivative.Mode()).Species()), vmrs, derivative.PropMatType());
}

Absorption::SingleLineExternal Absorption::ReadFromArtscat3Stream(istream& is) {
  // Default data and values for this type
  SingleLineExternal data;
  data.selfbroadening = true;
  data.bathbroadening = true;
  data.lineshapetype = LineShape::Type::VP;
  data.species.resize(2);
  
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

  // This always contains the rest of the line to parse. At the
  // beginning the entire line. Line gets shorter and shorter as we
  // continue to extract stuff from the beginning.
  String line;

  // Look for more comments?
  bool comment = true;

  while (comment) {
    // Return true if eof is reached:
    if (is.eof()) return data;

    // Throw runtime_error if stream is bad:
    if (!is) throw runtime_error("Stream bad.");

    // Read line from file into linebuffer:
    getline(is, line);

    // It is possible that we were exactly at the end of the file before
    // calling getline. In that case the previous eof() was still false
    // because eof() evaluates only to true if one tries to read after the
    // end of the file. The following check catches this.
    if (line.nelem() == 0 && is.eof()) return data;

    // @ as first character marks catalogue entry
    char c;
    extract(c, line, 1);

    // check for empty line
    if (c == '@') {
      comment = false;
    }
  }

  // read the arts identifier String
  istringstream icecream(line);

  String artsid;
  icecream >> artsid;

  if (artsid.length() != 0) {
    // ok, now for the cool index map:
    // is this arts identifier valid?
    const map<String, SpecIsoMap>::const_iterator i = ArtsMap.find(artsid);
    if (i == ArtsMap.end()) {
      ostringstream os;
      os << "ARTS Tag: " << artsid << " is unknown.";
      throw runtime_error(os.str());
    }

    SpecIsoMap id = i->second;

    // Set mspecies:
    data.quantumidentity.Species(id.Speciesindex());

    // Set misotopologue:
    data.quantumidentity.Isotopologue(id.Isotopologueindex());

    // Extract center frequency:
    icecream >> double_imanip() >> data.line.F0();

    Numeric psf;
    // Extract pressure shift:
    icecream >> double_imanip() >> psf;

    // Extract intensity:
    icecream >> double_imanip() >> data.line.I0();

    // Extract reference temperature for Intensity in K:
    icecream >> double_imanip() >> data.T0;

    // Extract lower state energy:
    icecream >> data.line.E0();

    // Extract air broadening parameters:
    Numeric agam, sgam;
    icecream >> double_imanip() >> agam;
    icecream >> double_imanip() >> sgam;

    // Extract temperature coefficient of broadening parameters:
    Numeric nair, nself;
    icecream >> double_imanip() >> nair;
    icecream >> double_imanip() >> nself;

    // Extract reference temperature for broadening parameter in K:
    Numeric tgam;
    icecream >> double_imanip() >> tgam;

    // Extract the aux parameters:
    Index naux;
    icecream >> naux;

    // resize the aux array and read it
    ArrayOfNumeric maux;
    maux.resize(naux);

    for (Index j = 0; j < naux; j++) {
      icecream >> maux[j];
      //cout << "maux" << j << " = " << maux[j] << "\n";
    }

    // Extract accuracies:
    try {
      Numeric unused_numeric;
      icecream >> double_imanip() >> unused_numeric /*mdf*/;
      icecream >> double_imanip() >> unused_numeric /*mdi0*/;
      icecream >> double_imanip() >> unused_numeric /*dagam*/;
      icecream >> double_imanip() >> unused_numeric /*dsgam*/;
      icecream >> double_imanip() >> unused_numeric /*dnair*/;
      icecream >> double_imanip() >> unused_numeric /*dnself*/;
      icecream >> double_imanip() >> unused_numeric /*dpsf*/;
    } catch (const std::runtime_error&) {
    }

    // Fix if tgam is different from ti0
    if (tgam != data.T0) {
      agam = agam * pow(tgam / data.T0, nair);
      sgam = sgam * pow(tgam / data.T0, nself);
      psf = psf * pow(tgam / data.T0, (Numeric).25 + (Numeric)1.5 * nair);
    }

    // Set line shape computer
    data.line.LineShape() = LineShape::Model(sgam, nself, agam, nair, psf);
  }

  // That's it!
  data.bad = false;
  return data;
}

Absorption::SingleLineExternal Absorption::ReadFromArtscat4Stream(istream& is) {
  // Default data and values for this type
  SingleLineExternal data;
  data.selfbroadening = true;
  data.bathbroadening = false;
  data.lineshapetype = LineShape::Type::VP;
  
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

  // This always contains the rest of the line to parse. At the
  // beginning the entire line. Line gets shorter and shorter as we
  // continue to extract stuff from the beginning.
  String line;

  // Look for more comments?
  bool comment = true;

  while (comment) {
    // Return true if eof is reached:
    if (is.eof()) return data;

    // Throw runtime_error if stream is bad:
    if (!is) throw runtime_error("Stream bad.");

    // Read line from file into linebuffer:
    getline(is, line);

    // It is possible that we were exactly at the end of the file before
    // calling getline. In that case the previous eof() was still false
    // because eof() evaluates only to true if one tries to read after the
    // end of the file. The following check catches this.
    if (line.nelem() == 0 && is.eof()) return data;

    // @ as first character marks catalogue entry
    char c;
    extract(c, line, 1);

    // check for empty line
    if (c == '@') {
      comment = false;
    }
  }

  // read the arts identifier String
  istringstream icecream(line);

  String artsid;
  icecream >> artsid;

  if (artsid.length() != 0) {
    // ok, now for the cool index map:
    // is this arts identifier valid?
    const map<String, SpecIsoMap>::const_iterator i = ArtsMap.find(artsid);
    if (i == ArtsMap.end()) {
      ostringstream os;
      os << "ARTS Tag: " << artsid << " is unknown.";
      throw runtime_error(os.str());
    }

    SpecIsoMap id = i->second;

    // Set line ID
    data.quantumidentity.Species(id.Speciesindex());
    data.quantumidentity.Isotopologue(id.Isotopologueindex());

    // Extract center frequency:
    icecream >> double_imanip() >> data.line.F0();

    // Extract intensity:
    icecream >> double_imanip() >> data.line.I0();

    // Extract reference temperature for Intensity in K:
    icecream >> double_imanip() >> data.T0;

    // Extract lower state energy:
    icecream >> double_imanip() >> data.line.E0();

    // Extract Einstein A-coefficient:
    icecream >> double_imanip() >> data.line.A();

    // Extract upper state stat. weight:
    icecream >> double_imanip() >> data.line.g_upp();

    // Extract lower state stat. weight:
    icecream >> double_imanip() >> data.line.g_low();

    LineShape::from_artscat4(icecream, 
                             data.lineshapetype,
                             data.selfbroadening,
                             data.bathbroadening,
                             data.line.LineShape(),
                             data.species,
                             data.quantumidentity);

    // Remaining entries are the quantum numbers
    String mquantum_numbers_str;
    getline(icecream, mquantum_numbers_str);

    mquantum_numbers_str.trim();
    // FIXME: OLE: Added this if to catch crash for species like CO, PH3
    // where the line in the catalog is too short. Better would be to
    // only read the n and j for Zeeman species, but we don't have that
    // information here

    if (species_data[data.quantumidentity.Species()].Name() == "SO") {
      // Note that atoi converts *** to 0.
      data.quantumidentity.UpperQuantumNumbers().Set(
          QuantumNumberType::N,
          Rational(atoi(mquantum_numbers_str.substr(0, 3).c_str())));
      data.quantumidentity.LowerQuantumNumbers().Set(
          QuantumNumberType::N,
          Rational(atoi(mquantum_numbers_str.substr(6, 3).c_str())));
      data.quantumidentity.UpperQuantumNumbers().Set(
          QuantumNumberType::J,
          Rational(atoi(mquantum_numbers_str.substr(3, 3).c_str())));
      data.quantumidentity.LowerQuantumNumbers().Set(
          QuantumNumberType::J,
          Rational(atoi(mquantum_numbers_str.substr(9, 3).c_str())));
    }

    if (mquantum_numbers_str.nelem() >= 25) {
      if (species_data[data.quantumidentity.Species()].Name() == "O2") {
        String vstr = mquantum_numbers_str.substr(0, 3);
        ArrayOfIndex v(3);
        for (Index vi = 0; vi < 3; vi++) {
          if (vstr[0] != ' ')
            extract(v[vi], vstr, 1);
          else
            v[vi] = -1;
        }

        if (v[2] > -1) {
          data.quantumidentity.UpperQuantumNumbers().Set(QuantumNumberType::v1, Rational(v[2]));
          data.quantumidentity.LowerQuantumNumbers().Set(QuantumNumberType::v1, Rational(v[2]));
        }

        String qstr1 = mquantum_numbers_str.substr(4, 12);
        String qstr2 = mquantum_numbers_str.substr(4 + 12 + 1, 12);
        ArrayOfIndex q(6);
        for (Index qi = 0; qi < 3; qi++) {
          if (qstr1.substr(0, 4) != "    ")
            extract(q[qi], qstr1, 4);
          else
            q[qi] = -1;
        }
        for (Index qi = 3; qi < 6; qi++) {
          if (qstr2.substr(0, 4) != "    ")
            extract(q[qi], qstr2, 4);
          else
            q[qi] = -1;
        }

        if (q[0] > -1)
          data.quantumidentity.UpperQuantumNumbers().Set(QuantumNumberType::N, Rational(q[0]));
        if (q[1] > -1)
          data.quantumidentity.UpperQuantumNumbers().Set(QuantumNumberType::J, Rational(q[1]));
        if (q[2] > -1)
          data.quantumidentity.UpperQuantumNumbers().Set(QuantumNumberType::F, q[2] - 1_2);
        if (q[3] > -1)
          data.quantumidentity.LowerQuantumNumbers().Set(QuantumNumberType::N, Rational(q[3]));
        if (q[4] > -1)
          data.quantumidentity.LowerQuantumNumbers().Set(QuantumNumberType::J, Rational(q[4]));
        if (q[5] > -1)
          data.quantumidentity.LowerQuantumNumbers().Set(QuantumNumberType::F, q[5] - 1_2);
      }
    }
  }

  // That's it!
  data.bad = false;
  return data;
}

Absorption::SingleLineExternal Absorption::ReadFromArtscat5Stream(istream& is) {
  // Default data and values for this type
  SingleLineExternal data;
  
  // Global species lookup data:
  using global_data::species_data;

  // We need a species index sorted by Arts identifier. Keep this in a
  // static variable, so that we have to do this only once.  The ARTS
  // species index is ArtsMap[<Arts String>].
  static map<String, SpecIsoMap> ArtsMap;

  // Remember if this stuff has already been initialized:
  static bool hinit = false;

  LineShape::Model line_mixing_model;
  bool lmd_found = false;

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

  // This always contains the rest of the line to parse. At the
  // beginning the entire line. Line gets shorter and shorter as we
  // continue to extract stuff from the beginning.
  String line;

  // Look for more comments?
  bool comment = true;

  while (comment) {
    // Return true if eof is reached:
    if (is.eof()) return data;

    // Throw runtime_error if stream is bad:
    if (!is) throw runtime_error("Stream bad.");

    // Read line from file into linebuffer:
    getline(is, line);

    // It is possible that we were exactly at the end of the file before
    // calling getline. In that case the previous eof() was still false
    // because eof() evaluates only to true if one tries to read after the
    // end of the file. The following check catches this.
    if (line.nelem() == 0 && is.eof()) return data;

    // @ as first character marks catalogue entry
    char c;
    extract(c, line, 1);

    // check for empty line
    if (c == '@') {
      comment = false;
    }
  }

  // read the arts identifier String
  istringstream icecream(line);

  try {
    String artsid;
    icecream >> artsid;

    if (artsid.length() != 0) {
      // ok, now for the cool index map:
      // is this arts identifier valid?
      const map<String, SpecIsoMap>::const_iterator i = ArtsMap.find(artsid);
      if (i == ArtsMap.end()) {
        ostringstream os;
        os << "ARTS Tag: " << artsid << " is unknown.";
        throw runtime_error(os.str());
      }

      SpecIsoMap id = i->second;

      // Set line ID:
      data.quantumidentity.Species(id.Speciesindex());
      data.quantumidentity.Isotopologue(id.Isotopologueindex());

      // Extract center frequency:
      icecream >> double_imanip() >> data.line.F0();

      // Extract intensity:
      icecream >> double_imanip() >> data.line.I0();

      // Extract reference temperature for Intensity in K:
      icecream >> double_imanip() >> data.T0;

      // Extract lower state energy:
      icecream >> double_imanip() >> data.line.E0();

      // Extract Einstein A-coefficient:
      icecream >> double_imanip() >> data.line.A();

      // Extract upper state stat. weight:
      icecream >> double_imanip() >> data.line.g_upp();

      // Extract lower state stat. weight:
      icecream >> double_imanip() >> data.line.g_low();

      String token;
      Index nelem;
      icecream >> token;

      while (icecream) {
        // Read pressure broadening (LEGACY)
        if (token == "PB") {
          LineShape::from_pressurebroadeningdata(
              icecream,
              data.lineshapetype,
              data.selfbroadening,
              data.bathbroadening,
              data.line.LineShape(),
              data.species,
              data.quantumidentity);
          icecream >> token;
        } else if (token == "QN") {
          // Quantum numbers

          icecream >> token;
          if (token != "UP") {
            ostringstream os;
            os << "Unknown quantum number tag: " << token;
            throw std::runtime_error(os.str());
          }

          icecream >> token;
          Rational r;
          while (icecream) {
            ThrowIfQuantumNumberNameInvalid(token);
            icecream >> r;
            data.quantumidentity.UpperQuantumNumbers().Set(token, r);
            icecream >> token;
            if (token == "LO") break;
          }

          if (!is || token != "LO") {
            std::ostringstream os;
            os << "Error in catalog. Lower quantum number tag 'LO' not found.";
            throw std::runtime_error(os.str());
          }

          icecream >> token;
          while (icecream && IsValidQuantumNumberName(token)) {
            icecream >> r;
            data.quantumidentity.LowerQuantumNumbers().Set(token, r);
            icecream >> token;
          }
        } else if (token == "LM") { // LEGACY
          LineShape::from_linemixingdata(icecream, line_mixing_model);
          icecream >> token;
          lmd_found = true;
        } else if (token == "LF") {
          LineShape::from_linefunctiondata(icecream,
                                           data.lineshapetype,
                                           data.selfbroadening,
                                           data.bathbroadening,
                                           data.line.LineShape(),
                                           data.species);
          icecream >> token;
        } else if (token == "ZM") {
          // Zeeman effect
          icecream >> data.line.Zeeman();
          icecream >> token;
        } else if (token == "LSM") {
          // Line shape modifications

          // Starts with the number of modifications
          icecream >> nelem;
          for (Index lsm = 0; lsm < nelem; lsm++) {
            icecream >> token;

            // cutoff frequency
            if (token == "CUT") {
              icecream >> data.cutofffreq;
              data.cutoff = CutoffType::LineByLineOffset;
            }

            // linemixing pressure limit
            if (token == "LML") {
              icecream >> data.linemixinglimit;
            }

            // mirroring
            else if (token == "MTM") {
              String value;
              icecream >> value;

              data.mirroring = string2mirroringtype(value);
            }

            // line normalization
            else if (token == "LNT") {
              String value;
              icecream >> value;

              data.normalization = string2normalizationtype(value);
            }

            else {
              ostringstream os;
              os << "Unknown line modifications given: " << token;
              throw std::runtime_error(os.str());
            }
          }
          icecream >> token;
        } else {
          ostringstream os;
          os << "Unknown line data tag: " << token;
          throw std::runtime_error(os.str());
        }
      }
    }
  } catch (const std::runtime_error& e) {
    ostringstream os;
    os << "Parse error in catalog line: " << endl;
    os << line << endl;
    os << e.what();
    throw std::runtime_error(os.str());
  }

  if (lmd_found)
    data.line.LineShape().SetLineMixingModel(line_mixing_model.Data()[0]);

  // That's it!
  data.bad = false;
  return data;
}

Absorption::SingleLineExternal Absorption::ReadFromHitran2004Stream(istream& is) {
  // Default data and values for this type
  SingleLineExternal data;
  data.selfbroadening = true;
  data.bathbroadening = true;
  data.lineshapetype = LineShape::Type::VP;
  data.species.resize(2);

  // Global species lookup data:
  using global_data::species_data;

  // This value is used to flag missing data both in species and
  // isotopologue lists. Could be any number, it just has to be made sure
  // that it is neither the index of a species nor of an isotopologue.
  const Index missing = species_data.nelem() + 100;

  // We need a species index sorted by HITRAN tag. Keep this in a
  // static variable, so that we have to do this only once.  The ARTS
  // species index is hind[<HITRAN tag>].
  //
  // Allow for up to 100 species in HITRAN in the future.
  static Array<Index> hspec(100);

  // This is  an array of arrays for each hitran tag. It contains the
  // ARTS indices of the HITRAN isotopologues.
  static Array<ArrayOfIndex> hiso(100);

  // Remember if this stuff has already been initialized:
  static bool hinit = false;

  // Remember, about which missing species we have already issued a
  // warning:
  static ArrayOfIndex warned_missing;

  if (!hinit) {
    // Initialize hspec.
    // The value of missing means that we don't have this species.
    hspec = missing;  // Matpack can set all elements like this.

    for (Index i = 0; i < species_data.nelem(); ++i) {
      const SpeciesRecord& sr = species_data[i];

      // We have to be careful and check for the case that all
      // HITRAN isotopologue tags are -1 (this species is missing in HITRAN).

      if (sr.Isotopologue().nelem() && 0 < sr.Isotopologue()[0].HitranTag()) {
        // The HITRAN tags are stored as species plus isotopologue tags
        // (MO and ISO)
        // in the Isotopologue() part of the species record.
        // We can extract the MO part from any of the isotopologue tags,
        // so we use the first one. We do this by taking an integer
        // division by 10.

        Index mo = sr.Isotopologue()[0].HitranTag() / 10;
        //          cout << "mo = " << mo << endl;
        hspec[mo] = i;

        // Get a nicer to handle array of HITRAN iso tags:
        Index n_iso = sr.Isotopologue().nelem();
        ArrayOfIndex iso_tags;
        iso_tags.resize(n_iso);
        for (Index j = 0; j < n_iso; ++j) {
          iso_tags[j] = sr.Isotopologue()[j].HitranTag();
        }

        // Reserve elements for the isotopologue tags. How much do we
        // need? This depends on the largest HITRAN tag that we know
        // about!
        // Also initialize the tags to missing.
        //          cout << "iso_tags = " << iso_tags << endl;
        //          cout << "static_cast<Index>(max(iso_tags))%10 + 1 = "
        //               << static_cast<Index>(max(iso_tags))%10 + 1 << endl;
        hiso[mo].resize(max(iso_tags) % 10 + 1);
        hiso[mo] = missing;  // Matpack can set all elements like this.

        // Set the isotopologue tags:
        for (Index j = 0; j < n_iso; ++j) {
          if (0 < iso_tags[j])  // ignore -1 elements
          {
            // To get the iso tags from HitranTag() we also have to take
            // modulo 10 to get rid of mo.
            hiso[mo][iso_tags[j] % 10] = j;
          }
        }
      }
    }

    hinit = true;
  }

  // This contains the rest of the line to parse. At the beginning the
  // entire line. Line gets shorter and shorter as we continue to
  // extract stuff from the beginning.
  String line;

  // The first item is the molecule number:
  Index mo;

  // Look for more comments?
  bool comment = true;

  while (comment) {
    // Return true if eof is reached:
    if (is.eof()) return data;

    // Throw runtime_error if stream is bad:
    if (!is) throw runtime_error("Stream bad.");

    // Read line from file into linebuffer:
    getline(is, line);

    // It is possible that we were exactly at the end of the file before
    // calling getline. In that case the previous eof() was still false
    // because eof() evaluates only to true if one tries to read after the
    // end of the file. The following check catches this.
    if (line.nelem() == 0 && is.eof()) return data;

    // If the catalogue is in dos encoding, throw away the
    // additional carriage return
    if (line[line.nelem() - 1] == 13) {
      line.erase(line.nelem() - 1, 1);
    }

    // Because of the fixed FORTRAN format, we need to break up the line
    // explicitly in apropriate pieces. Not elegant, but works!

    // Extract molecule number:
    mo = 0;
    // Initialization of mo is important, because mo stays the same
    // if line is empty.
    extract(mo, line, 2);
    //      cout << "mo = " << mo << endl;

    // If mo == 0 this is just a comment line:
    if (0 != mo) {
      // See if we know this species.
      if (missing != hspec[mo]) {
        comment = false;

        // Check if data record has the right number of characters for the
        // in Hitran 2004 format
        Index nChar = line.nelem() + 2;  // number of characters in data record;
        if ((nChar == 161 && line[158] != ' ') || nChar > 161) {
          ostringstream os;
          os << "Invalid HITRAN 2004 line data record with " << nChar
             << " characters (expected: 160).";
          throw std::runtime_error(os.str());
        }

      }
    }
  }

  // Ok, we seem to have a valid species here.

  // Set mspecies from my cool index table:
  data.quantumidentity.Species(hspec[mo]);

  // Extract isotopologue:
  Index iso;
  extract(iso, line, 1);
  //  cout << "iso = " << iso << endl;

  // Set misotopologue from the other cool index table.
  // We have to be careful to issue an error for unknown iso tags. Iso
  // could be either larger than the size of hiso[mo], or set
  // explicitly to missing. Unfortunately we have to test both cases.
  data.quantumidentity.Isotopologue(missing);
  if (iso < hiso[mo].nelem())
    if (missing != hiso[mo][iso]) data.quantumidentity.Isotopologue(hiso[mo][iso]);

  // Issue error message if misotopologue is still missing:
  if (missing == data.quantumidentity.Isotopologue()) {
    ostringstream os;
    os << "Species: " << species_data[data.quantumidentity.Species()].Name()
       << ", isotopologue iso = " << iso << " is unknown.";
    throw std::runtime_error(os.str());
  }

  // Position.
  {
    // HITRAN position in wavenumbers (cm^-1):
    Numeric v;
    // Conversion from wavenumber to Hz. If you multiply a line
    // position in wavenumber (cm^-1) by this constant, you get the
    // frequency in Hz.
    const Numeric w2Hz = Constant::c * 100.;

    // Extract HITRAN postion:
    extract(v, line, 12);

    // ARTS position in Hz:
    data.line.F0() = v * w2Hz;
  }

  // Intensity.
  {
    // HITRAN intensity is in cm-1/(molec * cm-2) at 296 Kelvin.
    // It already includes the isotpic ratio.
    // The first cm-1 is the frequency unit (it cancels with the
    // 1/frequency unit of the line shape function).
    //
    // We need to do the following:
    // 1. Convert frequency from wavenumber to Hz (factor 1e2 * c).
    // 2. Convert [molec * cm-2] to [molec * m-2] (factor 1e-4).
    // 3. Take out the isotopologue ratio.

    const Numeric hi2arts = 1e-2 * Constant::c;

    Numeric s;

    // Extract HITRAN intensity:
    extract(s, line, 10);
    // Convert to ARTS units (Hz / (molec * m-2) ), or shorter: Hz*m^2
    data.line.I0() = s * hi2arts;
    // Take out isotopologue ratio:
    data.line.I0() /= species_data[data.quantumidentity.Species()]
               .Isotopologue()[data.quantumidentity.Isotopologue()]
               .Abundance();
  }

  // Einstein coefficient
  {
    Numeric r;
    extract(r, line, 10);
    data.line.A() = r;
  }

  // Air broadening parameters.
  Numeric agam, sgam;
  {
    // HITRAN parameter is in cm-1/atm at 296 Kelvin
    // All parameters are HWHM (I hope this is true!)
    Numeric gam;
    // Conversion from wavenumber to Hz. If you multiply a value in
    // wavenumber (cm^-1) by this constant, you get the value in Hz.
    const Numeric w2Hz = Constant::c * 1e2;
    // Ok, put together the end-to-end conversion that we need:
    const Numeric hi2arts = w2Hz / Conversion::ATM2PA;

    // Extract HITRAN AGAM value:
    extract(gam, line, 5);

    // ARTS parameter in Hz/Pa:
    agam = gam * hi2arts;

    // Extract HITRAN SGAM value:
    extract(gam, line, 5);

    // ARTS parameter in Hz/Pa:
    sgam = gam * hi2arts;

    // If zero, set to agam:
    if (0 == sgam) sgam = agam;

    //    cout << "agam, sgam = " << magam << ", " << msgam << endl;
  }

  // Lower state energy.
  {
    // HITRAN parameter is in wavenumbers (cm^-1).
    // We have to convert this to the ARTS unit Joule.

    // Extract from Catalogue line
    extract(data.line.E0(), line, 10);

    // Convert to Joule:
    data.line.E0() = wavenumber_to_joule(data.line.E0());
  }

  // Temperature coefficient of broadening parameters.
  Numeric nair, nself;
  {
    // This is dimensionless, we can also extract directly.
    extract(nair, line, 4);

    // Set self broadening temperature coefficient to the same value:
    nself = nair;
    //    cout << "mnair = " << mnair << endl;
  }

  // Pressure shift.
  Numeric psf;
  {
    // HITRAN value in cm^-1 / atm. So the conversion goes exactly as
    // for the broadening parameters.
    Numeric d;
    // Conversion from wavenumber to Hz. If you multiply a value in
    // wavenumber (cm^-1) by this constant, you get the value in Hz.
    const Numeric w2Hz = Constant::c * 1e2;
    // Ok, put together the end-to-end conversion that we need:
    const Numeric hi2arts = w2Hz / Conversion::ATM2PA;

    // Extract HITRAN value:
    extract(d, line, 8);

    // ARTS value in Hz/Pa
    psf = d * hi2arts;
  }
  // Set the accuracies using the definition of HITRAN
  // indices. If some are missing, they are set to -1.

  static QuantumParserHITRAN2004 quantum_parser;
  const String qstr = line.substr(0, 15 * 4);

  // Upper state global quanta
  {
    Index eu;
    extract(eu, line, 15);
  }

  // Lower state global quanta
  {
    Index el;
    extract(el, line, 15);
  }

  // Upper state local quanta
  {
    Index eul;
    extract(eul, line, 15);
  }

  // Lower state local quanta
  {
    Index ell;
    extract(ell, line, 15);
  }

  // Parse quantum numbers.
  quantum_parser.Parse(data.quantumidentity, qstr);

  // Accuracy index for frequency
  {
    Index df;
    // Extract HITRAN value:
    extract(df, line, 1);
  }

  // Accuracy index for intensity
  {
    Index di0;
    // Extract HITRAN value:
    extract(di0, line, 1);
  }

  // Accuracy index for air-broadened halfwidth
  {
    Index dgam;
    // Extract HITRAN value:
    extract(dgam, line, 1);
  }

  // Accuracy index for self-broadened half-width
  {
    Index dgam;
    // Extract HITRAN value:
    extract(dgam, line, 1);
  }

  // Accuracy index for temperature-dependence exponent for agam
  {
    Index dn;
    // Extract HITRAN value:
    extract(dn, line, 1);
  }

  // Accuracy index for pressure shift
  {
    Index dpsfi;
    // Extract HITRAN value (given in cm-1):
    extract(dpsfi, line, 1);
  }

  // These were all the parameters that we can extract from
  // HITRAN 2004. However, we still have to set the reference temperatures
  // to the appropriate value:

  // Reference temperature for Intensity in K.
  data.T0 = 296.0;

  // Set line shape computer
  data.line.LineShape() = LineShape::Model(sgam, nself, agam, nair, psf);
  {
    Index garbage;
    extract(garbage, line, 13);

    // The statistical weights
    extract(data.line.g_upp(), line, 7);
    extract(data.line.g_low(), line, 7);
  }

  // That's it!
  data.bad = false;
  return data;
}

std::vector<std::array<String, 2>> split_quantum_numbers_from_hitran_online(String& qns)
{
  std::vector<std::array<String, 2>> out(0);
  
  if (qns.length()) {
  auto pos_semicolon = qns.find(";");
    while (pos_semicolon < qns.length() and (pos_semicolon > 0)) {
      String var = qns.substr(0, pos_semicolon);
      qns.erase(0, pos_semicolon + 1);
      
      const auto pos_equality = var.find("=");
      out.push_back({var.substr(0, pos_equality), var.substr(pos_equality+1, var.length())});
      pos_semicolon = qns.find(";");
    }
    
    const auto pos_equality = qns.find("=");
    out.push_back({qns.substr(0, pos_equality), qns.substr(pos_equality+1, qns.length())});
  }
  
  return out;
}

Absorption::SingleLineExternal Absorption::ReadFromHitranOnlineStream(istream& is) {
  // Default data and values for this type
  SingleLineExternal data;
  data.selfbroadening = true;
  data.bathbroadening = true;
  data.lineshapetype = LineShape::Type::VP;
  data.species.resize(2);

  // Global species lookup data:
  using global_data::species_data;

  // This value is used to flag missing data both in species and
  // isotopologue lists. Could be any number, it just has to be made sure
  // that it is neither the index of a species nor of an isotopologue.
  const Index missing = species_data.nelem() + 100;

  // We need a species index sorted by HITRAN tag. Keep this in a
  // static variable, so that we have to do this only once.  The ARTS
  // species index is hind[<HITRAN tag>].
  //
  // Allow for up to 100 species in HITRAN in the future.
  static Array<Index> hspec(100);

  // This is  an array of arrays for each hitran tag. It contains the
  // ARTS indices of the HITRAN isotopologues.
  static Array<ArrayOfIndex> hiso(100);

  // Remember if this stuff has already been initialized:
  static bool hinit = false;

  // Remember, about which missing species we have already issued a
  // warning:
  static ArrayOfIndex warned_missing;

  if (!hinit) {
    // Initialize hspec.
    // The value of missing means that we don't have this species.
    hspec = missing;  // Matpack can set all elements like this.

    for (Index i = 0; i < species_data.nelem(); ++i) {
      const SpeciesRecord& sr = species_data[i];

      // We have to be careful and check for the case that all
      // HITRAN isotopologue tags are -1 (this species is missing in HITRAN).

      if (sr.Isotopologue().nelem() && 0 < sr.Isotopologue()[0].HitranTag()) {
        // The HITRAN tags are stored as species plus isotopologue tags
        // (MO and ISO)
        // in the Isotopologue() part of the species record.
        // We can extract the MO part from any of the isotopologue tags,
        // so we use the first one. We do this by taking an integer
        // division by 10.

        Index mo = sr.Isotopologue()[0].HitranTag() / 10;
        //          cout << "mo = " << mo << endl;
        hspec[mo] = i;

        // Get a nicer to handle array of HITRAN iso tags:
        Index n_iso = sr.Isotopologue().nelem();
        ArrayOfIndex iso_tags;
        iso_tags.resize(n_iso);
        for (Index j = 0; j < n_iso; ++j) {
          iso_tags[j] = sr.Isotopologue()[j].HitranTag();
        }

        // Reserve elements for the isotopologue tags. How much do we
        // need? This depends on the largest HITRAN tag that we know
        // about!
        // Also initialize the tags to missing.
        //          cout << "iso_tags = " << iso_tags << endl;
        //          cout << "static_cast<Index>(max(iso_tags))%10 + 1 = "
        //               << static_cast<Index>(max(iso_tags))%10 + 1 << endl;
        hiso[mo].resize(max(iso_tags) % 10 + 1);
        hiso[mo] = missing;  // Matpack can set all elements like this.

        // Set the isotopologue tags:
        for (Index j = 0; j < n_iso; ++j) {
          if (0 < iso_tags[j])  // ignore -1 elements
          {
            // To get the iso tags from HitranTag() we also have to take
            // modulo 10 to get rid of mo.
            hiso[mo][iso_tags[j] % 10] = j;
          }
        }
      }
    }

    hinit = true;
  }

  // This contains the rest of the line to parse. At the beginning the
  // entire line. Line gets shorter and shorter as we continue to
  // extract stuff from the beginning.
  String line;

  // The first item is the molecule number:
  Index mo;

  // Look for more comments?
  bool comment = true;

  while (comment) {
    // Return true if eof is reached:
    if (is.eof()) return data;

    // Throw runtime_error if stream is bad:
    if (!is) throw runtime_error("Stream bad.");

    // Read line from file into linebuffer:
    getline(is, line);

    // It is possible that we were exactly at the end of the file before
    // calling getline. In that case the previous eof() was still false
    // because eof() evaluates only to true if one tries to read after the
    // end of the file. The following check catches this.
    if (line.nelem() == 0 && is.eof()) return data;

    // If the catalogue is in dos encoding, throw away the
    // additional carriage return
    if (line[line.nelem() - 1] == 13) {
      line.erase(line.nelem() - 1, 1);
    }

    // Because of the fixed FORTRAN format, we need to break up the line
    // explicitly in apropriate pieces. Not elegant, but works!

    // Extract molecule number:
    mo = 0;
    // Initialization of mo is important, because mo stays the same
    // if line is empty.
    extract(mo, line, 2);
    //      cout << "mo = " << mo << endl;

    // If mo == 0 this is just a comment line:
    if (0 != mo) {
      // See if we know this species.
      if (missing != hspec[mo]) {
        comment = false;
      }
    }
  }

  // Ok, we seem to have a valid species here.

  // Set mspecies from my cool index table:
  data.quantumidentity.Species(hspec[mo]);

  // Extract isotopologue:
  Index iso;
  extract(iso, line, 1);
  //  cout << "iso = " << iso << endl;

  // Set misotopologue from the other cool index table.
  // We have to be careful to issue an error for unknown iso tags. Iso
  // could be either larger than the size of hiso[mo], or set
  // explicitly to missing. Unfortunately we have to test both cases.
  data.quantumidentity.Isotopologue(missing);
  if (iso < hiso[mo].nelem())
    if (missing != hiso[mo][iso]) data.quantumidentity.Isotopologue(hiso[mo][iso]);

  // Issue error message if misotopologue is still missing:
  if (missing == data.quantumidentity.Isotopologue()) {
    ostringstream os;
    os << "Species: " << species_data[data.quantumidentity.Species()].Name()
       << ", isotopologue iso = " << iso << " is unknown.";
    throw std::runtime_error(os.str());
  }

  // Position.
  {
    // HITRAN position in wavenumbers (cm^-1):
    Numeric v;
    // Conversion from wavenumber to Hz. If you multiply a line
    // position in wavenumber (cm^-1) by this constant, you get the
    // frequency in Hz.
    const Numeric w2Hz = Constant::c * 100.;

    // Extract HITRAN postion:
    extract(v, line, 12);

    // ARTS position in Hz:
    data.line.F0() = v * w2Hz;
  }

  // Intensity.
  {
    // HITRAN intensity is in cm-1/(molec * cm-2) at 296 Kelvin.
    // It already includes the isotpic ratio.
    // The first cm-1 is the frequency unit (it cancels with the
    // 1/frequency unit of the line shape function).
    //
    // We need to do the following:
    // 1. Convert frequency from wavenumber to Hz (factor 1e2 * c).
    // 2. Convert [molec * cm-2] to [molec * m-2] (factor 1e-4).
    // 3. Take out the isotopologue ratio.

    const Numeric hi2arts = 1e-2 * Constant::c;

    Numeric s;

    // Extract HITRAN intensity:
    extract(s, line, 10);
    // Convert to ARTS units (Hz / (molec * m-2) ), or shorter: Hz*m^2
    data.line.I0() = s * hi2arts;
    // Take out isotopologue ratio:
    data.line.I0() /= species_data[data.quantumidentity.Species()]
               .Isotopologue()[data.quantumidentity.Isotopologue()]
               .Abundance();
  }

  // Einstein coefficient
  {
    Numeric r;
    extract(r, line, 10);
    data.line.A() = r;
  }

  // Air broadening parameters.
  Numeric agam, sgam;
  {
    // HITRAN parameter is in cm-1/atm at 296 Kelvin
    // All parameters are HWHM (I hope this is true!)
    Numeric gam;
    // Conversion from wavenumber to Hz. If you multiply a value in
    // wavenumber (cm^-1) by this constant, you get the value in Hz.
    const Numeric w2Hz = Constant::c * 1e2;
    // Ok, put together the end-to-end conversion that we need:
    const Numeric hi2arts = w2Hz / Conversion::ATM2PA;

    // Extract HITRAN AGAM value:
    extract(gam, line, 5);

    // ARTS parameter in Hz/Pa:
    agam = gam * hi2arts;

    // Extract HITRAN SGAM value:
    extract(gam, line, 5);

    // ARTS parameter in Hz/Pa:
    sgam = gam * hi2arts;

    // If zero, set to agam:
    if (0 == sgam) sgam = agam;

    //    cout << "agam, sgam = " << magam << ", " << msgam << endl;
  }

  // Lower state energy.
  {
    // HITRAN parameter is in wavenumbers (cm^-1).
    // We have to convert this to the ARTS unit Joule.

    // Extract from Catalogue line
    extract(data.line.E0(), line, 10);

    // Convert to Joule:
    data.line.E0() = wavenumber_to_joule(data.line.E0());
  }

  // Temperature coefficient of broadening parameters.
  Numeric nair, nself;
  {
    // This is dimensionless, we can also extract directly.
    extract(nair, line, 4);

    // Set self broadening temperature coefficient to the same value:
    nself = nair;
    //    cout << "mnair = " << mnair << endl;
  }

  // Pressure shift.
  Numeric psf;
  {
    // HITRAN value in cm^-1 / atm. So the conversion goes exactly as
    // for the broadening parameters.
    Numeric d;
    // Conversion from wavenumber to Hz. If you multiply a value in
    // wavenumber (cm^-1) by this constant, you get the value in Hz.
    const Numeric w2Hz = Constant::c * 1e2;
    // Ok, put together the end-to-end conversion that we need:
    const Numeric hi2arts = w2Hz / Conversion::ATM2PA;

    // Extract HITRAN value:
    extract(d, line, 8);

    // ARTS value in Hz/Pa
    psf = d * hi2arts;
  }
  // Set the accuracies using the definition of HITRAN
  // indices. If some are missing, they are set to -1.

  // Upper state global quanta
  {
    Index eu;
    extract(eu, line, 15);
  }

  // Lower state global quanta
  {
    Index el;
    extract(el, line, 15);
  }

  // Upper state local quanta
  {
    Index eul;
    extract(eul, line, 15);
  }

  // Lower state local quanta
  {
    Index ell;
    extract(ell, line, 15);
  }

  // Accuracy index for frequency
  {
    Index df;
    // Extract HITRAN value:
    extract(df, line, 1);
  }

  // Accuracy index for intensity
  {
    Index di0;
    // Extract HITRAN value:
    extract(di0, line, 1);
  }

  // Accuracy index for air-broadened halfwidth
  {
    Index dgam;
    // Extract HITRAN value:
    extract(dgam, line, 1);
  }

  // Accuracy index for self-broadened half-width
  {
    Index dgam;
    // Extract HITRAN value:
    extract(dgam, line, 1);
  }

  // Accuracy index for temperature-dependence exponent for agam
  {
    Index dn;
    // Extract HITRAN value:
    extract(dn, line, 1);
  }

  // Accuracy index for pressure shift
  {
    Index dpsfi;
    // Extract HITRAN value (given in cm-1):
    extract(dpsfi, line, 1);
  }

  // These were all the parameters that we can extract from
  // HITRAN 2004. However, we still have to set the reference temperatures
  // to the appropriate value:

  // Reference temperature for Intensity in K.
  data.T0 = 296.0;

  // Set line shape computer
  data.line.LineShape() = LineShape::Model(sgam, nself, agam, nair, psf);
  {
    Index garbage;
    extract(garbage, line, 13);

    // The statistical weights
    extract(data.line.g_upp(), line, 7);
    extract(data.line.g_low(), line, 7);
  }
  
  // ADD QUANTUM NUMBER PARSING HERE!
  String upper, lower;
  std::stringstream ss;
  ss.str(line);
  ss >> upper >> lower;
  auto upper_list = split_quantum_numbers_from_hitran_online(upper);
  auto lower_list = split_quantum_numbers_from_hitran_online(lower);
  update_id(data.quantumidentity, upper_list, lower_list);

  // That's it!
  data.bad = false;
  return data;
}

Absorption::SingleLineExternal Absorption::ReadFromHitran2001Stream(istream& is) {
  // Default data and values for this type
  SingleLineExternal data;
  data.selfbroadening = true;
  data.bathbroadening = true;
  data.lineshapetype = LineShape::Type::VP;
  data.species.resize(2);

  // Global species lookup data:
  using global_data::species_data;

  // This value is used to flag missing data both in species and
  // isotopologue lists. Could be any number, it just has to be made sure
  // that it is neither the index of a species nor of an isotopologue.
  const Index missing = species_data.nelem() + 100;

  // We need a species index sorted by HITRAN tag. Keep this in a
  // static variable, so that we have to do this only once.  The ARTS
  // species index is hind[<HITRAN tag>].
  //
  // Allow for up to 100 species in HITRAN in the future.
  static Array<Index> hspec(100);

  // This is  an array of arrays for each hitran tag. It contains the
  // ARTS indices of the HITRAN isotopologues.
  static Array<ArrayOfIndex> hiso(100);

  // Remember if this stuff has already been initialized:
  static bool hinit = false;

  // Remember, about which missing species we have already issued a
  // warning:
  static ArrayOfIndex warned_missing;

  if (!hinit) {
    // Initialize hspec.
    // The value of missing means that we don't have this species.
    hspec = missing;  // Matpack can set all elements like this.

    for (Index i = 0; i < species_data.nelem(); ++i) {
      const SpeciesRecord& sr = species_data[i];

      // We have to be careful and check for the case that all
      // HITRAN isotopologue tags are -1 (this species is missing in HITRAN).

      if (sr.Isotopologue().nelem() && 0 < sr.Isotopologue()[0].HitranTag()) {
        // The HITRAN tags are stored as species plus isotopologue tags
        // (MO and ISO)
        // in the Isotopologue() part of the species record.
        // We can extract the MO part from any of the isotopologue tags,
        // so we use the first one. We do this by taking an integer
        // division by 10.

        Index mo = sr.Isotopologue()[0].HitranTag() / 10;
        //          cout << "mo = " << mo << endl;
        hspec[mo] = i;

        // Get a nicer to handle array of HITRAN iso tags:
        Index n_iso = sr.Isotopologue().nelem();
        ArrayOfIndex iso_tags;
        iso_tags.resize(n_iso);
        for (Index j = 0; j < n_iso; ++j) {
          iso_tags[j] = sr.Isotopologue()[j].HitranTag();
        }

        // Reserve elements for the isotopologue tags. How much do we
        // need? This depends on the largest HITRAN tag that we know
        // about!
        // Also initialize the tags to missing.
        //          cout << "iso_tags = " << iso_tags << endl;
        //          cout << "static_cast<Index>(max(iso_tags))%10 + 1 = "
        //               << static_cast<Index>(max(iso_tags))%10 + 1 << endl;
        hiso[mo].resize(max(iso_tags) % 10 + 1);
        hiso[mo] = missing;  // Matpack can set all elements like this.

        // Set the isotopologue tags:
        for (Index j = 0; j < n_iso; ++j) {
          if (0 < iso_tags[j])  // ignore -1 elements
          {
            // To get the iso tags from HitranTag() we also have to take
            // modulo 10 to get rid of mo.
            hiso[mo][iso_tags[j] % 10] = j;
          }
        }
      }
    }

    hinit = true;
  }

  // This contains the rest of the line to parse. At the beginning the
  // entire line. Line gets shorter and shorter as we continue to
  // extract stuff from the beginning.
  String line;

  // The first item is the molecule number:
  Index mo;

  // Look for more comments?
  bool comment = true;

  while (comment) {
    // Return true if eof is reached:
    if (is.eof()) return data;

    // Throw runtime_error if stream is bad:
    if (!is) throw runtime_error("Stream bad.");

    // Read line from file into linebuffer:
    getline(is, line);

    // It is possible that we were exactly at the end of the file before
    // calling getline. In that case the previous eof() was still false
    // because eof() evaluates only to true if one tries to read after the
    // end of the file. The following check catches this.
    if (line.nelem() == 0 && is.eof()) return data;

    // If the catalogue is in dos encoding, throw away the
    // additional carriage return
    if (line[line.nelem() - 1] == 13) {
      line.erase(line.nelem() - 1, 1);
    }

    // Because of the fixed FORTRAN format, we need to break up the line
    // explicitly in apropriate pieces. Not elegant, but works!

    // Extract molecule number:
    mo = 0;
    // Initialization of mo is important, because mo stays the same
    // if line is empty.
    extract(mo, line, 2);
    //      cout << "mo = " << mo << endl;

    // If mo == 0 this is just a comment line:
    if (0 != mo) {
      // See if we know this species. Exit with an error if the species is unknown.
      if (missing != hspec[mo]) {
        comment = false;

        // Check if data record has the right number of characters for the
        // in Hitran 1986-2001 format
        Index nChar = line.nelem() + 2;  // number of characters in data record;
        if (nChar != 100) {
          ostringstream os;
          os << "Invalid HITRAN 1986-2001 line data record with " << nChar
             << " characters (expected: 100)." << endl
             << line << " n: " << line.nelem();
          throw runtime_error(os.str());
        }
      }
    }
  }

  // Ok, we seem to have a valid species here.

  // Set mspecies from my cool index table:
  data.quantumidentity.Species(hspec[mo]);

  // Extract isotopologue:
  Index iso;
  extract(iso, line, 1);
  //  cout << "iso = " << iso << endl;

  // Set misotopologue from the other cool index table.
  // We have to be careful to issue an error for unknown iso tags. Iso
  // could be either larger than the size of hiso[mo], or set
  // explicitly to missing. Unfortunately we have to test both cases.
  data.quantumidentity.Isotopologue(missing);
  if (iso < hiso[mo].nelem())
    if (missing != hiso[mo][iso]) data.quantumidentity.Isotopologue(hiso[mo][iso]);

  // Issue error message if misotopologue is still missing:
  if (missing == data.quantumidentity.Isotopologue()) {
    ostringstream os;
    os << "Species: " << species_data[data.quantumidentity.Species()].Name()
       << ", isotopologue iso = " << iso << " is unknown.";
    throw runtime_error(os.str());
  }

  // Position.
  {
    // HITRAN position in wavenumbers (cm^-1):
    Numeric v;
    // Conversion from wavenumber to Hz. If you multiply a line
    // position in wavenumber (cm^-1) by this constant, you get the
    // frequency in Hz.
    const Numeric w2Hz = Constant::c * 100.;

    // Extract HITRAN postion:
    extract(v, line, 12);

    // ARTS position in Hz:
    data.line.F0() = v * w2Hz;
    //    cout << "mf = " << mf << endl;
  }

  // Intensity.
  {
    // HITRAN intensity is in cm-1/(molec * cm-2) at 296 Kelvin.
    // It already includes the isotpic ratio.
    // The first cm-1 is the frequency unit (it cancels with the
    // 1/frequency unit of the line shape function).
    //
    // We need to do the following:
    // 1. Convert frequency from wavenumber to Hz (factor 1e2 * c).
    // 2. Convert [molec * cm-2] to [molec * m-2] (factor 1e-4).
    // 3. Take out the isotopologue ratio.

    const Numeric hi2arts = 1e-2 * Constant::c;

    Numeric s;

    // Extract HITRAN intensity:
    extract(s, line, 10);
    // Convert to ARTS units (Hz / (molec * m-2) ), or shorter: Hz*m^2
    data.line.I0() = s * hi2arts;
    // Take out isotopologue ratio:
    data.line.I0() /= species_data[data.quantumidentity.Species()]
    .Isotopologue()[data.quantumidentity.Isotopologue()]
               .Abundance();
  }

  // Skip transition probability:
  {
    Numeric r;
    extract(r, line, 10);
  }

  // Air broadening parameters.
  Numeric agam, sgam;
  {
    // HITRAN parameter is in cm-1/atm at 296 Kelvin
    // All parameters are HWHM (I hope this is true!)
    Numeric gam;
    // Conversion from wavenumber to Hz. If you multiply a value in
    // wavenumber (cm^-1) by this constant, you get the value in Hz.
    const Numeric w2Hz = Constant::c * 1e2;
    // Ok, put together the end-to-end conversion that we need:
    const Numeric hi2arts = w2Hz / Conversion::ATM2PA;

    // Extract HITRAN AGAM value:
    extract(gam, line, 5);

    // ARTS parameter in Hz/Pa:
    agam = gam * hi2arts;

    // Extract HITRAN SGAM value:
    extract(gam, line, 5);

    // ARTS parameter in Hz/Pa:
    sgam = gam * hi2arts;

    // If zero, set to agam:
    if (0 == sgam) sgam = agam;

    //    cout << "agam, sgam = " << magam << ", " << msgam << endl;
  }

  // Lower state energy.
  {
    // HITRAN parameter is in wavenumbers (cm^-1).
    // We have to convert this to the ARTS unit Joule.

    // Extract from Catalogue line
    extract(data.line.E0(), line, 10);

    // Convert to Joule:
    data.line.E0() = wavenumber_to_joule(data.line.E0());
  }

  // Temperature coefficient of broadening parameters.
  Numeric nair, nself;
  {
    // This is dimensionless, we can also extract directly.
    extract(nair, line, 4);

    // Set self broadening temperature coefficient to the same value:
    nself = nair;
    //    cout << "mnair = " << mnair << endl;
  }

  // Pressure shift.
  Numeric psf;
  {
    // HITRAN value in cm^-1 / atm. So the conversion goes exactly as
    // for the broadening parameters.
    Numeric d;
    // Conversion from wavenumber to Hz. If you multiply a value in
    // wavenumber (cm^-1) by this constant, you get the value in Hz.
    const Numeric w2Hz = Constant::c * 1e2;
    // Ok, put together the end-to-end conversion that we need:
    const Numeric hi2arts = w2Hz / Conversion::ATM2PA;

    // Extract HITRAN value:
    extract(d, line, 8);

    // ARTS value in Hz/Pa
    psf = d * hi2arts;
  }
  // Set the accuracies using the definition of HITRAN
  // indices. If some are missing, they are set to -1.

  //Skip upper state global quanta index
  {
    Index eu;
    extract(eu, line, 3);
  }

  //Skip lower state global quanta index
  {
    Index el;
    extract(el, line, 3);
  }

  //Skip upper state local quanta
  {
    Index eul;
    extract(eul, line, 9);
  }

  //Skip lower state local quanta
  {
    Index ell;
    extract(ell, line, 9);
  }

  // Accuracy index for frequency reference
  {
    Index df;
    // Extract HITRAN value:
    extract(df, line, 1);
  }

  // Accuracy index for intensity reference
  {
    Index di0;
    // Extract HITRAN value:
    extract(di0, line, 1);
  }

  // Accuracy index for halfwidth reference
  {
    Index dgam;
    // Extract HITRAN value:
    extract(dgam, line, 1);
  }

  // These were all the parameters that we can extract from
  // HITRAN. However, we still have to set the reference temperatures
  // to the appropriate value:

  // Reference temperature for Intensity in K.
  data.T0 = 296.0;

  // Set line shape computer
  data.line.LineShape() = LineShape::Model(sgam, nself, agam, nair, psf);

  // That's it!
  data.bad = false;
  return data;
}

Absorption::SingleLineExternal Absorption::ReadFromLBLRTMStream(istream& is) {
  // Default data and values for this type
  SingleLineExternal data;
  data.selfbroadening = true;
  data.bathbroadening = true;
  data.lineshapetype = LineShape::Type::VP;
  data.species.resize(2);

  // Global species lookup data:
  using global_data::species_data;

  // This value is used to flag missing data both in species and
  // isotopologue lists. Could be any number, it just has to be made sure
  // that it is neither the index of a species nor of an isotopologue.
  const Index missing = species_data.nelem() + 100;

  // We need a species index sorted by HITRAN tag. Keep this in a
  // static variable, so that we have to do this only once.  The ARTS
  // species index is hind[<HITRAN tag>].
  //
  // Allow for up to 100 species in HITRAN in the future.
  static Array<Index> hspec(100);

  // This is  an array of arrays for each hitran tag. It contains the
  // ARTS indices of the HITRAN isotopologues.
  static Array<ArrayOfIndex> hiso(100);

  // Remember if this stuff has already been initialized:
  static bool hinit = false;

  // Remember, about which missing species we have already issued a
  // warning:
  static ArrayOfIndex warned_missing;

  if (!hinit) {
    // Initialize hspec.
    // The value of missing means that we don't have this species.
    hspec = missing;  // Matpack can set all elements like this.
    for (Index i = 0; i < species_data.nelem(); ++i) {
      const SpeciesRecord& sr = species_data[i];
      // We have to be careful and check for the case that all
      // HITRAN isotopologue tags are -1 (this species is missing in HITRAN).
      if (sr.Isotopologue().nelem() && 0 < sr.Isotopologue()[0].HitranTag()) {
        // The HITRAN tags are stored as species plus isotopologue tags
        // (MO and ISO)
        // in the Isotopologue() part of the species record.
        // We can extract the MO part from any of the isotopologue tags,
        // so we use the first one. We do this by taking an integer
        // division by 10.

        Index mo = sr.Isotopologue()[0].HitranTag() / 10;
        //          cout << "mo = " << mo << endl;
        hspec[mo] = i;

        // Get a nicer to handle array of HITRAN iso tags:
        Index n_iso = sr.Isotopologue().nelem();
        ArrayOfIndex iso_tags;
        iso_tags.resize(n_iso);
        for (Index j = 0; j < n_iso; ++j) {
          iso_tags[j] = sr.Isotopologue()[j].HitranTag();
        }

        // Reserve elements for the isotopologue tags. How much do we
        // need? This depends on the largest HITRAN tag that we know
        // about!
        // Also initialize the tags to missing.
        //          cout << "iso_tags = " << iso_tags << endl;
        //          cout << "static_cast<Index>(max(iso_tags))%10 + 1 = "
        //               << static_cast<Index>(max(iso_tags))%10 + 1 << endl;
        hiso[mo].resize(max(iso_tags) % 10 + 1);
        hiso[mo] = missing;  // Matpack can set all elements like this.

        // Set the isotopologue tags:
        for (Index j = 0; j < n_iso; ++j) {
          if (0 < iso_tags[j])  // ignore -1 elements
          {
            // To get the iso tags from HitranTag() we also have to take
            // modulo 10 to get rid of mo.
            hiso[mo][iso_tags[j] % 10] = j;
          }
        }
      }
    }
    
    hinit = true;
  }

  // This contains the rest of the line to parse. At the beginning the
  // entire line. Line gets shorter and shorter as we continue to
  // extract stuff from the beginning.
  String line;

  // The first item is the molecule number:
  Index mo;

  // Look for more comments?
  bool comment = true;

  while (comment) {
    // Return true if eof is reached:
    if (is.eof()) return data;

    // Throw runtime_error if stream is bad:
    if (!is) throw std::runtime_error("Stream bad.");

    // Read line from file into linebuffer:
    getline(is, line);

    // It is possible that we were exactly at the end of the file before
    // calling getline. In that case the previous eof() was still false
    // because eof() evaluates only to true if one tries to read after the
    // end of the file. The following check catches this.
    if (line.nelem() == 0 && is.eof()) return data;

    // If the catalogue is in dos encoding, throw away the
    // additional carriage return
    if (line[line.nelem() - 1] == 13) {
      line.erase(line.nelem() - 1, 1);
    }

    // Because of the fixed FORTRAN format, we need to break up the line
    // explicitly in apropriate pieces. Not elegant, but works!

    // Extract molecule number:
    mo = 0;
    // Initialization of mo is important, because mo stays the same
    // if line is empty.
    extract(mo, line, 2);
    //      cout << "mo = " << mo << endl;

    // If mo == 0 this is just a comment line:
    if (0 != mo) {
      // See if we know this species. Exit with an error if the species is unknown.
      if (missing != hspec[mo]) {
        comment = false;

        // Check if data record has the right number of characters for the
        // in Hitran 1986-2001 format
        Index nChar = line.nelem() + 2;  // number of characters in data record;
        if (nChar != 100) {
          ostringstream os;
          os << "Invalid HITRAN 1986-2001 line data record with " << nChar
             << " characters (expected: 100)." << endl
             << line << " n: " << line.nelem();
          throw std::runtime_error(os.str());
        }

      }
    }
  }

  // Ok, we seem to have a valid species here.

  // Set mspecies from my cool index table:
  data.quantumidentity.Species(hspec[mo]);

  // Extract isotopologue:
  Index iso;
  extract(iso, line, 1);
  //  cout << "iso = " << iso << endl;

  // Set misotopologue from the other cool index table.
  // We have to be careful to issue an error for unknown iso tags. Iso
  // could be either larger than the size of hiso[mo], or set
  // explicitly to missing. Unfortunately we have to test both cases.
  data.quantumidentity.Isotopologue(missing);
  if (iso < hiso[mo].nelem())
    if (missing != hiso[mo][iso]) data.quantumidentity.Isotopologue(hiso[mo][iso]);

  // Issue error message if misotopologue is still missing:
  if (missing == data.quantumidentity.Isotopologue()) {
    ostringstream os;
    os << "Species: " << species_data[data.quantumidentity.Species()].Name()
       << ", isotopologue iso = " << iso << " is unknown.";
    throw std::runtime_error(os.str());
  }

  // Position.
  {
    // HITRAN position in wavenumbers (cm^-1):
    Numeric v;
    // Conversion from wavenumber to Hz. If you multiply a line
    // position in wavenumber (cm^-1) by this constant, you get the
    // frequency in Hz.
    const Numeric w2Hz = Constant::c * 100.;

    // Extract HITRAN postion:
    extract(v, line, 12);

    // ARTS position in Hz:
    data.line.F0() = v * w2Hz;
    //    cout << "mf = " << mf << endl;
  }

  // Intensity.
  {
    // HITRAN intensity is in cm-1/(molec * cm-2) at 296 Kelvin.
    // It already includes the isotpic ratio.
    // The first cm-1 is the frequency unit (it cancels with the
    // 1/frequency unit of the line shape function).
    //
    // We need to do the following:
    // 1. Convert frequency from wavenumber to Hz (factor 1e2 * c).
    // 2. Convert [molec * cm-2] to [molec * m-2] (factor 1e-4).
    // 3. Take out the isotopologue ratio.

    const Numeric hi2arts = 1e-2 * Constant::c;

    Numeric s;
    if (line[6] == 'D') line[6] = 'E';
    // Extract HITRAN intensity:
    extract(s,
            line,
            10);  // NOTE:  If error shooting, FORTRAN "D" is not read properly.
    // Convert to ARTS units (Hz / (molec * m-2) ), or shorter: Hz*m^2
    data.line.I0() = s * hi2arts;
    // Take out isotopologue ratio:
    data.line.I0() /= species_data[data.quantumidentity.Species()]
               .Isotopologue()[data.quantumidentity.Isotopologue()]
               .Abundance();
  }

  // Skip transition probability:
  {
    Numeric r;
    extract(r, line, 10);
  }

  // Air broadening parameters.
  Numeric sgam, agam;
  {
    // HITRAN parameter is in cm-1/atm at 296 Kelvin
    // All parameters are HWHM (I hope this is true!)
    Numeric gam;
    // Conversion from wavenumber to Hz. If you multiply a value in
    // wavenumber (cm^-1) by this constant, you get the value in Hz.
    const Numeric w2Hz = Constant::c * 1e2;
    // Ok, put together the end-to-end conversion that we need:
    const Numeric hi2arts = w2Hz / Conversion::ATM2PA;

    // Extract HITRAN AGAM value:
    extract(gam, line, 5);

    // ARTS parameter in Hz/Pa:
    agam = gam * hi2arts;

    // Extract HITRAN SGAM value:
    extract(gam, line, 5);

    // ARTS parameter in Hz/Pa:
    sgam = gam * hi2arts;

    // If zero, set to agam:
    if (0 == sgam) sgam = agam;

    //    cout << "agam, sgam = " << magam << ", " << msgam << endl;
  }

  // Lower state energy.
  {
    // HITRAN parameter is in wavenumbers (cm^-1).
    // We have to convert this to the ARTS unit Joule.

    // Extract from Catalogue line
    extract(data.line.E0(), line, 10);

    // Convert to Joule:
    data.line.E0() = wavenumber_to_joule(data.line.E0());
  }

  // Temperature coefficient of broadening parameters.
  Numeric nair, nself;
  {
    // This is dimensionless, we can also extract directly.
    extract(nair, line, 4);

    // Set self broadening temperature coefficient to the same value:
    nself = nair;
    //    cout << "mnair = " << mnair << endl;
  }

  // Pressure shift.
  Numeric psf;
  {
    // HITRAN value in cm^-1 / atm. So the conversion goes exactly as
    // for the broadening parameters.
    Numeric d;
    // Conversion from wavenumber to Hz. If you multiply a value in
    // wavenumber (cm^-1) by this constant, you get the value in Hz.
    const Numeric w2Hz = Constant::c * 1e2;
    // Ok, put together the end-to-end conversion that we need:
    const Numeric hi2arts = w2Hz / Conversion::ATM2PA;

    // Extract HITRAN value:
    extract(d, line, 8);

    // ARTS value in Hz/Pa
    psf = d * hi2arts;
  }
  // Set the accuracies using the definition of HITRAN
  // indices. If some are missing, they are set to -1.

  //Skip upper state global quanta index
  {
    Index eu;
    extract(eu, line, 3);
  }

  //Skip lower state global quanta index
  {
    Index el;
    extract(el, line, 3);
  }

  //Skip upper state local quanta
  {
    Index eul;
    extract(eul, line, 9);
  }

  //Skip lower state local quanta
  {
    Index ell;
    if (species_data[data.quantumidentity.Species()].Name() == "O2") {
      String helper = line.substr(0, 9);
      Index DJ = -helper.compare(3, 1, "Q");
      Index DN = -helper.compare(0, 1, "Q");
      Index N = atoi(helper.substr(1, 2).c_str());
      Index J = atoi(helper.substr(4, 2).c_str());

      data.quantumidentity.LowerQuantumNumbers().Set(QuantumNumberType::N, Rational(N));
      data.quantumidentity.LowerQuantumNumbers().Set(QuantumNumberType::J, Rational(J));
      data.quantumidentity.UpperQuantumNumbers().Set(QuantumNumberType::N, Rational(N - DN));
      data.quantumidentity.UpperQuantumNumbers().Set(QuantumNumberType::J, Rational(J - DJ));
    }

    extract(ell, line, 9);
  }

  // Accuracy index for frequency reference
  {
    Index df;
    // Extract HITRAN value:
    extract(df, line, 1);
  }

  // Accuracy index for intensity reference
  {
    Index di0;
    // Extract HITRAN value:
    extract(di0, line, 1);
  }

  // Accuracy index for halfwidth reference
  {
    Index dgam;
    // Extract HITRAN value:
    extract(dgam, line, 1);
  }

  // These were all the parameters that we can extract from
  // HITRAN. However, we still have to set the reference temperatures
  // to the appropriate value:

  // Reference temperature for Intensity in K.
  // (This is fix for HITRAN)
  data.T0 = 296.0;

  // Skip four
  {
    Index four;
    extract(four, line, 4);
  }

  // This is the test for the last two characters of the line
  {
    /* 
     *   0 is nothing, 
     *  -1 is linemixing on the next line, 
     *  -3 is the non-resonant line 
     */
    Index test;
    extract(test, line, 2);
    //If the tag is as it should be, then a minus one means that more should be read
    if (test == -1 || test == -3)
      getline(is, line);
    else  // the line is done and we are happy to leave
    {
      data.line.LineShape() = LineShape::Model(sgam, nself, agam, nair, psf);
      
      data.bad = false;
      return data;
    }
  }

  // In case we are unable to leave, the next line is a line mixing parameter line

  // First is the molecular number.  This should be the same as above.
  {
    Index mo2;
    extract(mo2, line, 2);
    // Skip one

    if (mo != mo2)
      throw std::runtime_error("There is an error in the line mixing\n");
  }

  Vector Y(4), G(4), T(4);

  // These are constants for AER but should be included because we need their grid.
  T[0] = 200;
  T[1] = 250;
  T[2] = 296;
  T[3] = 340;

  // Next is the Y  and G at various temperatures
  {
    Numeric Y_200K;
    extract(Y_200K, line, 13);
    Y[0] = Y_200K;
  }
  {
    Numeric G_200K;
    extract(G_200K, line, 11);
    G[0] = G_200K;
  }
  {
    Numeric Y_250K;
    extract(Y_250K, line, 13);
    Y[1] = Y_250K;
  }
  {
    Numeric G_250K;
    extract(G_250K, line, 11);
    G[1] = G_250K;
  }
  {
    Numeric Y_296K;
    extract(Y_296K, line, 13);
    Y[2] = Y_296K;
  }
  {
    Numeric G_296K;
    extract(G_296K, line, 11);
    G[2] = G_296K;
  }
  {
    Numeric Y_340K;
    extract(Y_340K, line, 13);
    Y[3] = Y_340K;
  }
  {
    Numeric G_340K;
    extract(G_340K, line, 11);
    G[3] = G_340K;
  }

  Y /= Conversion::ATM2PA;
  G /= Conversion::ATM2PA / Conversion::ATM2PA;
  Y *=
      -1;  // ARTS uses (1-iY) as line-mixing factor, LBLRTM CO2 uses (1+iY), so we must change sign
  
  // Test that this is the end
  {
    Index test;
    extract(test, line, 2);
    if (test == -1) {
      data.line.LineShape() = LineShape::Model(sgam,
                                         nself,
                                         agam,
                                         nair,
                                         psf,
                                         {T[0],
                                          T[1],
                                          T[2],
                                          T[3],
                                          Y[0],
                                          Y[1],
                                          Y[2],
                                          Y[3],
                                          G[0],
                                          G[1],
                                          G[2],
                                          G[3]});

      data.bad = false;
      return data;
    } else if (test == -3) {
      data.line.LineShape() = LineShape::Model(sgam,
                                         nself,
                                         agam,
                                         nair,
                                         psf,
                                         {T[0],
                                          T[1],
                                          T[2],
                                          T[3],
                                          Y[0],
                                          Y[1],
                                          Y[2],
                                          Y[3],
                                          G[0],
                                          G[1],
                                          G[2],
                                          G[3]});
      
      data.bad = false;
      return data;
    } else
      return data;
  }
}

std::vector<Absorption::Lines> Absorption::split_list_of_external_lines(std::vector<SingleLineExternal>& external_lines,
                                                                        const std::vector<QuantumNumberType>& localquantas,
                                                                        const std::vector<QuantumNumberType>& globalquantas)
{
  std::vector<Lines> lines(0);
  std::vector<Rational> lowerquanta_local(localquantas.size());
  std::vector<Rational> upperquanta_local(localquantas.size());
  std::vector<Rational> lowerquanta_global(globalquantas.size());
  std::vector<Rational> upperquanta_global(globalquantas.size());
  
  // Loop but make copies because we will need to modify some of the data
  while(external_lines.size()) {
    auto& sle = external_lines.back();
    
    // Set the quantum numbers
    std::transform(localquantas.cbegin(), localquantas.cend(), lowerquanta_local.begin(),
                   [&](auto qn){return sle.quantumidentity.LowerQuantumNumber(qn);});
    std::transform(localquantas.cbegin(), localquantas.cend(), upperquanta_local.begin(),
                   [&](auto qn){return sle.quantumidentity.UpperQuantumNumber(qn);});
    std::transform(globalquantas.cbegin(), globalquantas.cend(), lowerquanta_global.begin(),
                   [&](auto qn){return sle.quantumidentity.LowerQuantumNumber(qn);});
    std::transform(globalquantas.cbegin(), globalquantas.cend(), upperquanta_global.begin(),
                   [&](auto qn){return sle.quantumidentity.UpperQuantumNumber(qn);});
    
    // Set the line
    auto line = sle.line;
    line.LowerQuantumNumbers() = lowerquanta_local;
    line.UpperQuantumNumbers() = upperquanta_local;
    
    // Set the global quantum numbers
    const QuantumIdentifier qid(sle.quantumidentity.Species(), sle.quantumidentity.Isotopologue(),
                                globalquantas, upperquanta_global, lowerquanta_global);
    
    // Either find a line like this in the list of lines or start a new Lines
    auto band = std::find_if(lines.begin(), lines.end(), [&](const Lines& li){return li.MatchWithExternal(sle, qid);});
    if (band not_eq lines.end()) {
      band -> AppendSingleLine(line);
    } else {
      lines.push_back(Lines(sle.selfbroadening, sle.bathbroadening, sle.cutoff,
                            sle.mirroring, sle.population, sle.normalization,
                            sle.lineshapetype, sle.T0, sle.cutofffreq,
                            sle.linemixinglimit, qid, localquantas, sle.species, {line}));
    }
    external_lines.pop_back();
  }
  
  return lines;
}

std::ostream& Absorption::operator<<(std::ostream& os, const Absorption::Lines& lines)
{
  for(auto& line: lines.AllLines())
    os << line << '\n';
  return os;
}

std::istream& Absorption::operator>>(std::istream& is, Lines& lines) {
  for(auto& line: lines.AllLines())
    is >> line;
  return is;
}

std::ostream & Absorption::operator<<(std::ostream& os, const Absorption::SingleLine& line)
{
  os << line.F0() << ' '
     << line.I0() << ' '
     << line.E0() << ' '
     << line.g_low() << ' '
     << line.g_upp() << ' '
     << line.A() << ' '
     << line.Zeeman() << ' '
     << line.LineShape();
  for(auto& r: line.LowerQuantumNumbers())
    os << r << ' ';
  for(auto& r: line.UpperQuantumNumbers())
    os << r << ' ';
  return os;
}

std::istream& Absorption::operator>>(std::istream& is, Absorption::SingleLine& line)
{
  is >> double_imanip()
     >> line.F0()
     >> line.I0()
     >> line.E0()
     >> line.g_low()
     >> line.g_upp()
     >> line.A();
  
  is >> line.Zeeman()
     >> line.LineShape();
  for(auto& r: line.LowerQuantumNumbers())
    is >> r;
  for(auto& r: line.UpperQuantumNumbers())
    is >> r;
  return is;
}

std::ostream& operator<<(std::ostream& os, const ArrayOfAbsorptionLines& aol)
{
  for(auto& l: aol)
    os << l << '\n';
  return os;
}

std::ostream& operator<<(std::ostream& os, const ArrayOfArrayOfAbsorptionLines& aol)
{
  for(auto& l: aol)
    os << l << '\n';
  return os;
}

String Absorption::Lines::SpeciesName() const noexcept
{
  // Species lookup data:
  using global_data::species_data;
  
  // A reference to the relevant record of the species data:
  const SpeciesRecord& spr = species_data[Species()];
  
  // First the species name:
  return spr.Name() + "-" +
  spr.Isotopologue()[Isotopologue()].Name();;
}

String Absorption::Lines::UpperQuantumNumbers() const noexcept
{
  std::ostringstream out;
  out << mquantumidentity.UpperQuantumNumbers() << ' ';
  String s=out.str();
  if(s.back() == ' ')
    s.pop_back();
  return s;
}

String Absorption::Lines::LowerQuantumNumbers() const noexcept
{
  std::ostringstream out;
  out << mquantumidentity.LowerQuantumNumbers() << ' ';
  String s=out.str();
  if(s.back() == ' ')
    s.pop_back();
  return s;
}

String Absorption::Lines::MetaData() const
{
  std::ostringstream os;
  
  os << "\nLines meta-data:\n";
  os << '\t' << "Species identity:\n";
  os << "\t\tSpecies: "<< SpeciesName() << '\n';
  os << "\t\tLower Quantum Numbers: "<< LowerQuantumNumbers() << '\n';
  os << "\t\tUpper Quantum Numbers: "<< UpperQuantumNumbers() << '\n';
  os << '\t' << cutofftype2metadatastring(mcutoff, mcutofffreq);
  os << '\t' << populationtype2metadatastring(mpopulation);
  os << '\t' << normalizationtype2metadatastring(mnormalization);
  os << '\t' << LineShape::shapetype2metadatastring(mlineshapetype);
  os << '\t' << mirroringtype2metadatastring(mmirroring);
  os << '\t' << "The reference temperature for all line parameters is "
     << mT0 << " K.\n";
  if(mlinemixinglimit < 0)
    os << '\t' << "If applicable, there is no line mixing limit.\n";
  else
    os << '\t' << "If applicable, there is a line mixing limit at "
       << mlinemixinglimit << " Pa.\n";
  
  if (not NumLines()) {
    os << "\tNo line data is available.\n";
  }
  else {
    os << "\tThere are " << NumLines() << " lines available.\n";
    
    auto& line = mlines.front();
    os << "\tThe front line has:\n";
    os << "\t\t" << "f0: " << line.F0() << " Hz\n";
    os << "\t\t" << "i0: " << line.I0() << " m^2/Hz\n";
    os << "\t\t" << "e0: " << line.E0() << " J\n";
    os << "\t\t" << "Lower stat. weight: " << line.g_low() << " [-]\n";
    os << "\t\t" << "Upper stat. weight: " << line.g_upp() << " [-]\n";
    os << "\t\t" << "A: " << line.A() << " 1/s\n";
    os << "\t\t" << "Zeeman splitting of lower state: " << line.Zeeman().gl() << " [-]\n";
    os << "\t\t" << "Zeeman splitting of upper state: " << line.Zeeman().gu() << " [-]\n";
    os << "\t\t" << "Lower state local quantum numbers:";
    for(size_t i=0; i<mlocalquanta.size(); i++)
      os << " " << quantumnumbertype2string(mlocalquanta[i])
      << "=" << line.LowerQuantumNumber(i) << ";";
    os << "\n";
    os << "\t\t" << "Upper state local quantum numbers:";
    for(size_t i=0; i<mlocalquanta.size(); i++)
      os << " " << quantumnumbertype2string(mlocalquanta[i])
      << "=" << line.UpperQuantumNumber(i) << ";";
    os << "\n";
    ArrayOfString ls_meta = LineShape::ModelMetaDataArray(line.LineShape(),
                                                          mselfbroadening,
                                                          mbathbroadening, 
                                                          mbroadeningspecies,
                                                          mT0);
    os << "\t\t" << "Line shape parameters (are normalized by sum(VMR)):\n";
    for(auto& ls_form: ls_meta)
      os << "\t\t\t" << ls_form << "\n";
  }
  
  
  return os.str();
}


void Absorption::Lines::RemoveUnusedLocalQuantums()
{
  // Find all hits
  std::vector<size_t> hits(0);
  
  //
  for (size_t i=0; i<mlocalquanta.size(); i++) {
    bool found=false;
    for (auto& line: mlines) {
      if (line.LowerQuantumNumber(i).isDefined() or line.UpperQuantumNumber(i).isDefined()) {
        found = true;
      }
    }
    
    if (not found) {
      hits.push_back(i);
    }
  }
  
  // Remove from behind to not mess up
  while (not hits.empty()) {
    RemoveLocalQuantum(hits.back());
    hits.pop_back();
  }
}

void Absorption::Lines::RemoveLocalQuantum(size_t x)
{
  mlocalquanta.erase(mlocalquanta.begin() + x);
  for (auto& line: mlines) {
    line.LowerQuantumNumbers().erase(line.LowerQuantumNumbers().begin() + x);
    line.UpperQuantumNumbers().erase(line.UpperQuantumNumbers().begin() + x);
  }
}


bool Absorption::SingleLine::SameQuantumNumbers(const Absorption::SingleLine& sl) const noexcept
{
  return 
  std::equal(mlowerquanta.cbegin(), mlowerquanta.cend(), sl.mlowerquanta.cbegin(), sl.mlowerquanta.cend()) and 
  std::equal(mupperquanta.cbegin(), mupperquanta.cend(), sl.mupperquanta.cbegin(), sl.mupperquanta.cend()) ;
}


void Absorption::Lines::RemoveLine(Index i) noexcept
{
  mlines.erase(mlines.begin() + i);
}


Absorption::SingleLine Absorption::Lines::PopLine(Index i) noexcept
{
  auto line = mlines[i];
  RemoveLine(i);
  return line;
}


Absorption::Lines Absorption::createEmptyCopy(const Absorption::Lines& al) noexcept {
  return Absorption::Lines(al.Self(), al.Bath(), al.Cutoff(), al.Mirroring(),
                           al.Population(), al.Normalization(), al.LineShapeType(),
                           al.T0(), al.CutoffFreqValue(), al.LinemixingLimit(),
                           al.QuantumIdentity(), al.LocalQuanta(), al.BroadeningSpecies());
}


Absorption::SingleLine& Absorption::Lines::Line(Index i) noexcept
{
  return mlines[i];
}


const Absorption::SingleLine& Absorption::Lines::Line(Index i) const noexcept
{
  return mlines[i];
}

void Absorption::Lines::ReverseLines() noexcept
{
  std::reverse(mlines.begin(), mlines.end());
}

Numeric Absorption::Lines::SpeciesMass() const noexcept
{
  return global_data::species_data[Species()].Isotopologue()[Isotopologue()].Mass();
}

Vector Absorption::Lines::BroadeningSpeciesVMR(const ConstVectorView atm_vmrs,
                                               const ArrayOfArrayOfSpeciesTag& atm_spec) const
{
  return LineShape::vmrs(atm_vmrs, atm_spec, QuantumIdentity(),
                         BroadeningSpecies(), Self(), Bath(), LineShapeType());
}

Numeric Absorption::Lines::SelfVMR(const ConstVectorView atm_vmrs,
                                   const ArrayOfArrayOfSpeciesTag& atm_spec) const
{
  if (atm_vmrs.nelem() not_eq atm_spec.nelem())
    throw std::runtime_error("Bad species and vmr lists");
    
  for (Index i=0; i<atm_spec.nelem(); i++)
    if (atm_spec[i].nelem() and atm_spec[i][0].Species() == Species())
      return atm_vmrs[i];
  return 0;
}

bool Absorption::line_in_id(const Absorption::Lines& band, const QuantumIdentifier& id, size_t line_index)
{
  if (band.Species() != id.Species())
    return false;
  else if (band.Isotopologue() != id.Isotopologue())
    return false;
  else if (id.Type() == QuantumIdentifier::NONE)
    return false;
  else if (id.Type() == QuantumIdentifier::ALL)
    return true;
  else if (id.Type() == QuantumIdentifier::ENERGY_LEVEL)
    throw std::runtime_error("Cannot match energy level to line");
  else {
    for (Index iq=0; iq<Index(QuantumNumberType::FINAL_ENTRY); iq++) {
      auto qn_line = band.LowerQuantumNumber(line_index, QuantumNumberType(iq));
      auto qn_id = id.LowerQuantumNumber(QuantumNumberType(iq));
      
      if ((qn_id.isUndefined() and qn_line.isDefined()) or 
          (qn_line.isDefined() and qn_line not_eq qn_id)) {
        return false;
      }
    }
    
    for (Index iq=0; iq<Index(QuantumNumberType::FINAL_ENTRY); iq++) {
      auto qn_line = band.UpperQuantumNumber(line_index, QuantumNumberType(iq));
      auto qn_id = id.UpperQuantumNumber(QuantumNumberType(iq));
      
      if ((qn_id.isUndefined() and qn_line.isDefined()) or 
          (qn_line.isDefined() and qn_line not_eq qn_id)) {
        return false;
      }
    }
  }
  
  return true;
}

bool Absorption::line_upper_in_id(const Absorption::Lines& band, const QuantumIdentifier& id, size_t line_index)
{
  if (band.Species() != id.Species())
    return false;
  else if (band.Isotopologue() != id.Isotopologue())
    return false;
  else if (id.Type() == QuantumIdentifier::NONE)
    return false;
  else if (id.Type() == QuantumIdentifier::ALL)
    return true;
  else if (id.Type() == QuantumIdentifier::TRANSITION)
    throw std::runtime_error("Cannot match transition level to energy level");
  else {
    for (Index iq=0; iq<Index(QuantumNumberType::FINAL_ENTRY); iq++) {
      auto qn_line = band.UpperQuantumNumber(line_index, QuantumNumberType(iq));
      auto qn_id = id.EnergyLevelQuantumNumber(QuantumNumberType(iq));
      
      if ((qn_id.isUndefined() and qn_line.isDefined()) or 
          (qn_line.isDefined() and qn_line not_eq qn_id)) {
        return false;
      }
    }
  }
  
  return true;
}

bool Absorption::line_lower_in_id(const Absorption::Lines& band, const QuantumIdentifier& id, size_t line_index)
{
  if (band.Species() != id.Species())
    return false;
  else if (band.Isotopologue() != id.Isotopologue())
    return false;
  else if (id.Type() == QuantumIdentifier::NONE)
    return false;
  else if (id.Type() == QuantumIdentifier::ALL)
    return true;
  else if (id.Type() == QuantumIdentifier::TRANSITION)
    throw std::runtime_error("Cannot match transition level to energy level");
  else {
    for (Index iq=0; iq<Index(QuantumNumberType::FINAL_ENTRY); iq++) {
      auto qn_line = band.LowerQuantumNumber(line_index, QuantumNumberType(iq));
      auto qn_id = id.EnergyLevelQuantumNumber(QuantumNumberType(iq));
      
      if ((qn_id.isUndefined() and qn_line.isDefined()) or 
          (qn_line.isDefined() and qn_line not_eq qn_id)) {
        return false;
      }
    }
  }
  
  return true;
}

bool Absorption::id_in_line(const Absorption::Lines& band, const QuantumIdentifier& id, size_t line_index)
{
  if (band.Species() != id.Species())
    return false;
  else if (band.Isotopologue() != id.Isotopologue())
    return false;
  else if (id.Type() == QuantumIdentifier::NONE)
    return false;
  else if (id.Type() == QuantumIdentifier::ALL)
    return true;
  else if (id.Type() == QuantumIdentifier::ENERGY_LEVEL)
    throw std::runtime_error("Cannot match energy level to line");
  else {
    for (Index iq=0; iq<Index(QuantumNumberType::FINAL_ENTRY); iq++) {
      auto qn_line = band.LowerQuantumNumber(line_index, QuantumNumberType(iq));
      auto qn_id = id.LowerQuantumNumber(QuantumNumberType(iq));
      
      if ((qn_line.isUndefined() and qn_id.isDefined()) or 
          (qn_id.isDefined() and qn_id not_eq qn_line)) {
        return false;
      }
    }
    
    for (Index iq=0; iq<Index(QuantumNumberType::FINAL_ENTRY); iq++) {
      auto qn_line = band.UpperQuantumNumber(line_index, QuantumNumberType(iq));
      auto qn_id = id.UpperQuantumNumber(QuantumNumberType(iq));
      
      if ((qn_line.isUndefined() and qn_id.isDefined()) or 
          (qn_id.isDefined() and qn_id not_eq qn_line)) {
        return false;
      }
    }
  }
  
  return true;
}

bool Absorption::id_in_line_upper(const Absorption::Lines& band, const QuantumIdentifier& id, size_t line_index)
{
  if (band.Species() != id.Species())
    return false;
  else if (band.Isotopologue() != id.Isotopologue())
    return false;
  else if (id.Type() == QuantumIdentifier::NONE)
    return false;
  else if (id.Type() == QuantumIdentifier::ALL)
    return true;
  else if (id.Type() == QuantumIdentifier::TRANSITION)
    throw std::runtime_error("Cannot match transition level to energy level");
  else {
    for (Index iq=0; iq<Index(QuantumNumberType::FINAL_ENTRY); iq++) {
      auto qn_line = band.UpperQuantumNumber(line_index, QuantumNumberType(iq));
      auto qn_id = id.EnergyLevelQuantumNumber(QuantumNumberType(iq));
      
      if ((qn_line.isUndefined() and qn_id.isDefined()) or 
          (qn_id.isDefined() and qn_id not_eq qn_line)) {
        return false;
      }
    }
  }
  
  return true;
}

bool Absorption::id_in_line_lower(const Absorption::Lines& band, const QuantumIdentifier& id, size_t line_index)
{
  if (band.Species() != id.Species())
    return false;
  else if (band.Isotopologue() != id.Isotopologue())
    return false;
  else if (id.Type() == QuantumIdentifier::NONE)
    return false;
  else if (id.Type() == QuantumIdentifier::ALL)
    return true;
  else if (id.Type() == QuantumIdentifier::TRANSITION)
    throw std::runtime_error("Cannot match transition level to energy level");
  else {
    for (Index iq=0; iq<Index(QuantumNumberType::FINAL_ENTRY); iq++) {
      auto qn_line = band.LowerQuantumNumber(line_index, QuantumNumberType(iq));
      auto qn_id = id.EnergyLevelQuantumNumber(QuantumNumberType(iq));
      
      if ((qn_line.isUndefined() and qn_id.isDefined()) or 
          (qn_id.isDefined() and qn_id not_eq qn_line)) {
        return false;
      }
    }
  }
  
  return true;
}

bool Absorption::line_is_id(const Absorption::Lines& band, const QuantumIdentifier& id, size_t line_index)
{
  if (line_in_id(band, id, line_index) and id_in_line(band, id, line_index))
    return true;
  else
    return false;
}

Numeric Absorption::reduced_rovibrational_dipole(Rational Jf, Rational Ji, Rational lf, Rational li, Rational k) {
  if (not even(Jf + lf + 1))
    return - sqrt(2 * Jf + 1) * wigner3j(Jf, k, Ji, li, lf - li, -lf);
  else
    return + sqrt(2 * Jf + 1) * wigner3j(Jf, k, Ji, li, lf - li, -lf);
}

Numeric Absorption::reduced_magnetic_quadrapole(Rational Jf, Rational Ji, Rational N) {
  if (not even(Jf + N))
    return - sqrt(6 * (2 * Jf + 1) * (2 * Ji + 1)) * wigner6j(1_rat, 1_rat, 1_rat, Ji, Jf, N);
  else
    return + sqrt(6 * (2 * Jf + 1) * (2 * Ji + 1)) * wigner6j(1_rat, 1_rat, 1_rat, Ji, Jf, N);
}

Absorption::SingleLineExternal Absorption::ReadFromMytran2Stream(istream& is)
{
  // Default data and values for this type
  SingleLineExternal data;
  data.selfbroadening = true;
  data.bathbroadening = true;
  data.lineshapetype = LineShape::Type::VP;
  data.species.resize(2);
  
  // Global species lookup data:
  using global_data::species_data;

  // This value is used to flag missing data both in species and
  // isotopologue lists. Could be any number, it just has to be made sure
  // that it is neither the index of a species nor of an isotopologue.
  const Index missing = species_data.nelem() + 100;

  // We need a species index sorted by MYTRAN tag. Keep this in a
  // static variable, so that we have to do this only once.  The ARTS
  // species index is hind[<MYTRAN tag>]. The value of
  // missing means that we don't have this species.
  //
  // Allow for up to 100 species in MYTRAN in the future.
  static Array<Index> hspec(100, missing);

  // This is  an array of arrays for each mytran tag. It contains the
  // ARTS indices of the MYTRAN isotopologues.
  static Array<ArrayOfIndex> hiso(100);

  // Remember if this stuff has already been initialized:
  static bool hinit = false;

  // Remember, about which missing species we have already issued a
  // warning:
  static ArrayOfIndex warned_missing;

  if (!hinit) {
    for (Index i = 0; i < species_data.nelem(); ++i) {
      const SpeciesRecord& sr = species_data[i];

      // We have to be careful and check for the case that all
      // MYTRAN isotopologue tags are -1 (this species is missing in MYTRAN).

      if (0 < sr.Isotopologue()[0].MytranTag()) {
        // The MYTRAN tags are stored as species plus isotopologue tags
        // (MO and ISO)
        // in the Isotopologue() part of the species record.
        // We can extract the MO part from any of the isotopologue tags,
        // so we use the first one. We do this by taking an integer
        // division by 10.

        Index mo = sr.Isotopologue()[0].MytranTag() / 10;
        //          cout << "mo = " << mo << endl;
        hspec[mo] = i;

        // Get a nicer to handle array of MYTRAN iso tags:
        Index n_iso = sr.Isotopologue().nelem();
        ArrayOfIndex iso_tags;
        iso_tags.resize(n_iso);
        for (Index j = 0; j < n_iso; ++j) {
          iso_tags[j] = sr.Isotopologue()[j].MytranTag();
        }

        // Reserve elements for the isotopologue tags. How much do we
        // need? This depends on the largest MYTRAN tag that we know
        // about!
        // Also initialize the tags to missing.
        //          cout << "iso_tags = " << iso_tags << endl;
        //          cout << "static_cast<Index>(max(iso_tags))%10 + 1 = "
        //               << static_cast<Index>(max(iso_tags))%10 + 1 << endl;
        hiso[mo].resize(max(iso_tags) % 10 + 1);
        hiso[mo] = missing;  // Matpack can set all elements like this.

        // Set the isotopologue tags:
        for (Index j = 0; j < n_iso; ++j) {
          if (0 < iso_tags[j])  // ignore -1 elements
          {
            // To get the iso tags from MytranTag() we also have to take
            // modulo 10 to get rid of mo.
            //                  cout << "iso_tags[j] % 10 = " << iso_tags[j] % 10 << endl;
            hiso[mo][iso_tags[j] % 10] = j;
          }
        }
      }
    }

    hinit = true;
  }

  // This contains the rest of the line to parse. At the beginning the
  // entire line. Line gets shorter and shorter as we continue to
  // extract stuff from the beginning.
  String line;

  // The first item is the molecule number:
  Index mo;

  // Look for more comments?
  bool comment = true;

  while (comment) {
    // Return true if eof is reached:
    if (is.eof()) return data;

    // Throw runtime_error if stream is bad:
    if (!is) throw runtime_error("Stream bad.");

    // Read line from file into linebuffer:
    getline(is, line);

    // It is possible that we were exactly at the end of the file before
    // calling getline. In that case the previous eof() was still false
    // because eof() evaluates only to true if one tries to read after the
    // end of the file. The following check catches this.
    if (line.nelem() == 0 && is.eof()) return data;

    // Because of the fixed FORTRAN format, we need to break up the line
    // explicitly in apropriate pieces. Not elegant, but works!

    // Extract molecule number:
    mo = 0;
    // Initialization of mo is important, because mo stays the same
    // if line is empty.
    extract(mo, line, 2);
    //           cout << "mo = " << mo << endl;

    // If mo == 0 this is just a comment line:
    if (0 != mo) {
      // See if we know this species. We will give an error if a
      // species is not known.
      if (missing != hspec[mo])
        comment = false;
      else {
        // See if this is already in warned_missing, use
        // std::count for that:
        if (0 == std::count(warned_missing.begin(), warned_missing.end(), mo)) {
          warned_missing.push_back(mo);
        }
      }
    }
  }

  // Ok, we seem to have a valid species here.

  // Set mspecies from my cool index table:
  data.quantumidentity.Species(hspec[mo]);

  // Extract isotopologue:
  Index iso;
  extract(iso, line, 1);
  //  cout << "iso = " << iso << endl;

  // Set misotopologue from the other cool index table.
  // We have to be careful to issue an error for unknown iso tags. Iso
  // could be either larger than the size of hiso[mo], or set
  // explicitly to missing. Unfortunately we have to test both cases.
  data.quantumidentity.Isotopologue(missing);
  if (iso < hiso[mo].nelem())
    if (missing != hiso[mo][iso]) data.quantumidentity.Isotopologue(hiso[mo][iso]);

  // Issue error message if misotopologue is still missing:
  if (missing == data.quantumidentity.Isotopologue()) {
    ostringstream os;
    os << "Species: " << species_data[data.quantumidentity.Species()].Name()
       << ", isotopologue iso = " << iso << " is unknown.";
    throw runtime_error(os.str());
  }

  // Position.
  {
    // MYTRAN position in MHz:
    Numeric v;

    // Extract MYTRAN postion:
    extract(v, line, 13);

    // ARTS position in Hz:
    data.line.F0() = v * 1E6;
    //    cout << "mf = " << mf << endl;
  }

  // Accuracy for line position
  {
    // Extract MYTRAN postion accuracy:
    Numeric df;
    extract(df, line, 8);
  }

  // Intensity.
  {
    constexpr Numeric SPEED_OF_LIGHT = Constant::c;  // in [m/s]

    // MYTRAN2 intensity is in cm-1/(molec * cm-2) at 296 Kelvin.
    // (just like HITRAN, only isotopologue ratio is not included)
    // The first cm-1 is the frequency unit (it cancels with the
    // 1/frequency unit of the line shape function).
    //
    // We need to do the following:
    // 1. Convert frequency from wavenumber to Hz (factor 1e2 * c)
    // 2. Convert [molec * cm-2] to [molec * m-2] (factor 1e-4)

    const Numeric hi2arts = 1e-2 * SPEED_OF_LIGHT;

    Numeric s;

    // Extract HITRAN intensity:
    extract(s, line, 10);

    // Convert to ARTS units (Hz / (molec * m-2) ), or shorter: Hz*m^2
    data.line.I0() = s * hi2arts;
  }

  // Air broadening parameters.
  Numeric agam, sgam;
  {
    // MYTRAN parameter is in MHz/Torr at reference temperature
    // All parameters are HWHM
    Numeric gam;
    // External constant from constants.cc: Converts torr to
    // Pa. Multiply value in torr by this number to get value in Pa.
    constexpr Numeric TORR2PA = Conversion::TORR2PA;

    // Extract HITRAN AGAM value:
    extract(gam, line, 5);

    // ARTS parameter in Hz/Pa:
    agam = gam * 1E6 / TORR2PA;

    // Extract MYTRAN SGAM value:
    extract(gam, line, 5);

    // ARTS parameter in Hz/Pa:
    sgam = gam * 1E6 / TORR2PA;

    //    cout << "agam, sgam = " << magam << ", " << msgam << endl;
  }

  // Lower state energy.
  {
    // MYTRAN parameter is in wavenumbers (cm^-1).
    // We have to convert this to the ARTS unit Joule.

    // Extract from Catalogue line
    extract(data.line.E0(), line, 10);

    // Convert to Joule:
    data.line.E0() = wavenumber_to_joule(data.line.E0());
  }

  // Temperature coefficient of broadening parameters.
  Numeric nself, nair;
  {
    // This is dimensionless, we can also extract directly.
    extract(nair, line, 4);

    // Extract the self broadening parameter:
    extract(nself, line, 4);
    //    cout << "mnair = " << mnair << endl;
  }

  // Reference temperature for broadening parameter in K:
  Numeric tgam;
  {
    // correct units, extract directly
    extract(tgam, line, 7);
  }

  // Pressure shift.
  Numeric psf;
  {
    // MYTRAN value in MHz/Torr
    Numeric d;
    // External constant from constants.cc: Converts torr to
    // Pa. Multiply value in torr by this number to get value in Pa.
    constexpr Numeric TORR2PA = Conversion::TORR2PA;

    // Extract MYTRAN value:
    extract(d, line, 9);

    // ARTS value in Hz/Pa
    psf = d * 1E6 / TORR2PA;
  }
  // Set the accuracies using the definition of MYTRAN accuracy
  // indices. If some are missing, they are set to -1.

  //Skip upper state global quanta index
  {
    Index eu;
    extract(eu, line, 3);
  }

  //Skip lower state global quanta index
  {
    Index el;
    extract(el, line, 3);
  }

  //Skip upper state local quanta
  {
    Index eul;
    extract(eul, line, 9);
  }

  //Skip lower state local quanta
  {
    Index ell;
    extract(ell, line, 9);
  }
  // Accuracy index for intensity
  {
    Index di0;
    // Extract MYTRAN value:
    extract(di0, line, 1);
  }

  // Accuracy index for AGAM
  {
    Index dgam;
    // Extract MYTRAN value:
    extract(dgam, line, 1);
  }

  // Accuracy index for NAIR
  {
    Index dair;
    // Extract MYTRAN value:
    extract(dair, line, 1);
  }

  // These were all the parameters that we can extract from
  // MYTRAN. However, we still have to set the reference temperatures
  // to the appropriate value:

  // Reference temperature for Intensity in K.
  // (This is fix for MYTRAN2)
  data.T0 = 296.0;

  // It is important that you intialize here all the new parameters that
  // you added to the line record. (This applies to all the reading
  // functions, also for ARTS, JPL, and HITRAN format.) Parameters
  // should be either set from the catalogue, or set to -1.)

  // Convert to correct temperature if tgam != ti0
  if (tgam != data.T0) {
    agam *= pow(tgam / data.T0, nair);
    sgam *= pow(tgam / data.T0, nself);
    psf *= pow(tgam / data.T0, (Numeric).25 + (Numeric)1.5 * nair);
  }

  // Set line shape computer
  data.line.LineShape() = LineShape::Model(sgam, nself, agam, nair, psf);

  // That's it!
  data.bad = false;
  return data;
}

Absorption::SingleLineExternal Absorption::ReadFromJplStream(istream& is)
{
  // Default data and values for this type
  SingleLineExternal data;
  data.selfbroadening = true;
  data.bathbroadening = true;
  data.lineshapetype = LineShape::Type::VP;
  data.species.resize(2);

  // Global species lookup data:
  using global_data::species_data;

  // We need a species index sorted by JPL tag. Keep this in a
  // static variable, so that we have to do this only once.  The ARTS
  // species index is JplMap[<JPL tag>]. We need Index in this map,
  // because the tag array within the species data is an Index array.
  static map<Index, SpecIsoMap> JplMap;

  // Remember if this stuff has already been initialized:
  static bool hinit = false;

  if (!hinit) {
    for (Index i = 0; i < species_data.nelem(); ++i) {
      const SpeciesRecord& sr = species_data[i];

      for (Index j = 0; j < sr.Isotopologue().nelem(); ++j) {
        for (Index k = 0; k < sr.Isotopologue()[j].JplTags().nelem(); ++k) {
          SpecIsoMap indicies(i, j);

          JplMap[sr.Isotopologue()[j].JplTags()[k]] = indicies;
        }
      }
    }
    hinit = true;
  }

  // This contains the rest of the line to parse. At the beginning the
  // entire line. Line gets shorter and shorter as we continue to
  // extract stuff from the beginning.
  String line;

  // Look for more comments?
  bool comment = true;

  while (comment) {
    // Return true if eof is reached:
    if (is.eof()) return data;

    // Throw runtime_error if stream is bad:
    if (!is) throw runtime_error("Stream bad.");

    // Read line from file into linebuffer:
    getline(is, line);

    // It is possible that we were exactly at the end of the file before
    // calling getline. In that case the previous eof() was still false
    // because eof() evaluates only to true if one tries to read after the
    // end of the file. The following check catches this.
    if (line.nelem() == 0 && is.eof()) return data;

    // Because of the fixed FORTRAN format, we need to break up the line
    // explicitly in apropriate pieces. Not elegant, but works!

    // Extract center frequency:
    // Initialization of v is important, because v stays the same
    // if line is empty.
    // JPL position in MHz:
    Numeric v = 0.0;

    // Extract JPL position:
    extract(v, line, 13);

    // check for empty line
    if (v != 0.0) {
      // ARTS position in Hz:
      data.line.F0() = v * 1E6;

      comment = false;
    }
  }

  // Accuracy for line position
  {
    Numeric df;
    extract(df, line, 8);
  }

  // Intensity.
  {
    // JPL has log (10) of intensity in nm2 MHz at 300 Kelvin.
    //
    // We need to do the following:
    // 1. take 10^intensity
    // 2. convert to cm-1/(molecule * cm-2): devide by c * 1e10
    // 3. Convert frequency from wavenumber to Hz (factor 1e2 * c)
    // 4. Convert [molec * cm-2] to [molec * m-2] (factor 1e-4)

    Numeric s;

    // Extract JPL intensity:
    extract(s, line, 8);

    // remove log
    s = pow((Numeric)10., s);

    // Convert to ARTS units (Hz / (molec * m-2) ), or shorter: Hz*m^2
    data.line.I0() = s / 1E12;
  }

  // Degrees of freedom
  {
    Index dr;

    // Extract degrees of freedom
    extract(dr, line, 2);
  }

  // Lower state energy.
  {
    // JPL parameter is in wavenumbers (cm^-1).
    // We have to convert this to the ARTS unit Joule.

    // Extract from Catalogue line
    extract(data.line.E0(), line, 10);

    // Convert to Joule:
    data.line.E0() = wavenumber_to_joule(data.line.E0());
  }

  // Upper state degeneracy
  {
    Index gup;

    // Extract upper state degeneracy
    extract(gup, line, 3);
  }

  // Tag number
  Index tag;
  {
    // Extract Tag number
    extract(tag, line, 7);

    // make sure tag is not negative (damned jpl cat):
    tag = tag > 0 ? tag : -tag;
  }

  // ok, now for the cool index map:

  // is this tag valid?
  const map<Index, SpecIsoMap>::const_iterator i = JplMap.find(tag);
  if (i == JplMap.end()) {
    ostringstream os;
    os << "JPL Tag: " << tag << " is unknown.";
    throw runtime_error(os.str());
  }

  SpecIsoMap id = i->second;

  // Set line ID
  data.quantumidentity.Species(id.Speciesindex());
  data.quantumidentity.Isotopologue(id.Isotopologueindex());

  // Air broadening parameters: unknown to jpl, use old iup forward
  // model default values, which is mostly set to 0.0025 GHz/hPa, even
  // though for some lines the pressure broadening is given explicitly
  // in the program code. The explicitly given values are ignored and
  // only the default value is set. Self broadening was in general not
  // considered in the old forward model.
  Numeric agam, sgam;
  {
    // ARTS parameter in Hz/Pa:
    agam = 2.5E4;

    // ARTS parameter in Hz/Pa:
    sgam = agam;
  }

  // Temperature coefficient of broadening parameters. Was set to 0.75
  // in old forward model, even though for some lines the parameter is
  // given explicitly in the program code. The explicitly given values
  // are ignored and only the default value is set. Self broadening
  // not considered.
  Numeric nair, nself;
  {
    nair = 0.75;
    nself = 0.0;
  }

  // Reference temperature for broadening parameter in K, was
  // generally set to 300 K in old forward model, with the exceptions
  // as already mentioned above: //DEPRECEATED but is same as for mti0 so moving on
  //   {
  //     mtgam = 300.0;
  //   }

  // Pressure shift: not given in JPL, set to 0
  Numeric psf;
  { psf = 0.0; }

  // These were all the parameters that we can extract from
  // JPL. However, we still have to set the reference temperatures
  // to the appropriate value:

  // Reference temperature for Intensity in K.
  data.T0 = 300.0;

  // Set line shape computer
  data.line.LineShape() = LineShape::Model(sgam, nself, agam, nair, psf);

  // That's it!
  data.bad = false;
  return data;
}



bool Absorption::Lines::OK() const noexcept
{
  const Index nq = mlocalquanta.size();
  const Index nb = mbroadeningspecies.nelem();
  
  // Test that self and bath is covered by the range if set positive
  if (nb < (Index(mselfbroadening) + Index(mbathbroadening)))
    return false;
 
  // Test that the temperature is physical
  if (mT0 <= 0)
    return false;
  
  // Test that all lines have the correct sized line shape model
  if (std::any_of(mlines.cbegin(), mlines.cend(), [nb](auto& line){return line.LineShapeElems() != nb;}))
    return false;
  
  // Test that all lines have the correct sized local quantum numbers
  if (std::any_of(mlines.cbegin(), mlines.cend(), [nq](auto& line){return line.LowerQuantumElems() != nq or line.UpperQuantumElems() != nq;}))
    return false;
  
  // Otherwise everything is fine!
  return true;
}
