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

// Own include
#include <asiAlgo_ProjectPointOnMesh.h>

// asiAlgo includes
#include <asiAlgo_BVHIterator.h>

// OCCT includes
#include <Precision.hxx>

// Standard includes
#include <limits>
#include <set>

//-----------------------------------------------------------------------------

const static double TOLER   = Precision::Confusion();
const static double SQTOLER = TOLER * TOLER;

//-----------------------------------------------------------------------------

typedef int BVH_StackItem;

//! Projection information.
struct ProjectionInfoMesh
{
  //! index of the triangle.
  std::vector<int> myTriIdx;

  //! Actual index to fill.
  int myIdx;

  //! Resulting projected point.
  gp_Pnt myProjectedPoint;

  //! Current squared distance.
  double mySqDistance;

  //! Constructor. Sets default values.
  ProjectionInfoMesh()
    : myTriIdx(10),
      myIdx(-1),
      myProjectedPoint(Precision::Infinite(), Precision::Infinite(), Precision::Infinite()),
      mySqDistance(Precision::Infinite())
  {}

  //! Updates values.
  void UpdateValues(const gp_Pnt thePnt,
                    const double theSqDistance,
                    const int theIdx)
  {
    if ( theSqDistance > mySqDistance + SQTOLER )
      return;

    else if ( theSqDistance < mySqDistance - SQTOLER )
    {
      myProjectedPoint = thePnt;
      mySqDistance     = theSqDistance;

      myIdx           = 0;
      myTriIdx[myIdx] = theIdx;
    }
    else
    {
      myProjectedPoint = thePnt;
      mySqDistance     = theSqDistance;

      ++myIdx;
      if ( myIdx < int( myTriIdx.size() ) )
        myTriIdx[myIdx] = theIdx;
      else
        myTriIdx.push_back(theIdx);
    }
  }
};

//-----------------------------------------------------------------------------

static void projectPntToTriangle(const gp_Pnt&                     P,
                                 const asiAlgo_BVHFacets::t_facet& facet,
                                 ProjectionInfoMesh&               projection,
                                 const int                         idx)
{
  gp_Pnt T1( facet.P0.x(), facet.P0.y(), facet.P0.z() );
  gp_Pnt T2( facet.P1.x(), facet.P1.y(), facet.P1.z() );
  gp_Pnt T3( facet.P2.x(), facet.P2.y(), facet.P2.z() );

  gp_Pnt proj;
  const double sqDist = asiAlgo_BVHAlgo::squaredDistancePointTriangle(P, T1, T2, T3, facet.N, proj);
  projection.UpdateValues(proj, sqDist, idx);
}

//-----------------------------------------------------------------------------

asiAlgo_ProjectPointOnMesh::asiAlgo_ProjectPointOnMesh(const Handle(asiAlgo_BVHFacets)& facets,
                                                       ActAPI_ProgressEntry             progress,
                                                       ActAPI_PlotterEntry              plotter)
: asiAlgo_BVHAlgo(facets, progress, plotter)
{}

//-----------------------------------------------------------------------------

void asiAlgo_ProjectPointOnMesh::IntersectLeaves(const BVH_Vec4i&    leaf,
                                                 const gp_Pnt&       sourcePoint,
                                                 ProjectionInfoMesh& projection) const
{
  // Loop over the tentative facets.
  for ( int facetIdx = leaf.y(); facetIdx <= leaf.z(); ++facetIdx )
  {
    // Get facet.
    const asiAlgo_BVHFacets::t_facet& facet = m_facets->GetFacet(facetIdx);

    // Calculate projection and minimal distance between point and triangle.
    projectPntToTriangle(sourcePoint, facet, projection, facetIdx);
  }
}

//-----------------------------------------------------------------------------

gp_Pnt asiAlgo_ProjectPointOnMesh::Perform(const gp_Pnt& P)
{
  if ( m_facets.IsNull() )
    return P;

  ProjectionInfoMesh projected;

  const opencascade::handle<BVH_Tree<double, 3> >& BVH = m_facets->BVH();
  //
  if ( BVH.IsNull() )
    return projected.myProjectedPoint;

  // Compute initial approximation of the solution.
  const int size = m_facets->Size();
  int ids[7] = { 0, size / 6, size / 3, size / 2, (size * 2) / 3, (size * 5) / 6, size - 1 };
  for ( int i = 0; i < 7; ++i )
  {
    const asiAlgo_BVHFacets::t_facet& TD = m_facets->GetFacet(ids[i]);

    // Calculate projection and minimal distance between point and triangle.
    projectPntToTriangle(P, TD, projected, ids[i]);
  }

  // Perform filtering over BVH.
  for ( asiAlgo_BVHIterator it(BVH); it.More(); it.Next() )
  {
    const BVH_Vec4i& nodeData = it.Current();

    if ( it.IsLeaf() )
    {
      // Perform precise test.
      this->IntersectLeaves(nodeData, P, projected);
    }
    else // sub-volume
    {
      const BVH_Vec3d& minPntLft1 = BVH->MinPoint( nodeData.y() );
      const BVH_Vec3d& maxPntLft1 = BVH->MaxPoint( nodeData.y() );
      const BVH_Vec3d& minPntRgh1 = BVH->MinPoint( nodeData.z() );
      const BVH_Vec3d& maxPntRgh1 = BVH->MaxPoint( nodeData.z() );

      const bool out1 = isOut( minPntLft1, maxPntLft1, P, Sqrt(projected.mySqDistance) );
      const bool out2 = isOut( minPntRgh1, maxPntRgh1, P, Sqrt(projected.mySqDistance) );

      if ( out1 )
        it.BlockLeft();

      if ( out2 )
        it.BlockRight();
    }
  }

  m_facetIds.clear();
  for ( int i = 0; i <= projected.myIdx; ++i )
    m_facetIds.push_back(projected.myTriIdx[i]);

  return projected.myProjectedPoint;
}
