//-----------------------------------------------------------------------------
// Created on: 21 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_hit_facet.h>

// Geometry includes
#include <geom_bvh_iterator.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <gp_Lin.hxx>
#include <Precision.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
#include <BRepBuilderAPI_MakeEdge.hxx>
#endif

//-----------------------------------------------------------------------------

typedef int BVH_StackItem;

//-----------------------------------------------------------------------------

//! ctor accepting facets in form of accelerating structure. Initialized once,
//! this utility may perform multiple tests for different probe rays.
//! \param[in] facets   BVH-based structure of facets to test.
//! \param[in] progress progress notifier.
//! \param[in] plotter  imperative plotter.
geom_hit_facet::geom_hit_facet(const Handle(geom_bvh_facets)& facets,
                               ActAPI_ProgressEntry           progress,
                               ActAPI_PlotterEntry            plotter)
: ActAPI_IAlgorithm(progress, plotter),
  m_facets(facets)
{}

//-----------------------------------------------------------------------------

//! Performs hit check for a ray.
//! \param[in]  ray         probe ray.
//! \param[out] facet_index nearest facet.
//! \param[out] result      picked point.
//! \return true in case of success, false -- otherwise.
bool geom_hit_facet::operator()(const gp_Lin& ray,
                                int&          facet_index,
                                gp_XYZ&       result)
{
  const NCollection_Handle< BVH_Tree<double, 4> >& bvh = m_facets->BVH();
  if ( bvh.IsNull() )
    return false;

  // Initialize output index
  facet_index = -1;

  // Limit of the ray for hit test
  const double ray_limit = m_facets->GetBoundingDiag();

  // Precision for fast intersection test on AABB
  const double prec = Precision::Confusion();

  // Intersection parameter for sorting
  double resultRayParam = RealLast();

#if defined DRAW_DEBUG
  this->Plotter().CLEAN();
  this->Plotter().DRAW_POINT(ray.Location(), Color_Red, "ray_origin");
  this->Plotter().DRAW_LINK(ray.Location(), ray.Location().XYZ() + ray.Direction().XYZ()*ray_limit, Color_Green, "ray");
#endif

  // Traverse BVH
  for ( geom_bvh_iterator it(bvh); it.More(); it.Next() )
  {
    const BVH_Vec4i& nodeData = it.Current();

    if ( it.IsLeaf() )
    {
      // If we are here, then we are close to solution. It is a right
      // time now to perform a precise check

      int facet_candidate = -1;
      double hitParam;
      gp_XYZ hitPoint;
      //
      const bool isHit = this->testLeaf(ray, ray_limit, nodeData, facet_candidate, hitParam, hitPoint);
      //
      if ( isHit && hitParam < resultRayParam )
      {
        facet_index    = facet_candidate;
        resultRayParam = hitParam;
        result         = hitPoint;
      }
    }
    else // sub-volume
    {
      const BVH_Vec4d& minCorner_Left  = bvh->MinPoint( nodeData.y() );
      const BVH_Vec4d& maxCorner_Left  = bvh->MaxPoint( nodeData.y() );
      const BVH_Vec4d& minCorner_Right = bvh->MinPoint( nodeData.z() );
      const BVH_Vec4d& maxCorner_Right = bvh->MaxPoint( nodeData.z() );

      const bool isLeftOut  = this->isOut(ray, minCorner_Left, maxCorner_Left, prec);
      const bool isRightOut = this->isOut(ray, minCorner_Right, maxCorner_Right, prec);

      if ( isLeftOut )
        it.BlockLeft();
      if ( isRightOut )
        it.BlockRight();
    }
  }

#if defined DRAW_DEBUG
  if ( facet_index != -1 )
  {
    std::cout << "Found intersection with facet "
              << facet_index
              << " on face "
              << m_facets->GetFacet(facet_index).FaceIndex
              << " for ray parameter "
              << resultRayParam
              << std::endl;

    const geom_bvh_facets::t_facet& facet = m_facets->GetFacet(facet_index);
    //
    this->Plotter().DRAW_LINK( gp_Pnt(facet.P0.x(), facet.P0.y(), facet.P0.z()),
                               gp_Pnt(facet.P1.x(), facet.P1.y(), facet.P1.z()), Color_Red);
    this->Plotter().DRAW_LINK( gp_Pnt(facet.P0.x(), facet.P0.y(), facet.P0.z()),
                               gp_Pnt(facet.P2.x(), facet.P2.y(), facet.P2.z()), Color_Red);
    this->Plotter().DRAW_LINK( gp_Pnt(facet.P1.x(), facet.P1.y(), facet.P1.z()),
                               gp_Pnt(facet.P2.x(), facet.P2.y(), facet.P2.z()), Color_Red);
  }
  else
    std::cout << "Error: cannot find the intersected facet" << std::endl;
#endif

  return facet_index != -1;
}

//-----------------------------------------------------------------------------

//! Performs hit check for a point.
//! \param[in]  P               probe point.
//! \param[in]  membership_prec precision of membership test.
//! \param[out] facet_index     nearest facet.
//! \return distance to the nearest facet.
double geom_hit_facet::operator()(const gp_Pnt& P,
                                  const double  membership_prec,
                                  int&          facet_index)
{
  const NCollection_Handle< BVH_Tree<double, 4> >& bvh = m_facets->BVH();
  if ( bvh.IsNull() )
    return false;

  // Initialize output index
  facet_index = -1;

  // Precision for fast intersection test on AABB
  const double intersectPrec = Precision::Confusion();

  // Distance for sorting
  double minDist = RealLast();

  // Traverse BVH
  for ( geom_bvh_iterator it(bvh); it.More(); it.Next() )
  {
    const BVH_Vec4i& nodeData = it.Current();

    if ( it.IsLeaf() )
    {
      // If we are here, then we are close to solution. It is a right
      // time now to perform a precise check

      int  facet_candidate = -1;
      bool isInside        = false;
      //
      const double dist = this->testLeaf(P, nodeData, membership_prec, facet_candidate, isInside);
      //
      if ( isInside && dist < minDist )
      {
        facet_index = facet_candidate;
        minDist     = dist;
      }
    }
    else // sub-volume
    {
      const BVH_Vec4d& minCorner_Left  = bvh->MinPoint( nodeData.y() );
      const BVH_Vec4d& maxCorner_Left  = bvh->MaxPoint( nodeData.y() );
      const BVH_Vec4d& minCorner_Right = bvh->MinPoint( nodeData.z() );
      const BVH_Vec4d& maxCorner_Right = bvh->MaxPoint( nodeData.z() );

      const bool isLeftOut  = this->isOut(P, minCorner_Left, maxCorner_Left, intersectPrec);
      const bool isRightOut = this->isOut(P, minCorner_Right, maxCorner_Right, intersectPrec);

      if ( isLeftOut )
        it.BlockLeft();
      if ( isRightOut )
        it.BlockRight();
    }
  }

#if defined DRAW_DEBUG
  if ( facet_index != -1 )
  {
    std::cout << "Found host facet "
              << facet_index
              << " on face "
              << m_facets->GetFacet(facet_index).FaceIndex
              << " for min distance "
              << minDist
              << std::endl;

    const geom_bvh_facets::t_facet& facet = m_facets->GetFacet(facet_index);
    //
    this->Plotter().DRAW_LINK( gp_Pnt(facet.P0.x(), facet.P0.y(), facet.P0.z()),
                               gp_Pnt(facet.P1.x(), facet.P1.y(), facet.P1.z()), Color_Red);
    this->Plotter().DRAW_LINK( gp_Pnt(facet.P0.x(), facet.P0.y(), facet.P0.z()),
                               gp_Pnt(facet.P2.x(), facet.P2.y(), facet.P2.z()), Color_Red);
    this->Plotter().DRAW_LINK( gp_Pnt(facet.P1.x(), facet.P1.y(), facet.P1.z()),
                               gp_Pnt(facet.P2.x(), facet.P2.y(), facet.P2.z()), Color_Red);
  }
  else
    std::cout << "Error: cannot find the host facet" << std::endl;
#endif

  return facet_index != -1;
}

//-----------------------------------------------------------------------------

//! Performs narrow-phase testing for a BVH leaf.
//! \param[in]  P               probe point.
//! \param[in]  leaf            leaf node of the BVH tree.
//! \param[in]  membership_prec precision of PMC.
//! \param[out] resultFacet     found facet which yields the min distance.
//! \param[out] isInside        indicates whether a point lies inside the triangle.
//! \return distance from the point P to the facets of interest.
double geom_hit_facet::testLeaf(const gp_Pnt&    P,
                                const BVH_Vec4i& leaf,
                                const double     membership_prec,
                                int&             resultFacet,
                                bool&            isInside) const
{
  gp_XYZ p       = P.XYZ();
  double minDist = 1e100;

  // Loop over the tentative facets
  for ( int fidx = leaf.y(); fidx <= leaf.z(); ++fidx )
  {
    // Get facet to test
    const geom_bvh_facets::t_facet& facet = m_facets->GetFacet(fidx);

    // Get next facet to test
    const gp_XYZ p0( facet.P0.x(), facet.P0.y(), facet.P0.z() );
    const gp_XYZ p1( facet.P1.x(), facet.P1.y(), facet.P1.z() );
    const gp_XYZ p2( facet.P2.x(), facet.P2.y(), facet.P2.z() );

    const gp_XYZ p0p1     = p1 - p0;
    const gp_XYZ p0p2     = p2 - p0;
    const gp_XYZ p0p      = p - p0;
    const double p0p_dist = p0p.Modulus();

    // Compute normal vector
    const gp_XYZ Np = p0p1 ^ p0p2;
    if ( Np.SquareModulus() < RealEpsilon() )
      continue;

    const double cos_alpha = ( p0p * Np ) / ( p0p_dist * Np.Modulus() );
    const double dist      = p0p_dist * cos_alpha;
    const gp_XYZ pproj     = p - Np.Normalized()*dist;

#if defined COUT_DEBUG
    std::cout << "Probe's projection is ("
              << pproj.X() << ", "
              << pproj.Y() << ", "
              << pproj.Z() << ")" << std::endl;
#endif

    if ( this->isInsideBarycentric(pproj, p0, p1, p2, membership_prec) && Abs(dist) <= minDist )
    {
      isInside    = true;
      minDist     = Abs(dist);
      resultFacet = fidx;

#if defined DRAW_DEBUG
      this->Plotter().DRAW_LINK(p0, p1, Color_Green);
      this->Plotter().DRAW_LINK(p0, p2, Color_Green);
      this->Plotter().DRAW_LINK(p1, p2, Color_Green);
#endif
    }
  }

  return minDist;
}

//-----------------------------------------------------------------------------

//! Performs narrow-phase testing for a BVH leaf.
//! \param[in]  ray                      probe ray.
//! \param[in]  length                   length of the probe ray to take into account.
//! \param[in]  leaf                     leaf node of the BVH tree.
//! \param[out] resultFacet              found facet which yields the min distance.
//! \param[out] resultRayParamNormalized parameter [0,1] of the intersection point on the ray.
//! \param[out] hitPoint                 intersection point.
//! \return true if intersection detected, false -- otherwise.
bool geom_hit_facet::testLeaf(const gp_Lin&    ray,
                              const double     length,
                              const BVH_Vec4i& leaf,
                              int&             resultFacet,
                              double&          resultRayParamNormalized,
                              gp_XYZ&          hitPoint) const
{
  // Prepare a segment of the ray to pass for intersection test
  const gp_XYZ l0 = ray.Location().XYZ();
  const gp_XYZ l1 = l0 + ray.Direction().XYZ()*length;

  // Parameter on ray is used for intersection sorting. We are interested
  // in the closest point only
  resultFacet              = -1;
  resultRayParamNormalized = RealLast();

  // Loop over the tentative facets
  for ( int fidx = leaf.y(); fidx <= leaf.z(); ++fidx )
  {
    // Get facet to test
    const geom_bvh_facets::t_facet& facet = m_facets->GetFacet(fidx);

    // Get next facet to test
    const gp_XYZ p0( facet.P0.x(), facet.P0.y(), facet.P0.z() );
    const gp_XYZ p1( facet.P1.x(), facet.P1.y(), facet.P1.z() );
    const gp_XYZ p2( facet.P2.x(), facet.P2.y(), facet.P2.z() );

    // Hit test
    double currentParam;
    gp_XYZ currentPoint;
    //
    if ( this->isIntersected(l0, l1, p0, p1, p2, currentParam, currentPoint) )
    {
      if ( currentParam < resultRayParamNormalized )
      {
        resultFacet              = fidx;
        resultRayParamNormalized = currentParam;
        hitPoint                 = currentPoint;
      }
    }
  }

  return (resultFacet != -1);
}

//-----------------------------------------------------------------------------

//! Conducts basic intersection test of the given line with respect to the
//! bounding box defined by its corner points.
//! \param[in] L      line to test.
//! \param[in] boxMin lower corner of the box to test.
//! \param[in] boxMax upper corner of the box to test.
//! \return true/false.
bool geom_hit_facet::isOut(const gp_Lin&    L,
                           const BVH_Vec4d& boxMin,
                           const BVH_Vec4d& boxMax,
                           const double     prec) const
{
  double xmin = 0, xmax = 0, ymin = 0, ymax = 0, zmin = 0, zmax = 0;
  double parmin, parmax, par1, par2;
  bool xToSet, yToSet;

  // Protection from degenerated bounding box.
  double myXmin = boxMin.x() - prec;
  double myYmin = boxMin.y() - prec;
  double myZmin = boxMin.z() - prec;
  double myXmax = boxMax.x() + prec;
  double myYmax = boxMax.y() + prec;
  double myZmax = boxMax.z() + prec;

  if ( Abs(L.Direction().XYZ().X()) > 0.0 )
  {
    par1 = (myXmin - L.Location().XYZ().X()) / L.Direction().XYZ().X();
    par2 = (myXmax - L.Location().XYZ().X()) / L.Direction().XYZ().X();
    parmin = Min(par1, par2);
    parmax = Max(par1, par2);
    xToSet = true;
  }
  else 
  {
    if (L.Location().XYZ().X() < myXmin || myXmax<L.Location().XYZ().X())
      return true;

    xmin = L.Location().XYZ().X();
    xmax = L.Location().XYZ().X();
    parmin = -1.0e100;
    parmax = 1.0e100;
    xToSet = false;
  }

  if (Abs(L.Direction().XYZ().Y())>0.0)
  {
    par1 = (myYmin - L.Location().XYZ().Y()) / L.Direction().XYZ().Y();
    par2 = (myYmax - L.Location().XYZ().Y()) / L.Direction().XYZ().Y();
    if (parmax < Min(par1, par2) || parmin > Max(par1, par2))
      return true;
    parmin = Max(parmin, Min(par1, par2));
    parmax = Min(parmax, Max(par1, par2));
    yToSet = true;
  }
  else 
  {
    if (L.Location().XYZ().Y() < myYmin || myYmax<L.Location().XYZ().Y())
      return true;
    ymin = L.Location().XYZ().Y();
    ymax = L.Location().XYZ().Y();
    yToSet = false;
  }

  if (Abs(L.Direction().XYZ().Z())>0.0)
  {
    par1 = (myZmin - L.Location().XYZ().Z()) / L.Direction().XYZ().Z();
    par2 = (myZmax - L.Location().XYZ().Z()) / L.Direction().XYZ().Z();
    if (parmax < Min(par1, par2) || parmin > Max(par1, par2))
      return true;
    parmin = Max(parmin, Min(par1, par2));
    parmax = Min(parmax, Max(par1, par2));
    par1 = L.Location().XYZ().Z() + parmin*L.Direction().XYZ().Z();
    par2 = L.Location().XYZ().Z() + parmax*L.Direction().XYZ().Z();
    zmin = Min(par1, par2);
    zmax = Max(par1, par2);
  }
  else 
  {
    if (L.Location().XYZ().Z() < myZmin || myZmax < L.Location().XYZ().Z())
      return true;
    zmin = L.Location().XYZ().Z();
    zmax = L.Location().XYZ().Z();
  }
  if (zmax < myZmin || myZmax < zmin)
    return true;

  if (xToSet) 
  {
    par1 = L.Location().XYZ().X() + parmin*L.Direction().XYZ().X();
    par2 = L.Location().XYZ().X() + parmax*L.Direction().XYZ().X();
    xmin = Min(par1, par2);
    xmax = Max(par1, par2);
  }
  if (xmax < myXmin || myXmax < xmin)
    return true;

  if (yToSet) 
  {
    par1 = L.Location().XYZ().Y() + parmin*L.Direction().XYZ().Y();
    par2 = L.Location().XYZ().Y() + parmax*L.Direction().XYZ().Y();
    ymin = Min(par1, par2);
    ymax = Max(par1, par2);
  }
  if (ymax < myYmin || myYmax < ymin)
    return true;

  return false;
}

//-----------------------------------------------------------------------------

//! Conducts basic intersection test of the given point with respect to the
//! bounding box defined by its corner points.
//! \param[in] P      point to test.
//! \param[in] boxMin lower corner of the box to test.
//! \param[in] boxMax upper corner of the box to test.
//! \return true/false.
bool geom_hit_facet::isOut(const gp_Pnt&    P,
                           const BVH_Vec4d& boxMin,
                           const BVH_Vec4d& boxMax,
                           const double     prec) const
{
  const double x = P.X(),
               y = P.Y(),
               z = P.Z();
  const double xmin = boxMin.x() - prec,
               ymin = boxMin.y() - prec,
               zmin = boxMin.z() - prec,
               xmax = boxMax.x() + prec,
               ymax = boxMax.y() + prec,
               zmax = boxMax.z() + prec;

  if ( x < xmin || y < ymin || z < zmin ||
       x > xmax || y > ymax || z > zmax )
    return true;

  return false;
}

//-----------------------------------------------------------------------------

//! Checks if the two points p1 and p2 are on the same side with respect to
//! the line defined by points a and b.
//! \param[in] p1 first point to test.
//! \param[in] p2 second point to test.
//! \param[in] a  first point on the line.
//! \param[in] b  second point on the line.
//! \return true/false.
bool geom_hit_facet::isSameSide(const gp_Pnt& p1, const gp_Pnt& p2,
                                const gp_Pnt& a,  const gp_Pnt& b) const
{
  const gp_XYZ ab  = b.XYZ()  - a.XYZ();
  const gp_XYZ ap1 = p1.XYZ() - a.XYZ();
  const gp_XYZ ap2 = p2.XYZ() - a.XYZ();

  const gp_XYZ cp1 = ab ^ ap1;
  const gp_XYZ cp2 = ab ^ ap2;

  const double dot = cp1 * cp2;
  return dot >= 0;
}

//-----------------------------------------------------------------------------

//! Checks whether the point p belongs to a triangle (a, b, c).
//! \param[in] p point to test.
//! \param[in] a first node of the triangle to test.
//! \param[in] b second node of the triangle to test.
//! \param[in] c third node of the triangle to test.
//! \return true/false.
bool geom_hit_facet::isInside(const gp_Pnt& p,
                              const gp_Pnt& a,
                              const gp_Pnt& b,
                              const gp_Pnt& c) const
{
  return this->isSameSide(p, a, /* */ b, c) &&
         this->isSameSide(p, b, /* */ a, c) &&
         this->isSameSide(p, c, /* */ a, b);
}

//-----------------------------------------------------------------------------

//! Checks whether the point p belongs to a triangle (a, b, c). This is another
//! approach based on calculation of barycentric coordinates.
//! \param[in] p               point to test.
//! \param[in] a               first node of the triangle to test.
//! \param[in] b               second node of the triangle to test.
//! \param[in] c               third node of the triangle to test.
//! \param[in] membership_prec precision of PMC.
//! \return true/false.
bool geom_hit_facet::isInsideBarycentric(const gp_Pnt& p,
                                         const gp_Pnt& a,
                                         const gp_Pnt& b,
                                         const gp_Pnt& c,
                                         const double  membership_prec) const
{
  // Compute vectors
  gp_XYZ v0 = c.XYZ() - a.XYZ();
  gp_XYZ v1 = b.XYZ() - a.XYZ();
  gp_XYZ v2 = p.XYZ() - a.XYZ();

  // Compute dot products
  const double dot00 = v0.Dot(v0);
  const double dot01 = v0.Dot(v1);
  const double dot02 = v0.Dot(v2);
  const double dot11 = v1.Dot(v1);
  const double dot12 = v1.Dot(v2);

  // Compute barycentric coordinates
  const double invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
  const double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
  const double v = (dot00 * dot12 - dot01 * dot02) * invDenom;

  // Check if point is in triangle
  return (u >= 0.0) &&
         (v >= 0.0) &&
       ( (u + v < 1) || (Abs(u + v - 1) < membership_prec) );
}

//-----------------------------------------------------------------------------

bool geom_hit_facet::isIntersected(const gp_XYZ& rayStart,
                                   const gp_XYZ& rayFinish,
                                   const gp_XYZ& pntTri1,
                                   const gp_XYZ& pntTri2,
                                   const gp_XYZ& pntTri3,
                                   double&       hitParamNormalized,
                                   gp_XYZ&       hitPoint) const
{
  // Moller–Trumbore intersection algorithm
  // (T. Moller et al, Fast Minimum Storage Ray / Triangle Intersection)
  gp_Vec e1(pntTri1, pntTri2),
         e2(pntTri1, pntTri3);
  gp_Vec dir(rayStart, rayFinish);
  gp_Vec P = dir.Crossed(e2);

  // If determinant is near zero, ray lies in plane of triangle or ray
  // is parallel to plane of triangle
  double det = e1.Dot(P);
  if ( Abs(det) < 1.0e-16 )
    return false;

  double inv_det = 1.0 / det;

  gp_Vec T(pntTri1, rayStart);

  // Calculate u parameter and test bound.
  double u = T.Dot(P) * inv_det;
  if ( u < 0.0 || u > 1.0 )
    return false; // Intersection lies outside the triangle

  // Calculate V parameter and test bound.
  gp_Vec Q = T.Crossed(e1);
  double v = dir.Dot(Q) * inv_det;
  if ( v < 0.0 || (u + v)  > 1.0 )
    return false; // Intersection lies outside the triangle

  double t           = e2.Dot(Q) * inv_det;
  hitParamNormalized = t;
  hitPoint           = (1 - u - v)*pntTri1 + u*pntTri2 + v*pntTri3;

  if ( t < 0.0 || t > 1.0 )
    return false;

  return true;
}
