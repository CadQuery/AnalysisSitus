//-----------------------------------------------------------------------------
// Created on: 02 March 2018
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

#ifndef asiAlgo_IntersectCS_h
#define asiAlgo_IntersectCS_h

// asiAlgo includes
#include <asiAlgo_IntersectionPointCS.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>

//-----------------------------------------------------------------------------

//! Curve-Surface intersection tool.
class asiAlgo_IntersectCS : public ActAPI_IAlgorithm
{
public:

  //! Constructor.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_IntersectCS(ActAPI_ProgressEntry progress,
                        ActAPI_PlotterEntry  plotter);

public:

  //! Performs curve-surface intersection.
  //! \param[in]  S                   surface.
  //! \param[in]  C                   curve to intersect with the surface.
  //! \param[out] result              intersection points.
  //! \param[out] hasTangentialPoints indicates whether at least one tangential
  //!                                 intersection point was detected.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    operator()(const Handle(Geom_Surface)&   S,
               const Handle(Geom_Curve)&     C,
               asiAlgo_IntersectionPointsCS& result,
               bool&                         hasTangentialPoints);

public:

  //! Sets angular tolerance for resolving tangency situations at
  //! intersection points.
  //! \param[in] tolDeg angular tolerance (in degrees).
  void SetTangencyAngularTol(const double tolDeg)
  {
    m_fTangencyTolRad = tolDeg/180.0*M_PI;
  }

protected:

  double m_fTangencyTolRad; //!< Tolerance for resolving tangency situations (radians).

};

#endif
