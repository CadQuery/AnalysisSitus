//-----------------------------------------------------------------------------
// Created on: 23 August 2017
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

// cmdMisc includes
#include <cmdMisc.h>

// asiAlgo includes
#include <asiAlgo_BaseCloud.h>
#include <asiAlgo_Cloudify.h>
#include <asiAlgo_MeshGen.h>
#include <asiAlgo_MeshMerge.h>
#include <asiAlgo_MeshOBB.h>
#include <asiAlgo_PlaneOnPoints.h>
#include <asiAlgo_ProjectPointOnMesh.h>
#include <asiAlgo_Timer.h>
#include <asiAlgo_TopoKill.h>
#include <asiAlgo_Utils.h>

// asiUI includes
#include <asiUI_CommonFacilities.h>
#include <asiUI_IV.h>

// asiTcl includes
#include <asiTcl_PluginMacro.h>

// asiEngine includes
#include <asiEngine_Model.h>
#include <asiEngine_Part.h>
#include <asiEngine_Triangulation.h>

// asiVisu includes
#include <asiVisu_PartPrs.h>

// OCCT includes
#include <BOPAlgo_Splitter.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepGProp.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepTools.hxx>
#include <GCPnts_QuasiUniformAbscissa.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <GeomAPI.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <gp_Circ.hxx>
#include <gp_Pln.hxx>
#include <GProp_GProps.hxx>
#include <GProp_PrincipalProps.hxx>
#include <IntTools_FClass2d.hxx>
#include <NCollection_CellFilter.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <ShapeUpgrade_UnifySameDomain.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#ifdef USE_MOBIUS
  #include <mobius/bspl_FindSpan.h>
  #include <mobius/bspl_UnifyKnots.h>
  #include <mobius/cascade.h>
  #include <mobius/core_HeapAlloc.h>
  #include <mobius/geom_BSplineCurve.h>
  #include <mobius/geom_CoonsSurfaceCubic.h>
  #include <mobius/geom_CoonsSurfaceLinear.h>
  #include <mobius/geom_FairBCurve.h>
  #include <mobius/geom_FairBSurf.h>
  #include <mobius/geom_InterpolateMultiCurve.h>
  #include <mobius/geom_MakeBicubicBSurf.h>
  #include <mobius/geom_SkinSurface.h>

  using namespace mobius;
#endif

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #pragma message("===== warning: DRAW_DEBUG is enabled")
#endif

#ifdef USE_MOBIUS

void cmdMisc::DrawSurfPts(const Handle(asiTcl_Interp)&   interp,
                          const t_ptr<geom_Surface>&     surface,
                          const TCollection_AsciiString& name)
{
  // Sample patch.
  Handle(asiAlgo_BaseCloud<double>) pts = new asiAlgo_BaseCloud<double>;
  //
  const double uMin = surface->GetMinParameter_U();
  const double uMax = surface->GetMaxParameter_U();
  const double vMin = surface->GetMinParameter_V();
  const double vMax = surface->GetMaxParameter_V();
  //
  const double deltaU = (uMax - uMin)/100.;
  const double deltaV = (vMax - vMin)/100.;
  //
  double u = uMin;
  //
  bool stopU = false;
  do
  {
    if ( u > uMax )
      stopU = true;
    else
    {
      double v = vMin;
      //
      bool stopV = false;
      do
      {
        if ( v > vMax )
          stopV = true;
        else
        {
          t_xyz P;
          surface->Eval(u, v, P);

          pts->AddElement( P.X(), P.Y(), P.Z() );

          v += deltaV;
        }
      }
      while ( !stopV );

      u += deltaU;
    }
  }
  while ( !stopU );
  //
  interp->GetPlotter().REDRAW_POINTS(name, pts->GetCoordsArray(), Color_White);
}

#endif

//-----------------------------------------------------------------------------

Handle(asiEngine_Model)        cmdMisc::model = nullptr;
Handle(asiUI_CommonFacilities) cmdMisc::cf    = nullptr;

//-----------------------------------------------------------------------------

namespace
{
  //---------------------------------------------------------------------------
  // Inspectors for spatial nodes
  //---------------------------------------------------------------------------

  //! Spatial point enriched with numeric identifier.
  struct t_vertex
  {
    gp_XYZ        Point;  //!< Geometric representation.
    TopoDS_Vertex Vertex; //!< Topological representation.
  };

  //! Auxiliary class to search for coincident spatial points.
  class InspectPoint : public NCollection_CellFilter_InspectorXYZ
  {
  public:

    typedef gp_XYZ Target;

    //! Constructor accepting resolution distance and point.
    InspectPoint(const double tol, const gp_XYZ& P) : m_fTol(tol), m_bFound(false), m_P(P) {}

    //! \return true/false depending on whether the node was found or not.
    bool IsFound() const { return m_bFound; }

    //! Implementation of inspection method.
    NCollection_CellFilter_Action Inspect(const gp_XYZ& Target)
    {
      m_bFound = ( (m_P - Target).SquareModulus() <= Square(m_fTol) );
      return CellFilter_Keep;
    }

  private:

    gp_XYZ m_P;      //!< Source point.
    bool   m_bFound; //!< Whether two points are coincident or not.
    double m_fTol;   //!< Resolution to check for coincidence.

  };

  //! Auxiliary class to search for coincident tessellation nodes.
  class InspectVertex : public InspectPoint
  {
  public:

    typedef t_vertex Target;

    //! Constructor accepting resolution distance and point.
    InspectVertex(const double tol, const gp_XYZ& P) : InspectPoint(tol, P) {}

    //! \return vertex.
    const TopoDS_Vertex&
      GetVertex() const { return m_vertex; }

    //! Implementation of inspection method.
    NCollection_CellFilter_Action Inspect(const t_vertex& Target)
    {
      InspectPoint::Inspect(Target.Point);

      if ( InspectPoint::IsFound() )
        m_vertex = Target.Vertex;

      return CellFilter_Keep;
    }

  private:

    TopoDS_Vertex m_vertex; //!< Found target.

  };

  //! Returns a substitution vertex for the passed one.
  TopoDS_Vertex getCommonVertex(const TopoDS_Vertex&                   old,
                                NCollection_CellFilter<InspectVertex>& cellFilter)
  {
    gp_XYZ       oldCoords = BRep_Tool::Pnt(old).XYZ();
    const double prec      = Precision::Confusion();

    // Prepare inspection tool.
    InspectVertex Inspect(prec, oldCoords);
    gp_XYZ XYZ_min = Inspect.Shift( oldCoords, -prec );
    gp_XYZ XYZ_max = Inspect.Shift( oldCoords,  prec );

    // Result.
    TopoDS_Vertex res;

    // Coincidence test.
    cellFilter.Inspect(XYZ_min, XYZ_max, Inspect);
    const bool isFound = Inspect.IsFound();
    //
    if ( !isFound )
    {
      t_vertex V;
      V.Point  = oldCoords;
      V.Vertex = old;
      //
      cellFilter.Add(V, oldCoords);

      res = V.Vertex;
    }
    else
    {
      res = Inspect.GetVertex();
    }

    return res;
  }

}

//-----------------------------------------------------------------------------

double SignedDistance(const gp_Pln& pln, const gp_Pnt& P)
{
  const gp_Pnt& loc = pln.Position().Location();
  const gp_Dir& dir = pln.Position().Direction();
  double D = ( dir.X() * ( P.X() - loc.X() ) +
               dir.Y() * ( P.Y() - loc.Y() ) +
               dir.Z() * ( P.Z() - loc.Z() ) );
  return D;
}

//-----------------------------------------------------------------------------

int MISC_TestHexagonBops(const Handle(asiTcl_Interp)& interp,
                         int                          /*argc*/,
                         const char**                 /*argv*/)
{
  const double r = 0.02;
  const double h = 0.25;
  const double step = 0.08;

  /* =======================
   *  Build hexagonal prism
   * ======================= */

  TIMER_NEW
  TIMER_GO

  // Get hexagon poles in 2D
  gp_XY Poles2d[6];
  asiAlgo_Utils::HexagonPoles(gp::Origin2d().XY(),
                              1.0,
                              Poles2d[0],
                              Poles2d[1],
                              Poles2d[2],
                              Poles2d[3],
                              Poles2d[4],
                              Poles2d[5]);

  // Choose working plane
  Handle(Geom_Plane) workPlane = new Geom_Plane( gp_Pln( gp::Origin(), gp::DZ() ) );

  // Populate wire builder
  BRepBuilderAPI_MakePolygon mkPoly;
  for ( int k = 0; k < 6; ++k )
    mkPoly.Add( workPlane->Value( Poles2d[k].X(), Poles2d[k].Y() ) );
  //
  mkPoly.Add( workPlane->Value( Poles2d[0].X(), Poles2d[0].Y() ) );

  // Build wire
  mkPoly.Build();
  const TopoDS_Wire& wire = mkPoly.Wire();

  // Build face
  TopoDS_Face face = BRepBuilderAPI_MakeFace(wire);

  // Build prism
  TopoDS_Shape prism = BRepPrimAPI_MakePrism(face, gp_Vec( workPlane->Axis().Direction() )*h);

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Build hexagonal solid")

  interp->GetPlotter().REDRAW_SHAPE("contour", wire, Color_Red, 1.0, true);
  interp->GetPlotter().REDRAW_SHAPE("face", face);
  interp->GetPlotter().REDRAW_SHAPE("prism", prism);

  /* =================
   *  Build cylinders
   * ================= */

  TIMER_RESET
  TIMER_GO

  // Build bounding box for the hexa
  double xMin, yMin, zMin, xMax, yMax, zMax;
  asiAlgo_Utils::Bounds(prism, xMin, yMin, zMin, xMax, yMax, zMax);

  BRep_Builder BB;
  TopoDS_Compound cyls;
  BB.MakeCompound(cyls);

  // List for cutter
  TopTools_ListOfShape tools;

  // Prepare two-dimensional classifier
  IntTools_FClass2d classifier;
  classifier.Init( face, Precision::Confusion() );

  // Count number of primitives which pass the test
  int cylCount = 0;

  double xcurr = xMin;
  do
  {
    double ycurr = yMin;

    do
    {
      // Classify center point
      const TopAbs_State state = classifier.TestOnRestriction( gp_Pnt2d(xcurr, ycurr), 0.05 );
      //
      if ( state == TopAbs_IN )
      {
        gp_Pnt cylcenter(xcurr, ycurr, 0.0);
        TopoDS_Shape cyl = BRepPrimAPI_MakeCylinder( gp_Ax2(cylcenter, gp::DZ() ), r, h );
        //
        BB.Add(cyls, cyl);
        tools.Append(cyl);

        cylCount++;
      }

      ycurr += step;
    }
    while ( ycurr < yMax );

    xcurr += step;
  }
  while ( xcurr < xMax );

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Build cylinders")

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Number of cylinders: %1" << cylCount);
  interp->GetPlotter().REDRAW_SHAPE("cyls", cyls, Color_Yellow);

  /* ================
   *  Cut primitives
   * ================ */

  TIMER_RESET
  TIMER_GO

  TopoDS_Shape result = asiAlgo_Utils::BooleanCut(prism, tools, false);

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Cut cylinders")

  interp->GetPlotter().REDRAW_SHAPE("result", result);

  return 0;
}

//-----------------------------------------------------------------------------

int MISC_TestHexagonBopsFaces(const Handle(asiTcl_Interp)& interp,
                              int                          /*argc*/,
                              const char**                 /*argv*/)
{
  const double r = 0.02;
  const double h = 0.25;
  const double step = 0.06;

  /* ===============
   *  Build hexagon
   * =============== */

  TIMER_NEW
  TIMER_GO

  // Get hexagon poles in 2D
  gp_XY Poles2d[6];
  asiAlgo_Utils::HexagonPoles(gp::Origin2d().XY(),
                              1.0,
                              Poles2d[0],
                              Poles2d[1],
                              Poles2d[2],
                              Poles2d[3],
                              Poles2d[4],
                              Poles2d[5]);

  // Choose working plane
  Handle(Geom_Plane) workPlane = new Geom_Plane( gp_Pln( gp::Origin(), gp::DZ() ) );

  // Populate wire builder
  BRepBuilderAPI_MakePolygon mkPoly;
  for ( int k = 0; k < 6; ++k )
    mkPoly.Add( workPlane->Value( Poles2d[k].X(), Poles2d[k].Y() ) );
  //
  mkPoly.Add( workPlane->Value( Poles2d[0].X(), Poles2d[0].Y() ) );

  // Build wire
  mkPoly.Build();
  const TopoDS_Wire& wire = mkPoly.Wire();

  // Build face
  TopoDS_Face face = BRepBuilderAPI_MakeFace(wire);

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Build hexagon")

  interp->GetPlotter().REDRAW_SHAPE("contour", wire, Color_Red, 1.0, true);
  interp->GetPlotter().REDRAW_SHAPE("face", face);

  /* ===============
   *  Build circles
   * =============== */

  TIMER_RESET
  TIMER_GO

  // Build bounding box for the hexa
  double xMin, yMin, zMin, xMax, yMax, zMax;
  asiAlgo_Utils::Bounds(face, xMin, yMin, zMin, xMax, yMax, zMax);

  BRep_Builder BB;
  TopoDS_Compound circles;
  BB.MakeCompound(circles);

  // List for cutter
  TopTools_ListOfShape tools;

  // Prepare two-dimensional classifier
  IntTools_FClass2d classifier;
  classifier.Init( face, Precision::Confusion() );

  // Count number of primitives which pass the test
  int circCount = 0;

  double xcurr = xMin;
  do
  {
    double ycurr = yMin;

    do
    {
      // Classify center point
      const TopAbs_State state = classifier.TestOnRestriction( gp_Pnt2d(xcurr, ycurr), 0.05 );
      //
      if ( state == TopAbs_IN )
      {
        gp_Pnt       circcenter(xcurr, ycurr, 0.0);
        gp_Circ      circ(gp_Ax2(circcenter, gp::DZ() ), r);
        TopoDS_Wire  circWire = BRepBuilderAPI_MakeWire( BRepBuilderAPI_MakeEdge(circ) );
        TopoDS_Shape circFace = BRepBuilderAPI_MakeFace(circWire);
        //
        BB.Add(circles, circFace);
        tools.Append(circFace);

        circCount++;
      }

      ycurr += step;
    }
    while ( ycurr < yMax );

    xcurr += step;
  }
  while ( xcurr < xMax );

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Build circles")

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Number of circles: %1" << circCount);
  interp->GetPlotter().REDRAW_SHAPE("circles", circles, Color_Yellow);

  /* ================
   *  Cut primitives
   * ================ */

  TIMER_RESET
  TIMER_GO

  TopoDS_Shape cutres = asiAlgo_Utils::BooleanCut(face, tools, false);

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Cut circles")

  interp->GetPlotter().REDRAW_SHAPE("cutres", cutres);

  /* =========
   *  Extrude
   * ========= */

  TIMER_RESET
  TIMER_GO

  // Build prism
  TopoDS_Shape result = BRepPrimAPI_MakePrism(cutres, gp_Vec( workPlane->Axis().Direction() )*h);

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Extrude")

  interp->GetPlotter().REDRAW_SHAPE("result", result);

  return 0;
}

//-----------------------------------------------------------------------------

int MISC_PushPull(const Handle(asiTcl_Interp)& interp,
                  int                          /*argc*/,
                  const char**                 argv)
{
  TopoDS_Shape
    partShape = Handle(asiEngine_Model)::DownCast( interp->GetModel() )->GetPartNode()->GetShape();

  int faceId = atoi(argv[1]); // 1-based

  TopTools_IndexedMapOfShape M;
  TopExp::MapShapes(partShape, TopAbs_FACE, M);
  TopoDS_Face faceShape = TopoDS::Face( M.FindKey(faceId) );

  interp->GetPlotter().DRAW_SHAPE(faceShape, Color_Blue, 1.0, true, "faceShape");

  Handle(Geom_Surface) surf = BRep_Tool::Surface(faceShape);

  double uMin, uMax, vMin, vMax;
  BRepTools::UVBounds(faceShape, uMin, uMax, vMin, vMax);

  double uMid = (uMin + uMax)*0.5;
  double vMid = (vMin + vMax)*0.5;

  gp_Pnt P;
  gp_Vec dS_dU, dS_dV;
  surf->D1(uMid, vMid, P, dS_dU, dS_dV);

  gp_Dir norm = dS_dU^dS_dV;

  if ( faceShape.Orientation() == TopAbs_REVERSED )
    norm.Reverse();

  gp_Vec offset = norm.XYZ() * atof(argv[2]);

  TopoDS_Shape prism = BRepPrimAPI_MakePrism(faceShape, offset);
  interp->GetPlotter().REDRAW_SHAPE("prism", prism, Color_Blue);

  TopoDS_Shape fused = BRepAlgoAPI_Fuse(prism, partShape);

  ShapeUpgrade_UnifySameDomain Maximize(fused);
  Maximize.Build();
  fused = Maximize.Shape();

  interp->GetPlotter().REDRAW_SHAPE("fused", fused, Color_Green);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_TestBuilder(const Handle(asiTcl_Interp)& interp,
                     int                          argc,
                     const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  BRep_Builder BB;

  // Construct any face
  TopoDS_Face face;
  BB.MakeFace(face);
  face.Orientation(TopAbs_REVERSED);

  // Construct any wire
  TopoDS_Wire wire;
  BB.MakeWire(wire);
  wire.Orientation(TopAbs_REVERSED);

  // Add wire to face
  BB.Add(face, wire);

  for ( TopoDS_Iterator it(face, false, false); it.More(); it.Next() )
  {
    // We don't care of internal/external here
    std::cout << "Orientation of sub-shape is "
              << (it.Value().Orientation() == TopAbs_FORWARD ? "forward" : "reversed");
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_TestComposite(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 /*argv*/)
{
  if ( argc != 1 )
    return TCL_ERROR;

  // Build rectangular face.
  TopoDS_Face R;
  {
    gp_Pnt P1(0, 0, 0),
           P2(1, 0, 0),
           P3(1, 1, 0),
           P4(0, 1, 0);

    TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(P1, P2);
    TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(P2, P3);
    TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(P3, P4);
    TopoDS_Edge E4 = BRepBuilderAPI_MakeEdge(P4, P1);
    TopoDS_Wire W = BRepBuilderAPI_MakeWire(E1, E2, E3, E4);

    R = BRepBuilderAPI_MakeFace(W);
  }

  // Build circular face.
  TopoDS_Face C1;
  {
    gp_Ax2 localAxes = gp::XOY();
    TopoDS_Edge E = BRepBuilderAPI_MakeEdge( gp_Circ(localAxes, 0.25), 0, 2*M_PI );
    TopoDS_Wire W = BRepBuilderAPI_MakeWire(E);

    C1 = BRepBuilderAPI_MakeFace(W);
  }

  gp_Trsf T1; T1.SetTranslation( gp_Vec(1, 0, 0) );
  TopoDS_Face C2 = TopoDS::Face( BRepBuilderAPI_Transform(C1, T1, true) );

  gp_Trsf T2; T2.SetTranslation( gp_Vec(0, 1, 0) );
  TopoDS_Face C3 = TopoDS::Face( BRepBuilderAPI_Transform(C2, T2, true) );

  gp_Trsf T3; T3.SetTranslation( gp_Vec(-1, 0, 0) );
  TopoDS_Face C4 = TopoDS::Face( BRepBuilderAPI_Transform(C3, T3, true) );

  gp_Trsf T4; T4.SetTranslation( gp_Vec(0.5, 0.5, 0) );
  TopoDS_Face C5 = TopoDS::Face( BRepBuilderAPI_Transform(C1, T4, true) );

  interp->GetPlotter().REDRAW_SHAPE("R", R, Color_Red, 1.0);
  interp->GetPlotter().REDRAW_SHAPE("C1", C1, Color_Blue, 1.0);
  interp->GetPlotter().REDRAW_SHAPE("C2", C2, Color_Blue, 1.0);
  interp->GetPlotter().REDRAW_SHAPE("C3", C3, Color_Blue, 1.0);
  interp->GetPlotter().REDRAW_SHAPE("C4", C4, Color_Blue, 1.0);
  interp->GetPlotter().REDRAW_SHAPE("C5", C5, Color_Blue, 1.0);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_TestOffset(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 /*argv*/)
{
  if ( argc != 1 )
    return TCL_ERROR;

  // Build rectangular face.
  TopoDS_Face R;
  TopoDS_Edge E1, E2, E3, E4;
  {
    gp_Pnt P1(0, 0, 0),
           P2(1, 0, 0),
           P3(1, 1, 0),
           P4(0, 1, 0);

    E1 = BRepBuilderAPI_MakeEdge(P1, P2);
    E2 = BRepBuilderAPI_MakeEdge(P2, P3);
    E3 = BRepBuilderAPI_MakeEdge(P3, P4);
    E4 = BRepBuilderAPI_MakeEdge(P4, P1);

    BRepBuilderAPI_MakeWire mkWire;
    mkWire.Add(E1); E1 = mkWire.Edge();
    mkWire.Add(E2); E2 = mkWire.Edge();
    mkWire.Add(E3); E3 = mkWire.Edge();
    mkWire.Add(E4); E4 = mkWire.Edge();

    R = BRepBuilderAPI_MakeFace( mkWire.Wire() );
  }

  // Make offset (use any offset value)
  BRepOffsetAPI_MakeThickSolid mkOffset;
  mkOffset.MakeThickSolidBySimple(R, 0.5);

  // Get result
  TopoDS_Shape result;
  if ( mkOffset.IsDone() )
  {
    result = mkOffset.Shape();
  }

  // Images of E1
  {
    // MODIFIED
    const TopTools_ListOfShape& MODIFIED = mkOffset.Modified(E1);
    //
    if ( !MODIFIED.IsEmpty() )
    {
      for ( TopTools_ListIteratorOfListOfShape lit(MODIFIED); lit.More(); lit.Next() )
        interp->GetPlotter().DRAW_SHAPE(lit.Value(), "E1 MODIFIED");
    }

    // GENERATED
    const TopTools_ListOfShape& GENERATED = mkOffset.Generated(E1);
    //
    if ( !GENERATED.IsEmpty() )
    {
      for ( TopTools_ListIteratorOfListOfShape lit(GENERATED); lit.More(); lit.Next() )
        interp->GetPlotter().DRAW_SHAPE(lit.Value(), "E1 GENERATED");
    }
  }

  // Images of R
  {
    // MODIFIED
    const TopTools_ListOfShape& MODIFIED = mkOffset.Modified(R);
    //
    if ( !MODIFIED.IsEmpty() )
    {
      for ( TopTools_ListIteratorOfListOfShape lit(MODIFIED); lit.More(); lit.Next() )
        interp->GetPlotter().DRAW_SHAPE(lit.Value(), "R MODIFIED");
    }

    // GENERATED
    const TopTools_ListOfShape& GENERATED = mkOffset.Generated(R);
    //
    if ( !GENERATED.IsEmpty() )
    {
      for ( TopTools_ListIteratorOfListOfShape lit(GENERATED); lit.More(); lit.Next() )
        interp->GetPlotter().DRAW_SHAPE(lit.Value(), "R GENERATED");
    }
  }

  interp->GetPlotter().REDRAW_SHAPE("result", result);
  return TCL_OK;
}

//-----------------------------------------------------------------------------

#include <Geom_BezierCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomFill_Pipe.hxx>

int MISC_TestPipe1(const Handle(asiTcl_Interp)& interp,
                   int                          argc,
                   const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Construct path.
  TColgp_Array1OfPnt pathPoles(1, 3);
  pathPoles(1) = gp_Pnt(0,   0,   0);
  pathPoles(2) = gp_Pnt(100, 0,   0);
  pathPoles(3) = gp_Pnt(100, 100, 0);
  //
  Handle(Geom_BezierCurve) path = new Geom_BezierCurve(pathPoles);

  // Construct sections.
  Handle(Geom_Curve)
    c1 = GeomConvert::CurveToBSplineCurve(new Geom_TrimmedCurve(new Geom_Circle(gp_Ax2( gp::Origin(), gp::DX() ), 10.0), 0, 2*M_PI), Convert_Polynomial);
  //
  Handle(Geom_Curve)
    c2 = GeomConvert::CurveToBSplineCurve(new Geom_TrimmedCurve(new Geom_Circle(gp_Ax2( gp::Origin(), gp::DX() ), 20.0), 0, 2*M_PI), Convert_Polynomial);

  interp->GetPlotter().REDRAW_CURVE("c1", c1, Color_Red);
  interp->GetPlotter().REDRAW_CURVE("c2", c2, Color_Red);

  // Make pipe.
  GeomFill_Pipe Pipe(path, c1, c2);
  Pipe.Perform();

  // Get the result.
  const Handle(Geom_Surface)& result = Pipe.Surface();

  // Set the result as an output.
  interp->GetPlotter().REDRAW_SURFACE("result", result, Color_White);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_TestSweep1(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Construct path.
  BRepBuilderAPI_MakePolygon mkPath;
  mkPath.Add( gp_Pnt(0,   0,   0) );
  mkPath.Add( gp_Pnt(100, 0,   0) );
  mkPath.Add( gp_Pnt(100, 100, 0) );
  mkPath.Add( gp_Pnt(200, 50,  0) );
  //
  const TopoDS_Wire& pathWire = mkPath.Wire();
  //
  interp->GetPlotter().REDRAW_SHAPE("pathWire", pathWire, Color_Yellow, 1.0, true);

  // Initialize sweeping utility.
  BRepOffsetAPI_MakePipeShell mkPipeShell(pathWire);

  // Construct section.
  Handle(Geom_Curve)
    c1 = new Geom_Circle(gp_Ax2( gp::Origin(), gp::DX() ), 10.0);
  //
  TopoDS_Edge sectionEdge = BRepBuilderAPI_MakeEdge(c1);
  TopoDS_Wire sectionWire = BRepBuilderAPI_MakeWire(sectionEdge);
  //
  interp->GetPlotter().REDRAW_SHAPE("sectionWire", sectionWire, Color_Red, 1.0, true);

  // Add section to the sweeping tool.
  bool isT = false,
       isR = false;
  //
  mkPipeShell.Add(sectionWire, isT, isR);

  // Set evolution mode.
  mkPipeShell.SetMode(true);

  // Set transition strategy for C0 joints.
  mkPipeShell.SetTransitionMode(BRepBuilderAPI_RightCorner);

  // Build.
  mkPipeShell.Build();
  //
  if ( !mkPipeShell.IsDone() )
  {
    BRepBuilderAPI_PipeError Stat = mkPipeShell.GetStatus();
    if ( Stat == BRepBuilderAPI_PlaneNotIntersectGuide )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "One plane does not intersect the guide.");
    }
    if ( Stat == BRepBuilderAPI_ImpossibleContact )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "One section can not be in contact with the guide.");
    }
    return TCL_ERROR;
  }

  // Set the result.
  TopoDS_Shape result = mkPipeShell.Shape();
  //
  interp->GetPlotter().REDRAW_SHAPE("result", result);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_TestSweep2(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Construct section.
  Handle(Geom_Curve)
    c1 = new Geom_Circle(gp_Ax2( gp::Origin(), gp_Dir(1, 1, 0) ), 10.0);
  //
  TopoDS_Edge sectionEdge = BRepBuilderAPI_MakeEdge(c1);
  TopoDS_Wire sectionWire = BRepBuilderAPI_MakeWire(sectionEdge);
  //
  interp->GetPlotter().REDRAW_SHAPE("sectionWire", sectionWire, Color_Red, 1.0, true);

  BRepPrimAPI_MakePrism mkPrism( sectionWire, gp_Vec(100, 0, 0) );

  // Set the result.
  TopoDS_Shape result = mkPrism.Shape();
  //
  interp->GetPlotter().REDRAW_SHAPE("result", result);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

#include <asiAlgo_IneqOpt.h>

int MISC_TestIneq(const Handle(asiTcl_Interp)& interp,
                  int                          argc,
                  const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  /* ======================
   *  Set input parameters
   * ====================== */

  //---------------------------------------------------------------------------

  // Prepare inversion mode.
  Handle(asiAlgo_IneqOptParams) params = new asiAlgo_IneqOptParams(true);

  const int numVariables   = 3;
  const int numConstraints = 3;

  // Coefficients.
  const double bestPositionalTolerAbs = 0.99;
  const double bestAngularTolerAbs    = 0.99;
  //
  std::vector< t_ineqNCoord<double> >
    coeffs = { t_ineqNCoord<double>({1.0, -30.0, 0.0}),
               t_ineqNCoord<double>({1.0,  30.0, 0.0}),
               t_ineqNCoord<double>({0.0,   0.0, 1.0})
             };

  // Limits.
  std::vector<t_ineqRange>
    ANu = { t_ineqRange(-1.0, 1.0),
            t_ineqRange(-1.0, 1.0),
            t_ineqRange(-1.0, 1.0)
          };
  //
  std::vector<t_ineqRange>
    AKNu = { t_ineqRange(-bestPositionalTolerAbs, bestPositionalTolerAbs),
             t_ineqRange(-bestPositionalTolerAbs, bestPositionalTolerAbs),
             t_ineqRange(-bestAngularTolerAbs,    bestAngularTolerAbs)
            };

  //---------------------------------------------------------------------------

  //const int numVariables   = 6;
  //const int numConstraints = 6;

  //// Coefficients.
  //std::vector<t_ineqNCoord<double>>
  //  coeffs = { t_ineqNCoord<double>({1.0, -30.0, 0.0, 0.0, 0.0,  0.0}),
  //             t_ineqNCoord<double>({1.0,  30.0, 0.0, 0.0, 0.0,  0.0}),
  //             t_ineqNCoord<double>({0.0,   0.0, 1.0, 0.0, 0.0,  0.0}),
  //             t_ineqNCoord<double>({0.0,   0.0, 0.0, 1.0, 0.0,  0.0}),
  //             t_ineqNCoord<double>({0.0,   0.0, 0.0, 0.0, 1.0, -5.0}),
  //             t_ineqNCoord<double>({0.0,   0.0, 0.0, 0.0, 1.0,  5.0})
  //           };

  //// Limits.
  //std::vector<t_ineqRange>
  //  ANu = { t_ineqRange(-1.0,  1.0),
  //          t_ineqRange(-1.0,  1.0),
  //          t_ineqRange(-0.8,  0.8),
  //          t_ineqRange(-1.0,  1.0),
  //          t_ineqRange(-2.0,  2.0),
  //          t_ineqRange(-2.0,  2.0)
  //        };
  ////
  //std::vector<t_ineqRange>
  //  AKNu = { t_ineqRange(0.9,  1.0),
  //            t_ineqRange(0.5,  1.0),
  //            t_ineqRange(0.7,  0.8),
  //            t_ineqRange(0.9,  1.0),
  //            t_ineqRange(1.5,  2.0),
  //            t_ineqRange(1.5,  2.0)
  //          };

  //Handle(asiAlgo_IneqOptParams) params = new asiAlgo_IneqOptParams(false);
  ////
  //const int numVariables   = 3;
  //const int numConstraints = 4;

  //// Coefficients.
  //std::vector<t_ineqNCoord<double>>
  //  coeffs = { t_ineqNCoord<double>({-0.5,   0.0,  0.0}),
  //             t_ineqNCoord<double>({ 0.0,   3.0,  0.0}),
  //             t_ineqNCoord<double>({ 1.0,  -1.2,  2.0}),
  //             t_ineqNCoord<double>({ 0.5,  -0.2,  1.0}),
  //             t_ineqNCoord<double>({-0.15, -0.02, 3.0}) };

  //// Limits.
  //std::vector<t_ineqRange>
  //  ANu = { t_ineqRange(0.0,   40.0),
  //          t_ineqRange(0.0,   40.0),
  //          t_ineqRange(30.0,  120.0),
  //          t_ineqRange(10.0,  40.0),
  //          t_ineqRange(20.0,  80.0) };
  ////
  //std::vector<t_ineqRange>
  //  AKNu = { t_ineqRange(20.0, 40.0),
  //           t_ineqRange(30.0, 30.0),
  //           t_ineqRange(50.0, 120.0),
  //           t_ineqRange(20.0, 30.0),
  //           t_ineqRange(25.0, 70.0) };

  //---------------------------------------------------------------------------

  //Handle(asiAlgo_IneqOptParams) params = new asiAlgo_IneqOptParams(false);
  //const int numVariables   = 3;
  //const int numConstraints = 5;

  //// Coefficients.
  //std::vector<t_ineqNCoord<double>>
  //  coeffs = { t_ineqNCoord<double>({0.5, 0.0,  0.0}),
  //             t_ineqNCoord<double>({0.0, 3.0,  0.0}),
  //             t_ineqNCoord<double>({0.5, 0.5,  0.0}),
  //             t_ineqNCoord<double>({0.0, 0.25, 0.25}),
  //             t_ineqNCoord<double>({1.0, 1.2,  1.6}) };

  //// Limits.
  //std::vector<t_ineqRange>
  //  ANu = { t_ineqRange(0.0,   70.0),
  //          t_ineqRange(0.0,   80.0),
  //          t_ineqRange(0.0,   80.0),
  //          t_ineqRange(10.0,  100.0),
  //          t_ineqRange(10.0,  250.0) };
  ////
  //std::vector<t_ineqRange>
  //  AKNu = { t_ineqRange(20.0, 45.0),
  //           t_ineqRange(20.0, 30.0),
  //           t_ineqRange(40.0, 70.0),
  //           t_ineqRange(10.0, 50.0),
  //           t_ineqRange(50.0, 110.0) };

  //---------------------------------------------------------------------------

  //const int numVariables   = 3;
  //const int numConstraints = 9;

  //// Coefficients.
  //std::vector<t_ineqNCoord<double>>
  //  coeffs = { t_ineqNCoord<double>({1.0, 1.0, 1.0, 1.0, 1.0, 1.0}),
  //             t_ineqNCoord<double>({1.0, 1.0, 1.0, 0.0, 0.0, 0.0}),
  //             t_ineqNCoord<double>({0.0, 0.0, 0.0, 1.0, 1.0, 1.0}),
  //             t_ineqNCoord<double>({1.0, 0.0, 0.0, 0.0, 0.0, 0.0}),
  //             t_ineqNCoord<double>({0.0, 1.0, 0.0, 0.0, 0.0, 0.0}),
  //             t_ineqNCoord<double>({0.0, 0.0, 1.0, 0.0, 0.0, 0.0}),
  //             t_ineqNCoord<double>({0.0, 0.0, 0.0, 1.0, 0.0, 0.0}),
  //             t_ineqNCoord<double>({0.0, 0.0, 0.0, 0.0, 1.0, 0.0}),
  //             t_ineqNCoord<double>({0.0, 0.0, 0.0, 0.0, 0.0, 1.0})
  //           };

  //// Limits.
  //std::vector<t_ineqRange>
  //  ANu = { t_ineqRange(50.0, 80.0),
  //          t_ineqRange(10.0, 22.0),
  //          t_ineqRange(40.0, 60.0),
  //          t_ineqRange(9.0,  12.0),
  //          t_ineqRange(5.0,  11.0),
  //          t_ineqRange(3.0,  9.0),
  //          t_ineqRange(6.0,  6.0),
  //          t_ineqRange(10.0, 24.0),
  //          t_ineqRange(5.0,  19.0)
  //        };
  ////
  //std::vector<t_ineqRange>
  //  AKNu = { t_ineqRange(70.0, 80.0),
  //            t_ineqRange(10.0, 15.0),
  //            t_ineqRange(40.0, 60.0),
  //            t_ineqRange(9.0,  12.0),
  //            t_ineqRange(5.0,  11.0),
  //            t_ineqRange(3.0,  9.0),
  //            t_ineqRange(6.0,  6.0),
  //            t_ineqRange(10.0, 24.0),
  //            t_ineqRange(5.0,  19.0)
  //          };

  // Prepare parameters for the inequality optimizer.
  params->SetN (numVariables);
  params->SetM (numConstraints);
  //
  for ( int nu = 1; nu <= numConstraints; ++nu )
  {
    // Set the two-sided inequality constraint.
    params->SetAMinus (nu, ANu[nu-1].values.first);
    params->SetAPlus  (nu, ANu[nu-1].values.second);

    // Set 0-penalty ranges.
    params->SetInterval0(nu, AKNu[nu-1].values.first, AKNu[nu-1].values.second);

    // Set coefficients.
    params->SetCoeffs(nu, coeffs[nu-1]);
  }

  /* ===============
   *  Run optimizer
   * =============== */

  asiAlgo_IneqOpt Optimizer( params, interp->GetProgress(), interp->GetPlotter() );
  //
  if ( !Optimizer.Perform() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Optimizer failed.");
    return TCL_OK;
  }

  // Get solution.
  const t_ineqNCoord<int>&    SolR = Optimizer.GetSolR();
  const t_ineqNCoord<double>& SolX = Optimizer.GetSolX();

  // Draw solution.
  if ( SolX.Dim && numVariables == 2 )
    interp->GetPlotter().DRAW_POINT(gp_Pnt2d(SolX.V[0], SolX.V[1]), Color_Green, "SolX");
  else if ( SolX.Dim && numVariables == 3 )
    interp->GetPlotter().DRAW_POINT(gp_Pnt(SolX.V[0], SolX.V[1], SolX.V[2]), Color_Green, "SolX");

  TCollection_AsciiString SolRStr, SolXStr;
  //
  for ( size_t k = 0; k < SolR.Dim; ++k )
  {
    SolRStr += SolR.V[k];

    if ( k < SolR.Dim - 1 )
      SolRStr += " ";
  }
  //
  for ( size_t k = 0; k < SolX.Dim; ++k )
  {
    SolXStr += SolX.V[k];

    if ( k < SolX.Dim - 1 )
      SolXStr += " ";
  }

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Optimizer converged with\n\tX = %1\n\tR = %2."
                                                       << SolXStr << SolRStr);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_TestEvalCurve(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 argv)
{
  if ( argc < 5 || argc > 6 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Find Curve Node by name.
  Handle(ActAPI_INode) node = interp->GetModel()->FindNodeByName(argv[1]);
  //
  if ( node.IsNull() || !node->IsKind( STANDARD_TYPE(asiData_IVCurveNode) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Node '%1' is not a curve."
                                                        << argv[1]);
    return TCL_OK;
  }
  //
  Handle(asiData_IVCurveNode)
    curveNode = Handle(asiData_IVCurveNode)::DownCast(node);

  // Get curve.
  double f, l;
  Handle(Geom_Curve) occtCurve = curveNode->GetCurve(f, l);
  //
  if ( occtCurve.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The curve in question is null.");
    return TCL_OK;
  }

  // Get parameter value.
  const double u = atof(argv[2]);

  // Get number of iterations.
  const int numIters = atoi(argv[3]);
  //
  if ( numIters <= 0 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Number of iterations should be positive.");
    return TCL_OK;
  }

  // Get order of derivative to compute.
  const int order = atoi(argv[4]);

  // Check whether Mobius evaluation is requested.
  bool isMobius = interp->HasKeyword(argc, argv, "mobius");

  TIMER_NEW

  // Evaluate curve.
  if ( !isMobius )
  {
    TIMER_GO

    gp_Vec eval_DN;
    for ( int i = 0; i < numIters; ++i )
    {
      eval_DN = occtCurve->DN(u, order);
    }

    TIMER_FINISH
    TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Curve evaluation OCCT")

    interp->GetPlotter().REDRAW_VECTOR_AT("eval_DN", gp::Origin(), eval_DN, Color_Yellow);
  }
  else
  {
#ifndef USE_MOBIUS
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Mobius module is disabled.");
    return TCL_ERROR;
#else

    Handle(Geom_BSplineCurve)
      occtBCurve = Handle(Geom_BSplineCurve)::DownCast(occtCurve);
    //
    if ( occtBCurve.IsNull() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "The curve in question is not a B-curve.");
      return TCL_OK;
    }

    // Convert to Mobius curve.
    t_ptr<t_bcurve>
      mobCurve = cascade::GetMobiusBCurve(occtBCurve);

    TIMER_GO

    // Prepare matrix to hold evaluation results.
    core_HeapAlloc2D<double> Alloc;
    double** dN = Alloc.Allocate(mobCurve->GetDegree() + 1,
                                 mobCurve->GetDegree() + 1,
                                 true);

    // Evaluate.
    t_xyz eval_DN;
    for ( int i = 0; i < numIters; ++i )
    {
      mobCurve->Eval_Dk(dN, u, order, eval_DN);
    }

    TIMER_FINISH
    TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Curve evaluation Mobius")

    interp->GetPlotter().REDRAW_VECTOR_AT("eval_DN",
                                          gp::Origin(),
                                          gp_Vec( eval_DN.X(), eval_DN.Y(), eval_DN.Z() ),
                                          Color_Yellow);
#endif
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_TestEvalSurf(const Handle(asiTcl_Interp)& interp,
                      int                          argc,
                      const char**                 argv)
{
  if ( argc < 5 || argc > 6 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Find Surface Node by name.
  Handle(ActAPI_INode) node = interp->GetModel()->FindNodeByName(argv[1]);
  //
  if ( node.IsNull() || !node->IsKind( STANDARD_TYPE(asiData_IVSurfaceNode) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Node '%1' is not a surface."
                                                        << argv[1]);
    return TCL_OK;
  }
  //
  Handle(asiData_IVSurfaceNode)
    surfNode = Handle(asiData_IVSurfaceNode)::DownCast(node);

  // Get surface.
  Handle(Geom_Surface) occtSurface = surfNode->GetSurface();
  //
  if ( occtSurface.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The surface in question is null.");
    return TCL_OK;
  }

  // Get parameter pair.
  const double u = atof(argv[2]);
  const double v = atof(argv[3]);

  // Get number of iterations.
  const int numIters = atoi(argv[4]);
  //
  if ( numIters <= 0 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Number of iterations should be positive.");
    return TCL_OK;
  }

  // Get order of derivative to compute.
  //const int order = atoi(argv[5]);

  // Check whether Mobius evaluation is requested.
  bool isMobius = interp->HasKeyword(argc, argv, "mobius");

  TIMER_NEW

  // Evaluate surface.
  if ( !isMobius )
  {
    TIMER_GO

    gp_Pnt eval_P;
    for ( int i = 0; i < numIters; ++i )
    {
      occtSurface->D0(u, v, eval_P);
    }

    TIMER_FINISH
    TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Surface evaluation OCCT")

    interp->GetPlotter().REDRAW_POINT("eval_P", eval_P, Color_Yellow);
  }
  else
  {
#ifndef USE_MOBIUS
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Mobius module is disabled.");
    return TCL_ERROR;
#else

    Handle(Geom_BSplineSurface)
      occtBSurf = Handle(Geom_BSplineSurface)::DownCast(occtSurface);
    //
    if ( occtBSurf.IsNull() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "The surface in question is not a B-surface.");
      return TCL_OK;
    }

    // Convert to Mobius surface.
    t_ptr<t_bsurf>
      mobBSurf = cascade::GetMobiusBSurface(occtBSurf);

    TIMER_GO

    t_xyz eval_P;
    for ( int i = 0; i < numIters; ++i )
    {
      mobBSurf->Eval(u, v, eval_P);
    }

    TIMER_FINISH
    TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Surface evaluation Mobius")

    interp->GetPlotter().REDRAW_POINT("eval_P",
                                       gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                       Color_Yellow);
#endif
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_Test(const Handle(asiTcl_Interp)& interp,
              int                          argc,
              const char**                 argv)
{
  if ( argc != 3 && argc != 4 && argc != 5 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Test anything here.

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_TestFibers(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  /*const int    segments = 50;
  const int    poles    = 3;
  const double radius   = 2.5;*/

  Handle(asiEngine_Model) M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );
  //
  TopoDS_Shape partShape = M->GetPartNode()->GetShape();

  /* =============================
   *  Restore sharing on vertices.
   * ============================= */

  NCollection_CellFilter<InspectVertex> cellFilter( Precision::Confusion() );

  Handle(asiAlgo_TopoKill)
    reshape = new asiAlgo_TopoKill( partShape,
                                    interp->GetProgress(),
                                    interp->GetPlotter() );

  // Get all vertices.
  TopTools_IndexedMapOfShape allVertices;
  TopExp::MapShapes(partShape, TopAbs_VERTEX, allVertices);
  //
  interp->GetProgress().SendLogMessage( LogNotice(Normal) << "Num. of vertices: %1." << allVertices.Extent() );

  // Get all unique vertices.
  NCollection_DataMap<TopoDS_Vertex, TopoDS_Vertex> vvMap;
  TopTools_IndexedMapOfShape                        uniqueVertices;
  //
  for ( int k = 1; k < allVertices.Extent(); ++k )
  {
    TopoDS_Vertex oldV = TopoDS::Vertex( allVertices(k) );
    TopoDS_Vertex newV = getCommonVertex(oldV, cellFilter);
    //
    newV.Orientation( oldV.Orientation() );
    //
    uniqueVertices.Add(newV);
    vvMap.Bind(oldV, newV);

    if ( oldV.IsPartner(newV) )
      continue;

    if ( !reshape->AskReplace(oldV, newV) )
    {
      interp->GetProgress().SendLogMessage( LogWarn(Normal) << "Request on modification was declined." );
      continue;
    }
  }
  //
  interp->GetProgress().SendLogMessage( LogNotice(Normal) << "Num. of unique vertices: %1." << uniqueVertices.Extent() );

  if ( !reshape->Apply() )
  {
    interp->GetProgress().SendLogMessage( LogErr(Normal) << "Cannot apply the modification." );
    return TCL_ERROR;
  }

  // Update part shape.
  partShape = reshape->GetResult();

  // Modify Data Model.
  cmdMisc::model->OpenCommand();
  {
    asiEngine_Part(cmdMisc::model).Update(partShape);
  }
  cmdMisc::model->CommitCommand();

  // Update UI.
  if ( cmdMisc::cf && cmdMisc::cf->ViewerPart )
    cmdMisc::cf->ViewerPart->PrsMgr()->Actualize( M->GetPartNode() );

  /* =================================================
   *  Check valence of vertices and prepare compounds.
   * ================================================= */

  TopTools_IndexedDataMapOfShapeListOfShape allJoints;
  TopExp::MapShapesAndAncestors(partShape, TopAbs_VERTEX, TopAbs_EDGE, allJoints);

  BRep_Builder bbuilder;

  // Regular vertices.
  TopoDS_Compound regularVerts;
  bbuilder.MakeCompound(regularVerts);

  // Irregular vertices.
  TopoDS_Compound irregularVerts;
  bbuilder.MakeCompound(irregularVerts);

  // Compose the compounds of regular/irregular vertices.
  for ( TopTools_IndexedDataMapOfShapeListOfShape::Iterator vit(allJoints); vit.More(); vit.Next() )
  {
    const TopoDS_Shape&         joint        = vit.Key();
    const TopTools_ListOfShape& edgesAtJoint = vit.Value();
    //
    if ( edgesAtJoint.Extent() == 2 )
      bbuilder.Add(regularVerts, joint);
    else
      bbuilder.Add(irregularVerts, joint);
  }

  interp->GetPlotter().REDRAW_SHAPE("regularVerts",   regularVerts,   Color_Green,   1., true);
  interp->GetPlotter().REDRAW_SHAPE("irregularVerts", irregularVerts, Color_Magenta, 1., true);

  /* ========================
   *  Connect edges to wires.
   * ======================== */

  Handle(TopTools_HSequenceOfShape) hedges = new TopTools_HSequenceOfShape;
  //
  for ( TopExp_Explorer exp(partShape, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    hedges->Append( exp.Current() );
  }

  ///
  Handle(TopTools_HSequenceOfShape) wires;
  ShapeAnalysis_FreeBounds::ConnectEdgesToWires(hedges, Precision::Confusion(), 0, wires);
  //
  TopoDS_Compound wiresComp;
  bbuilder.MakeCompound(wiresComp);

  for ( TopTools_HSequenceOfShape::Iterator wit(*wires); wit.More(); wit.Next() )
    bbuilder.Add( wiresComp, wit.Value() );

  interp->GetPlotter().REDRAW_SHAPE("wires", wiresComp, Color_White, 1., true);
  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_TestTranformAxes(const Handle(asiTcl_Interp)& interp,
                          int                          argc,
                          const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  /* ==============
   *  Get CAD part
   * ============== */

  Handle(asiEngine_Model) M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );
  //
  TopoDS_Shape partShape = M->GetPartNode()->GetShape();

  /* =================
   *  Get point cloud
   * ================= */

  // Get points.
  Handle(asiData_IVPointSetNode)
    pointsNode = Handle(asiData_IVPointSetNode)::DownCast( interp->GetModel()->FindNodeByName(argv[1]) );
  //
  if ( pointsNode.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find Points Node with name %1." << argv[2]);
    return TCL_OK;
  }
  //
  Handle(asiAlgo_BaseCloud<double>) ptsCloud = pointsNode->GetPoints();

  // Get bounding box of the point cloud to scale axes.
  double xMin, xMax, yMin, yMax, zMin, zMax;
  ptsCloud->ComputeBoundingBox(xMin, xMax, yMin, yMax, zMin, zMax);
  //
  const double ptsDiag    = gp_Pnt(xMin, yMin, zMin).Distance( gp_Pnt(xMax, yMax, zMax) );
  const double scaleCoeff = ptsDiag*0.1;

  /* ================================
   *  Compute axes of the part shape
   * ================================ */

  GProp_GProps gProps;
  BRepGProp::VolumeProperties(partShape, gProps, 1.e-4);

  gp_Vec partPrincipalX = gProps.PrincipalProperties().ThirdAxisOfInertia();
  gp_Vec partPrincipalY = gProps.PrincipalProperties().SecondAxisOfInertia();
  gp_Vec partPrincipalZ = gProps.PrincipalProperties().FirstAxisOfInertia();

  // First Ax3.
  //gp_Ax3 A( gp_Pnt(-1., -2., -3.), gp_Dir(1., 1., 1.), gp_Dir(1., -1., -1.) );
  gp_Ax3 A(gProps.CentreOfMass(), partPrincipalZ, partPrincipalX);
  //
  interp->GetPlotter().DRAW_POINT     (A.Location(),                                            Color_Yellow, "A");
  interp->GetPlotter().DRAW_VECTOR_AT (A.Location(), gp_Vec( A.XDirection().XYZ() )*scaleCoeff, Color_Red,    "A_DX");
  interp->GetPlotter().DRAW_VECTOR_AT (A.Location(), gp_Vec( A.YDirection().XYZ() )*scaleCoeff, Color_Green,  "A_DY");
  interp->GetPlotter().DRAW_VECTOR_AT (A.Location(), gp_Vec( A.Direction() .XYZ() )*scaleCoeff, Color_Blue,   "A_DZ");

  /* =================================
   *  Compute axes of the point cloud
   * ================================= */

  // Second Ax3.
  //gp_Ax3 B( gp_Pnt(4., 2., 3.), gp_Dir(1., -1., 0.25), gp_Dir(-1., 1., 0.5) );
  gp_Ax3 B;
  ptsCloud->ComputeInertiaAxes(B);
  //
  interp->GetPlotter().DRAW_POINT     (B.Location(),                                            Color_Yellow, "B");
  interp->GetPlotter().DRAW_VECTOR_AT (B.Location(), gp_Vec( B.XDirection().XYZ() )*scaleCoeff, Color_Red,    "B_DX");
  interp->GetPlotter().DRAW_VECTOR_AT (B.Location(), gp_Vec( B.YDirection().XYZ() )*scaleCoeff, Color_Green,  "B_DY");
  interp->GetPlotter().DRAW_VECTOR_AT (B.Location(), gp_Vec( B.Direction() .XYZ() )*scaleCoeff, Color_Blue,   "B_DZ");

  /* ===========
   *  Move axes
   * =========== */

  // B goes to global origin.
  gp_Trsf T1;
  T1.SetTransformation(B);

  // Global origin goes to A.
  gp_Trsf T2;
  T2.SetTransformation(A);
  T2.Invert();

  // Final transformation from B to A.
  gp_Trsf T = T2*T1;

  // Transformed B.
  gp_Ax3 TB = B.Transformed(T);
  //
  interp->GetPlotter().DRAW_POINT     (TB.Location(),                                             Color_White, "TB");
  interp->GetPlotter().DRAW_VECTOR_AT (TB.Location(), gp_Vec( TB.XDirection().XYZ() )*scaleCoeff, Color_White, "TB_DX");
  interp->GetPlotter().DRAW_VECTOR_AT (TB.Location(), gp_Vec( TB.YDirection().XYZ() )*scaleCoeff, Color_White, "TB_DY");
  interp->GetPlotter().DRAW_VECTOR_AT (TB.Location(), gp_Vec( TB.Direction() .XYZ() )*scaleCoeff, Color_White, "TB_DZ");

  /* ==================
   *  Move point cloud
   * ================== */

  Handle(asiAlgo_BaseCloud<double>) movedPtsCloud = new asiAlgo_BaseCloud<double>;
  ptsCloud->CopyTo(*movedPtsCloud);
  //
  for ( int k = 0; k < movedPtsCloud->GetNumberOfElements(); ++k )
  {
    gp_Pnt P( movedPtsCloud->GetElement(k) );
    P.Transform(T);
    movedPtsCloud->SetElement( k, P.X(), P.Y(), P.Z() );
  }

  interp->GetPlotter().REDRAW_POINTS("res", movedPtsCloud->GetCoordsArray(), Color_Default);

  gp_Ax3 BAfter;
  movedPtsCloud->ComputeInertiaAxes(BAfter);
  //
  interp->GetPlotter().DRAW_POINT     (BAfter.Location(),                                                 Color_Yellow, "BAfter");
  interp->GetPlotter().DRAW_VECTOR_AT (BAfter.Location(), gp_Vec( BAfter.XDirection().XYZ() )*scaleCoeff, Color_Red,    "BAfter_DX");
  interp->GetPlotter().DRAW_VECTOR_AT (BAfter.Location(), gp_Vec( BAfter.YDirection().XYZ() )*scaleCoeff, Color_Green,  "BAfter_DY");
  interp->GetPlotter().DRAW_VECTOR_AT (BAfter.Location(), gp_Vec( BAfter.Direction() .XYZ() )*scaleCoeff, Color_Blue,   "BAfter_DZ");

  return TCL_OK;
}

#if defined USE_MOBIUS
//-----------------------------------------------------------------------------

int MISC_TestFair(const Handle(asiTcl_Interp)& interp,
                  int                          argc,
                  const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node to access the selected faces.
  Handle(asiData_PartNode)
    partNode = Handle(asiEngine_Model)::DownCast( interp->GetModel() )->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  Handle(asiAlgo_AAG) aag   = partNode->GetAAG();
  TopoDS_Shape        shape = partNode->GetShape();

  // Get face.
  const int fid = atoi(argv[1]);
  //
  if ( !partNode->GetAAG()->HasFace(fid) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Face %1 does not exist in the working part."
                                                        << fid);
    return TCL_ERROR;
  }
  //
  const TopoDS_Face& face = aag->GetFace(fid);

  // Get host B-surface.
  Handle(Geom_BSplineSurface)
    occtBSurf = Handle(Geom_BSplineSurface)::DownCast( BRep_Tool::Surface(face) );
  //
  if ( occtBSurf.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Face %1 is not based on B-spline surface."
                                                        << fid);
    return TCL_ERROR;
  }

  // Convert to Mobius B-surface.
  t_ptr<t_bsurf>
    mobSurf = cascade::GetMobiusBSurface(occtBSurf);

  // Print bending energy.
  const double initEnergy = mobSurf->ComputeBendingEnergy();
  //
  interp->GetProgress().SendLogMessage( LogNotice(Normal) << "Initial bending energy: %1"
                                                          << initEnergy );

  // Utilities to find spans.
  std::vector<double> U = mobSurf->GetKnots_U();
  std::vector<double> V = mobSurf->GetKnots_V();
  const int           p = mobSurf->GetDegree_U();
  const int           q = mobSurf->GetDegree_V();
  //
  bspl_FindSpan findSpanU(U, p);
  bspl_FindSpan findSpanV(V, q);

  // Prepare fairing operator.
  geom_FairBSurf F(mobSurf, 1.0, nullptr, nullptr);

  // Calculate characteristics size for discretization of boundary.
  double uMin, uMax, vMin, vMax;
  BRepTools::UVBounds(face, uMin, uMax, vMin, vMax);
  //
  const double featSize = gp_Pnt2d(uMin, vMin).Distance( gp_Pnt2d(uMax, vMax) )*0.01;

  // Get edges of the face which are the constraints to satisfy.
  TopTools_IndexedMapOfShape faceEdges;
  TopExp::MapShapes(face, TopAbs_EDGE, faceEdges);

  // For each edge, take its parameteric curve and discretize it to obtain
  // series of (u,v) coordinates. These coordinates will be used to find
  // indices of the effective control points which should be constrained.
  std::vector<gp_XY> pts;
  for ( int eidx = 1; eidx <= faceEdges.Extent(); ++eidx )
  {
    const TopoDS_Edge& E = TopoDS::Edge( faceEdges(eidx) );

    // Discretize with a uniform curvilinear step.
    BRepAdaptor_Curve2d gac(E, face);
    GCPnts_UniformAbscissa Defl(gac, featSize);
    //
    if ( !Defl.IsDone() )
    {
      interp->GetPlotter().DRAW_SHAPE(E, Color_Red, 1.0, true, "cannotDiscr");
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Edge discretization failed.");
      return TCL_OK;
    }

    // Calculate discretization points.
    for ( int i = 1; i <= Defl.NbPoints(); ++i )
    {
      gp_XY P = gac.Value( Defl.Parameter(i) ).XY();
      //
      pts.push_back(P);

      //interp->GetPlotter().DRAW_POINT(P, Color_Red, "pts");

      // Get (u,v) coordinates.
      const double u = P.X();
      const double v = P.Y();

      // Find effective span.
      const int spanU = findSpanU(u);
      const int spanV = findSpanV(v);

      std::cout << "Found span in [u,v]: [" << spanU << "," << spanV << "]" << std::endl;

      gp_Pnt2d S1( U[spanU], V[spanV] );
      gp_Pnt2d S2( U[spanU + 1], V[spanV] );
      gp_Pnt2d S3( U[spanU + 1], V[spanV + 1] );
      gp_Pnt2d S4( U[spanU], V[spanV + 1] );

      /*interp->GetPlotter().DRAW_LINK(S1, S2, Color_Green, "S12");
      interp->GetPlotter().DRAW_LINK(S2, S3, Color_Green, "S23");
      interp->GetPlotter().DRAW_LINK(S3, S4, Color_Green, "S34");
      interp->GetPlotter().DRAW_LINK(S4, S1, Color_Green, "S41");*/

      const t_xyz& mobPole1 = mobSurf->GetPoles()[spanU][spanV];

      // Pin point.
      F.AddPinnedPole(spanU, spanV);

      interp->GetPlotter().DRAW_POINT(gp_Pnt( mobPole1.X(), mobPole1.Y(), mobPole1.Z() ), Color_Blue, "mobPole");

      for ( int k = 0; k < p; ++k )
      {
        //const t_xyz& PPP = mobSurf->Poles()[spanU - k][spanV];
        F.AddPinnedPole(spanU-k, spanV);
        //interp->GetPlotter().DRAW_POINT(gp_Pnt( PPP.X(), PPP.Y(), PPP.Z() ), Color_Red, "mobPoleU");
      }

      for ( int k = 0; k < q; ++k )
      {
        //const t_xyz& PPP = mobSurf->Poles()[spanU][spanV - k];
        F.AddPinnedPole(spanU, spanV-k);
        //interp->GetPlotter().DRAW_POINT(gp_Pnt( PPP.X(), PPP.Y(), PPP.Z() ), Color_Green, "mobPoleV");
      }
    }
  }

  TIMER_NEW
  TIMER_GO

  // Perform fairing.
  if ( !F.Perform() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Fairing failed.");
    return TCL_OK;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Fair surface")

  // Get fairing result.
  const t_ptr<t_bsurf>& mobFaired = F.GetResult();

  // Print bending energy.
  const double resEnergy = mobFaired->ComputeBendingEnergy();
  //
  interp->GetProgress().SendLogMessage( LogNotice(Normal) << "Resulting bending energy: %1"
                                                          << resEnergy );

  // Convert to OpenCascade surface.
  Handle(Geom_BSplineSurface)
    occtFaired = cascade::GetOpenCascadeBSurface(mobFaired);
  //
  interp->GetPlotter().DRAW_SURFACE(occtFaired, (resEnergy < initEnergy) ? Color_Green : Color_Red, "faired");

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_TestCoons1(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  /* ===================
   *  Build rail curves
   * =================== */

  t_ptr<t_bcurve> c0, c1, b0, b1;
  t_xyz p00, p01, p10, p11;

  // Build `c` curves.
  {
    std::vector<t_xyz> c0_pts = {
      t_xyz(0.,  0.,  0.),
      t_xyz(0.,  1.,  0.1),
      t_xyz(0.,  2.,  0.2),
      t_xyz(0.,  3.,  0.3),
      t_xyz(0.,  4.,  0.2),
      t_xyz(0.,  4.5, 0.)
    };

    std::vector<t_xyz> c1_pts = {
      t_xyz(5.,  0.,  0.),
      t_xyz(5.,  1.,  0.2),
      t_xyz(5.,  2.,  0.1),
      t_xyz(5.,  3.,  0.2),
      t_xyz(5.,  4.,  0.1),
      t_xyz(5.,  5.,  0.)
    };

    // Corner points.
    p00 = c0_pts[0];
    p01 = c1_pts[0];
    p10 = c0_pts[c0_pts.size() - 1];
    p11 = c1_pts[c1_pts.size() - 1];

    interp->GetPlotter().REDRAW_POINT("p00", cascade::GetOpenCascadePnt(p00), Color_Yellow);
    interp->GetPlotter().REDRAW_POINT("p01", cascade::GetOpenCascadePnt(p01), Color_Yellow);
    interp->GetPlotter().REDRAW_POINT("p10", cascade::GetOpenCascadePnt(p10), Color_Yellow);
    interp->GetPlotter().REDRAW_POINT("p11", cascade::GetOpenCascadePnt(p11), Color_Yellow);

    // Build sample curves.
    geom_InterpolateMultiCurve multiInterp(3,
                                           ParamsSelection_Centripetal,
                                           KnotsSelection_Average);
    //
    multiInterp.AddRow(c0_pts);
    multiInterp.AddRow(c1_pts);

    if ( multiInterp.Perform() )
    {
      // Get rail curves.
      c0 = multiInterp.GetResult(0);
      c1 = multiInterp.GetResult(1);
      //
      interp->GetPlotter().REDRAW_CURVE("c0", cascade::GetOpenCascadeBCurve(c0), Color_Default);
      interp->GetPlotter().REDRAW_CURVE("c1", cascade::GetOpenCascadeBCurve(c1), Color_Default);
    }
    else
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Multi-curve interpolation failed.");
      return TCL_ERROR;
    }
  }

  // Build `b` curves.
  {
    std::vector<t_xyz> b0_pts = {
      p00,
      t_xyz(2., 0.0, 0.1),
      t_xyz(3., 0.0, 0.2),
      t_xyz(4., 0.0, 0.1),
      p01
    };

    std::vector<t_xyz> b1_pts = {
      p10,
      t_xyz(2., 5.0, 0.2),
      t_xyz(3., 5.0, 0.1),
      t_xyz(4., 5.0, 0.2),
      p11
    };

    // Build sample curves.
    geom_InterpolateMultiCurve multiInterp(3,
                                           ParamsSelection_Centripetal,
                                           KnotsSelection_Average);
    //
    multiInterp.AddRow(b0_pts);
    multiInterp.AddRow(b1_pts);

    if ( multiInterp.Perform() )
    {
      // Get rail curves.
      b0 = multiInterp.GetResult(0);
      b1 = multiInterp.GetResult(1);
      //
      interp->GetPlotter().REDRAW_CURVE("b0", cascade::GetOpenCascadeBCurve(b0), Color_Default);
      interp->GetPlotter().REDRAW_CURVE("b1", cascade::GetOpenCascadeBCurve(b1), Color_Default);
    }
    else
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Multi-curve interpolation failed.");
      return TCL_ERROR;
    }
  }

  /* ===================
   *  Build Coons patch
   * =================== */

  // Build Coons surface.
  t_ptr<geom_CoonsSurfaceLinear>
    coons = new geom_CoonsSurfaceLinear(c0, c1, b0, b1, p00, p01, p10, p11);

  // Sample patch.
  cmdMisc::DrawSurfPts(interp, coons, "coons-pts");

  // Use constrained filling for demo.
  std::vector<Handle(Geom_BSplineCurve)>
    curves = { cascade::GetOpenCascadeBCurve(c0),
               cascade::GetOpenCascadeBCurve(c1),
               cascade::GetOpenCascadeBCurve(b0),
               cascade::GetOpenCascadeBCurve(b1) };
  //
  Handle(Geom_BSplineSurface) surf;
  //
  if ( !asiAlgo_Utils::Fill4Contour(curves, surf) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Filling 4-sides failed.");
    return TCL_ERROR;
  }
  //
  interp->GetPlotter().DRAW_SURFACE(surf, Color_White, "coonssurf");

  /* =========================================================
   *  Use Boolean sum approach to build an equivalent surface
   * ========================================================= */

  // Build P1S.
  t_ptr<t_bsurf> P1S;
  Handle(Geom_BSplineSurface) P1Socc;
  {
    // Prepare rail curves.
    std::vector< t_ptr<t_bcurve> > rails = {c0, c1};

    // Skin ruled surface through c0 and c1 rails.
    geom_SkinSurface skinner(rails, 1, false);
    //
    if ( !skinner.Perform() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot build P1S.");
      return TCL_ERROR;
    }
    //
    P1S = skinner.GetResult();

    // Convert to OCC form to run IncreaseDegree() method which does not
    // currently exist in Mobius.
    P1Socc = cascade::GetOpenCascadeBSurface(P1S);

    // Elevate degree.
    P1Socc->IncreaseDegree(3, 3);

    // Convert back to Mobius.
    P1S = cascade::GetMobiusBSurface(P1Socc);

    // Draw.
    interp->GetPlotter().REDRAW_SURFACE("P1S", cascade::GetOpenCascadeBSurface(P1S), Color_Default);
  }

  // Build P2S.
  t_ptr<t_bsurf>  P2S;
  Handle(Geom_BSplineSurface) P2Socc;
  {
    // Prepare rail curves.
    std::vector< t_ptr<t_bcurve> > rails = {b0, b1};

    // Skin ruled surface through b0 and b1 rails.
    geom_SkinSurface skinner(rails, 1, false);
    //
    if ( !skinner.Perform() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot build P2S.");
      return TCL_ERROR;
    }
    //
    P2S = skinner.GetResult();

    // Flip UV coordinates of P2S to be compliant with other surfaces.
    P2S->ExchangeUV();

    // Convert to OCC form to run IncreaseDegree() method which does not
    // currently exist in Mobius.
    P2Socc = cascade::GetOpenCascadeBSurface(P2S);

    // Elevate degree.
    P2Socc->IncreaseDegree(3, 3);

    // Convert back to Mobius.
    P2S = cascade::GetMobiusBSurface(P2Socc);

    // Draw.
    interp->GetPlotter().REDRAW_SURFACE("P2S", cascade::GetOpenCascadeBSurface(P2S), Color_Default);
  }

  // Build P12S.
  t_ptr<t_bsurf>  P12S;
  Handle(Geom_BSplineSurface) P12Socc;
  {
    std::vector< std::vector<t_xyz> >
      poles = { {p00, p01},
                {p10, p11} };

    // Define knot vector.
    std::vector<double> T = {0, 0, 1, 1};

    // Construct directly.
    P12S = new geom_BSplineSurface(poles, T, T, 1, 1);

    // Convert to OCC form to run IncreaseDegree() method which does not
    // currently exist in Mobius.
    P12Socc = cascade::GetOpenCascadeBSurface(P12S);

    // Elevate degree.
    P12Socc->IncreaseDegree(3, 3);

    // Convert back to Mobius.
    P12S = cascade::GetMobiusBSurface(P12Socc);

    // Draw.
    interp->GetPlotter().REDRAW_SURFACE("P12S", cascade::GetOpenCascadeBSurface(P12S), Color_Default);
  }

  enum PS
  {
    PS_P1S = 0,
    PS_P2S,
    PS_P12S
  };

  // All U knots.
  std::vector< std::vector<double> >
    knotVectors_U = { P1S->GetKnots_U(),
                      P2S->GetKnots_U(),
                      P12S->GetKnots_U() };

  // All V knots.
  std::vector< std::vector<double> >
    knotVectors_V = { P1S->GetKnots_V(),
                      P2S->GetKnots_V(),
                      P12S->GetKnots_V() };

  // Compute addendum knots.
  bspl_UnifyKnots unifyKnots;
  //
  std::vector< std::vector<double> > U_addendums = unifyKnots(knotVectors_U);
  std::vector< std::vector<double> > V_addendums = unifyKnots(knotVectors_V);

  // Insert U knots to P1S.
  for ( size_t ii = 0; ii < U_addendums[PS_P1S].size(); ++ii )
    P1S->InsertKnot_U(U_addendums[PS_P1S][ii]);

  // Insert V knots to P1S.
  for ( size_t ii = 0; ii < V_addendums[PS_P1S].size(); ++ii )
    P1S->InsertKnot_V(V_addendums[PS_P1S][ii]);

  // Insert U knots to P2S.
  for ( size_t ii = 0; ii < U_addendums[PS_P2S].size(); ++ii )
    P2S->InsertKnot_U(U_addendums[PS_P2S][ii]);

  // Insert V knots to P2S.
  for ( size_t ii = 0; ii < V_addendums[PS_P2S].size(); ++ii )
    P2S->InsertKnot_V(V_addendums[PS_P2S][ii]);

  // Insert U knots to P12S.
  for ( size_t ii = 0; ii < U_addendums[PS_P12S].size(); ++ii )
    P12S->InsertKnot_U(U_addendums[PS_P12S][ii]);

  // Insert V knots to P12S.
  for ( size_t ii = 0; ii < V_addendums[PS_P12S].size(); ++ii )
    P12S->InsertKnot_V(V_addendums[PS_P12S][ii]);

  // Common knots.
  const std::vector<double>& Ucommon = P1S->GetKnots_U();
  const std::vector<double>& Vcommon = P1S->GetKnots_V();

  // Common degrees.
  const int pcommon = P1S->GetDegree_U();
  const int qcommon = P1S->GetDegree_V();

  // Draw.
  interp->GetPlotter().REDRAW_SURFACE("P1S",  cascade::GetOpenCascadeBSurface(P1S),  Color_Default);
  interp->GetPlotter().REDRAW_SURFACE("P2S",  cascade::GetOpenCascadeBSurface(P2S),  Color_Default);
  interp->GetPlotter().REDRAW_SURFACE("P12S", cascade::GetOpenCascadeBSurface(P12S), Color_Default);

  // Now all patches are of the same degrees and defined on identical knot
  // vectors. It means that all patches are defined on the same basis. Therefore,
  // we can now produce a Boolean sum.

  const std::vector< std::vector<t_xyz> >& polesP1S  = P1S->GetPoles();
  const std::vector< std::vector<t_xyz> >& polesP2S  = P2S->GetPoles();
  const std::vector< std::vector<t_xyz> >& polesP12S = P12S->GetPoles();

  const int numPolesU = P1S->GetNumOfPoles_U();
  const int numPolesV = P1S->GetNumOfPoles_V();

  // Compute the resulting poles.
  std::vector< std::vector<t_xyz> > resPoles;
  //
  for ( int i = 0; i < numPolesU; ++i )
  {
    std::vector<t_xyz> col;
    for ( int j = 0; j < numPolesV; ++j )
    {
      t_xyz resPole = polesP1S[i][j] + polesP2S[i][j] - polesP12S[i][j];
      //
      col.push_back(resPole);
    }
    resPoles.push_back(col);
  }

  // Construct the resulting surface.
  t_ptr<t_bsurf>
    mobRes = new t_bsurf(resPoles, Ucommon, Vcommon, pcommon, qcommon);
  //
  interp->GetPlotter().REDRAW_SURFACE("res-simple", cascade::GetOpenCascadeBSurface(mobRes), Color_Default);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

#define c0_D1 t_xyz(0.0, 0.0, 1.0)
#define c1_D1 t_xyz(1.0, 0.0, 0.0)
#define b0_D1 t_xyz(0.0, 1.0, 0.0)
#define b1_D1 t_xyz(0.0, 1.0, 0.0)

class derivConstraint : public geom_Curve
{
public:

  derivConstraint(const t_ptr<t_curve>& c0, const t_ptr<t_curve>& c1, const t_ptr<t_curve>& b0, const t_ptr<t_curve>& b1)
  : geom_Curve(), m_c0(c0), m_c1(c1), m_b0(b0), m_b1(b1) {}

public:

  virtual void   GetBounds(double&, double&, double&, double&, double&, double&) const {}
  virtual double GetMinParameter() const { return 0.0; }
  virtual double GetMaxParameter() const { return 1.0; }

protected:

  t_ptr<t_curve> m_c0, m_c1, m_b0, m_b1; //!< Boundary curves.

};

// Boundary constraint for derivative `s_v(u,0)`.
class dSu0_dv : public derivConstraint
{
public:

  dSu0_dv(const t_ptr<t_curve>& c0, const t_ptr<t_curve>& c1, const t_ptr<t_curve>& b0, const t_ptr<t_curve>& b1)
  : derivConstraint(c0, c1, b0, b1) {}

public:

  virtual void Eval(const double u, t_xyz& P) const
  {
    const double uMin = this->GetMinParameter();
    const double uMax = this->GetMinParameter();

    if ( fabs(u - uMin) < DBL_EPSILON )
      P = m_b0->D1(uMin);
    else if ( fabs(u - uMax) < DBL_EPSILON )
      P = m_b1->D1(uMin);
    else
    {
      P = (1 - u)*m_b0->D1(uMin) + u*m_b1->D1(uMin);
      //P = c0_D1;
    }
  }

  virtual void Eval_D1(const double /*u*/, t_xyz& P) const
  {
    const double uMin = this->GetMinParameter();
    //const double uMax = this->GetMinParameter();

    P = m_b1->D1(uMin) - m_b0->D1(uMin);
  }

};

// Boundary constraint for derivative `s_v(u,1)`.
class dSu1_dv : public derivConstraint
{
public:

  dSu1_dv(const t_ptr<t_curve>& c0, const t_ptr<t_curve>& c1, const t_ptr<t_curve>& b0, const t_ptr<t_curve>& b1)
  : derivConstraint(c0, c1, b0, b1) {}

public:

  virtual void Eval(const double u, t_xyz& P) const
  {
    const double uMin = this->GetMinParameter();
    const double uMax = this->GetMinParameter();

    if ( fabs(u - uMin) < DBL_EPSILON )
      P = m_b0->D1(uMax);
    else if ( fabs(u - uMax) < DBL_EPSILON )
      P = m_b1->D1(uMax);
    else
    {
      P = (1 - u)*m_b0->D1(uMax) + u*m_b1->D1(uMax);

      // P = c1_D1;
    }
  }

  virtual void Eval_D1(const double /*u*/, t_xyz& P) const
  {
    //const double uMin = this->GetMinParameter();
    const double uMax = this->GetMinParameter();

    P = m_b1->D1(uMax) - m_b0->D1(uMax);
  }

};

// Boundary constraint for derivative `s_u(0,v)`.
class dS0v_du : public derivConstraint
{
public:

  dS0v_du(const t_ptr<t_curve>& c0, const t_ptr<t_curve>& c1, const t_ptr<t_curve>& b0, const t_ptr<t_curve>& b1)
  : derivConstraint(c0, c1, b0, b1) {}

public:

  virtual void Eval(const double u, t_xyz& P) const
  {
    const double uMin = this->GetMinParameter();
    const double uMax = this->GetMinParameter();

    if ( fabs(u - uMin) < DBL_EPSILON )
      P = m_c0->D1(uMin);
    else if ( fabs(u - uMax) < DBL_EPSILON )
      P = m_c1->D1(uMin);
    else
    {
      P = (1 - u)*m_c0->D1(uMin) + u*m_c1->D1(uMin);

      // P = b0_D1;
    }
  }

  virtual void Eval_D1(const double /*u*/, t_xyz& P) const
  {
    const double uMin = this->GetMinParameter();
    //const double uMax = this->GetMinParameter();

    P = m_c1->D1(uMin) - m_c0->D1(uMin);
  }

};

// Boundary constraint for derivative `s_u(1,v)`.
class dS1v_du : public derivConstraint
{
public:

  dS1v_du(const t_ptr<t_curve>& c0, const t_ptr<t_curve>& c1, const t_ptr<t_curve>& b0, const t_ptr<t_curve>& b1)
  : derivConstraint(c0, c1, b0, b1) {}

public:

  virtual void Eval(const double u, t_xyz& P) const
  {
    const double uMin = this->GetMinParameter();
    const double uMax = this->GetMinParameter();

    if ( fabs(u - uMin) < DBL_EPSILON )
      P = m_c0->D1(uMax);
    else if ( fabs(u - uMax) < DBL_EPSILON )
      P = m_c1->D1(uMax);
    else
    {
      P = (1 - u)*m_c0->D1(uMax) + u*m_c1->D1(uMax);

      // P = b1_D1;
    }
  }

  virtual void Eval_D1(const double /*u*/, t_xyz& P) const
  {
    //const double uMin = this->GetMinParameter();
    const double uMax = this->GetMinParameter();

    P = m_c1->D1(uMax) - m_c0->D1(uMax);
  }

};

int MISC_TestCoons2(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Corner points.
  t_xyz p00(0., 0.,  0.);
  t_xyz p10(0., 4.5, 0.);
  t_xyz p20(0., 8.5, 0.);
  t_xyz p01(5., 0.,  0.);
  t_xyz p11(5., 5.,  0.);
  t_xyz p21(5., 9.,  0.);

  // Draw points.
  interp->GetPlotter().REDRAW_POINT("p00", cascade::GetOpenCascadePnt(p00), Color_Yellow);
  interp->GetPlotter().REDRAW_POINT("p10", cascade::GetOpenCascadePnt(p10), Color_Yellow);
  interp->GetPlotter().REDRAW_POINT("p20", cascade::GetOpenCascadePnt(p20), Color_Yellow);
  interp->GetPlotter().REDRAW_POINT("p01", cascade::GetOpenCascadePnt(p01), Color_Yellow);
  interp->GetPlotter().REDRAW_POINT("p11", cascade::GetOpenCascadePnt(p11), Color_Yellow);
  interp->GetPlotter().REDRAW_POINT("p21", cascade::GetOpenCascadePnt(p21), Color_Yellow);

  /* ===================
   *  Build rail curves
   * =================== */

  t_ptr<t_bcurve> c0, c1, c2, c3, b0, b1, b2;
  std::vector<t_xyz> c0_D1_vec, c1_D1_vec, b0_D1_vec, b1_D1_vec;

  // Build `c` curves.
  {
    /* c0 */

    std::vector<t_xyz> c0_pts = {
      p00,
      t_xyz(0., 1., 0.1),
      t_xyz(0., 2., 0.2),
      t_xyz(0., 3., 0.3),
      t_xyz(0., 4., 0.2),
      p10
    };
    //
    c0_D1_vec = {
      t_xyz(0.,  0.,  1.),
      t_xyz(0.,  0.,  1.),
      t_xyz(0.,  0.,  1.),
      t_xyz(0.,  0.,  1.),
      t_xyz(0.,  0.,  1.),
      t_xyz(0.,  0.,  1.)
    };
    //
    for ( size_t k = 0; k < c0_D1_vec.size(); ++k )
      interp->GetPlotter().DRAW_VECTOR_AT(cascade::GetOpenCascadePnt(c0_pts[k]),
                                          cascade::GetOpenCascadeVec(c0_D1_vec[k]),
                                          Color_Red, "c0_D1");

    /* c1 */

    std::vector<t_xyz> c1_pts = {
      p01,
      t_xyz(5., 1., 0.2),
      t_xyz(5., 2., 0.1),
      t_xyz(5., 3., 0.2),
      t_xyz(5., 4., 0.1),
      p11
    };
    //
    c1_D1_vec = {
      t_xyz(1., 0., 0.),
      t_xyz(1., 0., 0.),
      t_xyz(1., 0., 0.),
      t_xyz(1., 0., 0.),
      t_xyz(1., 0., 0.),
      t_xyz(1., 0., 0.)
    };
    //
    for ( size_t k = 0; k < c1_D1_vec.size(); ++k )
      interp->GetPlotter().DRAW_VECTOR_AT(cascade::GetOpenCascadePnt(c1_pts[k]),
                                          cascade::GetOpenCascadeVec(c1_D1_vec[k]),
                                          Color_Red, "c1_D1");

    /* c2 */

    std::vector<t_xyz> c2_pts = {
      p10,
      t_xyz(0.,  5., 0.2),
      t_xyz(0.1, 6., 0.1),
      t_xyz(0.1, 7., 0.2),
      t_xyz(0.,  8., 0.1),
      p20
    };

    /* c3 */

    std::vector<t_xyz> c3_pts = {
      p11,
      t_xyz(5.,  5.5, 0.),
      t_xyz(5.1, 6.,  0.1),
      t_xyz(5.1, 7.,  0.2),
      t_xyz(5.,  8.,  0.1),
      p21
    };

    // Build sample curves.
    geom_InterpolateMultiCurve multiInterp(3,
                                                   ParamsSelection_Centripetal,
                                                   KnotsSelection_Average);
    //
    multiInterp.AddRow(c0_pts);
    multiInterp.AddRow(c1_pts);
    multiInterp.AddRow(c2_pts);
    multiInterp.AddRow(c3_pts);

    if ( multiInterp.Perform() )
    {
      // Get rail curves.
      c0 = multiInterp.GetResult(0);
      c1 = multiInterp.GetResult(1);
      c2 = multiInterp.GetResult(2);
      c3 = multiInterp.GetResult(3);
      //
      interp->GetPlotter().REDRAW_CURVE("c0", cascade::GetOpenCascadeBCurve(c0), Color_Default);
      interp->GetPlotter().REDRAW_CURVE("c1", cascade::GetOpenCascadeBCurve(c1), Color_Default);
      interp->GetPlotter().REDRAW_CURVE("c2", cascade::GetOpenCascadeBCurve(c2), Color_Default);
      interp->GetPlotter().REDRAW_CURVE("c3", cascade::GetOpenCascadeBCurve(c3), Color_Default);
    }
    else
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Multi-curve interpolation failed.");
      return TCL_ERROR;
    }
  }

  // Build `b` curves.
  {
    /* b0 */

    std::vector<t_xyz> b0_pts = {
      p00,
      t_xyz(2., 0.0, 0.1),
      t_xyz(3., 0.0, 0.2),
      t_xyz(4., 0.0, 0.1),
      p01
    };
    //
    b0_D1_vec = {
      t_xyz(0., 1., 0.),
      t_xyz(0., 1., 0.),
      t_xyz(0., 1., 0.),
      t_xyz(0., 1., 0.),
      t_xyz(0., 1., 0.)
    };
    //
    for ( size_t k = 0; k < b0_D1_vec.size(); ++k )
      interp->GetPlotter().DRAW_VECTOR_AT(cascade::GetOpenCascadePnt(b0_pts[k]),
                                          cascade::GetOpenCascadeVec(b0_D1_vec[k]),
                                          Color_Blue, "b0_D1");

    /* b1 */

    std::vector<t_xyz> b1_pts = {
      p10,
      t_xyz(2., 5.0, 0.2),
      t_xyz(3., 5.0, 0.1),
      t_xyz(4., 5.0, 0.2),
      p11
    };
    //
    b1_D1_vec = {
      t_xyz(0., 1., 0.),
      t_xyz(0., 1., 0.),
      t_xyz(0., 1., 0.),
      t_xyz(0., 1., 0.),
      t_xyz(0., 1., 0.)
    };
    //
    for ( size_t k = 0; k < b1_D1_vec.size(); ++k )
      interp->GetPlotter().DRAW_VECTOR_AT(cascade::GetOpenCascadePnt(b1_pts[k]),
                                          cascade::GetOpenCascadeVec(b1_D1_vec[k]),
                                          Color_Blue, "b1_D1");

    /* b2 */

    std::vector<t_xyz> b2_pts = {
      p20,
      t_xyz(2., 8.5, -0.2),
      t_xyz(3., 8.9, -0.1),
      t_xyz(4., 8.7,  0.2),
      p21
    };

    // Build sample curves.
    geom_InterpolateMultiCurve multiInterp(3,
                                           ParamsSelection_Centripetal,
                                           KnotsSelection_Average);
    //
    multiInterp.AddRow(b0_pts);
    multiInterp.AddRow(b1_pts);
    multiInterp.AddRow(b2_pts);

    if ( multiInterp.Perform() )
    {
      // Get rail curves.
      b0 = multiInterp.GetResult(0);
      b1 = multiInterp.GetResult(1);
      b2 = multiInterp.GetResult(2);
      //
      interp->GetPlotter().REDRAW_CURVE("b0", cascade::GetOpenCascadeBCurve(b0), Color_Default);
      interp->GetPlotter().REDRAW_CURVE("b1", cascade::GetOpenCascadeBCurve(b1), Color_Default);
      interp->GetPlotter().REDRAW_CURVE("b2", cascade::GetOpenCascadeBCurve(b2), Color_Default);
    }
    else
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Multi-curve interpolation failed.");
      return TCL_ERROR;
    }
  }

  /* =====================
   *  Make P1 by skinning
   * ===================== */

  t_ptr<t_bsurf> P1S;
  {
    std::vector< t_ptr<t_bcurve> > rails = {c0, c1};

    // Skin ruled surface through the rail curves.
    geom_SkinSurface skinner(rails, 3, false);
    //
    skinner.AddLeadingTangencies(c0_D1_vec);
    skinner.AddTrailingTangencies(c1_D1_vec);
    //
    if ( !skinner.Perform() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot build skinning surface.");
      return TCL_ERROR;
    }
    //
    P1S = skinner.GetResult();

    // Convert to OCC form to run IncreaseDegree() method which does not
    // currently exist in Mobius.
    Handle(Geom_BSplineSurface) P1Socc = cascade::GetOpenCascadeBSurface(P1S);

    // Convert back to Mobius.
    P1S = cascade::GetMobiusBSurface(P1Socc);
  }

  // Draw.
  interp->GetPlotter().REDRAW_SURFACE("P1", cascade::GetOpenCascadeBSurface(P1S), Color_White);

  /* =====================
   *  Make P2 by skinning
   * ===================== */

  t_ptr<t_bsurf> P2S;
  {
    std::vector< t_ptr<t_bcurve> > rails = {b0, b1};

    // Skin ruled surface through the rail curves.
    geom_SkinSurface skinner(rails, 3, false);
    //
    skinner.AddLeadingTangencies(b0_D1_vec);
    skinner.AddTrailingTangencies(b1_D1_vec);
    //
    if ( !skinner.Perform() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot build skinning surface.");
      return TCL_ERROR;
    }
    //
    P2S = skinner.GetResult();

    // Flip UV coordinates of P2S to be compliant with other surfaces.
    P2S->ExchangeUV();
  }

  // Draw.
  interp->GetPlotter().REDRAW_SURFACE("P2", cascade::GetOpenCascadeBSurface(P2S), Color_White);

  /* ============================================
   *  Make P12 as bicubic patch with constraints
   * ============================================ */

  // Prepare derivative curves.
  t_ptr<t_curve> c0_v = new dSu0_dv(c0, c1, b0, b1);
  t_ptr<t_curve> c1_v = new dSu1_dv(c0, c1, b0, b1);
  t_ptr<t_curve> b0_u = new dS0v_du(c0, c1, b0, b1);
  t_ptr<t_curve> b1_u = new dS1v_du(c0, c1, b0, b1);

  // Define constraints.
  const t_xyz S00        = p00;
  const t_xyz S01        = p01;
  const t_xyz S10        = p10;
  const t_xyz S11        = p11;
  //
  const t_xyz dS_du00    = b0_u->D0(0.0);
  const t_xyz dS_du01    = b0_u->D0(1.0);
  const t_xyz dS_du10    = b1_u->D0(0.0);
  const t_xyz dS_du11    = b1_u->D0(1.0);
  //
  const t_xyz dS_dv00    = c0_v->D0(0.0);
  const t_xyz dS_dv01    = c1_v->D0(0.0);
  const t_xyz dS_dv10    = c0_v->D0(1.0);
  const t_xyz dS_dv11    = c1_v->D0(1.0);
  //
  const t_xyz d2S_dudv00 = t_xyz(0., 0., 0.);
  const t_xyz d2S_dudv01 = t_xyz(0., 0., 0.);
  const t_xyz d2S_dudv10 = t_xyz(0., 0., 0.);
  const t_xyz d2S_dudv11 = t_xyz(0., 0., 0.);

  // Build Coons surface.
  t_ptr<geom_CoonsSurfaceCubic>
    coons = new geom_CoonsSurfaceCubic(c0, c1, b0, b1,
                                       c0_v, c1_v, b0_u, b1_u,
                                       d2S_dudv00, d2S_dudv01, d2S_dudv10, d2S_dudv11);

  // Calculate finite difference to check constraints.
  {
    t_xyz S0, S1;
    double u0 = 0.5;
    double v0 = 0.0;
    double delta = 1.0e-4;

    coons->Eval(u0 - delta, v0, S0);
    coons->Eval(u0 + delta, v0, S1);

    t_xyz fd = (S1 - S0)/(2*delta);

    std::cout << "fd = " << fd.X() << ", " << fd.Y() << ", " << fd.Z() << std::endl;
  }

  // Sample patch.
  cmdMisc::DrawSurfPts(interp, coons, "coons-pts");
  //
  coons->SetEvalComponent(geom_CoonsSurfaceCubic::EvalComponent_P1S);
  cmdMisc::DrawSurfPts(interp, coons, "P1S-pts");
  //
  coons->SetEvalComponent(geom_CoonsSurfaceCubic::EvalComponent_P2S);
  cmdMisc::DrawSurfPts(interp, coons, "P2S-pts");
  //
  coons->SetEvalComponent(geom_CoonsSurfaceCubic::EvalComponent_P12S);
  cmdMisc::DrawSurfPts(interp, coons, "P12S-pts");

  // Coons D1u.
  coons->SetEvalComponent(geom_CoonsSurfaceCubic::EvalComponent_All);
  //
  {
    double usample = 0., vsample = 0.35;
    t_xyz P, D1u, D1v;
    //
    coons->Eval(usample, vsample, P);
    coons->Eval_D1(usample, vsample, D1u, D1v);
    //
    interp->GetPlotter().REDRAW_POINT("P", cascade::GetOpenCascadePnt(P), Color_Green);
    interp->GetPlotter().REDRAW_VECTOR_AT("D1u", cascade::GetOpenCascadePnt(P), cascade::GetOpenCascadeVec(D1u), Color_Green);
    //
    interp->GetProgress().SendLogMessage( LogNotice(Normal) << "D1u(%1,%2) = (%3,%4,%5)." << usample << vsample << D1u.X() << D1u.Y() << D1u.Z() );

    // Compute by finite difference.
    double udelta = 0.0001;
    double unext = usample + udelta;
    //
    t_xyz Pnext;
    coons->Eval(unext, vsample, Pnext);
    //
    t_xyz D1u_fd = (Pnext - P)*(1.0/udelta);
    //
    interp->GetProgress().SendLogMessage( LogNotice(Normal) << "D1u_fd(%1,%2) = (%3,%4,%5)." << usample << vsample << D1u_fd.X() << D1u_fd.Y() << D1u_fd.Z() );
  }

  // P1S_D1u
  {
    double usample = 0., vsample = 0.35;
    t_xyz P1S_P, P1S_D1u, P1S_D1v;
    //
    coons->Eval_P1S(usample, vsample, P1S_P);
    coons->Eval_D1_P1S(usample, vsample, P1S_D1u, P1S_D1v);
    //
    interp->GetPlotter().REDRAW_POINT("P1S_P", cascade::GetOpenCascadePnt(P1S_P), Color_Green);
    interp->GetPlotter().REDRAW_VECTOR_AT("P1S_D1u", cascade::GetOpenCascadePnt(P1S_P), cascade::GetOpenCascadeVec(P1S_D1u), Color_Green);
    //
    interp->GetProgress().SendLogMessage( LogNotice(Normal) << "P1S_D1u(0,0.35) = (%1,%2,%3)." << P1S_D1u.X() << P1S_D1u.Y() << P1S_D1u.Z() );

    // Compute by finite difference.
    double udelta = 0.0001;
    double unext = usample + udelta;
    //
    t_xyz P1S_Pnext;
    coons->Eval_P1S(unext, vsample, P1S_Pnext);
    //
    t_xyz P1S_D1u_fd = (P1S_Pnext - P1S_P)*(1.0/udelta);
    //
    interp->GetProgress().SendLogMessage( LogNotice(Normal) << "P1S_D1u_fd(0,0.35) = (%1,%2,%3)." << P1S_D1u_fd.X() << P1S_D1u_fd.Y() << P1S_D1u_fd.Z() );
  }

  // P2S_D1u
  {
    double usample = 0., vsample = 0.35;
    t_xyz P2S_P, P2S_D1u, P2S_D1v;
    //
    coons->Eval_P2S(usample, vsample, P2S_P);
    coons->Eval_D1_P2S(usample, vsample, P2S_D1u, P2S_D1v);
    //
    interp->GetPlotter().REDRAW_POINT("P2S_P", cascade::GetOpenCascadePnt(P2S_P), Color_Green);
    interp->GetPlotter().REDRAW_VECTOR_AT("P2S_D1u", cascade::GetOpenCascadePnt(P2S_P), cascade::GetOpenCascadeVec(P2S_D1u), Color_Green);
    //
    interp->GetProgress().SendLogMessage( LogNotice(Normal) << "P2S_D1u(0,0.35) = (%1,%2,%3)." << P2S_D1u.X() << P2S_D1u.Y() << P2S_D1u.Z() );

    // Compute by finite difference.
    double udelta = 0.0001;
    double unext = usample + udelta;
    //
    t_xyz P2S_Pnext;
    coons->Eval_P2S(unext, vsample, P2S_Pnext);
    //
    t_xyz P2S_D1u_fd = (P2S_Pnext - P2S_P)*(1.0/udelta);
    //
    interp->GetProgress().SendLogMessage( LogNotice(Normal) << "P2S_D1u_fd(0,0.35) = (%1,%2,%3)." << P2S_D1u_fd.X() << P2S_D1u_fd.Y() << P2S_D1u_fd.Z() );
  }

  geom_MakeBicubicBSurf mkBicubic(S00, S01, S10, S11,
                                  dS_du00, dS_du01, dS_du10, dS_du11,
                                  dS_dv00, dS_dv01, dS_dv10, dS_dv11,
                                  d2S_dudv00, d2S_dudv01, d2S_dudv10, d2S_dudv11, nullptr, nullptr);

  if ( !mkBicubic.Perform() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Failed to make a bicubic patch.");
    return TCL_ERROR;
  }

  const t_ptr<t_bsurf>& P12S = mkBicubic.GetResult();
  //
  interp->GetPlotter().REDRAW_SURFACE("P12", cascade::GetOpenCascadeBSurface(P12S), Color_White);

  // Some tests.
  t_xyz P1_S, P1S_d1U, P1S_d1V;
  P1S->Eval_D1(0., 0.5, P1_S, P1S_d1U, P1S_d1V);
  //
  //interp->GetProgress().SendLogMessage( LogInfo(Normal) << "P1S(0,0) = (%1,%2,%3)." << P1_S.X() << P1_S.Y() << P1_S.Z() );
  interp->GetProgress().SendLogMessage( LogInfo(Normal) << "BSpl P1S_d1U(0,0.35) = (%1,%2,%3)." << P1S_d1U.X() << P1S_d1U.Y() << P1S_d1U.Z() );
  //interp->GetProgress().SendLogMessage( LogInfo(Normal) << "P1S_d1V(0,0) = (%1,%2,%3)." << P1S_d1V.X() << P1S_d1V.Y() << P1S_d1V.Z() );

  t_xyz P2_S, P2S_d1U, P2S_d1V;
  P2S->Eval_D1(0., 0.35, P2_S, P2S_d1U, P2S_d1V);
  //
  //interp->GetProgress().SendLogMessage( LogInfo(Normal) << "P1S(0,0) = (%1,%2,%3)." << P1_S.X() << P1_S.Y() << P1_S.Z() );
  interp->GetProgress().SendLogMessage( LogInfo(Normal) << "BSpl P2S_d1U(0,0.35) = (%1,%2,%3)." << P2S_d1U.X() << P2S_d1U.Y() << P2S_d1U.Z() );

  /* =========================================
   *  Unify knot vectors of P1S, P2S and P12S
   * ========================================= */

  enum PS
  {
    PS_P1S = 0,
    PS_P2S,
    PS_P12S
  };

  // All U knots.
  std::vector< std::vector<double> >
    knotVectors_U = { P1S->GetKnots_U(),
                      P2S->GetKnots_U(),
                      P12S->GetKnots_U() };

  // All V knots.
  std::vector< std::vector<double> >
    knotVectors_V = { P1S->GetKnots_V(),
                      P2S->GetKnots_V(),
                      P12S->GetKnots_V() };

  // Compute addendum knots.
  bspl_UnifyKnots unifyKnots;
  //
  std::vector< std::vector<double> > U_addendums = unifyKnots(knotVectors_U);
  std::vector< std::vector<double> > V_addendums = unifyKnots(knotVectors_V);

  // Insert U knots to P1S.
  for ( size_t ii = 0; ii < U_addendums[PS_P1S].size(); ++ii )
    P1S->InsertKnot_U(U_addendums[PS_P1S][ii]);

  // Insert V knots to P1S.
  for ( size_t ii = 0; ii < V_addendums[PS_P1S].size(); ++ii )
    P1S->InsertKnot_V(V_addendums[PS_P1S][ii]);

  // Insert U knots to P2S.
  for ( size_t ii = 0; ii < U_addendums[PS_P2S].size(); ++ii )
    P2S->InsertKnot_U(U_addendums[PS_P2S][ii]);

  // Insert V knots to P2S.
  for ( size_t ii = 0; ii < V_addendums[PS_P2S].size(); ++ii )
    P2S->InsertKnot_V(V_addendums[PS_P2S][ii]);

  // Insert U knots to P12S.
  for ( size_t ii = 0; ii < U_addendums[PS_P12S].size(); ++ii )
    P12S->InsertKnot_U(U_addendums[PS_P12S][ii]);

  // Insert V knots to P12S.
  for ( size_t ii = 0; ii < V_addendums[PS_P12S].size(); ++ii )
    P12S->InsertKnot_V(V_addendums[PS_P12S][ii]);

  // Common knots.
  const std::vector<double>& Ucommon = P1S->GetKnots_U();
  const std::vector<double>& Vcommon = P1S->GetKnots_V();

  // Common degrees.
  const int pcommon = P1S->GetDegree_U();
  const int qcommon = P1S->GetDegree_V();

  // Draw.
  interp->GetPlotter().REDRAW_SURFACE("P1S",  cascade::GetOpenCascadeBSurface(P1S),  Color_Blue);
  interp->GetPlotter().REDRAW_SURFACE("P2S",  cascade::GetOpenCascadeBSurface(P2S),  Color_Blue);
  interp->GetPlotter().REDRAW_SURFACE("P12S", cascade::GetOpenCascadeBSurface(P12S), Color_Blue);

  // Now all patches are of the same degrees and defined on identical knot
  // vectors. It means that all patches are defined on the same basis. Therefore,
  // we can now produce a Boolean sum.

  const std::vector< std::vector<t_xyz> >& polesP1S  = P1S->GetPoles();
  const std::vector< std::vector<t_xyz> >& polesP2S  = P2S->GetPoles();
  const std::vector< std::vector<t_xyz> >& polesP12S = P12S->GetPoles();

  const int numPolesU = P1S->GetNumOfPoles_U();
  const int numPolesV = P1S->GetNumOfPoles_V();

  // Compute the resulting poles.
  std::vector< std::vector<t_xyz> > resPoles;
  //
  for ( int i = 0; i < numPolesU; ++i )
  {
    std::vector<t_xyz> col;
    for ( int j = 0; j < numPolesV; ++j )
    {
      t_xyz resPole = polesP1S[i][j] + polesP2S[i][j] - polesP12S[i][j];
      //
      col.push_back(resPole);
    }
    resPoles.push_back(col);
  }

  // Construct the resulting surface.
  t_ptr<t_bsurf>
    mobRes = new t_bsurf(resPoles, Ucommon, Vcommon, pcommon, qcommon);
  //
  interp->GetPlotter().REDRAW_SURFACE("res-constrained", cascade::GetOpenCascadeBSurface(mobRes), Color_Default);

  t_xyz S, D1u, D1v;
  const double usample = 0.0;
  const double vsample = 0.35;
  mobRes->Eval_D1(usample, vsample, S, D1u, D1v);

  //interp->GetProgress().SendLogMessage( LogInfo(Normal) << "P1S(0,0) = (%1,%2,%3)." << P1_S.X() << P1_S.Y() << P1_S.Z() );
  interp->GetProgress().SendLogMessage( LogInfo(Normal) << "BSpl Coons D1U(%1,%2) = (%3,%4,%5)." << usample << vsample << D1u.X() << D1u.Y() << D1u.Z() );

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_TestSkinning1(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  /* ===================
   *  Build rail curves
   * =================== */

  t_ptr<t_bcurve> c0, c1, cn;

  std::vector<t_xyz> c0_D1_vec, cn_D1_vec;

  // Build curves.
  {
    std::vector<t_xyz> c0_pts = {
      t_xyz(0.1,  0.,  0.),
      t_xyz(0.,   1.,  0.),
      t_xyz(0.1,  2.,  0.)
    };
    //
    c0_D1_vec = {
      t_xyz(0.,  0.,  3.),
      t_xyz(0.,  0.,  3.),
      t_xyz(0.,  0.,  3.)
    };

    std::vector<t_xyz> c1_pts = {
      t_xyz(2.1,  0.,  0.),
      t_xyz(2.2,  1.,  1.),
      t_xyz(2.1,  2.,  0.)
    };

    std::vector<t_xyz> cn_pts = {
      t_xyz(5.1,  0.,  0.),
      t_xyz(5.,   1.,  0.),
      t_xyz(5.1,  2.,  0.)
    };
    //
    cn_D1_vec = {
      t_xyz(0.,  0.,  3.),
      t_xyz(0.,  0.,  3.),
      t_xyz(0.,  0.,  3.)
    };

    // Interpolate points to build curves.
    geom_InterpolateMultiCurve multiInterp(2,
                                           ParamsSelection_Centripetal,
                                           KnotsSelection_Average);
    //
    multiInterp.AddRow(c0_pts);
    multiInterp.AddRow(c1_pts);
    multiInterp.AddRow(cn_pts);

    if ( multiInterp.Perform() )
    {
      // Get rail curves.
      c0 = multiInterp.GetResult(0);
      c1 = multiInterp.GetResult(1);
      cn = multiInterp.GetResult(2);
      //
      interp->GetPlotter().REDRAW_CURVE("c0", cascade::GetOpenCascadeBCurve(c0), Color_Default);
      interp->GetPlotter().REDRAW_CURVE("c1", cascade::GetOpenCascadeBCurve(c1), Color_Default);
      interp->GetPlotter().REDRAW_CURVE("cn", cascade::GetOpenCascadeBCurve(cn), Color_Default);
    }
    else
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Multi-curve interpolation failed.");
      return TCL_ERROR;
    }
  }

  /* ========================
   *  Build skinning surface
   * ======================== */

  std::vector< t_ptr<t_bcurve> > rails = {c0, c1, cn};

  // Skin ruled surface through the rail curves.
  geom_SkinSurface skinner(rails, 2, false);
  //
  skinner.AddLeadingTangencies(c0_D1_vec);
  skinner.AddTrailingTangencies(cn_D1_vec);
  //
  if ( !skinner.Perform() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot build skinning surface.");
    return TCL_ERROR;
  }
  //
  const t_ptr<t_bsurf>& mobRes = skinner.GetResult();

  // Draw.
  interp->GetPlotter().REDRAW_SURFACE("res", cascade::GetOpenCascadeBSurface(mobRes), Color_White);

  return TCL_OK;
}
#endif

//-----------------------------------------------------------------------------

int MISC_TestDist(const Handle(asiTcl_Interp)& interp,
                  int                          argc,
                  const char**                 argv)
{
  if ( argc != 4 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  // Find topo item.
  Handle(asiData_IVTopoItemNode)
    topoNode = Handle(asiData_IVTopoItemNode)::DownCast( M->FindNodeByName(argv[1]) );
  //
  if ( topoNode.IsNull() )
    return TCL_ERROR;

  // Get shape
  TopoDS_Shape shape = topoNode->GetShape();

  // Cloudify shape.
  Handle(asiAlgo_BaseCloud<double>) sampledPts;
  //
  asiAlgo_Cloudify cloudify( std::min( atof(argv[2]), atof(argv[3]) ) );
  //
  cloudify.SetParametricSteps( atof(argv[2]), atof(argv[3]) );
  //
  if ( !cloudify.Sample_Faces(shape, sampledPts) )
  {
    interp->GetProgress().SendLogMessage( LogErr(Normal) << "Cannot sample shape." );
    return TCL_ERROR;
  }

  interp->GetPlotter().REDRAW_POINTS("sampledPts", sampledPts->GetCoordsArray(), Color_Yellow);

  // Prepare triangulation.
  Handle(asiAlgo_BVHFacets) bvh;
  //
  asiEngine_Triangulation trisAPI( M, interp->GetProgress(), interp->GetPlotter() );
  //
  M->OpenCommand();
  {
    // Build BVH for CAD-agnostic mesh.
    bvh = trisAPI.BuildBVH();
  }
  M->CommitCommand();

  // Project each point.
  asiAlgo_ProjectPointOnMesh projectTool(bvh);
  //
  double dist = -DBL_MAX;
  gp_Pnt P1, P2;
  //
  for ( int k = 0; k < sampledPts->GetNumberOfElements(); ++k )
  {
    gp_Pnt       P        = sampledPts->GetElement(k);
    gp_Pnt       Pproj    = projectTool.Perform(P);
    const double currDist = P.Distance(Pproj);

    if ( currDist > dist )
    {
      dist = currDist;
      P1   = P;
      P2   = Pproj;
    }
  }

  interp->GetPlotter().REDRAW_POINT("P1", P1, Color_Red);
  interp->GetPlotter().REDRAW_POINT("P2", P2, Color_Red);
  interp->GetPlotter().REDRAW_LINK("P12", P1, P2, Color_Red);

  interp->GetProgress().SendLogMessage( LogInfo(Normal) << "Max deviation: %1." << P1.Distance(P2) );

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_DumpBVH(const Handle(asiTcl_Interp)& interp,
                 int                          argc,
                 const char**                 argv)
{
  if ( argc != 1 && argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  const bool doBuild = interp->HasKeyword(argc, argv, "build");

  // Get BVH from Part Node.
  Handle(asiAlgo_BVHFacets) bvh = M->GetPartNode()->GetBVH();
  //
  if ( bvh.IsNull() && !doBuild )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "BVH is not initialized.");
    return TCL_ERROR;
  }

  if ( doBuild )
  {
    TIMER_NEW
    TIMER_GO

    // Construct BVH right here.
    M->OpenCommand();
    {
      bvh = asiEngine_Part(M).BuildBVH();
    }
    M->CommitCommand();

    TIMER_FINISH
    TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Build BVH")
  }

  // Dump.
  bvh->Dump( interp->GetPlotter() );
  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_InvertBPoles(const Handle(asiTcl_Interp)& interp,
                      int                          argc,
                      const char**                 argv)
{
#if defined USE_MOBIUS
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Find Node by name.
  Handle(ActAPI_INode) node = interp->GetModel()->FindNodeByName(argv[1]);
  //
  if ( node.IsNull() || !node->IsKind( STANDARD_TYPE(asiData_IVSurfaceNode) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Node '%1' is not a surface."
                                                        << argv[1]);
    return TCL_OK;
  }
  //
  Handle(asiData_IVSurfaceNode)
    surfNode = Handle(asiData_IVSurfaceNode)::DownCast(node);

  // Get B-surface.
  Handle(Geom_BSplineSurface)
    occtBSurface = Handle(Geom_BSplineSurface)::DownCast( surfNode->GetSurface() );
  //
  if ( occtBSurface.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The surface in question is not a B-spline surface.");
    return TCL_OK;
  }

  // Convert to Mobius B-surface.
  t_ptr<t_bsurf>
    mobSurf = cascade::GetMobiusBSurface(occtBSurface);

  // Disable UI immediate updates for better performance.
  Handle(asiUI_IV) IV = Handle(asiUI_IV)::DownCast( interp->GetPlotter().Access() );
  //
  IV->REPAINT_OFF();
  IV->BROWSER_OFF();

  // Loop over the control points and invert each one to the surface.
  std::vector< std::vector<t_xyz> > poles = mobSurf->GetPoles();
  //
  for ( size_t i = 0; i < poles.size(); ++i )
    for ( size_t j = 0; j < poles[0].size(); ++j )
    {
      // Invert point.
      t_uv projUV;
      //
      if ( !mobSurf->InvertPoint(poles[i][j], projUV) )
      {
        interp->GetProgress().SendLogMessage(LogErr(Normal) << "Point inversion failed.");
        interp->GetPlotter().DRAW_POINT(cascade::GetOpenCascadePnt(poles[i][j]), Color_Red, "pfaulty");
        continue;
      }

      // Evaluate surface for the obtained (u,v) coordinates.
      t_xyz S;
      mobSurf->Eval(projUV.U(), projUV.V(), S);
      //
      interp->GetPlotter().DRAW_POINT(cascade::GetOpenCascadePnt(poles[i][j]), Color_Yellow, "p");
      interp->GetPlotter().DRAW_POINT(cascade::GetOpenCascadePnt(S), Color_Green, "proj");
      interp->GetPlotter().DRAW_LINK(cascade::GetOpenCascadePnt(poles[i][j]), cascade::GetOpenCascadePnt(S), Color_Red, "plink");

      interp->GetProgress().SendLogMessage( LogInfo(Normal) << "Projection (u, v) = (%1, %2)."
                                                            << projUV.U() << projUV.V() );
    }

  // Update UI.
  IV->REPAINT_ON();
  IV->BROWSER_ON();

  return TCL_OK;
#else
  cmdMisc_NotUsed(argc);
  cmdMisc_NotUsed(argv);

  interp->GetProgress().SendLogMessage(LogErr(Normal) << "Mobius is not available.");
  return TCL_ERROR;
#endif
}

//-----------------------------------------------------------------------------

void cmdMisc::Factory(const Handle(asiTcl_Interp)&      interp,
                      const Handle(Standard_Transient)& data)
{
  static const char* group = "cmdMisc";
  
  /* ==========================
   *  Initialize UI facilities
   * ========================== */

  // Get common facilities
  Handle(asiUI_CommonFacilities)
    passedCF = Handle(asiUI_CommonFacilities)::DownCast(data);
  //
  if ( passedCF.IsNull() )
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "[cmdMisc] UI facilities are not available. GUI may not be updated.");
  else
    cf = passedCF;

  /* ================================
   *  Initialize Data Model instance
   * ================================ */

  model = Handle(asiEngine_Model)::DownCast( interp->GetModel() );
  //
  if ( model.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "[cmdMisc] Data Model instance is null or not of asiEngine_Model kind.");
    return;
  }

  /* ==================
   *  Add Tcl commands
   * ================== */

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-hexagon-bops",
    //
    "test-hexagon-bops \n"
    "\t No arguments are expected. This command demonstrates solid Booleans \n"
    "\t of OpenCascade. It cuts many cylinders from a hexagonal prism.",
    //
    __FILE__, group, MISC_TestHexagonBops);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-hexagon-bops-faces",
    //
    "test-hexagon-bops-faces \n"
    "\t No arguments are expected. This command demonstrates solid Booleans \n"
    "\t of OpenCascade. It cuts many circles from a hexagon and then \n"
    "\t constructs a prism.",
    //
    __FILE__, group, MISC_TestHexagonBopsFaces);

  //-------------------------------------------------------------------------//
  interp->AddCommand("push-pull",
    //
    "push-pull faceId offset \n"
    "\t Push/pull operation.",
    //
    __FILE__, group, MISC_PushPull);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-builder",
    //
    "test-builder \n"
    "\t Reproducer for issue with BRep_Builder.",
    //
    __FILE__, group, MISC_TestBuilder);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-composite",
    //
    "test-composite \n"
    "\t Constructs sample non-manifold CAD model.",
    //
    __FILE__, group, MISC_TestComposite);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-offset",
    //
    "test-offset \n"
    "\t Constructs simple solid by offset.",
    //
    __FILE__, group, MISC_TestOffset);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-pipe1",
    //
    "test-pipe \n"
    "\t Problem reproducer for pipes.",
    //
    __FILE__, group, MISC_TestPipe1);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-sweep1",
    //
    "test-sweep1 \n"
    "\t Problem reproducer for sweeping.",
    //
    __FILE__, group, MISC_TestSweep1);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-sweep2",
    //
    "test-sweep2 \n"
    "\t Problem reproducer for sweeping.",
    //
    __FILE__, group, MISC_TestSweep2);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-ineq",
    //
    "test-ineq \n"
    "\t Test for inequality solver.",
    //
    __FILE__, group, MISC_TestIneq);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-eval-curve",
    //
    "test-eval-curve curveName u num order [-mobius]\n"
    "\t Evaluates curve <t_curveName> for the given parameter value <u>.\n"
    "\t If <-mobius> keyword is used, evaluation is performed using Mobius\n"
    "\t functions. The argument <num> specifies how many iterations to use.\n"
    "\t This function is used to test performance of curve evaluation.",
    //
    __FILE__, group, MISC_TestEvalCurve);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-eval-surf",
    //
    "test-eval-surf surfName u v num order [-mobius]\n"
    "\t Evaluates surface <surfName> for the given parameter pair <u, v>.\n"
    "\t If <-mobius> keyword is used, evaluation is performed using Mobius\n"
    "\t functions. The argument <num> specifies how many iterations to use.\n"
    "\t This function is used to test performance of surface evaluation.",
    //
    __FILE__, group, MISC_TestEvalSurf);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test",
    //
    "test \n"
    "\t Problem reproducer for anything.",
    //
    __FILE__, group, MISC_Test);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-fibers",
    //
    "test-fibers \n"
    "\t Tests fiber polyhedrization algorithm.",
    //
    __FILE__, group, MISC_TestFibers);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-transform-axes",
    //
    "test-transform-axes \n"
    "\t Transforms axes B to place them coincidently with axes A.",
    //
    __FILE__, group, MISC_TestTranformAxes);

#if defined USE_MOBIUS
  //-------------------------------------------------------------------------//
  interp->AddCommand("test-fair",
    //
    "test-fair fid\n"
    "\t Test for fairing function.",
    //
    __FILE__, group, MISC_TestFair);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-coons1",
    //
    "test-coons1\n"
    "\t Test for Coons patch.",
    //
    __FILE__, group, MISC_TestCoons1);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-coons2",
    //
    "test-coons2\n"
    "\t Test for Coons patch.",
    //
    __FILE__, group, MISC_TestCoons2);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-skinning1",
    //
    "test-skinning1\n"
    "\t Test for skinning.",
    //
    __FILE__, group, MISC_TestSkinning1);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-dist",
    //
    "test-dist surfName ustep vstep\n"
    "\t Test for distance evaluation.",
    //
    __FILE__, group, MISC_TestDist);
#endif

  //-------------------------------------------------------------------------//
  interp->AddCommand("dump-bvh",
    //
    "dump-bvh [-build]\n"
    "\t Dumps BVH to viewer.",
    //
    __FILE__, group, MISC_DumpBVH);

  //-------------------------------------------------------------------------//
  interp->AddCommand("misc-invert-bpoles",
    //
    "misc-invert-bpoles surfName\n"
    "\t Inverts the control points of a B-surface to itself.",
    //
    __FILE__, group, MISC_InvertBPoles);

  // Load sub-modules.
  Commands_Coons(interp, data);
}

// Declare entry point
ASIPLUGIN(cmdMisc)
