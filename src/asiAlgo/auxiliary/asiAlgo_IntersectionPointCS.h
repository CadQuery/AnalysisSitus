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

#ifndef asiAlgo_IntersectionPointCS_h
#define asiAlgo_IntersectionPointCS_h

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>

//! This class represents a 0-dimensional intersection point between a curve
//! and a surface.
class asiAlgo_IntersectionPointCS : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_IntersectionPointCS, Standard_Transient)

public:

  //! Default constructor.
  asiAlgo_IntersectionPointCS()
  : W           (0.0),
    Uncertainty (0.0)
  {}

  //! Constructor.
  //! \param[in] _P three-dimensional point.
  asiAlgo_IntersectionPointCS(const gp_Pnt& _P)
  : P           (_P),
    W           (0.0),
    Uncertainty (0.0)
  {}

  //! Constructor.
  //! \param[in] _P           three-dimensional point.
  //! \param[in] _W           parameter value of the intersection on a curve.
  //! \param[in] _UV          parameters on the surface.
  //! \param[in] _uncertainty uncertainty of the solution.
  asiAlgo_IntersectionPointCS(const gp_Pnt&   _P,
                              const double    _W,
                              const gp_Pnt2d& _UV,
                              const double    _uncertainty)
  : P           (_P),
    W           (_W),
    UV          (_UV),
    Uncertainty (_uncertainty)
  {}

public:

  gp_Pnt   P;           //!< Intersection point in 3D.
  double   W;           //!< Intersection parameter on the curve.
  gp_Pnt2d UV;          //!< Intersection parameters on the surface.
  double   Uncertainty; //!< Uncertainty in 3D point position.

};

//! Collection of intersection points.
typedef NCollection_Sequence<Handle(asiAlgo_IntersectionPointCS)> asiAlgo_IntersectionPointsCS;

#endif
