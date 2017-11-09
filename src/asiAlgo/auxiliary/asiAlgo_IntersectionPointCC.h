//-----------------------------------------------------------------------------
// Created on: 06 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiAlgo_IntersectionPointCC_h
#define asiAlgo_IntersectionPointCC_h

// Analysis Situs includes
#include <asiAlgo.h>

// Geometry includes
#include <asiAlgo_IntersectionCurveSS.h>

// OCCT includes
#include <gp_Pnt.hxx>

DEFINE_STANDARD_HANDLE(asiAlgo_IntersectionPointCC, Standard_Transient)

//! Point representing 0-dimensional intersection between two curves in 3D. The
//! first intersected curve corresponds to the parameters with index 1, e.g. W1.
class asiAlgo_IntersectionPointCC : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_IntersectionPointCC, Standard_Transient)

public:

  //! Default constructor.
  asiAlgo_IntersectionPointCC()
  : Uncertainty (0.0),
    W1          (0.0),
    W2          (0.0)
  {}

  //! Constructor.
  //! \param _P [in] three-dimensional point.
  inline asiAlgo_IntersectionPointCC(const gp_Pnt& _P)
  : P           (_P),
    W1          (0.0),
    W2          (0.0),
    Uncertainty (0.0)
  {}

  //! Constructor.
  //! \param _P  [in] three-dimensional point.
  //! \param _W1 [in] intersection parameter on the first curve.
  //! \param _W2 [in] intersection parameter on the second curve.
  asiAlgo_IntersectionPointCC(const gp_Pnt& _P,
                              const double  _W1,
                              const double  _W2)
  : P           (_P),
    Uncertainty (0.0),
    W1          (_W1),
    W2          (_W2)
  {}

  //! Constructor.
  //! \param _P  [in] three-dimensional point.
  //! \param _W1 [in] intersection parameter on the first curve.
  //! \param _W2 [in] intersection parameter on the second curve.
  //! \param _T1 [in] tangency of the first curve at the intersection point.
  //! \param _T2 [in] tangency of the second curve at the intersection point.
  asiAlgo_IntersectionPointCC(const gp_Pnt& _P,
                              const double  _W1,
                              const double  _W2,
                              const gp_Dir& _T1,
                              const gp_Dir& _T2)
  : P           (_P),
    Uncertainty (0.0),
    W1          (_W1),
    W2          (_W2),
    T1          (_T1),
    T2          (_T2)
  {}

  //! Constructor.
  //! \param _P  [in] three-dimensional point.
  //! \param _U  [in] uncertainty.
  //! \param _W1 [in] intersection parameter on the first curve.
  //! \param _W2 [in] intersection parameter on the second curve.
  //! \param _T1 [in] tangency of the first curve at the intersection point.
  //! \param _T2 [in] tangency of the second curve at the intersection point.
  asiAlgo_IntersectionPointCC(const gp_Pnt& _P,
                              const double  _U,
                              const double  _W1,
                              const double  _W2,
                              const gp_Dir& _T1,
                              const gp_Dir& _T2)
  : P           (_P),
    Uncertainty (_U),
    W1          (_W1),
    W2          (_W2),
    T1          (_T1),
    T2          (_T2)
  {}

  //! Constructor.
  //! \param _P   [in] three-dimensional point.
  //! \param _U   [in] uncertainty.
  //! \param _W1  [in] intersection parameter on the first curve.
  //! \param _W2  [in] intersection parameter on the second curve.
  //! \param _T1  [in] tangency of the first curve at the intersection point.
  //! \param _T2  [in] tangency of the second curve at the intersection point.
  //! \param _pC1 [in] first intersected curve.
  //! \param _pC2 [in] second intersected curve.
  asiAlgo_IntersectionPointCC(const gp_Pnt&                             _P,
                              const double                              _U,
                              const double                              _W1,
                              const double                              _W2,
                              const gp_Dir&                             _T1,
                              const gp_Dir&                             _T2,
                              const Handle(asiAlgo_IntersectionCurveSS)& _pC1,
                              const Handle(asiAlgo_IntersectionCurveSS)& _pC2)
  : P           (_P),
    Uncertainty (_U),
    W1          (_W1),
    W2          (_W2),
    T1          (_T1),
    T2          (_T2),
    pC1         (_pC1),
    pC2         (_pC2)
  {}

public:

  //! Comparator.
  //! \param pt [in] point to compare with.
  //! \return true/false.
  bool operator>(const asiAlgo_IntersectionPointCC& pt) const
  {
    return this->W1 > pt.W1;
  }

  //! Equality comparator.
  //! \param pt [in] point to compare with.
  //! \return true/false.
  bool operator==(const asiAlgo_IntersectionPointCC& pt) const
  {
    return this->W1 == pt.W1;
  }

public:

  gp_Pnt P;           //!< Intersection point in 3D.
  double Uncertainty; //!< Uncertainty in 3D point position.
  double W1;          //!< Intersection parameter on the first curve.
  double W2;          //!< Intersection parameter on the second curve.
  gp_Dir T1;          //!< Tangent direction of the first curve at the intersection point.
  gp_Dir T2;          //!< Tangent direction of the second curve at the intersection point.
  //
  Handle(asiAlgo_IntersectionCurveSS) pC1; //!< First intersected curve.
  Handle(asiAlgo_IntersectionCurveSS) pC2; //!< Second intersected curve.

};

//! Collection of intersection points.
typedef NCollection_Sequence<Handle(asiAlgo_IntersectionPointCC)> asiAlgo_IntersectionPointsCC;

#endif
