/* Copyright (C) 2020
 * Richard Larsson <larsson@mps.mpg.de>
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
 * @file   m_raw.cc
 * @author Richard Larsson
 * @date   2020-04-13
 * 
 * @brief  Stuff related to generating y-data from raw data
 */


#include "artstime.h"
#include "raw.h"


void yColdAtmHot(Vector& y,
                 const Vector& cold,
                 const Vector& atm,
                 const Vector& hot,
                 const Numeric& cold_temp,
                 const Numeric& hot_temp,
                 const Index& calib,
                 const Verbosity&)
{
  if(cold.nelem() not_eq atm.nelem() or atm.nelem() not_eq hot.nelem()) {
    throw std::runtime_error("Length of vectors must be correct");
  }
   
  y.resize(atm.nelem());
  if (calib) {
    for (Index i=0; i<y.nelem(); i++) {
      y[i] = calibration(cold[i], atm[i], hot[i], cold_temp, hot_temp);
    }
  } else {
    for (Index i=0; i<y.nelem(); i++) {
      y[i] = systemtemp(cold[i], hot[i], cold_temp, hot_temp);
    }
  }
}


void ybatchTimeAveraging(ArrayOfVector& ybatch,
                         ArrayOfTime& time_grid,
                         ArrayOfMatrix& covmat_sepsbatch,
                         ArrayOfIndex& counts,
                         const String& time_step,
                         const Index& disregard_first,
                         const Index& disregard_last,
                         const Verbosity&)
{
  // Size of problem
  const Index n=time_grid.nelem();
  if (time_grid.nelem() not_eq n) {
    throw std::runtime_error("Time vector length must match input data length");
  }
  
  // Time is not decreasing
  if (not std::is_sorted(time_grid.cbegin(), time_grid.cend()))
    throw std::runtime_error("Time vector cannot decrease");
  
  // Find the limits of the range
  auto lims = time_steps(time_grid, time_step);
  
  // Output variables
  ArrayOfVector ybatch_out;
  ArrayOfTime time_grid_out;
  
  if (lims.front() == n) {
    ybatch_out.resize(0);
    time_grid_out.resize(0);
    covmat_sepsbatch.resize(0);
    counts.resize(0);
  } else {
    
    // Frequency grids
    const Index k = ybatch[0].nelem();
    if (not std::all_of(ybatch.cbegin(), ybatch.cend(), [k](auto& x){return x.nelem() == k;})) {
      throw std::runtime_error("Bad frequency grid size in input data; expects all equal");
    }
    
    // Allocate output
    const Index m = lims.nelem() - 1;
    if (m < 0)
      throw std::runtime_error("Must include last if time step covers all of the range");
    ybatch_out = ArrayOfVector(m, Vector(k));
    time_grid_out = ArrayOfTime(m);
    covmat_sepsbatch = ArrayOfMatrix(m, Matrix(k, k));
    counts.resize(m);
    
    // Fill output
    #pragma omp parallel for if (not arts_omp_in_parallel()) schedule(guided)
    for (Index i=0; i<m; i++) {
      counts[i] = lims[i+1] - lims[i];
      time_grid_out[i] = mean_time(time_grid, lims[i], lims[i+1]);
      linalg::avg(ybatch_out[i], ybatch, lims[i], lims[i+1]);
      linalg::cov(covmat_sepsbatch[i], ybatch_out[i], ybatch, lims[i], lims[i+1]);
    }
  }
  
  if (disregard_first) {
    counts.erase(counts.begin());
    ybatch_out.erase(ybatch_out.begin());
    time_grid_out.erase(time_grid_out.begin());
    covmat_sepsbatch.erase(covmat_sepsbatch.begin());
  }
  
  if (disregard_last) {
    counts.pop_back();
    ybatch_out.pop_back();
    time_grid_out.pop_back();
    covmat_sepsbatch.pop_back();
  }
  
  ybatch = ybatch_out;
  time_grid = time_grid_out;
}


void ybatchTroposphericCorrectionNaiveMedianForward(ArrayOfVector& ybatch_corr,
                                                    ArrayOfVector& ybatch,
                                                    const ArrayOfIndex& range,
                                                    const Vector& trop_temp,
                                                    const Numeric& targ_temp,
                                                    const Verbosity&)
{
  // Size of problem
  const Index n=ybatch.nelem();
  
  const Index m=n?ybatch[0].nelem():0;
  if (std::any_of(ybatch.begin(), ybatch.end(), [m](auto& y){return y.nelem() not_eq m;})) {
    throw std::runtime_error("Bad input size, all of ybatch must match itself");
  } else if (trop_temp.nelem() not_eq n) {
    throw std::runtime_error("Bad input size, trop_temp must match ybatch");
  }
  
  // This algorithm stores partial transmission and median and tropospheric temperature in the correction terms
  ybatch_corr = ArrayOfVector(n, Vector(3));
  
  // Compute tropospheric correction
  for (Index i=0; i<n; i++) {
    ybatch_corr[i][2] = trop_temp[i];
    ybatch_corr[i][0] = linalg::median(ybatch[i], range);
    ybatch_corr[i][1] = std::exp(- std::log((ybatch_corr[i][2] - ybatch_corr[i][0])  / (ybatch_corr[i][2] - targ_temp)));
  }
  
  // Apply correction
  for (Index i=0; i<n; i++) {
    ybatch[i] *= ybatch_corr[i][1];
    ybatch[i] += ybatch_corr[i][2] * (1 - ybatch_corr[i][1]);
  }
}


void ybatchTroposphericCorrectionNaiveMedianInverse(ArrayOfVector& ybatch,
                                                    const ArrayOfVector& ybatch_corr,
                                                    const Verbosity&)
{
  // Size of problem
  const Index n=ybatch.nelem();
  if ((std::any_of(ybatch_corr.begin(), ybatch_corr.end(), [](auto& corr){return corr.nelem() not_eq 3;})) or ybatch_corr.nelem() not_eq n) {
    throw std::runtime_error("Bad input size, all of ybatch_corr must match ybatch and have three elements each");
  }
  
  // Apply inverse of correction
  for (Index i=0; i<n; i++) {
    ybatch[i] -= ybatch_corr[i][2] * (1 - ybatch_corr[i][1]);
    ybatch[i] /= ybatch_corr[i][1];
  }
}
