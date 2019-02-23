//-----------------------------------------------------------------------------
// Created on: 22 August 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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

#ifndef asiAlgo_InterpolateSurfMesh_h
#define asiAlgo_InterpolateSurfMesh_h

// asiAlgo includes
#include <asiAlgo_BVHFacets.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <Geom_BSplineSurface.hxx>

//-----------------------------------------------------------------------------

//! Utility to build interpolation surface by mesh.
class asiAlgo_InterpolateSurfMesh : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_InterpolateSurfMesh, ActAPI_IAlgorithm)

public:

  //! Ctor.
  //! \param[in] tris     triangulation to interpolate.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_InterpolateSurfMesh(const Handle(Poly_Triangulation)& tris,
                                ActAPI_ProgressEntry              progress,
                                ActAPI_PlotterEntry               plotter);

public:

  asiAlgo_EXPORT bool
    Perform(const std::vector<gp_XYZ>&   contour,
            const double                 grainCoeff,
            const int                    degU,
            const int                    degV,
            Handle(Geom_BSplineSurface)& result);

public:

  //! Sets externally contructed BVH structure for acceleration of computations.
  //! \param[in] bvh BVH structure to set.
  void SetBVH(const Handle(asiAlgo_BVHFacets)& bvh)
  {
    m_bvh = bvh;
  }

protected:

  asiAlgo_EXPORT bool
    performInternal(const std::vector<gp_XYZ>&   contour,
                    const double                 grainCoeff,
                    const int                    degU,
                    const int                    degV,
                    Handle(Geom_BSplineSurface)& result);

protected:

  Handle(Poly_Triangulation) m_tris; //!< Triangulation in question.
  Handle(asiAlgo_BVHFacets)  m_bvh;  //!< Accelerating structure for triangulation.

};

#endif
