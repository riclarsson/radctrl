/* Copyright (C) 2018
   Richard Larsson
                            
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
 * @file m_nlte.cc
 * @author Richard Larsson
 * @date 2018-03-07
 * 
 * @brief User interface to NLTE variables and functions
 */

#include "absorption.h"
#include "arts.h"
#include "auto_md.h"
#include "lin_alg.h"
#include "nlte.h"

/* Workspace method: Doxygen documentation will be auto-generated */
void ArrayOfQuantumIdentifierFromLines(
    ArrayOfQuantumIdentifier& qid,
    const ArrayOfArrayOfAbsorptionLines& abs_lines_per_species,
    const Index& global,
    const Verbosity&) {
  // Defined only as output not input so resizing
  qid.resize(0);

  QuantumIdentifier lower, upper;
  
  // For all lines' upper and lower energy levels
  for (const auto& lines: abs_lines_per_species) {
    for (const auto& band: lines) {
      for (Index k=0; k<band.NumLines() and (global ? (k==0) : false); k++) {
        if (global) {
          lower = band.QuantumIdentity().LowerQuantumId();
          upper = band.QuantumIdentity().UpperQuantumId();
        } else {
          auto x = band.QuantumIdentityOfLine(k);
          lower = x.LowerQuantumId();
          upper = x.UpperQuantumId();
        }
        
        const bool canbeinlower = lower.any_quantumnumbers(),
                   canbeinupper = upper.any_quantumnumbers();

        // Test if the level has already been treated
        const Index n = qid.nelem();
        bool inlower = false, inupper = false;
        for (Index i = 0; i < n; i++) {
          if (not inlower and canbeinlower)
            if (qid[i] == lower) inlower = true;
          if (not inupper and canbeinupper)
            if (qid[i] == upper) inupper = true;
        }

        // If the level has not been treated and has any quantum numbers, then store it
        if (not inlower and canbeinlower) qid.push_back(lower);
        if (not inupper and canbeinupper)
          if (not(lower == upper)) qid.push_back(upper);
      }
    }
  }
}

/* Workspace method: Doxygen documentation will be auto-generated */
void nlte_fieldRescalePopulationLevels(EnergyLevelMap& nlte_field,
                                       const Numeric& scale,
                                       const Verbosity&) {
  nlte_field.Data() *= scale;
}

/* Workspace method: Doxygen documentation will be auto-generated */
void nlte_fieldForSingleSpeciesNonOverlappingLines(
    Workspace& ws,
    EnergyLevelMap& nlte_field,
    const ArrayOfArrayOfSpeciesTag& abs_species,
    const ArrayOfArrayOfAbsorptionLines& abs_lines_per_species,
    const ArrayOfArrayOfGriddedField1& collision_coefficients,
    const ArrayOfQuantumIdentifier& collision_line_identifiers,
    const SpeciesAuxData& isotopologue_ratios,
    const Agenda& iy_main_agenda,
    const Agenda& ppath_agenda,
    const Agenda& iy_space_agenda,
    const Agenda& iy_surface_agenda,
    const Agenda& iy_cloudbox_agenda,
    const Agenda& propmat_clearsky_agenda,
    const Agenda& /*water_p_eq_agenda*/,
    const Tensor4& vmr_field,
    const Tensor3& t_field,
    const Tensor3& z_field,
    const Vector& p_grid,
    const Index& atmosphere_dim,
    const Vector& refellipsoid,
    const Tensor3& surface_props_data,
    const Index& nlte_do,
    const Numeric& df,
    const Numeric& convergence_limit,
    const Index& nz,
    const Index& nf,
    const Index& dampened,
    const Index& iteration_limit,
    const Verbosity& verbosity)
{
  CREATE_OUT2;

  if (not nlte_do) throw std::runtime_error("Must be set to do NLTE");
  if (nlte_field.Data().empty())
    throw std::runtime_error("Error in NLTE field, it is empty");

  Matrix line_irradiance;
  Tensor3 line_transmission;

  const Index nlevels = nlte_field.Levels().nelem(), np = p_grid.nelem();
  if (nlevels < 5)
    throw std::runtime_error("Must have more than a four levels");

  if (atmosphere_dim not_eq 1)
    throw std::runtime_error("Only for 1D atmosphere");

  const Index nlines = nelem(abs_lines_per_species);
  if (nlevels >= nlines)
    throw std::runtime_error(
        "Bad number of lines... overlapping lines in nlte_level_identifiers?");

  // Create basic compute vectors
  const Vector Aij = createAij(abs_lines_per_species);
  const Vector Bij = createBij(abs_lines_per_species);
  const Vector Bji = createBji(Bij, abs_lines_per_species);
  Vector Cij(nlines), Cji(nlines);

  ArrayOfIndex upper, lower;
  nlte_positions_in_statistical_equilibrium_matrix(
    upper, lower, abs_lines_per_species, nlte_field);
  const Index unique = find_first_unique_in_lower(upper, lower);

  // Compute arrays
  Matrix SEE(nlevels, nlevels, 0.0);
  Vector r(nlevels, 0.0), x(nlevels, 0.0);
  Numeric max_change = convergence_limit + 1;

  Index i = 0;
  while (i < iteration_limit and max_change > convergence_limit) {
    // Reset change
    max_change = 0.0;

    //     //Compute radiation and transmission
    line_irradianceCalcForSingleSpeciesNonOverlappingLinesPseudo2D(
        ws,
        line_irradiance,
        line_transmission,
        abs_species,
        abs_lines_per_species,
        nlte_field,
        vmr_field,
        t_field,
        z_field,
        p_grid,
        refellipsoid,
        surface_props_data,
        ppath_agenda,
        iy_main_agenda,
        iy_space_agenda,
        iy_surface_agenda,
        iy_cloudbox_agenda,
        propmat_clearsky_agenda,
        df,
        nz,
        nf,
        1.0,
        verbosity);

    for (Index ip = 0; ip < np; ip++) {
      r = nlte_field.Data()(joker, ip, 0, 0);
      nlte_collision_factorsCalcFromCoeffs(Cij,
                                           Cji,
                                           abs_lines_per_species,
                                           abs_species,
                                           collision_coefficients,
                                           collision_line_identifiers,
                                           isotopologue_ratios,
                                           vmr_field(joker, ip, 0, 0),
                                           t_field(ip, 0, 0),
                                           p_grid[ip]);
      

      if (dampened)
        dampened_statistical_equilibrium_equation(
            SEE,
            r,
            Aij,
            Bij,
            Bji,
            Cij,
            Cji,
            line_irradiance(joker, ip),
            line_transmission(0, joker, ip),
            upper,
            lower);
      else
        statistical_equilibrium_equation(SEE,
                                         Aij,
                                         Bij,
                                         Bji,
                                         Cij,
                                         Cji,
                                         line_irradiance(joker, ip),
                                         upper,
                                         lower);

      set_constant_statistical_equilibrium_matrix(SEE, x, r.sum(), unique);
      solve(nlte_field.Data()(joker, ip, 0, 0), SEE, x);

      for (Index il = 0; il < nlevels; il++) {
        max_change =
            max(abs(nlte_field.Data()(il, ip, 0, 0) - r[il]) / r[il], max_change);
      }
    }
    i++;
  }

  if (i < iteration_limit)
    out2 << "Converged NLTE ratios (within convergence_limit) returned after "
         << i << " iterations\n";
  else
    out2
        << "No convergence of NLTE ratios (within convergence_limit) returned even after "
        << iteration_limit << " iterations\n";
}

/* Workspace method: Doxygen documentation will be auto-generated */
void collision_coefficientsFromSplitFiles(
    ArrayOfArrayOfGriddedField1& collision_coefficients,
    ArrayOfQuantumIdentifier& collision_line_identifiers,
    const ArrayOfArrayOfSpeciesTag& abs_species,
    const String& basename,
    const Verbosity& verbosity) {
  // Standard ARTS basename-assumption
  String tmp_basename = basename, filename;
  if (basename.length() && basename[basename.length() - 1] != '/')
    tmp_basename += ".";

  // Read the identification file
  filename = tmp_basename + "qid.xml";
  xml_read_from_file(filename, collision_line_identifiers, verbosity);
  check_collision_line_identifiers(collision_line_identifiers);

  // Inner array size has to be this constantly
  const Index n = collision_line_identifiers.nelem();

  // Set species dimensions and fill the array
  collision_coefficients.resize(abs_species.nelem());
  for (Index i = 0; i < collision_coefficients.nelem(); i++) {
    ArrayOfGriddedField1 aogf1;

    // Read the file for a species and check that the size is correct of the array
    filename = tmp_basename + abs_species[i][0].SpeciesNameMain() + ".xml";
    xml_read_from_file(filename, aogf1, verbosity);
    if (aogf1.nelem() not_eq n)
      throw std::runtime_error(
          "Mismatch between collision_line_identifiers and some collision_coefficients");
    collision_coefficients[i] = aogf1;
  }
}

/* Workspace method: Doxygen documentation will be auto-generated */
void nlteOff(Index& nlte_do,
             EnergyLevelMap& nlte_field,
             ArrayOfQuantumIdentifier& nlte_level_identifiers,
             const Verbosity&) {
  nlte_do = 0;
  nlte_field = EnergyLevelMap();
  nlte_level_identifiers.resize(0);
}
