//-----------------------------------------------------------------------------
// Created on: 27 October 2018
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

#ifndef asiAlgo_MeshProjectLine_HeaderFile
#define asiAlgo_MeshProjectLine_HeaderFile

// asiAlgo includes
#include <asiAlgo_ProjectPointOnMesh.h>

// Active Data (API) includes
#include <ActAPI_IAlgorithm.h>

//-----------------------------------------------------------------------------

//! Utility to project straight line segments on mesh.
class asiAlgo_MeshProjectLine : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_MeshProjectLine, ActAPI_IAlgorithm)

public:

  //! Ctor.
  //! \param[in] facets   accelerating structure for facets.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_MeshProjectLine(const Handle(asiAlgo_BVHFacets)& facets,
                            ActAPI_ProgressEntry             progress,
                            ActAPI_PlotterEntry              plotter);

public:

  //! Performs projection.
  //! \param[in]  P1        starting point.
  //! \param[in]  P2        ending point.
  //! \param[out] result    result polyline.
  //! \param[in]  diagRatio AABB ratio to control the precision.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Perform(const gp_XYZ&        P1,
            const gp_XYZ&        P2,
            std::vector<gp_XYZ>& result,
            const double         diagRatio = 0.001) const;

protected:

  //! Mesh packaged into accelerating structure.
  Handle(asiAlgo_BVHFacets) m_facets;

  //! Algorithm to project single point on mesh.
  Handle(asiAlgo_ProjectPointOnMesh) m_projectPtMesh;

};

#endif
