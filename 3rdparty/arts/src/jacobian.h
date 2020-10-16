/* Copyright (C) 2004-2012 Mattias Ekstrom <ekstrom@rss.chalmers.se>

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
  @file   jacobian.h
  @author Mattias Ekstrom <ekstrom@rss.chalmers.se>
  @date   2004-09-14

  @brief  Routines for setting up the jacobian.
 */

#ifndef jacobian_h
#define jacobian_h

#include <iostream>
#include <map>
#include <stdexcept>
#include "abs_species_tags.h"
#include "agenda_class.h"
#include "array.h"
#include "bifstream.h"
#include "interpolation.h"
#include "logic.h"
#include "matpackI.h"
#include "methods.h"
#include "mystring.h"
#include "ppath.h"

#include "quantum.h"

/** List of Jacobian properties for analytical line shape related derivatives */
enum class JacPropMatType : Index {
  VMR,
  Electrons,
  Particulates,
  Temperature,

  MagneticMagnitude,
  MagneticU,
  MagneticV,
  MagneticW,

  WindMagnitude,
  WindU,
  WindV,
  WindW,
  Frequency,  // Note:  This is how wind is done internal to propmat agenda

  LineStrength,
  LineCenter,
  
  LineSpecialParameter1,

  LineShapeG0X0,
  LineShapeG0X1,
  LineShapeG0X2,
  LineShapeG0X3,

  LineShapeD0X0,
  LineShapeD0X1,
  LineShapeD0X2,
  LineShapeD0X3,

  LineShapeG2X0,
  LineShapeG2X1,
  LineShapeG2X2,
  LineShapeG2X3,

  LineShapeD2X0,
  LineShapeD2X1,
  LineShapeD2X2,
  LineShapeD2X3,

  LineShapeFVCX0,
  LineShapeFVCX1,
  LineShapeFVCX2,
  LineShapeFVCX3,

  LineShapeETAX0,
  LineShapeETAX1,
  LineShapeETAX2,
  LineShapeETAX3,

  LineShapeYX0,
  LineShapeYX1,
  LineShapeYX2,
  LineShapeYX3,

  LineShapeGX0,
  LineShapeGX1,
  LineShapeGX2,
  LineShapeGX3,

  LineShapeDVX0,
  LineShapeDVX1,
  LineShapeDVX2,
  LineShapeDVX3,

  NLTE,

  NotPropagationMatrixType
};


/** Deals with internal derivatives, Jacobian definition, and OEM calculations */
class RetrievalQuantity {
 public:
  /** Default constructor. Needed by make_array. */
  RetrievalQuantity()
      : mmaintag(),
        msubtag(),
        msubsubtag(),
        mmode(),
        manalytical(-1),
        mperturbation(0.),
        mgrids(),
        mquantumidentifier(),
        mproptype(JacPropMatType::NotPropagationMatrixType),
        mintegration_flag(false) { /* Nothing to do here. */
  }

  /** Constructor that sets the values
   * 
   * @param[in] maintag The main derivative
   * @param[in] subtag The sub-derivative
   * @param[in] subsubtag The sub-sub-derivative
   * @param[in] mode The mode of the derivative
   * @param[in] analytical Boolean analytical tag
   * @param[in] perturbation The size of the perturbation required
   * @param[in] grids The retrieval grid
   */
  RetrievalQuantity(const String& maintag,
                    const String& subtag,
                    const String& subsubtag,
                    const String& mode,
                    const Index& analytical,
                    const Numeric& perturbation,
                    const ArrayOfVector& grids)
      : mmaintag(maintag),
        msubtag(subtag),
        msubsubtag(subsubtag),
        mmode(mode),
        manalytical(analytical),
        mperturbation(perturbation),
        mgrids(grids),
        mquantumidentifier(),
        mproptype(JacPropMatType::NotPropagationMatrixType),
        mintegration_flag(false) {
    // With Matpack, initialization of mgrids from grids should work correctly.
  }

  /** Returns the main tag
   *
   * @return A representation of the main tag
   */
  const String& MainTag() const { return mmaintag; }
  
  /** Sets the main tag
   *
   * @param[in] mt A main tag
   */
  void MainTag(const String& mt) { mmaintag = mt; }
  
  /** Returns the sub-tag
   * 
   * Subtag. Eg. for gas species: O3, ClO
   * 
   * @return A representation of the sub-tag
   */
  const String& Subtag() const { return msubtag; }
  
  /** Sets the sub-tag
   * 
   * @param[in] st A sub-tag
   */
  void Subtag(const String& st) { msubtag = st; }
  
  /** Returns the sub-sub-tag
   * 
   * SubSubtag. Eg. for scat species fields: mass_density, mass_flux, ...
   * 
   * @return A representation of the sub-sub-tag
   */
  const String& SubSubtag() const { return msubsubtag; }
  
  /** Sets the sub-sub-tag
   * 
   * @param[in] sst A sub-sub-tag
   */
  void SubSubtag(const String& sst) { msubsubtag = sst; }
  
  /** Returns the mode
   * 
   * Calculation mode. Eg. "abs", "rel", "vmr", "nd", "From propagation matrix". 
   * Note that the latter of these only supports "vmr" for abs species.
   * 
   * @return A representation of the mode
   */
  const String& Mode() const { return mmode; }
  
  /** Sets the mode
   * 
   * @param[in] m A mode
   */
  void Mode(const String& m) { mmode = m; }
  
  /** Returns the analytical tag
   * 
   * Boolean to make analytical calculations (if possible).
   * 
   * @return A representation of analytical tag
   */
  const Index& Analytical() const { return manalytical; }
  
  /** Sets the analytical tag
   * 
   * @param[in] m An analytical tag
   */
  void Analytical(const Index& m) { manalytical = m; }
  
  /** Returns the size of perturbation
   * 
   * Size of perturbation used for perturbation calculations
   * 
   * @return The size of perturbation
   */
  const Numeric& Perturbation() const { return mperturbation; }
  
  /** Sets the size of perturbation
   * 
   * @param[in] p The size of perturbation
   */
  void Perturbation(const Numeric& p) { mperturbation = p; }
  
  /** Returns the grids of the retrieval
   * 
   * Grids. Definition grids for the jacobian, eg. p, lat and lon.
   * 
   * @return The grids of the retrieval
   */
  const ArrayOfVector& Grids() const { return mgrids; }
  
  /** Sets the grids of the retrieval
   * 
   * @param[in] g The grids of the retrieval
   */
  void Grids(const ArrayOfVector& g) { mgrids = g; }
  
  /** Returns the propagation matrix derivative type
   * 
   * @return The propagation matrix derivative type
   */
  JacPropMatType PropMatType() const { return mproptype; }
  
  /** Sets the propagation matrix derivative type
   * 
   * @param[in] t The propagation matrix derivative type
   */
  void PropType(const JacPropMatType t) { mproptype = t; }
  
  /** Checks if this represents the propagation matrix derivative type
   * 
   * @param[in] t A propagation matrix derivative type
   * @return true if the input propagation matrix derivative type matches PropMatType()
   * @return false otherwise
   */
  bool operator==(const JacPropMatType t) const { return t == mproptype; }
  
  /** Returns "not operator==(t)"
   * 
   * @param[in] t A propagation matrix derivative type
   * @return false if the input propagation matrix derivative type matches PropMatType()
   * @return true otherwise
   */
  bool operator!=(const JacPropMatType t) const { return not operator==(t); }

  /** Number of elements in the grids 
   * 
   * The multiplicative accumulation of grid elements
   * 
   * @return The number of elements if each grid represents a dimension
   */
  Index nelem() const {
    Index i = 1;
    for (Index j = 0; j < mgrids.nelem(); ++j) {
      i *= mgrids[j].nelem();
    }
    return i;
  }
  
  /** Returns the identity of this Jacobian
   * 
   * QuantumIdentifier as necessary for matching line specific parameters to Jacobian grid
   * 
   * @return The identity of this Jacobian
   */
  const QuantumIdentifier& QuantumIdentity() const {
    return mquantumidentifier;
  }
  
  /** Sets the identity of this Jacobian
   * 
   * @param[in] qi The identity of this Jacobian
   */
  void QuantumIdentity(const QuantumIdentifier& qi) { mquantumidentifier = qi; }

  /** Do integration?
   * 
   * @return true if the values should be integrated over all the grids
   * @return false otherwise
   */
  bool Integration() const { return mintegration_flag; }
  
  /** Sets the integration flag to true */
  void IntegrationOn() { mintegration_flag = true; }
  
  /** Sets the integration flag to false */
  void IntegrationOff() { mintegration_flag = false; }

  /** Transformation
   * 
   * FIXMEDOC@Simon The transformations are yours to fix and document
   * FIXMEDOC@Patrick The transformations are yours to fix and document
   */
  void SetTransformationFunc(const String& s) { transformation_func = s; }
  void SetTFuncParameters(const Vector& p) { tfunc_parameters = p; }
  void SetTransformationMatrix(const Matrix& A) { transformation_matrix = A; }
  void SetOffsetVector(const Vector& b) { offset_vector = b; }
  bool HasAffine() const { return !transformation_matrix.empty(); }
  const String& TransformationFunc() const { return transformation_func; }
  const Vector& TFuncParameters() const { return tfunc_parameters; }
  const Matrix& TransformationMatrix() const { return transformation_matrix; }
  const Vector& OffsetVector() const { return offset_vector; }

  /** Checks that all the internal variables of *this match with those of the input
   * 
   * @param[in] a Another retrieval quantity object
   * @return true if the input performs the exact same Jacobian calculations
   * @return false otherwise
   */
  bool HasSameInternalsAs(const RetrievalQuantity& a) const {
    return a.mmaintag == mmaintag and a.msubtag == msubtag and
           a.msubsubtag == msubsubtag and a.mmode == mmode and
           a.manalytical == manalytical and
           a.mquantumidentifier == mquantumidentifier and
           a.mproptype == mproptype;
  }
  
  String& MainTag() {return mmaintag;}
  String& SubTag() {return msubtag;}
  String& SubSubTag() {return msubsubtag;}
  String& Mode() {return mmode;}
  Index& Analytical() {return manalytical;}
  Numeric& Perturbation() {return mperturbation;}
  ArrayOfVector& Grids() {return mgrids;}
  QuantumIdentifier& QuantumIdentity() {return mquantumidentifier;}
  JacPropMatType Proptype() const {return mproptype;}
  Index Proptype(JacPropMatType x) {if (validProptype(x)) {mproptype = x; return EXIT_SUCCESS;} else return EXIT_FAILURE;}
  static bool validProptype(JacPropMatType x) noexcept {return Index(x) <= Index(JacPropMatType::NotPropagationMatrixType) and Index(x) >= 0;}
  void Integration(bool x) {mintegration_flag = x;}
  String& TransformationFunc() {return transformation_func;}
  Vector& TFuncParameters() {return tfunc_parameters;}
  Matrix& Transformation() {return transformation_matrix;}
  Vector& Offset() {return offset_vector;}

 private:
  String mmaintag;
  String msubtag;
  String msubsubtag;
  String mmode;
  Index manalytical;
  Numeric mperturbation;
  ArrayOfVector mgrids;
  QuantumIdentifier mquantumidentifier;
  JacPropMatType mproptype;
  bool mintegration_flag;

  String transformation_func;
  Vector tfunc_parameters;

  Matrix transformation_matrix;
  Vector offset_vector;
};

/** Output operator for RetrievalQuantity */
ostream& operator<<(ostream& os, const RetrievalQuantity& ot);

typedef Array<RetrievalQuantity> ArrayOfRetrievalQuantity;

// A macro to loop analytical jacobian quantities
#define FOR_ANALYTICAL_JACOBIANS_DO(what_to_do)                \
  for (Index iq = 0; iq < jacobian_quantities.nelem(); iq++) { \
    if (jacobian_quantities[iq].Analytical()) {                \
      what_to_do                                               \
    }                                                          \
  }
// A macro to loop analytical jacobian quantities
#define FOR_ANALYTICAL_JACOBIANS_DO2(what_to_do)                \
  for (Index iq = 0; iq < jacobian_quantities.nelem(); iq++) {  \
    if (jacobian_quantities[iq].Analytical() ||                 \
        jacobian_quantities[iq].MainTag() == SURFACE_MAINTAG) { \
      what_to_do                                                \
    }                                                           \
  }

//======================================================================
//             Index ranges and transformation functions
//======================================================================

/** Determines the index range inside x and the Jacobian for each retrieval quantity

    The ranges are given as an ArrayOfArrayOfIndex, where outermost dimension
    corresponds to retrieval quantity. The inner dimension has throughout size
    2, where element 0 is the first index and element 1 is the last index of
    the range.

    @param[out]   jis            Indices, as described above
    @param[in]    any_affine     True if at least one  quantity has affine
                                 transformation. 
    @param[in]   jqs             The WSV jacobian_quantities
    @param[in]   before_affine   Set to true to get indices without any affine
                                 transformation. Default is false.

    @author Simon Pfreundschuh and Patrick Eriksson 
    @date   2017-12-30
 */
void jac_ranges_indices(ArrayOfArrayOfIndex& jis,
                        bool& any_affine,
                        const ArrayOfRetrievalQuantity& jqs,
                        const bool& before_affine = false);

/** Applies both functional and affine transformations.
 *
 *  @param[in] jacobian As the WSV jacobian
 *  @param[in] jqs As the WSV jacobian_quantities
 *
 *  @author Simon Pfreundschuh and Patrick Eriksson 
 *  @date   2017-12-30
 */
void transform_jacobian(Matrix& jacobian,
                        const Vector x,
                        const ArrayOfRetrievalQuantity& jqs);
 
/** Handles transformations of the state vector
 *
 * Applies both functional and affine transformations.
 *
 *  @param[in,out] x    As the WSV x
 *  @param[in]     jqs  As the WSV jacobian_quantities
 *
 *  @author Simon Pfreundschuh and Patrick Eriksson 
 *  @date   2017-12-30
 */
void transform_x(Vector& x, const ArrayOfRetrievalQuantity& jqs);
 
/** Handles back-transformations of the state vector
 *
 * Applies both functional and affine transformations.
 *
 *  @param[in] x As the WSV x
 *  @param[in] jqs As the WSV jacobian_quantities
 *
 *  @author Simon Pfreundschuh and Patrick Eriksson 
 *  @date   2017-12-30
 */
void transform_x_back(Vector& x_t,
                      const ArrayOfRetrievalQuantity& jqs,
                      bool revert_functional_transforms = true);

//======================================================================
//             Functions related to calculation of Jacobian
//======================================================================

/** Check that the retrieval grids are defined for each atmosphere dim

   Use this version for atmospheric fields.

   This function checks for the given atmosphere dimension that 
     I)  the retrieval grids are defined 
     II) and that they are covered by the corresponding atmospheric grid. 
   If not the return is false and an output string is created to print 
   the error to the user. If the grids are ok they are stored in an array 
   and true is  returned.
   
   @param[out] grids        The array of retrieval grids.
   @param[in,out] os        The output string stream.
   @param[in] p_grid        The atmospheric pressure grid
   @param[in] lat_grid      The atmospheric latitude grid
   @param[in] lon_grid      The atmospheric longitude grid
   @param[in] p_retr        The pressure retrieval grid.
   @param[in] lat_retr      The latitude retrieval grid.
   @param[in] lon_retr      The longitude retrieval grid.
   @param[in] p_retr_name   The control file name used for the pressure retrieval grid.
   @param[in] lat_retr_name The control file name for the latitude retrieval grid.
   @param[in] lon_retr_name The control file name for the longitude retrieval grid.
   @param[in] dim           The atmosphere dimension

   @return              Boolean for check.
   
   @author Mattias Ekstrom
   @date   2005-05-11
 */
bool check_retrieval_grids(ArrayOfVector& grids,
                           ostringstream& os,
                           const Vector& p_grid,
                           const Vector& lat_grid,
                           const Vector& lon_grid,
                           const Vector& p_retr,
                           const Vector& lat_retr,
                           const Vector& lon_retr,
                           const String& p_retr_name,
                           const String& lat_retr_name,
                           const String& lon_retr_name,
                           const Index& dim);


/** Check that the retrieval grids are defined for each atmosphere dim

   Use this version for surface variables

   This function checks for the given atmosphere dimension that 
     I)  the retrieval grids are defined 
     II) and that they are covered by the corresponding atmospheric grid. 
   If not the return is false and an output string is created to print 
   the error to the user. If the grids are ok they are stored in an array 
   and true is  returned.
   
   @param[out] grids        The array of retrieval grids.
   @param[in,out] os        The output string stream.
   @param[in] lat_grid      The atmospheric latitude grid
   @param[in] lon_grid      The atmospheric longitude grid
   @param[in] lat_retr      The latitude retrieval grid.
   @param[in] lon_retr      The longitude retrieval grid.
   @param[in] lat_retr_name The control file name for the latitude retrieval grid.
   @param[in] lon_retr_name The control file name for the longitude retrieval grid.
   @param[in] dim           The atmosphere dimension
   @return              Boolean for check.
   
   @author Mattias Ekstrom
   @date   2005-05-11
 */
bool check_retrieval_grids(ArrayOfVector& grids,
                           ostringstream& os,
                           const Vector& lat_grid,
                           const Vector& lon_grid,
                           const Vector& lat_retr,
                           const Vector& lon_retr,
                           const String& lat_retr_name,
                           const String& lon_retr_name,
                           const Index& dim);

/** Maps jacobian data for points along the propagation path, to
    jacobian retrieval grid data.

    @param[out]  diy_dx              One element of the WSV *diy_dx*.
    @param[in]   jacobian_quantity   One element of of the WSV *jacobian_quantities*.
    @param[in]   diy_dpath           Jacobians along the propagation path.
    @param[in]   atmosphere_dim      As the WSV.
    @param[in]   ppath               As the WSV.
    @param[in]   ppath_p             The pressure at each ppath point.

    @author Patrick Eriksson 
    @date   2009-10-08
 */
void diy_from_path_to_rgrids(Tensor3View diy_dx,
                             const RetrievalQuantity& jacobian_quantity,
                             ConstTensor3View diy_dpath,
                             const Index& atmosphere_dim,
                             const Ppath& ppath,
                             ConstVectorView ppath_p);

/** diy_from_pos_to_rgrids

    Maps jacobian data for a surface position, to jacobian retrieval grid data.

    @param[out]   diy_dx             One element of the WSV *diy_dx*.
    @param[in]   jacobian_quantity   One element of of the WSV *jacobian_quantities*.
    @param[in]   diy_dpos            Jacobian for the position itself.
    @param[in]   atmosphere_dim      As the WSV.
    @param[in]   rtp_pos             As the WSV.

    @author Patrick Eriksson 
    @date   2018-04-10
 */
void diy_from_pos_to_rgrids(Tensor3View diy_dx,
                            const RetrievalQuantity& jacobian_quantity,
                            ConstMatrixView diy_dpos,
                            const Index& atmosphere_dim,
                            ConstVectorView rtp_pos);
 
/** Help function for analytical jacobian calculations

    The function determines which terms in jacobian_quantities that are 
    analytical absorption species and temperature jacobians. 

    *abs_species_i* and *is_t* shall be sized to have the same length
    as *jacobian_quantities*. For analytical absorption species
    jacobians, *abs_species_i* is set to the matching index in
    *abs_species*. Otherwise, to -1. For analytical temperature
    jacobians, *is_t* is set to 1. Otherwise to 0.

    @param[out]   abs_species_i         Matching index in abs_species 
    @param[out]   scat_species_i        Matching index among scattering species 
    @param[out]   is_t                  Flag for: Is a temperature jacobian?
    @param[in]    jacobian_quantities   As the WSV.
    @param[in]    abs_species           As the WSV.
    @param[in]    cloudbox_on           As the WSV.
    @param[in]    scat_species          As the WSV.

    @author Patrick Eriksson 
    @date   2009-10-07
 */
void get_pointers_for_analytical_jacobians(
    ArrayOfIndex& abs_species_i,
    ArrayOfIndex& scat_species_i,
    ArrayOfIndex& is_t,
    ArrayOfIndex& wind_i,
    ArrayOfIndex& magfield_i,
    const ArrayOfRetrievalQuantity& jacobian_quantities,
    const ArrayOfArrayOfSpeciesTag& abs_species,
    const Index& cloudbox_on,
    const ArrayOfString& scat_species);

/** Adopts grid positions to extrapolation used for jacobians

  The standard interpolation scheme applies a linear extrapolation, while for
  the jacobians the extrapolation can be seen as a "closest" interpolation.
  That is, for points outisde the covered grid, the value at closest end point
  is taken. And here extrapolation to +-Inf is allowed.

  This function modifies grid positions to jacobaina extrapolation approach.
  For efficiency, the input grid positions are not asserted at all, and
  "extrapolation points" are identified simply  by a fd outside [0,1].

  @param[in/out] gp   Array of grid positions.

  @author Patrick Eriksson 
  @date   2015-09-10
 */
void jacobian_type_extrapol(ArrayOfGridPos& gp);

/** Calculates polynomial basis functions

   The basis function is b(x) = 1 for poly_coeff = 0. For higher
   coefficients, x^poly_coeff - m, where first the range covered by
   *x* is normalised to [-1,1] and m is selected in such way that
   sum(b) = 0.
   
   @param[out] b            Calculated basis function.
   @param[in] x            The grid over which the fit shall be performed.
   @param[in] poly_coeff   Polynomial coefficient.
   
   @author Patrick Eriksson
   @date   2008-11-07
 */
void polynomial_basis_func(Vector& b, const Vector& x, const Index& poly_coeff);

/** Calculate baseline fit
 *
 * Computes the baseline fit from a given state vector.
 *
 * Given a retrieval quantitiy which is either a polynomial or a sine baseline fit
 * this function computes the baseline offset in y_baseline.
 *
 *  @param[out] y_baseline The computed baseline offset. Computed baseline offset are
 *                         accumulated into this vector, so it must be
 *                         initialized externally!
 *  @param[in] x State vector consisten with given retrieval quantity
 *  @param[in] mblock_index The index of the measurement block.
 *  @param[in] sensor_response Must be consistent with size of y_baseline.
 *  @param[in] sensor_response_pol_grid Must be consistent with size of y_baseline.
 *  @param[in] sensor_response_f_grid Must be consistent with size of y_baseline.
 *  @param[in] sensor_dlos_grid Must be consistent with size of y_baseline.
 *  @param[in] rq The poly- or sinefit retrieval quantity
 *  @param[in] rq_index The index of the retrieval quantity
 *  @param[in] jacobian_indices
 */
void calcBaselineFit(Vector& y_baseline,
                     const Vector& x,
                     const Index& mblock_index,
                     const Sparse& sensor_response,
                     const ArrayOfIndex& sensor_response_pol_grid,
                     const Vector& sensor_response_f_grid,
                     const Matrix& sensor_response_dlos_grid,
                     const RetrievalQuantity& rq,
                     const Index rq_index,
                     const ArrayOfArrayOfIndex& jacobian_indices);

/** Scale factor for conversion between gas species units.

    The function finds the factor with which the total absorption of a
    gas species shall be multiplicated to match the selected
    (jacobian) unit. 

    @param[out]   x     Scale factor
    @param[in]   unit   Unit selected.
    @param[in]   vmr    VMR value.
    @param[in]   p      Pressure
    @param[in]   t      Temperature.

    @author Patrick Eriksson 
    @date   2009-10-08
 */
void vmrunitscf(Numeric& x,
                const String& unit,
                const Numeric& vmr,
                const Numeric& p,
                const Numeric& t);

/** Scale factor for conversion of derivatives with respect to VMR.

    The function finds the factor with which a partial derivative with respect
    to gas species VMR shall be multiplicated to match the selected (jacobian)
    unit. The function was implemented for scaling of *dpropmat_clearsky_dx*
    but it could also be of use in other contexts.

    @param[out]   x      Scale factor
    @param[in]   unit   Unit selected.
    @param[in]   vmr    VMR value.
    @param[in]   p      Pressure
    @param[in]   t      Temperature.

    @author Patrick Eriksson 
    @date   2015-12-11
 */
void dxdvmrscf(Numeric& x,
               const String& unit,
               const Numeric& vmr,
               const Numeric& p,
               const Numeric& t);

/** Enum for knowing what Jacobian scheme is in-play in the m_rte.cc methods */
enum class JacobianType : Index {
  None = 0,  // Setting to nil means that (bool)0 and (bool)N still works.
  Temperature,
  WindFieldU,
  WindFieldV,
  WindFieldW,
  AbsWind,
  MagFieldU,
  MagFieldV,
  MagFieldW,
  AbsMag,
  Other
};

/*! FIXMEDOC: Remove this function when iyHybrid2 has become new iyHybrid
 *   The function helps to calculate the partial derivative of iy with respect
 *   to one input at one pressure.  The formalism here assumes that the radiation
 *   terms are averaged rather than the absorption parameters, thus this can be 
 *   solved per layer rather than for two layers at a time.  Still, the absorption
 *   parameters for the transmission needs to be considered by the two layer derivatives
 * 
 *   FIXME:  Add HSE support
 * 
 * @param[in,out] diy1 Derivative if iy for level 1
 * @param[in,out] diy2 Derivative if iy for level 2
 * @param[in] ImT Identity matrix minus the transmission matrix
 * @param[in] cumulative_transmission Accumulative transmission from level to sensor
 * @param[in] dT1 Transmission matrix derivative for level 1
 * @param[in] dT2 Transmission matrix derivative for level 2
 * @param[in] iYmJ iy minus the source
 * @param[in] dJ1 Source vector derivative for level 1
 * @param[in] dJ2 Source vector derivative for level 2
 * @param[in] stokes_dim As WSV
 * @param[in] transmission_only Bool for not doing emission calculations
 * 
 * @author Richard Larsson
 * @date   2017-09-20
 */
void get_diydx(VectorView diy1,
               VectorView diy2,
               ConstMatrixView ImT,
               ConstMatrixView cumulative_transmission,
               ConstMatrixView dT1,
               ConstMatrixView dT2,
               ConstVectorView iYmJ,
               ConstVectorView dJ1,
               ConstVectorView dJ2,
               const Index stokes_dim,
               const bool transmission_only = false);

//======================================================================
//             Propmat partials descriptions
//======================================================================

/** Returns a list of positions for the derivatives in Propagation Matrix calculations
 * 
 * @param[in] js As jacobian_quantities WSV
 * @return List of positions
 */
ArrayOfIndex equivalent_propmattype_indexes(const ArrayOfRetrievalQuantity& js);

/** Returns a list of positions for the derivatives in Propagation Matrix calculations
 * 
 * @param[in] js As jacobian_quantities WSV
 * @param[in] i A position in jacobian_quantities
 * @return List of position
 */
Index equivalent_propmattype_index(const ArrayOfRetrievalQuantity& js,
                                  const Index i) noexcept;

/** Returns the temperature perturbation if it exists
 * 
 * @param[in] js As jacobian_quantities WSV
 * @return Temperature perturbation
 */
Numeric temperature_perturbation(const ArrayOfRetrievalQuantity& js) noexcept;

/** Returns the frequency perturbation if it exists
 * 
 * @param[in] js As jacobian_quantities WSV
 * @return Frequency perturbation
 */
Numeric frequency_perturbation(const ArrayOfRetrievalQuantity& js) noexcept;

/** Returns the magnetic field perturbation if it exists
 * 
 * @param[in] js As jacobian_quantities WSV
 * @return Magnetic field perturbation
 */
Numeric magnetic_field_perturbation(
    const ArrayOfRetrievalQuantity& js) noexcept;

/** Returns a string of the retrieval quantity propagation matrix type
 * 
 * Only use for debugging purpose
 * 
 * @param[in] rq A retrieval quantity
 * @return Text representation
 */
String propmattype_string(const RetrievalQuantity& rq);

//======================================================================
//             Propmat partials boolean functions
//======================================================================

/** Returns if the Retrieval quantity is a wind parameter
 * 
 * @param[in] t A retrieval quantity
 * @return true if it is
 * @return false if it is not
 */
bool is_wind_parameter(const RetrievalQuantity& t) noexcept;

/** Returns if the Retrieval quantity is a frequency parameter in propagation matrix calculations
 * 
 * @param[in] t A retrieval quantity
 * @return true if it is
 * @return false if it is not
 */
bool is_frequency_parameter(const RetrievalQuantity& t) noexcept;

/** Returns if the Retrieval quantity is a derived magnetic parameter
 * 
 * @param[in] t A retrieval quantity
 * @return true if it is
 * @return false if it is not
 */
bool is_derived_magnetic_parameter(const RetrievalQuantity& t) noexcept;

/** Returns if the Retrieval quantity is a magnetic parameter
 * 
 * @param[in] t A retrieval quantity
 * @return true if it is
 * @return false if it is not
 */
bool is_magnetic_parameter(const RetrievalQuantity& t) noexcept;

/** Returns if the Retrieval quantity is a NLTE parameter
 * 
 * @param[in] t A retrieval quantity
 * @return true if it is
 * @return false if it is not
 */
bool is_nlte_parameter(const RetrievalQuantity& t) noexcept;

/** Returns if the Retrieval quantity is a G0 derivative
 * 
 * @param[in] t A retrieval quantity
 * @return true if it is
 * @return false if it is not
 */
bool is_pressure_broadening_G0(const RetrievalQuantity& t) noexcept;

/** Returns if the Retrieval quantity is a D0 derivative
 * 
 * See LineShape::Model for details on parameter
 * 
 * @param[in] t A retrieval quantity
 * @return true if it is
 * @return false if it is not
 */
bool is_pressure_broadening_D0(const RetrievalQuantity& t) noexcept;

/** Returns if the Retrieval quantity is a G0 derivative
 * 
 * @param[in] t A retrieval quantity
 * @return true if it is
 * @return false if it is not
 */
bool is_pressure_broadening_G2(const RetrievalQuantity& t) noexcept;

/** Returns if the Retrieval quantity is a D2 derivative
 * 
 * See LineShape::Model for details on parameter
 * 
 * @param[in] t A retrieval quantity
 * @return true if it is
 * @return false if it is not
 */
bool is_pressure_broadening_D2(const RetrievalQuantity& t) noexcept;

/** Returns if the Retrieval quantity is a FVC derivative
 * 
 * See LineShape::Model for details on parameter
 * 
 * @param[in] t A retrieval quantity
 * @return true if it is
 * @return false if it is not
 */
bool is_pressure_broadening_FVC(const RetrievalQuantity& t) noexcept;

/** Returns if the Retrieval quantity is a ETA derivative
 * 
 * See LineShape::Model for details on parameter
 * 
 * @param[in] t A retrieval quantity
 * @return true if it is
 * @return false if it is not
 */
bool is_pressure_broadening_ETA(const RetrievalQuantity& t) noexcept;

/** Returns if the Retrieval quantity is a Y derivative
 * 
 * See LineShape::Model for details on parameter
 * 
 * @param[in] t A retrieval quantity
 * @return true if it is
 * @return false if it is not
 */
bool is_pressure_broadening_Y(const RetrievalQuantity& t) noexcept;

/** Returns if the Retrieval quantity is a G derivative
 * 
 * See LineShape::Model for details on parameter
 * 
 * @param[in] t A retrieval quantity
 * @return true if it is
 * @return false if it is not
 */
bool is_pressure_broadening_G(const RetrievalQuantity& t) noexcept;

/** Returns if the Retrieval quantity is a DV derivative
 * 
 * See LineShape::Model for details on parameter
 * 
 * @param[in] t A retrieval quantity
 * @return true if it is
 * @return false if it is not
 */
bool is_pressure_broadening_DV(const RetrievalQuantity& t) noexcept;

/** Returns if the Retrieval quantity is a X0 derivative
 * 
 * See LineShape::Model for details on parameter
 * 
 * @param[in] t A retrieval quantity
 * @return true if it is
 * @return false if it is not
 */
bool is_lineshape_parameter_X0(const RetrievalQuantity& t) noexcept;

/** Returns if the Retrieval quantity is a X1 derivative
 * 
 * See LineShape::Model for details on parameter
 * 
 * @param[in] t A retrieval quantity
 * @return true if it is
 * @return false if it is not
 */
bool is_lineshape_parameter_X1(const RetrievalQuantity& t) noexcept;

/** Returns if the Retrieval quantity is a X2 derivative
 * 
 * See LineShape::Model for details on parameter
 * 
 * @param[in] t A retrieval quantity
 * @return true if it is
 * @return false if it is not
 */
bool is_lineshape_parameter_X2(const RetrievalQuantity& t) noexcept;

/** Returns if the Retrieval quantity is a G0, D0, G2, D2, ETA, FVC derivative
 * 
 * See LineShape::Model for details on parameter
 * 
 * @param[in] t A retrieval quantity
 * @return true if it is
 * @return false if it is not
 */
bool is_lineshape_parameter_bar_linemixing(const RetrievalQuantity& t) noexcept;

/** Returns if the Retrieval quantity is a G0, D0, G2, D2, ETA, FVC, Y, G, DV derivative
 * 
 * See LineShape::Model for details on parameter
 * 
 * @param[in] t A retrieval quantity
 * @return true if it is
 * @return false if it is not
 */
bool is_lineshape_parameter(const RetrievalQuantity& t) noexcept;

/** Returns if the Retrieval quantity is related to the absorption line
 * 
 * @param[in] t A retrieval quantity
 * @return true if it is
 * @return false if it is not
 */
bool is_line_parameter(const RetrievalQuantity& t) noexcept;

/** Returns if the array supports CIA derivatives
 * 
 * @param[in] js As jacobian_quantities WSV
 * @return true if it does
 * @return false if it does not
 */
bool supports_CIA(const ArrayOfRetrievalQuantity& js);

/** Returns if the array supports HITRAN cross-section derivatives
 * 
 * @param[in] js As jacobian_quantities WSV
 * @return true if it does
 * @return false if it does not
 */
bool supports_hitran_xsec(const ArrayOfRetrievalQuantity& js);


/** Returns if the array supports continuum derivatives
 * 
 * @param[in] js As jacobian_quantities WSV
 * @return true if it does
 * @return false if it does not
 */
bool supports_continuum(const ArrayOfRetrievalQuantity& js);

/** Returns if the array supports line-by-line derivatives without requiring the phase
 * 
 * FIXME: This should be removed when updating away from the old XSEC routine
 * 
 * @param[in] js As jacobian_quantities WSV
 * @return true if it does
 * @return false if it does not
 */
bool supports_LBL_without_phase(const ArrayOfRetrievalQuantity& js);

/** Returns if the array supports relaxation matrix derivatives
 * 
 * @param[in] js As jacobian_quantities WSV
 * @return true if it does
 * @return false if it does not
 */
bool supports_relaxation_matrix(const ArrayOfRetrievalQuantity& js);

/** Returns if the array supports lookup table derivatives
 * 
 * @param[in] js As jacobian_quantities WSV
 * @return true if it does
 * @return false if it does not
 */
bool supports_lookup(const ArrayOfRetrievalQuantity& js);

/** Returns if the array supports Zeeman derivatives
 * 
 * @param[in] js As jacobian_quantities WSV
 * @return true if it does
 * @return false if it does not
 */
bool supports_zeeman(const ArrayOfRetrievalQuantity& js);

/** Returns if the array supports Faraday derivatives
 * 
 * @param[in] js As jacobian_quantities WSV
 * @return true if it does
 * @return false if it does not
 */
bool supports_faraday(const ArrayOfRetrievalQuantity& js);

/** Returns if the array supports particulate derivatives
 * 
 * @param[in] js As jacobian_quantities WSV
 * @return true if it does
 * @return false if it does not
 */
bool supports_particles(const ArrayOfRetrievalQuantity& js);

/** Returns if the array supports propagation matrix derivatives
 * 
 * @param[in] js As jacobian_quantities WSV
 * @return true if it does
 * @return false if it does not
 */
bool supports_propmat_clearsky(const ArrayOfRetrievalQuantity& js);

/** Returns if the Retrieval quantity is VMR derivative for all the species in the species tags
 * 
 * Very slow compared to index input
 * 
 * @param[in] rq A retrieval quantity
 * @param[in] ast A list of species tags
 * @return true if all species in the tags list matches the species in the retrieval quantity
 * @return false otherwise
 */
bool species_match(const RetrievalQuantity& rq, const ArrayOfSpeciesTag& ast);

/** Returns if the Retrieval quantity is VMR derivative for all the species in the species tags
 * 
 * @param[in] rq A retrieval quantity
 * @param[in] species An index-mapped species
 * @return true the species match the species in the retrieval quantity
 * @return false otherwise
 */
bool species_match(const RetrievalQuantity& rq, const Index species);

/** Returns if the Retrieval quantity is VMR derivative for all the species in the species tags
 * 
 * @param[in] rq A retrieval quantity
 * @param[in] species An index-mapped species
 * @param[in] iso An index-mapped isotopologue
 * @return true the species and isotopologue match the species and isotopologue in the retrieval quantity
 * @return false otherwise
 */
bool species_iso_match(const RetrievalQuantity& rq,
                       const Index species,
                       const Index iso);

/** Returns if the array wants the temperature derivative
 * 
 * @param[in] js As jacobian_quantities WSV
 * @return true if it does
 * @return false if it does not
 */
bool do_temperature_jacobian(const ArrayOfRetrievalQuantity& js) noexcept;

/** Deals with whether or not we should do a VMR derivative */
struct jacobianVMRcheck {
  bool test;
  const QuantumIdentifier& qid;
};

/** Returns the required info for VMR Jacobian
 * 
 * @param[in] js As jacobian_quantities WSV
 * @param[in] line_qid A line identifier
 * @return true and retrieval quantity quantum identity pointer if available
 * @return false and line_qid pointer if not available
 */
jacobianVMRcheck do_vmr_jacobian(const ArrayOfRetrievalQuantity& js,
                                 const QuantumIdentifier& line_qid) noexcept;

/** Returns if the array wants a line center derivative
 * 
 * @param[in] js As jacobian_quantities WSV
 * @return true if it does
 * @return false if it does not
 */
bool do_line_center_jacobian(const ArrayOfRetrievalQuantity& js) noexcept;

/** Returns if the array wants a frequency derivative
 * 
 * @param[in] js As jacobian_quantities WSV
 * @return true if it does
 * @return false if it does not
 */
bool do_frequency_jacobian(const ArrayOfRetrievalQuantity& js) noexcept;

/** Returns if the array wants a magnetic derivative
 * 
 * @param[in] js As jacobian_quantities WSV
 * @return true if it does
 * @return false if it does not
 */
bool do_magnetic_jacobian(const ArrayOfRetrievalQuantity& js) noexcept;

#endif  // jacobian_h
