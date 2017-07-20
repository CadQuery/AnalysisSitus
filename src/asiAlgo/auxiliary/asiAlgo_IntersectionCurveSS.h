//-----------------------------------------------------------------------------
// Created on: 06 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
