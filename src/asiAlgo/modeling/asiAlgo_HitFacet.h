//-----------------------------------------------------------------------------
// Created on: 21 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
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
