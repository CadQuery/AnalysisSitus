//-----------------------------------------------------------------------------
// Created on: 03 March 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
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

#ifndef asiAlgo_FairingBjFunc_HeaderFile
#define asiAlgo_FairingBjFunc_HeaderFile

// asiAlgo includes
#include <asiAlgo_FairingCoeffFunc.h>

// OCCT includes
#include <Geom_BSplineCurve.hxx>
#include <TColStd_Array1OfReal.hxx>

//! Univariate function to interface right-hand-side of fairing linear problem
//! via OpenCascade math_Function API.
class asiAlgo_FairingBjFunc : public asiAlgo_FairingCoeffFunc
{
public:

  //! ctor.
  //! \param[in] curve  B-spline curve in question (the one to fair).
  //! \param[in] coord  index of coordinate to use (0 for X, 1 for Y, and 2 for Z).
  //! \param[in] U      knot vector.
  //! \param[in] p      B-spline degree.
  //! \param[in] i      0-based index of the B-spline function.
  //! \param[in] lambda fairing coefficent.
  asiAlgo_EXPORT
    asiAlgo_FairingBjFunc(const Handle(Geom_BSplineCurve)& curve,
                          const int                        coord,
                          const TColStd_Array1OfReal&      U,
                          const int                        p,
                          const int                        i,
                          const double                     lambda);

public:

  //! Evaluates function.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Value(const double u, double& f);

private:

  asiAlgo_FairingBjFunc() = delete;
  void operator=(const asiAlgo_FairingBjFunc&) = delete;

protected:

  Handle(Geom_BSplineCurve)   m_curve;   //!< Curve in question.
  int                         m_iCoord;  //!< Coordinate in question.
  const TColStd_Array1OfReal& m_U;       //!< Knot vector ("flat" knots).
  int                         m_iDegree; //!< Degree of the spline function.
  int                         m_iIndex;  //!< 0-based index of the spline function.

};

#endif
