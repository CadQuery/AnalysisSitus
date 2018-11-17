//-----------------------------------------------------------------------------
// Created on: 21 September 2016
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

#ifndef asiAlgo_HitFacet_h
#define asiAlgo_HitFacet_h

// asiAlgo includes
#include <asiAlgo_BVHAlgo.h>

//-----------------------------------------------------------------------------

//! Finds a facet which is intersected by the probe ray.
class asiAlgo_HitFacet : public asiAlgo_BVHAlgo
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_HitFacet, asiAlgo_BVHAlgo)

public:

  asiAlgo_EXPORT
    asiAlgo_HitFacet(const Handle(asiAlgo_BVHFacets)& facets,
                     ActAPI_ProgressEntry             progress,
                     ActAPI_PlotterEntry              plotter);

public:

  asiAlgo_EXPORT bool
    operator() (const gp_Lin& ray, int& facet_index, gp_XYZ& result) const;

  asiAlgo_EXPORT double
    operator() (const gp_Pnt& P, const double membership_prec, gp_Pnt& P_proj, int& facet_index) const;

protected:

  double testLeaf(const gp_Pnt&    P,
                  const BVH_Vec4i& leaf,
                  const double     membership_prec,
                  gp_Pnt&          P_proj,
                  int&             resultFacet,
                  bool&            isInside) const;

  bool testLeaf(const gp_Lin&    ray,
                const double     length,
                const BVH_Vec4i& leaf,
                int&             resultFacet,
                double&          resultRayParamNormalized,
                gp_XYZ&          hitPoint) const;

  bool isOut(const gp_Lin&    ray,
             const BVH_Vec4d& boxMin,
             const BVH_Vec4d& boxMax,
             const double     prec) const;

  bool isSameSide(const gp_Pnt& p1, const gp_Pnt& p2,
                  const gp_Pnt& a,  const gp_Pnt& b) const;

  bool isInside(const gp_Pnt& p,
                const gp_Pnt& a,
                const gp_Pnt& b,
                const gp_Pnt& c) const;

  bool isInsideBarycentric(const gp_Pnt& p,
                           const gp_Pnt& a,
                           const gp_Pnt& b,
                           const gp_Pnt& c,
                           const double  membership_prec) const;

  bool isIntersected(const gp_XYZ& rayStart,
                     const gp_XYZ& rayFinish,
                     const gp_XYZ& pntTri1,
                     const gp_XYZ& pntTri2,
                     const gp_XYZ& pntTri3,
                     double&       hitParamNormalized,
                     gp_XYZ&       hitPoint) const;

};

#endif // asiAlgo_HitFacet_h
