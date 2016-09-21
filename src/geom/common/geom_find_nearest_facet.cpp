//-----------------------------------------------------------------------------
// Created on: 21 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_find_nearest_facet.h>

#define DRAW_DEBUG
#if defined DRAW_DEBUG
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#endif

//-----------------------------------------------------------------------------

//! ctor accepting facets in form of accelerating structure. Initialized once,
//! this utility may perform multiple tests for different probe points.
//! \param[in] facets   BVH-based structure of facets to test.
//! \param[in] progress progress notifier.
//! \param[in] plotter  imperative plotter.
geom_find_nearest_facet::geom_find_nearest_facet(const Handle(geom_bvh_facets)& facets,
                                                 ActAPI_ProgressEntry           progress,
                                                 ActAPI_PlotterEntry            plotter)
: ActAPI_IAlgorithm(progress, plotter),
  m_facets(facets)
{}

// =======================================================================
// function : anonymous name space
// purpose  :
// =======================================================================
namespace
{
  struct StackItem
  {
    int Node1;

    StackItem (const int theNode1 = 0)
      : Node1 (theNode1)
    {
      //
    }
  };
}

// =======================================================================
// function : SegmentTriangleIntersection
// purpose  : static, directly check whether triangle intersects line segment or not.
// =======================================================================
bool SegmentTriangleIntersection( const gp_Pnt& rayStart,
                                  const gp_Pnt& rayFinish,
                                  const gp_Pnt& pntTri1,
                                  const gp_Pnt& pntTri2,
                                  const gp_Pnt& pntTri3)
{
  // Moller–Trumbore intersection algorithm
  // (T. Moller et al, Fast Minimum Storage Ray / Triangle Intersection)
  gp_Vec e1(pntTri1, pntTri2),
         e2(pntTri1, pntTri3);
  gp_Vec dir(rayStart, rayFinish);
  gp_Vec P = dir.Crossed(e2);

  // If determinant is near zero, ray lies in plane of triangle or ray is parallel to plane of triangle.
  double det = e1.Dot(P);
  if (Abs(det) < 1.0e-16)
  {
    return false;
  }
  double inv_det = 1.0 / det;

  gp_Vec T(pntTri1, rayStart);

  // Calculate u parameter and test bound.
  double u = T.Dot(P) * inv_det;
  // The intersection lies outside of the triangle.
  if(u < 0.0 || u > 1.0)
  {
    return false;
  }

  // Calculate V parameter and test bound.
  gp_Vec Q = T.Crossed(e1);
  double v = dir.Dot(Q) * inv_det;

  // The intersection lies outside of the triangle.
  if(v < 0.0 || u + v  > 1.0)
  {
    return false;
  }

  double t = e2.Dot(Q) * inv_det;

  if(t < 0.0 || t > 1.0) 
  {
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------

//! Performs narrow-phase distance testing for a BVH leaf.
//! \param[in] P    probe point.
//! \param[in] leaf leaf node of the BVH tree.
//! \return true in case of success, false -- otherwise.
bool geom_find_nearest_facet::testLeaf(const gp_Pnt&    P,
                                       const BVH_Vec4i& leaf) const
{
  for ( int fidx = leaf.y(); fidx <= leaf.z(); ++fidx )
  {
    // Get facet to test
    const geom_bvh_facets::t_facet& facet = m_facets->GetFacet(fidx);

    //const TopoDS_Shape& aTriangleSolid = myTriangleSet->GetProperties(aTrgIdx).Shape;

    //// Get line segment and parent solid.
    //const TopoDS_Shape& aSegmentSolid = myLineSet->GetPropertiesSolid(theLD.mySolidPropIdx).Shape;

    //  // Triangle and line segment should belong to different topological items.
    //  if (aTriangleSolid.IsSame(aSegmentSolid))
    //    continue;

    //  gp_Pnt aTrgPnt0(aTD.myPnt0.x(), aTD.myPnt0.y(), aTD.myPnt0.z());
    //  gp_Pnt aTrgPnt1(aTD.myPnt1.x(), aTD.myPnt1.y(), aTD.myPnt1.z());
    //  gp_Pnt aTrgPnt2(aTD.myPnt2.x(), aTD.myPnt2.y(), aTD.myPnt2.z());
    //  gp_Pnt aLinePnt0(theLD.myPntStart.x(), theLD.myPntStart.y(), theLD.myPntStart.z());
    //  gp_Pnt aLinePnt1(theLD.myPntFinish.x(), theLD.myPntFinish.y(), theLD.myPntFinish.z());

    //  if (SegmentTriangleIntersection(aLinePnt0, aLinePnt1, aTrgPnt0, aTrgPnt1, aTrgPnt2))
    //    return true;

    return false;
  }

  return false;
}

//-----------------------------------------------------------------------------

bool geom_find_nearest_facet::isOut(const gp_Pnt&    P,
                                    const BVH_Vec4d& boxMin,
                                    const BVH_Vec4d& boxMax) const
{
  const double x = P.X(),
               y = P.Y(),
               z = P.Z();
  const double xmin = boxMin.x(),
               ymin = boxMin.y(),
               zmin = boxMin.z(),
               xmax = boxMax.x(),
               ymax = boxMax.y(),
               zmax = boxMax.z();

  if ( x < xmin || y < ymin || z < zmin ||
       x > xmax || y > ymax || z > zmax )
    return true;

  return false;
}

//-----------------------------------------------------------------------------

//! Performs nearest position check.
//! \param[in]  P           probe point.
//! \param[out] facet_index nearest facet.
//! \return true in case of success, false -- otherwise.
bool geom_find_nearest_facet::operator()(const gp_Pnt& P,
                                         int&          facet_index)
{
  // Stored data of each BVH node has the following meaning:
  // ========================================================
  //  coord |         leaf        |        sub-volume       |
  // ========================================================
  //    x   |          !0         |             0           |
  // -------+---------------------+-------------------------+
  //    y   | idx of start grain  | idx of left child node  |
  // -------+---------------------+-------------------------+
  //    z   |  idx of end grain   | idx of right child node |
  // ========================================================
  //

  const NCollection_Handle<BVH_Tree<double, 4> >& bvh = m_facets->BVH();
  if ( bvh.IsNull() )
    return false;

  // Line segment-BVH intersection.
  //const LineData& aLD = myLineSet->GetLineSegment(anIdx);
  //const BVH_Vec4d& aPntS = aLD.myPntStart;
  //const BVH_Vec4d& aPntF = aLD.myPntFinish;
  //gp_Vec aVec(gp_Pnt(aPntS.x(), aPntS.y(), aPntS.z()),
  //            gp_Pnt(aPntF.x(), aPntF.y(), aPntF.z()));
  //gp_Lin L(gp_Pnt(aPntS.x(), aPntS.y(), aPntS.z()), aVec);

  TopoDS_Compound comp_vol, comp_leaf;
  BRep_Builder BB;
  BB.MakeCompound(comp_vol);
  BB.MakeCompound(comp_leaf);

  StackItem aStack[96];
  int aHead = -1;
  StackItem aNodes;
  StackItem anItemsToProcess[2];
  for (;;)
  {
    BVH_Vec4i nodeData = bvh->NodeInfoBuffer()[aNodes.Node1];

    if ( nodeData.x() != 0 ) // leaf
    {
      // If we are here, then we are close to solution. It is a right
      // time now to perform a precise check
      const bool isIntFound = this->testLeaf(P, nodeData);
      if(isIntFound)
      {
        //++myNbIntersected;
        break;
      }

      if (aHead < 0)
      {
        break;
      }

      aNodes = aStack[aHead--];
    }
    else // sub-volume
    {
      int anItemsCount = 0;

      const BVH_Vec4d& minCorner_Left  = bvh->MinPoint( nodeData.y() );
      const BVH_Vec4d& maxCorner_Left  = bvh->MaxPoint( nodeData.y() );
      const BVH_Vec4d& minCorner_Right = bvh->MinPoint( nodeData.z() );
      const BVH_Vec4d& maxCorner_Right = bvh->MaxPoint( nodeData.z() );

      bool aOut1 = this->isOut(P, minCorner_Left, maxCorner_Left);
      bool aOut2 = this->isOut(P, minCorner_Right, maxCorner_Right);

      if (!aOut1)
        anItemsToProcess[anItemsCount++] = StackItem( nodeData.y() );

      if (!aOut2)
        anItemsToProcess[anItemsCount++] = StackItem( nodeData.z() );

      if (anItemsCount > 0)
      {
#if defined DRAW_DEBUG
        if (!aOut1)
        {
          gp_Pnt Pmin( minCorner_Left.x(), minCorner_Left.y(), minCorner_Left.z() );
          gp_Pnt Pmax( maxCorner_Left.x(), maxCorner_Left.y(), maxCorner_Left.z() );

          const gp_Pnt P2 = gp_Pnt( Pmax.X(), Pmin.Y(), Pmin.Z() );
          const gp_Pnt P3 = gp_Pnt( Pmax.X(), Pmax.Y(), Pmin.Z() );
          const gp_Pnt P4 = gp_Pnt( Pmin.X(), Pmax.Y(), Pmin.Z() );
          const gp_Pnt P5 = gp_Pnt( Pmin.X(), Pmin.Y(), Pmax.Z() );
          const gp_Pnt P6 = gp_Pnt( Pmax.X(), Pmin.Y(), Pmax.Z() );
          const gp_Pnt P8 = gp_Pnt( Pmin.X(), Pmax.Y(), Pmax.Z() );

          if ( Pmin.Distance(P2) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(Pmin, P2) );

          if ( P2.Distance(P3) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(P2,   P3) );

          if ( P3.Distance(P4) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(P3,   P4) );

          if ( P4.Distance(Pmin) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(P4,   Pmin) );

          if ( P5.Distance(P6) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(P5,   P6) );

          if ( P6.Distance(Pmax) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(P6,   Pmax) );

          if ( Pmax.Distance(P8) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(Pmax, P8) );

          if ( P8.Distance(P5) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(P8,   P5) );

          if ( P6.Distance(P2) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(P6,   P2) );

          if ( Pmax.Distance(P3) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(Pmax, P3) );

          if ( P8.Distance(P4) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(P8,   P4) );

          if ( P5.Distance(Pmin) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(P5,   Pmin) );
        }
        if (!aOut2)
        {
          gp_Pnt Pmin( minCorner_Right.x(), minCorner_Right.y(), minCorner_Right.z() );
          gp_Pnt Pmax( maxCorner_Right.x(), maxCorner_Right.y(), maxCorner_Right.z() );

          const gp_Pnt P2 = gp_Pnt( Pmax.X(), Pmin.Y(), Pmin.Z() );
          const gp_Pnt P3 = gp_Pnt( Pmax.X(), Pmax.Y(), Pmin.Z() );
          const gp_Pnt P4 = gp_Pnt( Pmin.X(), Pmax.Y(), Pmin.Z() );
          const gp_Pnt P5 = gp_Pnt( Pmin.X(), Pmin.Y(), Pmax.Z() );
          const gp_Pnt P6 = gp_Pnt( Pmax.X(), Pmin.Y(), Pmax.Z() );
          const gp_Pnt P8 = gp_Pnt( Pmin.X(), Pmax.Y(), Pmax.Z() );

          if ( Pmin.Distance(P2) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(Pmin, P2) );

          if ( P2.Distance(P3) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(P2,   P3) );

          if ( P3.Distance(P4) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(P3,   P4) );

          if ( P4.Distance(Pmin) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(P4,   Pmin) );

          if ( P5.Distance(P6) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(P5,   P6) );

          if ( P6.Distance(Pmax) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(P6,   Pmax) );

          if ( Pmax.Distance(P8) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(Pmax, P8) );

          if ( P8.Distance(P5) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(P8,   P5) );

          if ( P6.Distance(P2) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(P6,   P2) );

          if ( Pmax.Distance(P3) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(Pmax, P3) );

          if ( P8.Distance(P4) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(P8,   P4) );

          if ( P5.Distance(Pmin) > 1.0e-6 )
            BB.Add( comp_vol, BRepBuilderAPI_MakeEdge(P5,   Pmin) );
        }
#endif

        aNodes = anItemsToProcess[0];

        for (int anIdx = 1; anIdx < anItemsCount; ++anIdx)
          aStack[++aHead] = anItemsToProcess[anIdx];
      }
      else
      {
        if (aHead < 0)
          break;

        aNodes = aStack[aHead--];
      }
    }
  }

  this->Plotter().CLEAN();
  this->Plotter().DRAW_SHAPE(comp_vol, Color_Yellow, 1.0, true, "BVH");

  return true;
}
