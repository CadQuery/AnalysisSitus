//-----------------------------------------------------------------------------
// Created on: 28 April 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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

// Own include
#include <asiAlgo_FindVisibleFaces.h>

// asiAlgo includes
#include <asiAlgo_BVHIterator.h>
#include <asiAlgo_HitFacet.h>

// OpenCascade includes
#include <Precision.hxx>

//-----------------------------------------------------------------------------

asiAlgo_FindVisibleFaces::asiAlgo_FindVisibleFaces(const TopoDS_Shape&  shape,
                                                   ActAPI_ProgressEntry progress,
                                                   ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{
  this->init(shape);
}

//-----------------------------------------------------------------------------

bool asiAlgo_FindVisibleFaces::Perform()
{
  if ( m_bvh.IsNull() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "BVH is not initialized.");
    return false;
  }

  for ( size_t rbIdx = 0; rbIdx < m_rayBundles.size(); ++rbIdx )
  {
    const t_rayBundle& rb = m_rayBundles[rbIdx];

    // Test for intersections.
    const bool isVisible = this->isVisible(rb);

    // Store the result.
    t_score* pScore = m_scores.ChangeSeek(rb.FaceIndex);
    //
    if ( pScore == nullptr )
    {
      m_scores.Bind( rb.FaceIndex, t_score(isVisible ? 0 : 1,
                                           isVisible ? 1 : 0) );
    }
    else
    {
      isVisible ? (pScore->NumVoids++) : (pScore->NumHits++);
    }
  }

  return true;
}

//-----------------------------------------------------------------------------

const NCollection_DataMap<t_topoId , asiAlgo_FindVisibleFaces::t_score>&
  asiAlgo_FindVisibleFaces::GetResultScores() const
{
  return m_scores;
}

//-----------------------------------------------------------------------------

void
  asiAlgo_FindVisibleFaces::GetResultFaces(TColStd_PackedMapOfInteger& faces,
                                           const int                   maxHits) const
{
  for ( NCollection_DataMap<int, t_score>::Iterator it(m_scores);
        it.More(); it.Next() )
  {
    const t_score& score = it.Value();
    //
    if ( (score.NumVoids > 0) && (score.NumHits <= maxHits) )
    {
      const int fid = it.Key();
      faces.Add(fid);
    }
  }
}

//-----------------------------------------------------------------------------

void asiAlgo_FindVisibleFaces::init(const TopoDS_Shape& shape)
{
  // Build BVH from the shape. The indices of faces will be stored together
  // with the corresponding triangles.
  m_bvh = new asiAlgo_BVHFacets(shape);

  // Initialize ray bundles.
  for ( int facetIdx = 0; facetIdx < m_bvh->Size(); ++facetIdx )
  {
    const asiAlgo_BVHFacets::t_facet& facet = m_bvh->GetFacet(facetIdx);

    BVH_Vec3d Pm = (facet.P0 + facet.P1 + facet.P2) / 3.;
    gp_Lin ray( gp_Pnt( Pm.x(), Pm.y(), Pm.z() ), facet.N);

    m_rayBundles.push_back( t_rayBundle(ray, facet.FaceIndex) );
  }
}

//-----------------------------------------------------------------------------

bool asiAlgo_FindVisibleFaces::isVisible(const t_rayBundle& rb) const
{
  const opencascade::handle< BVH_Tree<double, 3> >& bvh = m_bvh->BVH();

  // Limit of the ray for hit test.
  const double ray_limit = m_bvh->GetBoundingDiag()*100;

  // Precision for fast intersection test on AABB.
  const double prec = Precision::Confusion();

  // Test each ray.
  for ( size_t iray = 0; iray < rb.Rays.size(); ++iray )
  {
    const gp_Lin& ray = rb.Rays[iray];

    // Intersection parameter for sorting.
    double resultRayParam = RealLast();

    // Initialize facet index for a single-point test.
    int    facetId = -1;
    gp_XYZ hit;

    // Traverse BVH.
    for ( asiAlgo_BVHIterator it(bvh); it.More(); it.Next() )
    {
      const BVH_Vec4i& nodeData = it.Current();

      if ( it.IsLeaf() )
      {
        // If we are here, then we are close to solution. It is a right
        // time now to perform a precise check.

        int    facet_candidate = -1;
        double hitParam;
        gp_XYZ hitPoint;
        //
        const bool isHit = this->testLeaf(ray,
                                          ray_limit,
                                          nodeData,
                                          rb.FaceIndex, // Face to skip.
                                          facet_candidate,
                                          hitParam,
                                          hitPoint);
        //
        if ( isHit )
        {
          if ( hitParam < resultRayParam )
          {
            facetId        = facet_candidate;
            resultRayParam = hitParam;
            hit            = hitPoint;
          }
        }
      }
      else // sub-volume.
      {
        const BVH_Vec3d& minCorner_Left  = bvh->MinPoint( nodeData.y() );
        const BVH_Vec3d& maxCorner_Left  = bvh->MaxPoint( nodeData.y() );
        const BVH_Vec3d& minCorner_Right = bvh->MinPoint( nodeData.z() );
        const BVH_Vec3d& maxCorner_Right = bvh->MaxPoint( nodeData.z() );

        const bool isLeftOut  = this->isOut(ray, minCorner_Left, maxCorner_Left, prec);
        const bool isRightOut = this->isOut(ray, minCorner_Right, maxCorner_Right, prec);

        if ( isLeftOut )
          it.BlockLeft();
        if ( isRightOut )
          it.BlockRight();
      }
    }

    if ( facetId != -1 )
      return false;
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_FindVisibleFaces::testLeaf(const gp_Lin&    ray,
                                        const double     length,
                                        const BVH_Vec4i& leaf,
                                        const t_topoId   face2Skip,
                                        int&             resultFacet,
                                        double&          resultRayParamNormalized,
                                        gp_XYZ&          hitPoint) const
{
  // Prepare a segment of the ray to pass for intersection test.
  const gp_XYZ l0 = ray.Location().XYZ();
  const gp_XYZ l1 = l0 + ray.Direction().XYZ()*length;

  // Parameter on ray is used for intersection sorting. We are interested
  // in the nearest or the farthest point depending on the mode specified
  // by the user.
  resultFacet              = -1;
  resultRayParamNormalized = RealLast();

  // Loop over the tentative facets.
  for ( int fidx = leaf.y(); fidx <= leaf.z(); ++fidx )
  {
    // Get facet to test.
    const asiAlgo_BVHFacets::t_facet& facet = m_bvh->GetFacet(fidx);

    if ( facet.FaceIndex == face2Skip )
      continue; // Skip facet which is explicitly excluded from the intersection test.

    // Get next facet to test.
    const gp_XYZ p0( facet.P0.x(), facet.P0.y(), facet.P0.z() );
    const gp_XYZ p1( facet.P1.x(), facet.P1.y(), facet.P1.z() );
    const gp_XYZ p2( facet.P2.x(), facet.P2.y(), facet.P2.z() );

    // Hit test.
    double currentParam;
    gp_XYZ currentPoint;
    //
    if ( this->isIntersected(l0, l1, p0, p1, p2, currentParam, currentPoint) )
    {
      if ( currentParam < resultRayParamNormalized )
      {
        /*if ( !m_plotter.Access().IsNull() )
        {
          TCollection_AsciiString name("facet_");
          name += fidx;
          m_plotter.REDRAW_TRIANGLE(name, p0, p1, p2, Color_Red);
        }*/

        resultFacet              = fidx;
        resultRayParamNormalized = currentParam;
        hitPoint                 = currentPoint;
      }
    }
  }

  return (resultFacet != -1);
}

//-----------------------------------------------------------------------------

bool asiAlgo_FindVisibleFaces::isOut(const gp_Lin&    L,
                                     const BVH_Vec3d& boxMin,
                                     const BVH_Vec3d& boxMax,
                                     const double     prec) const
{
  double xmin = 0, xmax = 0, ymin = 0, ymax = 0, zmin = 0, zmax = 0;
  double parmin, parmax, par1, par2;
  bool   xToSet, yToSet;

  // Protection from degenerated bounding box.
  double myXmin = boxMin.x() - prec;
  double myYmin = boxMin.y() - prec;
  double myZmin = boxMin.z() - prec;
  double myXmax = boxMax.x() + prec;
  double myYmax = boxMax.y() + prec;
  double myZmax = boxMax.z() + prec;

  if ( Abs( L.Direction().XYZ().X() ) > 0.0 )
  {
    par1   = (myXmin - L.Location().XYZ().X()) / L.Direction().XYZ().X();
    par2   = (myXmax - L.Location().XYZ().X()) / L.Direction().XYZ().X();
    parmin = Min(par1, par2);
    parmax = Max(par1, par2);
    xToSet = true;
  }
  else
  {
    if ( L.Location().XYZ().X() < myXmin || myXmax < L.Location().XYZ().X() )
      return true;

    xmin   = L.Location().XYZ().X();
    xmax   = L.Location().XYZ().X();
    parmin = -1.0e100;
    parmax = 1.0e100;
    xToSet = false;
  }

  if ( Abs( L.Direction().XYZ().Y() ) > 0.0 )
  {
    par1 = ( myYmin - L.Location().XYZ().Y() ) / L.Direction().XYZ().Y();
    par2 = ( myYmax - L.Location().XYZ().Y() ) / L.Direction().XYZ().Y();

    if ( parmax < Min(par1, par2) || parmin > Max(par1, par2) )
      return true;

    parmin = Max(parmin, Min(par1, par2));
    parmax = Min(parmax, Max(par1, par2));
    yToSet = true;
  }
  else 
  {
    if ( L.Location().XYZ().Y() < myYmin || myYmax < L.Location().XYZ().Y() )
      return true;

    ymin   = L.Location().XYZ().Y();
    ymax   = L.Location().XYZ().Y();
    yToSet = false;
  }

  if ( Abs( L.Direction().XYZ().Z() ) > 0.0 )
  {
    par1 = (myZmin - L.Location().XYZ().Z()) / L.Direction().XYZ().Z();
    par2 = (myZmax - L.Location().XYZ().Z()) / L.Direction().XYZ().Z();

    if ( parmax < Min(par1, par2) || parmin > Max(par1, par2) )
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
    if ( L.Location().XYZ().Z() < myZmin || myZmax < L.Location().XYZ().Z() )
      return true;

    zmin = L.Location().XYZ().Z();
    zmax = L.Location().XYZ().Z();
  }
  if ( zmax < myZmin || myZmax < zmin )
    return true;

  if ( xToSet )
  {
    par1 = L.Location().XYZ().X() + parmin*L.Direction().XYZ().X();
    par2 = L.Location().XYZ().X() + parmax*L.Direction().XYZ().X();
    xmin = Min(par1, par2);
    xmax = Max(par1, par2);
  }
  if ( xmax < myXmin || myXmax < xmin )
    return true;

  if ( yToSet )
  {
    par1 = L.Location().XYZ().Y() + parmin*L.Direction().XYZ().Y();
    par2 = L.Location().XYZ().Y() + parmax*L.Direction().XYZ().Y();
    ymin = Min(par1, par2);
    ymax = Max(par1, par2);
  }
  if ( ymax < myYmin || myYmax < ymin )
    return true;

  return false;
}

//-----------------------------------------------------------------------------

bool asiAlgo_FindVisibleFaces::isIntersected(const gp_XYZ& rayStart,
                                             const gp_XYZ& rayFinish,
                                             const gp_XYZ& pntTri1,
                                             const gp_XYZ& pntTri2,
                                             const gp_XYZ& pntTri3,
                                             double&       hitParamNormalized,
                                             gp_XYZ&       hitPoint) const
{
  // Moller?Trumbore intersection algorithm
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
