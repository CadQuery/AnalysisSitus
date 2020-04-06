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

  //! Ctor accepting facets in the form of accelerating structure. Initialized once,
  //! this utility may perform multiple tests for different probe rays.
  //! \param[in] facets   BVH-based structure of facets to test.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_HitFacet(const Handle(asiAlgo_BVHFacets)& facets,
                     ActAPI_ProgressEntry             progress = nullptr,
                     ActAPI_PlotterEntry              plotter  = nullptr);

public:

  //! Sets farthest/nearest mode of testing.
  //! \param[in] on mode to set.
  void SetFarthestMode(const bool on)
  {
    m_bIsFarthest = on;
  }

  //! Sets the index of a facet to exclude from the intersection test.
  //! \param[in] fidx id of the facet to skip.
  void SetFacetToSkip(const int fidx)
  {
    m_iFacetToSkip = fidx;
  }

public:

  //! Performs intersection test for the passed ray.
  //! \param[in]  ray         probe ray.
  //! \param[out] facet_index index of nearest facet or -1 if the intersection
  //!                         cannot be found.
  //! \param[out] result      intersection point.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    operator() (const gp_Lin& ray,
                int&          facet_index,
                gp_XYZ&       result) const;

  //! Performs membership test for a point.
  //! \param[in]  P               probe point.
  //! \param[in]  membership_prec precision of membership test.
  //! \param[out] P_proj          projected point.
  //! \param[out] facet_index     nearest facet.
  //! \return distance to the nearest facet.
  asiAlgo_EXPORT double
    operator() (const gp_Pnt& P,
                const double  membership_prec,
                gp_Pnt&       P_proj,
                int&          facet_index) const;

protected:

  //! Performs narrow-phase testing for a BVH leaf.
  //! \param[in]  P               probe point.
  //! \param[in]  leaf            leaf node of the BVH tree.
  //! \param[in]  membership_prec precision of PMC.
  //! \param[out] P_proj          projected point.
  //! \param[out] resultFacet     found facet which yields the min distance.
  //! \param[out] isInside        indicates whether a point lies inside the triangle.
  //! \return distance from the point P to the facets of interest.
  double testLeaf(const gp_Pnt&    P,
                  const BVH_Vec4i& leaf,
                  const double     membership_prec,
                  gp_Pnt&          P_proj,
                  int&             resultFacet,
                  bool&            isInside) const;

  //! Performs narrow-phase testing for a BVH leaf.
  //! \param[in]  ray                      probe ray.
  //! \param[in]  length                   length of the probe ray to take into account.
  //! \param[in]  leaf                     leaf node of the BVH tree.
  //! \param[out] resultFacet              found facet which yields the min distance.
  //! \param[out] resultRayParamNormalized parameter [0,1] of the intersection point on the ray.
  //! \param[out] hitPoint                 intersection point.
  //! \return true if intersection detected, false -- otherwise.
  bool testLeaf(const gp_Lin&    ray,
                const double     length,
                const BVH_Vec4i& leaf,
                int&             resultFacet,
                double&          resultRayParamNormalized,
                gp_XYZ&          hitPoint) const;

  //! Conducts basic intersection test of the given line with respect to the
  //! bounding box defined by its corner points.
  //! \param[in] L      line to test.
  //! \param[in] boxMin lower corner of the box to test.
  //! \param[in] boxMax upper corner of the box to test.
  //! \return true/false.
  bool isOut(const gp_Lin&    ray,
             const BVH_Vec3d& boxMin,
             const BVH_Vec3d& boxMax,
             const double     prec) const;

  //! Checks if the two points p1 and p2 are on the same side with respect to
  //! the line defined by points a and b.
  //! \param[in] p1 first point to test.
  //! \param[in] p2 second point to test.
  //! \param[in] a  first point on the line.
  //! \param[in] b  second point on the line.
  //! \return true/false.
  bool isSameSide(const gp_Pnt& p1, const gp_Pnt& p2,
                  const gp_Pnt& a,  const gp_Pnt& b) const;

  //! Checks whether the point p belongs to a triangle (a, b, c).
  //! \param[in] p point to test.
  //! \param[in] a first node of the triangle to test.
  //! \param[in] b second node of the triangle to test.
  //! \param[in] c third node of the triangle to test.
  //! \return true/false.
  bool isInside(const gp_Pnt& p,
                const gp_Pnt& a,
                const gp_Pnt& b,
                const gp_Pnt& c) const;

  //! Checks whether the point p belongs to a triangle (a, b, c). This is another
  //! approach based on calculation of barycentric coordinates.
  //! \param[in] p               point to test.
  //! \param[in] a               first node of the triangle to test.
  //! \param[in] b               second node of the triangle to test.
  //! \param[in] c               third node of the triangle to test.
  //! \param[in] membership_prec precision of PMC.
  //! \return true/false.
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

  //! Indicates whether we're looking for the farthest facet.
  bool m_bIsFarthest;

  //! Facet to skip.
  int m_iFacetToSkip;

};

#endif // asiAlgo_HitFacet_h
