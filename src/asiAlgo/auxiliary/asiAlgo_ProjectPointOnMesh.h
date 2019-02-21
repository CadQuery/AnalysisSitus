//-----------------------------------------------------------------------------
// Created on: 29 September 2018
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

#ifndef asiAlgo_ProjectPointOnMesh_h
#define asiAlgo_ProjectPointOnMesh_h

// asiAlgo includes
#include <asiAlgo_BVHAlgo.h>

struct ProjectionInfoMesh;

//-----------------------------------------------------------------------------

//! Class intended for projection of the point onto triangulation.
class asiAlgo_ProjectPointOnMesh : public asiAlgo_BVHAlgo
{
public:

  //! Ctor accepting facets in form of accelerating structure.
  //! \param[in] facets   BVH-based structure of facets to test.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_ProjectPointOnMesh(const Handle(asiAlgo_BVHFacets)& facets,
                               ActAPI_ProgressEntry             progress = NULL,
                               ActAPI_PlotterEntry              plotter  = NULL);

public:

  //! \brief Returns projection of the given point onto a triangulation
  asiAlgo_EXPORT gp_Pnt
    Perform(const gp_Pnt& thePnt);

public:

  //! \brief Returns triangle id from the last call.
  const std::vector<int>& GetTriIdx() const
  {
    return m_triIdx;
  }

protected:

  void IntersectLeaves(const BVH_Vec4i& theLeaf,
                       const gp_Pnt& theSourcePoint,
                       ProjectionInfoMesh& theProjection) const;

private:

  asiAlgo_ProjectPointOnMesh& operator=(const asiAlgo_ProjectPointOnMesh&) = delete;

protected:

  //! Index of the triangle with minimal distance for the last Perform call.
  std::vector<int> m_triIdx;

};

#endif // asiAlgo_ProjectPointOnMesh_h
