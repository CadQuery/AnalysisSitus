//-----------------------------------------------------------------------------
// Created on: 22 August 2018
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
#include <asiAlgo_InterpolateSurfMesh.h>

// asiAlgo includes
#include <asiAlgo_ClassifyPointFace.h>
#include <asiAlgo_HitFacet.h>
#include <asiAlgo_PlaneOnPoints.h>

#ifdef USE_MOBIUS
  // Mobius includes
  #include <mobius/cascade_BSplineSurface.h>
  #include <mobius/geom_InterpolateSurface.h>
#endif

// OCCT includes
#include <Bnd_Box.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <Geom_Plane.hxx>
#include <GeomLib.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_Face.hxx>

// Standard includes
#include <algorithm>

#define DRAW_DEBUG
#if defined DRAW_DEBUG
  #pragma message("===== warning: DRAW_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

void RemoveCoincidentPoints(const double                                          prec,
                            std::vector<asiAlgo_InterpolateSurfMesh::t_gridNode>& IntListNC,
                            TColStd_PackedMapOfInteger&                           keptIndices)
{
  std::vector<asiAlgo_InterpolateSurfMesh::t_gridNode> purifiedList;
  int ip = 0;
  do
  {
    const asiAlgo_InterpolateSurfMesh::t_gridNode& p = IntListNC[ip];

    // Traverse till the end skipping all coincident points
    int jp = ip + 1, p_best_idx = ip;
    asiAlgo_InterpolateSurfMesh::t_gridNode p_next, p_best = p;
    for ( ; ; )
    {
      ip = jp;

      if ( jp >= IntListNC.size() )
        break;

      p_next = IntListNC[jp++];

      if ( (p_next.uvInit - p.uvInit).Modulus() > prec )
        break;
    }

    keptIndices.Add(p_best_idx);
    purifiedList.push_back(p_best);
  }
  while ( ip < IntListNC.size() );

  // Override the passed list
  IntListNC = purifiedList;
}

//-----------------------------------------------------------------------------

bool appendNodeInGlobalCollection(asiAlgo_InterpolateSurfMesh::t_gridNode&                          node,
                                  int&                                                              globalNodeId,
                                  std::vector<asiAlgo_InterpolateSurfMesh::t_gridNode>&             allNodes,
                                  NCollection_CellFilter<asiAlgo_InterpolateSurfMesh::InspectNode>& NodeFilter)
{
  const double prec = Precision::Confusion();
  asiAlgo_InterpolateSurfMesh::InspectNode Inspect(prec, node);
  gp_XY XY_min = Inspect.Shift( node.uvInit, -prec );
  gp_XY XY_max = Inspect.Shift( node.uvInit,  prec );

  // Coincidence test.
  NodeFilter.Inspect(XY_min, XY_max, Inspect);
  const bool isFound = Inspect.IsFound();
  //
  if ( !isFound )
  {
    node.id = globalNodeId;
    //
    NodeFilter.Add(node, node.uvInit);
    allNodes.push_back(node);

    // (!!!) Increment global node ID.
    ++globalNodeId;

    return true; // Added.
  }

  return false; // Not added.
}

//-----------------------------------------------------------------------------

bool asiAlgo_InterpolateSurfMesh::CollectInteriorNodes(const Handle(Poly_Triangulation)&  tris,
                                                       const std::vector<gp_XYZ>&         contour,
                                                       const bool                         boxClipping,
                                                       Handle(asiAlgo_BaseCloud<double>)& pts,
                                                       ActAPI_ProgressEntry               progress,
                                                       ActAPI_PlotterEntry                plotter)
{
  // Working variables.
  Handle(Geom_Plane) plane;     // Average plane.
  int                curID = 0; // Index of the last added points.
  double             size = 0.; // Characteristics size.

  // Cell filter.
  NCollection_CellFilter<InspectNode> NodeFilter( Precision::Confusion() );

  // Get all mesh nodes bounded by the contour.
  std::vector<t_gridNode> reperNodes;
  //
  if ( !collectInteriorNodes(tris, contour, boxClipping,
                             NodeFilter, plane, reperNodes, curID, size,
                             progress, plotter) )
  {
    progress.SendLogMessage(LogErr(Normal) << "Cannot collect interior mesh nodes.");
    return false;
  }

  // Construct the result.
  pts = new asiAlgo_BaseCloud<double>;
  //
  for ( size_t k = 0; k < reperNodes.size(); ++k )
    pts->AddElement(reperNodes[k].xyzInit);

  return true;
}

//-----------------------------------------------------------------------------

asiAlgo_InterpolateSurfMesh::asiAlgo_InterpolateSurfMesh(const Handle(Poly_Triangulation)& tris,
                                                         ActAPI_ProgressEntry              progress,
                                                         ActAPI_PlotterEntry               plotter)
: ActAPI_IAlgorithm (progress, plotter),
  m_tris            (tris)
{
}

//-----------------------------------------------------------------------------

bool asiAlgo_InterpolateSurfMesh::Perform(const std::vector<gp_XYZ>&   contour,
                                          const double                 grainCoeff,
                                          const int                    degU,
                                          const int                    degV,
                                          Handle(Geom_BSplineSurface)& result)
{
  if ( m_bvh.IsNull() )
    m_bvh = new asiAlgo_BVHFacets(m_tris);

  return this->performInternal(contour, grainCoeff, degU, degV, result);
}

//-----------------------------------------------------------------------------

bool asiAlgo_InterpolateSurfMesh::performInternal(const std::vector<gp_XYZ>&   contour,
                                                  const double                 grainCoeff,
                                                  const int                    degU,
                                                  const int                    degV,
                                                  Handle(Geom_BSplineSurface)& result)
{
#ifdef USE_MOBIUS
  // Average plane.
  Handle(Geom_Plane) plane;

  // Index of the last added points.
  int curID = 0;

  // Characteristics size.
  double size = 0.;

  // Cell filter.
  NCollection_CellFilter<InspectNode> NodeFilter( Precision::Confusion() );

  // Get all mesh nodes bounded by the contour.
  std::vector<t_gridNode> allNodes, reperNodes;
  //
  if ( !collectInteriorNodes(m_tris, contour, true,
                             NodeFilter, plane, reperNodes, curID, size,
                             m_progress, m_plotter) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot collect interior mesh nodes.");
    return false;
  }

  /* ====================================================================
   *  For each image point, generate all possible variants of grid nodes
   * ==================================================================== */

  for ( size_t k = 0; k < reperNodes.size(); ++k )
  {
    const gp_XY& uv_k = reperNodes[k].uvInit;
    const double u_k  = uv_k.X();
    const double v_k  = uv_k.Y();

    for ( size_t s = 0; s < reperNodes.size(); ++s )
    {
      if ( s == k ) continue;

      const gp_XY& uv_s = reperNodes[s].uvInit;
      const double u_s  = uv_s.X();
      const double v_s  = uv_s.Y();

      // Prepare a grid node structure.
      {
        t_gridNode node;
        node.isContourPoint  = false;
        node.isSampledPoint  = true;
        node.isMeshNodePoint = false;
        node.uvInit          = gp_XY(u_k, v_s);
        node.xyzInit         = plane->Value( node.uvInit.X(), node.uvInit.Y() ).XYZ();

        // Add node.
        ::appendNodeInGlobalCollection(node, curID, allNodes, NodeFilter);
      }

      // Prepare a grid node structure.
      {
        t_gridNode node;
        node.isContourPoint  = false;
        node.isSampledPoint  = true;
        node.isMeshNodePoint = false;
        node.uvInit          = gp_XY(u_s, v_k);
        node.xyzInit         = plane->Value(node.uvInit.X(), node.uvInit.Y()).XYZ();

        // Add node
        ::appendNodeInGlobalCollection(node, curID, allNodes, NodeFilter);
      }
    }
  }

  /* =============================================================
   *  Sort nodes to get a structured grid (make topology of grid)
   * ============================================================= */

  TColStd_PackedMapOfInteger visited;

  std::vector< std::vector<t_gridNode> > uvGridNodes;

  const double prec = RealEpsilon();
  bool stop = false;
  do
  {
    // Find minimum V coordinate.
    double minV = DBL_MAX;
    //
    for ( int i = 0; i < int( allNodes.size() ); ++i )
    {
      if ( visited.Contains(i) )
        continue;

      const double currV = allNodes[i].uvInit.Y();
      //
      if ( currV < minV )
        minV = currV;
    }

    if ( minV == DBL_MAX )
      stop = true;
    else
    {
      std::vector<t_gridNode> vIsoNodes; // Fixed level of V.

      // Collect all points with V coordinate equal to the passed value.
      for ( int i = 0; i < int( allNodes.size() ); ++i )
      {
        if ( visited.Contains(i) )
          continue;

        const double currV = allNodes[i].uvInit.Y();
        //
        if ( Abs(currV - minV) < prec )
        {
          vIsoNodes.push_back(allNodes[i]);
          visited.Add(i);
        }
      }

      // Sort by value of U coordinate.
      std::sort( vIsoNodes.begin(), vIsoNodes.end() );

      // Add to grid.
      uvGridNodes.push_back(vIsoNodes);
    }
  }
  while ( !stop );

  // Sparse rows (in U direction)
  std::vector< std::vector<t_gridNode> > uvGridNodesSparsedU;
  //
  double coincConf = size*grainCoeff;
  {
    TColStd_PackedMapOfInteger keptIndices;
    RemoveCoincidentPoints(coincConf, uvGridNodes[0], keptIndices);
    //
    uvGridNodesSparsedU.push_back(uvGridNodes[0]);
    //
    for ( int i = 1; i < int( uvGridNodes.size() ); ++i )
    {
      std::vector<t_gridNode> sparsedIso;
      for ( int j = 0; j < int( uvGridNodes[i].size() ); ++j )
      {
        if ( keptIndices.Contains(j) )
          sparsedIso.push_back(uvGridNodes[i][j]);
      }
      uvGridNodesSparsedU.push_back(sparsedIso);
    }
  }

  // Sparse cols (in V direction)
  std::vector< std::vector<t_gridNode> > uvGridNodesSparsed;
  //
  std::vector<t_gridNode> col;
  //
  for ( int i = 0; i < int( uvGridNodesSparsedU.size() ); ++i )
  {
    col.push_back(uvGridNodesSparsedU[i][0]);
  }
  //
  {
    TColStd_PackedMapOfInteger keptIndices;
    RemoveCoincidentPoints(coincConf, col, keptIndices);
    //
    for ( int i = 0; i < int( uvGridNodesSparsedU.size() ); ++i )
    {
      if ( keptIndices.Contains(i) )
        uvGridNodesSparsed.push_back(uvGridNodesSparsedU[i]);
    }
  }

  /*for ( size_t i = 0; i < uvGridNodesSparsed.size(); ++i )
  {
    for ( size_t j = 0; j < uvGridNodesSparsed[i].size(); ++j )
    {
      m_plotter.DRAW_POINT(uvGridNodesSparsed[i][j].uvInit,  uvGridNodesSparsed[i][j].isContourPoint ? Color_Red : Color_Blue, "uvInit");
      m_plotter.DRAW_POINT(uvGridNodesSparsed[i][j].xyzInit, uvGridNodesSparsed[i][j].isContourPoint ? Color_Red : Color_Blue, "xyzInit");
    }
  }*/

  /* =========================================================
   *  Correct 3D positions of sampled nodes according to mesh
   * ========================================================= */

  asiAlgo_HitFacet picker(m_bvh, m_progress, m_plotter);

  // Get norm of the plane.
  const gp_Dir& planeNorm = plane->Axis().Direction();

  for ( size_t i = 0; i < uvGridNodesSparsed.size(); ++i )
  {
    for ( size_t j = 0; j < uvGridNodesSparsed[i].size(); ++j )
    {
      gp_Lin ray1( uvGridNodesSparsed[i][j].xyzInit, planeNorm );
      gp_Lin ray2( uvGridNodesSparsed[i][j].xyzInit, planeNorm.Reversed() );

      int facetId1, facetId2;
      gp_XYZ xyz1, xyz2;
      //
      const bool isOk1 = picker(ray1, facetId1, xyz1);
      const bool isOk2 = picker(ray2, facetId2, xyz2);
      //
      if ( !isOk1 && !isOk2 )
      {
        m_progress.SendLogMessage(LogWarn(Normal) << "Cannot precise grid point.");

#if defined DRAW_DEBUG
        m_plotter.DRAW_POINT(uvGridNodesSparsed[i][j].xyzInit, Color_Red, "cannot_precise");
#endif
      }
      //
      if ( isOk1 && isOk2 )
      {
        // Take closest.
        const double d1 = (uvGridNodesSparsed[i][j].xyzInit - xyz1).Modulus();
        const double d2 = (uvGridNodesSparsed[i][j].xyzInit - xyz2).Modulus();
        //
        uvGridNodesSparsed[i][j].xyz = ( (d1 < d2) ? xyz1 : xyz2 );
      }
      else if ( isOk1 )
        uvGridNodesSparsed[i][j].xyz = xyz1;
      else if ( isOk2 )
        uvGridNodesSparsed[i][j].xyz = xyz2;
      else
        uvGridNodesSparsed[i][j].xyz = uvGridNodesSparsed[i][j].xyzInit;
    }
  }

#if defined DRAW_DEBUG
  Handle(HRealArray) pts = new HRealArray(0, int( uvGridNodesSparsed.size()*uvGridNodesSparsed[0].size() )*3 - 1);
  int ptidx = 0;
  //
  for ( size_t i = 0; i < uvGridNodesSparsed.size(); ++i )
  {
    for ( size_t j = 0; j < uvGridNodesSparsed[i].size(); ++j )
    {
      pts->SetValue( ptidx++, uvGridNodesSparsed[i][j].xyz.X() );
      pts->SetValue( ptidx++, uvGridNodesSparsed[i][j].xyz.Y() );
      pts->SetValue( ptidx++, uvGridNodesSparsed[i][j].xyz.Z() );
    }
  }
  m_plotter.DRAW_POINTS(pts, Color_Blue, "vIsoNodes_xyz");
#endif

  /* ==================
   *  Interpolate grid
   * ================== */

  // Get grid in Mobius form.
  std::vector< std::vector<mobius::t_xyz> > mobGrid;
  //
  for ( size_t i = 0; i < uvGridNodesSparsed.size(); ++i )
  {
    std::vector<mobius::t_xyz> isoPoints;

    for ( size_t j = 0; j < uvGridNodesSparsed[i].size(); ++j )
    {
      isoPoints.push_back( mobius::t_xyz( uvGridNodesSparsed[i][j].xyz.X(),
                                          uvGridNodesSparsed[i][j].xyz.Y(),
                                          uvGridNodesSparsed[i][j].xyz.Z() ) );
    }

    mobGrid.push_back(isoPoints);
  }

  const mobius::bspl_ParamsSelection paramsType = mobius::ParamsSelection_ChordLength;
  const mobius::bspl_KnotsSelection  knotsType  = mobius::KnotsSelection_Average;

  // Interpolate.
  mobius::geom_InterpolateSurface interpSurf(mobGrid, degU, degV, paramsType, knotsType);
  //
  if ( !interpSurf.Perform() )
  {
    m_progress.SendLogMessage( LogErr(Normal) << "Interpolation failed with error code %1."
                                              << interpSurf.GetErrorCode() );
    return false;
  }

  // Get Mobius surface.
  const mobius::t_ptr<mobius::t_bsurf>& mobSurf = interpSurf.GetResult();

  // Convert to OpenCascade surface.
  mobius::cascade_BSplineSurface toOpenCascade(mobSurf);
  toOpenCascade.DirectConvert();
  result = toOpenCascade.GetOpenCascadeSurface();

  // Extend in all parametric directions
  //GeomLib::ExtendSurfByLength(result, size*0.1, 1, 1, 0);
  //GeomLib::ExtendSurfByLength(result, size*0.1, 1, 1, 1);
  //GeomLib::ExtendSurfByLength(result, size*0.1, 1, 0, 0);
  //GeomLib::ExtendSurfByLength(result, size*0.1, 1, 0, 1);

#if defined DRAW_DEBUG
  m_plotter.DRAW_SURFACE(result, Color_White, "surf");
#endif

  return true;

#else
  asiAlgo_NotUsed(contour);
  asiAlgo_NotUsed(grainCoeff);
  asiAlgo_NotUsed(degU);
  asiAlgo_NotUsed(degV);
  asiAlgo_NotUsed(result);

  m_progress.SendLogMessage(LogErr(Normal) << "Mobius library is not available.");
  return false;
#endif
}

//-----------------------------------------------------------------------------

bool asiAlgo_InterpolateSurfMesh::collectInteriorNodes(const Handle(Poly_Triangulation)&    tris,
                                                       const std::vector<gp_XYZ>&           contour,
                                                       const bool                           boxClipping,
                                                       NCollection_CellFilter<InspectNode>& filter,
                                                       Handle(Geom_Plane)&                  avrPlane,
                                                       std::vector<t_gridNode>&             pts,
                                                       int&                                 lastPtIdx,
                                                       double&                              size,
                                                       ActAPI_ProgressEntry                 progress,
                                                       ActAPI_PlotterEntry                  plotter)
{
#ifdef USE_MOBIUS
  // Get center point and bounding box of the contour.
  gp_XYZ  P_center;
  Bnd_Box contourAABB;
  //
  for ( size_t k = 0; k < contour.size(); ++k )
  {
    P_center += contour[k];
    contourAABB.Update( contour[k].X(), contour[k].Y(), contour[k].Z() );
  }
  //
  P_center /= int( contour.size() );

  // Build average plane.
  gp_Pln pln;
  asiAlgo_PlaneOnPoints planeBuilder(progress, plotter);
  //
  if ( !planeBuilder.Build(contour, pln) )
  {
    progress.SendLogMessage(LogErr(Normal) << "Cannot build average plane.");
    return false;
  }
  //
  avrPlane = new Geom_Plane(pln);

  // Get characteristic size.
  size = 0.0;
  //
  for ( size_t k = 0; k < contour.size(); ++k )
    size = Max( size, (contour[k] - P_center).Modulus() );
  //
  size *= 1.1;

#if defined DRAW_DEBUG
  plotter.DRAW_SURFACE(avrPlane, -size, size, -size, size, Color_Green, 0.5, "avrgPlane");

  if ( boxClipping )
  {
    // Get bounding box of the contour.
    double xMin, yMin, zMin, xMax, yMax, zMax;
    contourAABB.Get(xMin, yMin, zMin, xMax, yMax, zMax);
    //
    TopoDS_Shape bndbox = BRepPrimAPI_MakeBox( gp_Pnt(xMin, yMin, zMin), gp_Pnt(xMax, yMax, zMax) );
    //
    plotter.DRAW_SHAPE(bndbox, Color_Yellow, 1.0, true, "contourAABB");
  }
#endif

  /* ==============================================================
   *  Get image of the contour in (U,V) space of the average plane
   * ============================================================== */

  // Project points on surface.
  for ( size_t k = 0; k < contour.size(); ++k )
  {
    ShapeAnalysis_Surface SAS(avrPlane);
    gp_Pnt2d UV = SAS.ValueOfUV(contour[k], 1.0e-1);

#if defined DRAW_DEBUG
    plotter.DRAW_POINT(UV,                      Color_Yellow, "UV");
    plotter.DRAW_POINT(gp_XY( UV.X(), 0      ), Color_Red,    "U");
    plotter.DRAW_POINT(gp_XY( 0,      UV.Y() ), Color_Green,  "V");

    plotter.DRAW_LINK(gp_XY( UV.X(), 0      ), gp_XY( UV.X(),  size   ), Color_White, "U_iso_p");
    plotter.DRAW_LINK(gp_XY( UV.X(), 0      ), gp_XY( UV.X(), -size   ), Color_White, "U_iso_m");
    plotter.DRAW_LINK(gp_XY( 0,      UV.Y() ), gp_XY( size,    UV.Y() ), Color_White, "V_iso_p");
    plotter.DRAW_LINK(gp_XY( 0,      UV.Y() ), gp_XY(-size,    UV.Y() ), Color_White, "V_iso_m");
#endif

    // Prepare a grid node structure.
    t_gridNode node;
    node.isContourPoint  = true;
    node.isSampledPoint  = false;
    node.isMeshNodePoint = false;
    node.uvInit          = UV.XY();
    node.xyzInit         = contour[k];

    // Add node.
    ::appendNodeInGlobalCollection(node, lastPtIdx, pts, filter);
  }

  ShapeExtend_WireData mkWire;
  for ( size_t k = 1; k < pts.size(); ++k )
  {
    mkWire.Add( BRepBuilderAPI_MakeEdge(GCE2d_MakeSegment(pts[k-1].uvInit, pts[k].uvInit).Value(), avrPlane).Edge() );
  }
  mkWire.Add( BRepBuilderAPI_MakeEdge(GCE2d_MakeSegment(pts[pts.size()-1].uvInit, pts[0].uvInit).Value(), avrPlane).Edge() );
  //
  const TopoDS_Wire& W = mkWire.WireAPIMake();

  TopoDS_Face F = BRepBuilderAPI_MakeFace(avrPlane->Pln(), W, false);
  //
  ShapeFix_Face ShapeHealer(F);
  ShapeHealer.Perform();
  F = ShapeHealer.Face();

#if defined DRAW_DEBUG
  plotter.REDRAW_SHAPE("W", W, Color_Red, 1.0, true);
  plotter.REDRAW_SHAPE("F", F);
#endif

  /* ====================================
   *  Add points representing mesh nodes
   * ==================================== */

  asiAlgo_ClassifyPointFace classifier(F, 1e-4, 1e-4);

  const TColgp_Array1OfPnt& triNodes = tris->Nodes();

  for ( int k = triNodes.Lower(); k <= triNodes.Upper(); ++k )
  {
    const gp_Pnt& triNode = triNodes(k);
    //
    if ( boxClipping && contourAABB.IsOut(triNode) )
      continue;

    ShapeAnalysis_Surface SAS(avrPlane);
    gp_Pnt2d UV = SAS.ValueOfUV(triNode, 1.0e-1);

    // Classify against the two-dimensional contour.
    if ( classifier(UV) != Membership_In )
      continue;

    // Prepare a grid node structure.
    t_gridNode node;
    node.isContourPoint  = false;
    node.isSampledPoint  = false;
    node.isMeshNodePoint = true;
    node.uvInit          = UV.XY();
    node.xyzInit         = triNode.XYZ();

    // Add node.
    ::appendNodeInGlobalCollection(node, lastPtIdx, pts, filter);
  }

#if defined DRAW_DEBUG
  for ( size_t k = 0; k < pts.size(); ++k )
  {
    plotter.DRAW_POINT(pts[k].uvInit,  pts[k].isContourPoint ? Color_Green : Color_Blue, "uvInit");
    plotter.DRAW_POINT(pts[k].xyzInit, pts[k].isContourPoint ? Color_Green : Color_Blue, "xyzInit");
  }
#endif

  return true;
#else
  asiAlgo_NotUsed(contour);
  asiAlgo_NotUsed(pts);

  m_progress.SendLogMessage(LogErr(Normal) << "Mobius library is not available.");
  return false;
#endif
}
