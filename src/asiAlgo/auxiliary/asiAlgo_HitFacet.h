//-----------------------------------------------------------------------------
// Created on: 21 September 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiAlgo_HitFacet_h
#define asiAlgo_HitFacet_h

// Geometry includes
#include <asiAlgo_BVHFacets.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

DEFINE_STANDARD_HANDLE(asiAlgo_HitFacet, ActAPI_IAlgorithm)

//! Finds a facet which is intersected by the probe ray.
class asiAlgo_HitFacet : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_HitFacet, ActAPI_IAlgorithm)

public:

  asiAlgo_EXPORT
    asiAlgo_HitFacet(const Handle(asiAlgo_BVHFacets)& facets,
                     ActAPI_ProgressEntry             progress,
                     ActAPI_PlotterEntry              plotter);

public:

  asiAlgo_EXPORT bool
    operator() (const gp_Lin& ray, int& facet_index, gp_XYZ& result);

  asiAlgo_EXPORT double
    operator() (const gp_Pnt& P, const double membership_prec, int& facet_index);

protected:

  double testLeaf(const gp_Pnt&    P,
                  const BVH_Vec4i& leaf,
                  const double     membership_prec,
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

  bool isOut(const gp_Pnt&    P,
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

protected:

  //! Facets in form of acceleration structure.
  Handle(asiAlgo_BVHFacets) m_facets;

};

#endif // asiAlgo_HitFacet_h
