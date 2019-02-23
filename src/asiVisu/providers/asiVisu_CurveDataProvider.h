//-----------------------------------------------------------------------------
// Created on: 10 February 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Sergey Slyadnev
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
//    * Neither the name of the copyright holder(s) nor the
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

#ifndef asiVisu_CurveDataProvider_h
#define asiVisu_CurveDataProvider_h

// asiVisu includes
#include <asiVisu_DataProvider.h>

// OCCT includes
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>

//-----------------------------------------------------------------------------

//! Data provider for parametric curves (either 3D or 2D).
class asiVisu_CurveDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_CurveDataProvider, asiVisu_DataProvider)

public:

  virtual Handle(Standard_Type) GetCurveType ()                     const = 0;
  virtual Handle(Geom2d_Curve)  GetCurve2d   (double& f, double& l) const = 0;
  virtual Handle(Geom_Curve)    GetCurve     (double& f, double& l) const = 0;

  //! Returns the array of curve parameters serving as interaction handles.
  //! Not all curve types support this mechanism of interaction, so this method
  //! comes with a default empty realization here.
  //!
  //! \return array of curve parameters representing the interaction handles.
  virtual Handle(HRealArray) GetHandles() const { return NULL; }

  //! Returns handle ID which is currently active. Not all curve types use
  //! interactive handles, so this method returns -1 (no handle) by default.
  //!
  //! \return 0-based ID of the active handle or -1 if no handle is active.
  virtual int GetActiveHandle() const { return -1; }

  //! Returns an ordered collection of reper points, i.e., the points which
  //! were used to build this curve. Since not for any curve these points are
  //! recorded, this method has an empty default implementation.
  //!
  //! \param[out] pts vector of reper points which were used to build the curve.
  virtual void GetReperPoints(std::vector<gp_XYZ>& pts) const { asiVisu_NotUsed(pts); }

  //! Returns reper point ID which is currently active. Not all curve types use
  //! reper points, so this method returns -1 (no point) by default.
  //!
  //! \return 0-based ID of the active reper point or -1 if no reper is active.
  virtual int GetActiveReper() const { return -1; }

};

#endif
