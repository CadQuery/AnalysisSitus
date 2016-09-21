//-----------------------------------------------------------------------------
// Created on: 21 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_find_nearest_facet_h
#define geom_find_nearest_facet_h

// Geometry includes
#include <geom_bvh_facets.h>

DEFINE_STANDARD_HANDLE(geom_find_nearest_facet, ActAPI_IAlgorithm)

//! Finds a facet which is nearest to the probe point.
class geom_find_nearest_facet : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_find_nearest_facet, ActAPI_IAlgorithm)

public:

  geom_find_nearest_facet(const Handle(geom_bvh_facets)& facets,
                          ActAPI_ProgressEntry           progress,
                          ActAPI_PlotterEntry            plotter);

public:

  bool operator()(const gp_Pnt& P, int& facet_index);

protected:

  bool testLeaf(const gp_Pnt&    P,
                const BVH_Vec4i& leaf) const;

  bool isOut(const gp_Pnt&    P,
             const BVH_Vec4d& boxMin,
             const BVH_Vec4d& boxMax) const;

protected:

  //! Facets in form of acceleration structure.
  Handle(geom_bvh_facets) m_facets;

};

#endif // geom_find_nearest_facet_h
