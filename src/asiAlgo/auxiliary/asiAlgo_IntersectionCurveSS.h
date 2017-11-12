//-----------------------------------------------------------------------------
// Created on: 06 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sergey Slyadnev nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiAlgo_IntersectionCurveSS_h
#define asiAlgo_IntersectionCurveSS_h

// Analysis Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_Sequence.hxx>

DEFINE_STANDARD_HANDLE(asiAlgo_IntersectionCurveSS, Standard_Transient)

//! Curve representing 1-dimensional intersection between two surfaces. The
//! first intersected surface corresponds to the parameters with index 1.
class asiAlgo_IntersectionCurveSS : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_IntersectionCurveSS, Standard_Transient)

public:

  //! Default constructor.
  asiAlgo_IntersectionCurveSS() : Uncertainty(0.0)
  {}

  //! Constructor.
  //! \param _U  [in] uncertainty of the intersection method.
  //! \param _C  [in] intersection result as a parametric curve.
  //! \param _S1 [in] first intersected surface.
  //! \param _S2 [in] second intersected surface.
  asiAlgo_IntersectionCurveSS(const double                _U,
                              const Handle(Geom_Curve)&   _C,
                              const Handle(Geom_Surface)& _S1,
                              const Handle(Geom_Surface)& _S2)
  : Uncertainty (_U),
    C           (_C),
    S1          (_S1),
    S2          (_S2)
  {}

public:

  double               Uncertainty; //!< Precision of the numerical method.
  Handle(Geom_Curve)   C;           //!< Parametric curve itself.
  Handle(Geom_Surface) S1;          //!< First intersected surface.
  Handle(Geom_Surface) S2;          //!< Second intersected surface.

};

//! Collection of intersection curves.
typedef NCollection_Sequence<Handle(asiAlgo_IntersectionCurveSS)> asiAlgo_IntersectionCurvesSS;

#endif
