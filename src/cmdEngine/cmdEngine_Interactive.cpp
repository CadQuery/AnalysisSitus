//-----------------------------------------------------------------------------
// Created on: 24 August 2017
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

// cmdEngine includes
#include <cmdEngine.h>

// asiEngine includes
#include <asiEngine_Part.h>
#include <asiEngine_Triangulation.h>

// asiUI includes
#include <asiUI_PickContourCallback.h>

// asiAlgo includes
#include <asiAlgo_HitFacet.h>
#include <asiAlgo_PlaneOnPoints.h>

// asiTcl includes
#include <asiTcl_PluginMacro.h>

// Mobius includes
#include <mobius/cascade_BSplineSurface.h>
#include <mobius/geom_InterpolateSurface.h>

// OCCT includes
#include <BRepBndLib.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom_Plane.hxx>
#include <ShapeAnalysis_Surface.hxx>

//-----------------------------------------------------------------------------

int ENGINE_StartContour(const Handle(asiTcl_Interp)& interp,
                        int                          argc,
                        const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part & Triangulaion Nodes.
  Handle(asiData_PartNode)          part_n = cmdEngine::model->GetPartNode();
  Handle(asiData_TriangulationNode) tris_n = cmdEngine::model->GetTriangulationNode();

  // Get Contour Node.
  Handle(asiData_ContourNode) contour_n = part_n->GetContour();
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "No persistent data for contour available.");
    return TCL_OK;
  }

  // Prepare data model.
  asiEngine_Part          partAPI ( cmdEngine::model, NULL, interp->GetProgress(), interp->GetPlotter() );
  asiEngine_Triangulation trisAPI ( cmdEngine::model, NULL, interp->GetProgress(), interp->GetPlotter() );
  //
  cmdEngine::model->OpenCommand();
  {
    // First we build BVH for facets (both part and CAD-agnostic mesh).
    partAPI.BuildBVH();
    trisAPI.BuildBVH();

    // Clear existing contour.
    contour_n->Init();
  }
  cmdEngine::model->CommitCommand();

  // Get Part presentation manager.
  const vtkSmartPointer<asiVisu_PrsManager>& PM = cmdEngine::cf->ViewerPart->PrsMgr();

  // Set picker type to world picker.
  cmdEngine::cf->ViewerPart->GetPickCallback()->SetPickerType(PickType_World);

  // Set selection mode.
  PM->SetSelectionMode(SelectionMode_Workpiece);

  // Add observer which takes responsibility to interact with the user.
  if ( !PM->HasObserver(EVENT_SELECT_WORLD_POINT) )
  {
    vtkSmartPointer<asiUI_PickContourCallback>
      cb = vtkSmartPointer<asiUI_PickContourCallback>::New();
    //
    cb->SetViewer ( cmdEngine::cf->ViewerPart );
    cb->SetModel  ( cmdEngine::model );
    //
    if ( !part_n->GetShape().IsNull() )
      cb->AddBVH( part_n->GetBVH() );
    if ( !tris_n->GetTriangulation().IsNull() )
      cb->AddBVH( tris_n->GetBVH() );
    //
    cb->SetDiagnosticTools ( interp->GetProgress(), interp->GetPlotter() );

    // Add observer.
    PM->AddObserver(EVENT_SELECT_WORLD_POINT, cb);
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_FinishContour(const Handle(asiTcl_Interp)& interp,
                         int                          argc,
                         const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Get Contour Node.
  Handle(asiData_ContourNode) contour_n = part_n->GetContour();
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "No persistent data for contour available.");
    return TCL_OK;
  }

  cmdEngine::model->OpenCommand();
  {
    // Finalize contour by setting it closed. Setting this flag does not
    // make any difference unless you ask the Contour Node to build a wire.
    contour_n->SetClosed(true);
  }
  cmdEngine::model->CommitCommand();

  // Get Part presentation manager.
  const vtkSmartPointer<asiVisu_PrsManager>& PM = cmdEngine::cf->ViewerPart->PrsMgr();

  // Set picker type to cell picker.
  cmdEngine::cf->ViewerPart->GetPickCallback()->SetPickerType(PickType_Cell);

  // Set selection mode.
  PM->SetSelectionMode(SelectionMode_Face);

  // Remove observer.
  PM->RemoveObserver(EVENT_SELECT_WORLD_POINT);

  // Update contour.
  PM->Actualize(contour_n);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_ContourToWire(const Handle(asiTcl_Interp)& interp,
                         int                          argc,
                         const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Get Contour Node.
  Handle(asiData_ContourNode) contour_n = part_n->GetContour();
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "No persistent data for contour available.");
    return TCL_OK;
  }

  // Ask Contour Node to return a wire.
  TopoDS_Wire contourGeom = contour_n->AsShape(false);

  // Draw it to pass to the Data Model.
  interp->GetPlotter().REDRAW_SHAPE(argv[1], contourGeom, Color_Red, 1.0, true);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_MakeContourPlane(const Handle(asiTcl_Interp)& interp,
                            int                          argc,
                            const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Get Contour Node.
  Handle(asiData_ContourNode) contour_n = part_n->GetContour();
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "No persistent data for contour available.");
    return TCL_OK;
  }

  // Get coordinates of the contour's poles.
  std::vector<gp_XYZ> points;
  gp_XYZ P_center;
  //
  contour_n->GetPoints(points, P_center);

  // Get characteristic size.
  double size = 0.0;
  //
  for ( size_t k = 0; k < points.size(); ++k )
    size = Max( size, (points[k] - P_center).Modulus() );
  //
  size *= 1.1;

  // Prepare utility to build average plane.
  gp_Pln plane;
  asiAlgo_PlaneOnPoints planeBuilder( interp->GetProgress(), interp->GetPlotter() );
  //
  if ( !planeBuilder.Build(points, plane) )
  {
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "Cannot build average plane.");
    return TCL_OK;
  }
  //
  interp->GetPlotter().REDRAW_SURFACE(argv[1], new Geom_Plane(plane), size, size, Color_Green, 0.5);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

#include <asiAlgo_ClassifyPointFace.h>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <NCollection_CellFilter.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_Face.hxx>

struct t_gridNode
{
  int    id;              //!< Global 0-based ID.
  bool   isContourPoint;  //!< Whether this point is a pole on a contour.
  bool   isSampledPoint;  //!< Whether this point is a sampled point.
  bool   isMeshNodePoint; //!< Whether this point corresponds to a mesh node.
  gp_XY  uvInit;          //!< (u,v) coordinates on the initial surface.
  gp_XYZ xyzInit;         //!< (x,y,z) coordinates on the initial surface.
  gp_XY  uv;              //!< (u,v) coordinates on the final surface.
  gp_XYZ xyz;             //!< (x,y,z) coordinates on the final surface.

  //! Default ctor.
  t_gridNode()
  : id              (-1),
    isContourPoint  (false),
    isSampledPoint  (false),
    isMeshNodePoint (false)
  {}

  bool operator<(const t_gridNode& other)
  {
    return this->uvInit.X() < other.uvInit.X();
  }
};

void RemoveCoincidentPoints(const double                prec,
                            std::vector<t_gridNode>&    IntListNC,
                            TColStd_PackedMapOfInteger& keptIndices)
{
  std::vector<t_gridNode> purifiedList;
  int ip = 0;
  do
  {
    const t_gridNode& p = IntListNC[ip];

    // Traverse till the end skipping all coincident points
    int jp = ip + 1, p_best_idx = ip;
    t_gridNode p_next, p_best = p;
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

//! Auxiliary class to search for coincident spatial points.
class InspectXY : public NCollection_CellFilter_InspectorXY
{
public:

  typedef gp_XY Target;

  //! Constructor accepting resolution distance and point.
  InspectXY(const double resolution, const gp_XY& P) : m_fResolution(resolution), m_bFound(false), m_P(P) {}

  //! \return true/false depending on whether the node was found or not.
  bool IsFound() const { return m_bFound; }

  //! Implementation of inspection method.
  NCollection_CellFilter_Action Inspect(const gp_XY& Target)
  {
    m_bFound = ( (m_P - Target).SquareModulus() <= Square(m_fResolution) );
    return CellFilter_Keep;
  }

private:

  gp_XY  m_P;           //!< Source point.
  bool   m_bFound;      //!< Whether two points are coincident or not.
  double m_fResolution; //!< Resolution to check for coincidence.

};

//! Auxiliary class to search for coincident tessellation nodes.
class InspectNode : public InspectXY
{
public:

  typedef t_gridNode Target;

  //! Constructor accepting resolution distance and point.
  InspectNode(const double resolution, const t_gridNode& P) : InspectXY(resolution, P.uvInit), m_iID(-1) {}

  int GetID() const { return m_iID; }

  //! Implementation of inspection method.
  NCollection_CellFilter_Action Inspect(const t_gridNode& Target)
  {
    InspectXY::Inspect(Target.uvInit);

    if ( InspectXY::IsFound() )
      m_iID = Target.id;

    return CellFilter_Keep;
  }

private:

  int m_iID; //!< Found target ID.

};

void appendNodeInGlobalCollection(t_gridNode&                          node,
                                  int&                                 globalNodeId,
                                  std::vector<t_gridNode>&             allNodes,
                                  NCollection_CellFilter<InspectNode>& NodeFilter)
{
  const double prec = Precision::Confusion();
  InspectNode Inspect(prec, node);
  gp_XY XY_min = Inspect.Shift( node.uvInit, -prec );
  gp_XY XY_max = Inspect.Shift( node.uvInit,  prec );

  // Coincidence test
  NodeFilter.Inspect(XY_min, XY_max, Inspect);
  const bool isFound = Inspect.IsFound();
  //
  if ( !isFound )
  {
    node.id = globalNodeId;
    //
    NodeFilter.Add(node, node.uvInit);
    allNodes.push_back(node);

    // (!!!) Increment global node ID
    ++globalNodeId;
  }
}

int ENGINE_InterpolateSurfMesh(const Handle(asiTcl_Interp)& interp,
                               int                          argc,
                               const char**                 argv)
{
  if ( argc != 4 && argc != 5 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Surface Node.
  Handle(asiData_IVSurfaceNode)
    node = Handle(asiData_IVSurfaceNode)::DownCast( cmdEngine::model->FindNodeByName(argv[1]) );
  //
  if ( node.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find a Surface object with name %1." << argv[2]);
    return TCL_OK;
  }

  // Get geometry of a surface.
  Handle(Geom_Surface) surface = node->GetSurface();
  //
  if ( surface.IsNull() || !surface->IsKind( STANDARD_TYPE(Geom_Plane) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Surface in question is null or not a plane.");
    return TCL_OK;
  }
  //
  Handle(Geom_Plane) plane = Handle(Geom_Plane)::DownCast(surface);

  // Get Part and Triangulation Nodes.
  Handle(asiData_PartNode)          part_n = cmdEngine::model->GetPartNode();
  Handle(asiData_TriangulationNode) tris_n = cmdEngine::model->GetTriangulationNode();

  // Get Contour Node.
  Handle(asiData_ContourNode) contour_n = part_n->GetContour();
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "No persistent data for contour available.");
    return TCL_OK;
  }

  /* =============
   *  Get contour
   * ============= */

  // Get coordinates of the contour's poles.
  std::vector<gp_XYZ> points;
  gp_XYZ P_center;
  //
  contour_n->GetPoints(points, P_center);

  // Get characteristic size.
  double size = 0.0;
  //
  for ( size_t k = 0; k < points.size(); ++k )
    size = Max( size, (points[k] - P_center).Modulus() );
  //
  size *= 1.1;

  // Get bounding box of the contour.
  Bnd_Box contourAABB;
  //
  for ( size_t k = 0; k < points.size(); ++k )
    contourAABB.Update( points[k].X(), points[k].Y(), points[k].Z() );
  //
  double xMin, yMin, zMin, xMax, yMax, zMax;
  contourAABB.Get(xMin, yMin, zMin, xMax, yMax, zMax);

  {
    // Create bounding box to draw it
    TopoDS_Shape bndbox = BRepPrimAPI_MakeBox( gp_Pnt(xMin, yMin, zMin), gp_Pnt(xMax, yMax, zMax) );
    //
    interp->GetPlotter().DRAW_SHAPE(bndbox, Color_Yellow, 1.0, true, "bounding box");
  }

  /* ==============================================================
   *  Get image of the contour in (U,V) space of the average plane
   * ============================================================== */

  // Working tools and variables
  int curID = 0;
  NCollection_CellFilter<InspectNode> NodeFilter( Precision::Confusion() );

  // Prepare a collection of grid points which will be interpolated ultimately.
  std::vector<t_gridNode> allNodes;

  // Project points on surface.
  for ( size_t k = 0; k < points.size(); ++k )
  {
    ShapeAnalysis_Surface SAS(plane);
    gp_Pnt2d UV = SAS.ValueOfUV(points[k], 1.0e-1);

    //interp->GetPlotter().DRAW_POINT(UV,                      Color_Yellow, "UV");
    //interp->GetPlotter().DRAW_POINT(gp_XY( UV.X(), 0      ), Color_Red,    "U");
    //interp->GetPlotter().DRAW_POINT(gp_XY( 0,      UV.Y() ), Color_Green,  "V");

    //interp->GetPlotter().DRAW_LINK(gp_XY( UV.X(), 0      ), gp_XY( UV.X(),  size   ), Color_White, "U_iso_p");
    //interp->GetPlotter().DRAW_LINK(gp_XY( UV.X(), 0      ), gp_XY( UV.X(), -size   ), Color_White, "U_iso_m");
    //interp->GetPlotter().DRAW_LINK(gp_XY( 0,      UV.Y() ), gp_XY( size,    UV.Y() ), Color_White, "V_iso_p");
    //interp->GetPlotter().DRAW_LINK(gp_XY( 0,      UV.Y() ), gp_XY(-size,    UV.Y() ), Color_White, "V_iso_m");

    // Prepare a grid node structure.
    t_gridNode node;
    node.isContourPoint  = true;
    node.isSampledPoint  = false;
    node.isMeshNodePoint = false;
    node.uvInit          = UV.XY();
    node.xyzInit         = points[k];

    // Add node
    ::appendNodeInGlobalCollection(node, curID, allNodes, NodeFilter);
  }

  ShapeExtend_WireData mkWire;
  for ( size_t k = 1; k < allNodes.size(); ++k )
  {
    mkWire.Add( BRepBuilderAPI_MakeEdge(GCE2d_MakeSegment(allNodes[k-1].uvInit, allNodes[k].uvInit).Value(), plane).Edge() );
  }
  mkWire.Add( BRepBuilderAPI_MakeEdge(GCE2d_MakeSegment(allNodes[allNodes.size()-1].uvInit, allNodes[0].uvInit).Value(), plane).Edge() );
  //
  const TopoDS_Wire& W = mkWire.WireAPIMake();

  TopoDS_Face F = BRepBuilderAPI_MakeFace(plane->Pln(), W, false);
  //
  ShapeFix_Face ShapeHealer(F);
  ShapeHealer.Perform();
  F = ShapeHealer.Face();

  interp->GetPlotter().REDRAW_SHAPE("W", W, Color_Red, 1.0, true);
  interp->GetPlotter().REDRAW_SHAPE("F", F);

  /* ====================================
   *  Add points representing mesh nodes
   * ==================================== */

  asiAlgo_ClassifyPointFace classifier(F, 1e-4, 1e-4);

  const Handle(Poly_Triangulation)& tris     = tris_n->GetTriangulation();
  const TColgp_Array1OfPnt&         triNodes = tris->Nodes();

  for ( int k = triNodes.Lower(); k <= triNodes.Upper(); ++k )
  {
    const gp_Pnt& triNode = triNodes(k);
    //
    if ( contourAABB.IsOut(triNode) )
      continue;

    ShapeAnalysis_Surface SAS(plane);
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

    // Add node
    ::appendNodeInGlobalCollection(node, curID, allNodes, NodeFilter);
  }

  //for ( size_t k = 0; k < allNodes.size(); ++k )
  //{
  //  interp->GetPlotter().DRAW_POINT(allNodes[k].uvInit,  allNodes[k].isContourPoint ? Color_Red : Color_Blue, "uvInit");
  //  interp->GetPlotter().DRAW_POINT(allNodes[k].xyzInit, allNodes[k].isContourPoint ? Color_Red : Color_Blue, "xyzInit");
  //}

  std::vector<t_gridNode> reperNodes = allNodes;

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

        // Add node
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
  double grain_factor = ( (argc == 5) ? atof(argv[4]) : 0.1 );
  double coincConf    = size*grain_factor;
  {
    TColStd_PackedMapOfInteger keptIndices;
    RemoveCoincidentPoints(coincConf, uvGridNodes[0], keptIndices);
    //
    uvGridNodesSparsedU.push_back(uvGridNodes[0]);
    //
    for ( size_t i = 1; i < uvGridNodes.size(); ++i )
    {
      std::vector<t_gridNode> sparsedIso;
      for ( size_t j = 0; j < uvGridNodes[i].size(); ++j )
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
  for ( size_t i = 0; i < uvGridNodesSparsedU.size(); ++i )
  {
    col.push_back(uvGridNodesSparsedU[i][0]);
  }
  //
  {
    TColStd_PackedMapOfInteger keptIndices;
    RemoveCoincidentPoints(coincConf, col, keptIndices);
    //
    for ( size_t i = 0; i < uvGridNodesSparsedU.size(); ++i )
    {
      if ( keptIndices.Contains(i) )
        uvGridNodesSparsed.push_back(uvGridNodesSparsedU[i]);
    }
  }

  //for ( size_t i = 0; i < uvGridNodesSparsed.size(); ++i )
  //{
  //  for ( size_t j = 0; j < uvGridNodesSparsed[i].size(); ++j )
  //  {
  //    interp->GetPlotter().DRAW_POINT(uvGridNodesSparsed[i][j].uvInit,  uvGridNodesSparsed[i][j].isContourPoint ? Color_Red : Color_Blue, "uvInit");
  //    interp->GetPlotter().DRAW_POINT(uvGridNodesSparsed[i][j].xyzInit, uvGridNodesSparsed[i][j].isContourPoint ? Color_Red : Color_Blue, "xyzInit");
  //  }
  //}

  /* =========================================================
   *  Correct 3D positions of sampled nodes according to mesh
   * ========================================================= */

  Handle(asiAlgo_BVHFacets) triBVH = tris_n->GetBVH();
  //
  if ( triBVH.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "BVH is not ready for Triangulation Node.");
    return TCL_OK;
  }

  asiAlgo_HitFacet picker( triBVH, interp->GetProgress(), interp->GetPlotter() );

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
        interp->GetProgress().SendLogMessage(LogWarn(Normal) << "Cannot precise grid point.");
        interp->GetPlotter().DRAW_POINT(uvGridNodesSparsed[i][j].xyzInit, Color_Red, "cannot_precise");
        continue;
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
    }
  }

  Handle(HRealArray) pts = new HRealArray(0, uvGridNodesSparsed.size()*uvGridNodesSparsed[0].size()*3 - 1);
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
  //
  interp->GetPlotter().DRAW_POINTS(pts, Color_Blue, "vIsoNodes_xyz");

  /* ==================
   *  Interpolate grid
   * ================== */

  // Get grid in Mobius form.
  std::vector< std::vector<mobius::xyz> > mobGrid;
  //
  for ( size_t i = 0; i < uvGridNodesSparsed.size(); ++i )
  {
    std::vector<mobius::xyz> isoPoints;

    for ( size_t j = 0; j < uvGridNodesSparsed[i].size(); ++j )
      isoPoints.push_back( mobius::xyz( uvGridNodesSparsed[i][j].xyz.X(),
                                        uvGridNodesSparsed[i][j].xyz.Y(),
                                        uvGridNodesSparsed[i][j].xyz.Z() ) );

    mobGrid.push_back(isoPoints);
  }

  const int                          deg_U      = atoi(argv[2]);
  const int                          deg_V      = atoi(argv[3]);
  const mobius::bspl_ParamsSelection paramsType = mobius::ParamsSelection_ChordLength;
  const mobius::bspl_KnotsSelection  knotsType  = mobius::KnotsSelection_Average;

  // Interpolate.
  mobius::geom_InterpolateSurface interpSurf(mobGrid, deg_U, deg_V, paramsType, knotsType);
  //
  interpSurf.Perform();
  //
  if ( interpSurf.ErrorCode() != mobius::geom_InterpolateSurface::ErrCode_NoError )
  {
    interp->GetProgress().SendLogMessage( LogErr(Normal) << "Interpolation failed with error code %1."
                                                         << interpSurf.ErrorCode() );
    return TCL_OK;
  }

  // Get Mobius surface.
  const mobius::ptr<mobius::bsurf>& mobSurf = interpSurf.Result();

  // Convert to OpenCascade surface.
  mobius::cascade_BSplineSurface toOpenCascade(mobSurf);
  toOpenCascade.DirectConvert();
  const Handle(Geom_Surface)& surf = toOpenCascade.GetOpenCascadeSurface();

  interp->GetPlotter().DRAW_SURFACE(surf, Color_White, "surf");

  // TODO: NYI

  return TCL_OK;
}

//-----------------------------------------------------------------------------

void cmdEngine::Commands_Interactive(const Handle(asiTcl_Interp)&      interp,
                                     const Handle(Standard_Transient)& data)
{
  cmdEngine_NotUsed(data);
  //
  static const char* group = "cmdEngine";

  //-------------------------------------------------------------------------//
  interp->AddCommand("start-contour",
    //
    "start-contour\n"
    "\t Enables interactive contour picking.",
    //
    __FILE__, group, ENGINE_StartContour);

  //-------------------------------------------------------------------------//
  interp->AddCommand("finish-contour",
    //
    "finish-contour\n"
    "\t Finalizes interactive contour picking.",
    //
    __FILE__, group, ENGINE_FinishContour);

  //-------------------------------------------------------------------------//
  interp->AddCommand("contour-to-wire",
    //
    "contour-to-wire varName\n"
    "\t Converts contour to topological wire.",
    //
    __FILE__, group, ENGINE_ContourToWire);

  //-------------------------------------------------------------------------//
  interp->AddCommand("make-contour-plane",
    //
    "make-contour-plane planeName\n"
    "\t Creates average plane for the active contour.",
    //
    __FILE__, group, ENGINE_MakeContourPlane);

  //-------------------------------------------------------------------------//
  interp->AddCommand("interpolate-surf-mesh",
    //
    "interpolate-surf-mesh planeName udeg vdeg [grain_factor]\n"
    "\t Projects active contour to the plane with the given name.",
    //
    __FILE__, group, ENGINE_InterpolateSurfMesh);
}
