//-----------------------------------------------------------------------------
// Created on: 09 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_PlateOnEdges.h>

// asiAlgo includes
#include <asiAlgo_Timer.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgo.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepLib.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomPlate_BuildPlateSurface.hxx>
#include <GeomPlate_MakeApprox.hxx>
#include <GeomPlate_PlateG0Criterion.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_Shape.hxx>
#include <TColGeom2d_HArray1OfCurve.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

//! Constructs the tool.
//! \param aag      [in] Attributed Adjacency Graph (AAG).
//! \param progress [in] progress indicator.
//! \param plotter  [in] imperative plotter.
asiAlgo_PlateOnEdges::asiAlgo_PlateOnEdges(const Handle(asiAlgo_AAG)& aag,
                                           ActAPI_ProgressEntry       progress,
                                           ActAPI_PlotterEntry        plotter)
: ActAPI_IAlgorithm(progress, plotter), m_aag(aag)
{}

//-----------------------------------------------------------------------------

//! Constructs TPS (Thin Plate Spline) approximation for the passed edges.
//! \param edgeIndices [in]  edges to build the approximation surface for.
//! \param support     [out] support b-surface.
//! \param result      [out] reconstructed face.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_PlateOnEdges::Build(const TColStd_PackedMapOfInteger& edgeIndices,
                                 Handle(Geom_BSplineSurface)&      support,
                                 TopoDS_Face&                      result)
{
  /* ==============================
   *  STAGE 1: prepare constraints
   * ============================== */

  const int nbedges = edgeIndices.Extent();

  // Prepare working collections
  Handle(GeomPlate_HArray1OfHCurveOnSurface)
    fronts = new GeomPlate_HArray1OfHCurveOnSurface(1, nbedges);
  //
  Handle(TColStd_HArray1OfInteger)
    tang = new TColStd_HArray1OfInteger(1, nbedges);
  //
  Handle(TColStd_HArray1OfInteger)
    nbPtsCur = new TColStd_HArray1OfInteger(1, nbedges);

  // Get model
  const TopoDS_Shape& model = m_aag->GetMasterCAD();

  // Build edge-face map
  TopTools_IndexedDataMapOfShapeListOfShape M;
  TopExp::MapShapesAndAncestors(model, TopAbs_EDGE, TopAbs_FACE, M);

  // Loop over the edges to prepare constraints
  int i = 0;
  for ( TColStd_MapIteratorOfPackedMapOfInteger eit(edgeIndices); eit.More(); eit.Next() )
  {
    i++;
    tang->SetValue(i, 1);
    nbPtsCur->SetValue(i, 10);

    const int          eidx = eit.Key();
    const TopoDS_Edge& E    = TopoDS::Edge( m_aag->GetMapOfEdges().FindKey(eidx) );
    const TopoDS_Face& F    = TopoDS::Face( M.FindFromKey(E).First() );

    // Get CONS for each edge and fill the constraint
    BRepAdaptor_Surface S(F);
    GeomAdaptor_Surface GAS = S.Surface();
    Handle(GeomAdaptor_HSurface) HGAS = new GeomAdaptor_HSurface(GAS);
    //
    Handle(BRepAdaptor_HCurve2d) C = new BRepAdaptor_HCurve2d();
    C->ChangeCurve2d().Initialize(E, F);
    //
    Adaptor3d_CurveOnSurface ConS(C, HGAS);
    Handle(Adaptor3d_HCurveOnSurface) HConS = new Adaptor3d_HCurveOnSurface(ConS);
    fronts->SetValue(i, HConS);
  }

  /* ======================
   *  STAGE 2: build plate
   * ====================== */

  TIMER_NEW
  TIMER_GO

  // Build plate
  GeomPlate_BuildPlateSurface BuildPlate(nbPtsCur, fronts, tang, 3);
  BuildPlate.Perform();
  //
  if ( !BuildPlate.IsDone() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Plating failed");
    return false;
  }
  Handle(GeomPlate_Surface) plate = BuildPlate.Surface();

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Build plate")

  /* =======================================
   *  STAGE 3: approximate plate with NURBS
   * ======================================= */

  TIMER_RESET
  TIMER_GO

  GeomPlate_MakeApprox MKS(plate, Precision::Approximation(), 4, 7, 0.001, 1);
  support = MKS.Surface();

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Approximate plate with B-surface")

  /* ===============================
   *  STAGE 4: reconstruct topology
   * =============================== */

  TIMER_RESET
  TIMER_GO

  TopTools_Array1OfShape tab(1, nbedges);

  // Build edges
  Handle(ShapeExtend_WireData) WD = new ShapeExtend_WireData;
  for ( TColStd_MapIteratorOfPackedMapOfInteger eit(edgeIndices); eit.More(); eit.Next() )
  {
    const int          eidx = eit.Key();
    const TopoDS_Edge& E    = TopoDS::Edge( m_aag->GetMapOfEdges().FindKey(eidx) );
    //if ( BuildPlate.Sense()->Value( BuildPlate.Order()->Value(i) ) == 1 )
    //{
    //  BRepBuilderAPI_MakeEdge ME( BuildPlate.Curves2d()->Value( BuildPlate.Order()->Value(i) ),
    //                              support,
    //                              fronts->Value( BuildPlate.Order()->Value(i) )->LastParameter(),
    //                              fronts->Value( BuildPlate.Order()->Value(i) )->FirstParameter() );
    //  //
    //  TopoDS_Edge E = ME.Edge();
    //  BRepLib::BuildCurves3d(E);
    //  tab( BuildPlate.Order()->Value(i) ) = E;
    //}
    //else
    //{
      //BRepBuilderAPI_MakeEdge ME( BuildPlate.Curves2d()->Value(i),
      //                            support,
      //                            fronts->Value(i)->FirstParameter(),
      //                            fronts->Value(i)->LastParameter() );
      ////
      //TopoDS_Edge E = ME.Edge();
      //BRepLib::BuildCurves3d(E);

      WD->Add(E);
      //tab( BuildPlate.Order()->Value(i) ) = E;
    //}
  }

  TopoDS_Wire W = WD->WireAPIMake();
  //
  if ( W.IsNull() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Wire reconstruction failed: null wire");
    return false;
  }

  //// Build wire
  //BRepBuilderAPI_MakeWire MW;
  //for ( i = 1; i <= nbedges; ++i )
  //{
  //  const TopoDS_Edge& E = TopoDS::Edge( tab(i) );
  //  //
  //  if ( i == 3 )
  //  {
  //    //m_plotter.DRAW_SHAPE(E, Color_Green, 1.0, true, "plate edge");

  //    TopoDS_Vertex Vf = TopExp::FirstVertex(E);
  //    TopoDS_Vertex Vl = TopExp::LastVertex(E);
  //    //
  //    m_plotter.DRAW_POINT( BRep_Tool::Pnt(Vf), Color_Green, "Vf" );
  //    m_plotter.DRAW_POINT( BRep_Tool::Pnt(Vl), Color_Green, "Vl" );
  //  }
  //  if ( i == 4 )
  //  {
  //   // m_plotter.DRAW_SHAPE(E, Color_Blue, 1.0, true, "plate edge");

  //    TopoDS_Vertex Vf = TopExp::FirstVertex(E);
  //    TopoDS_Vertex Vl = TopExp::LastVertex(E);
  //    //
  //    m_plotter.DRAW_POINT( BRep_Tool::Pnt(Vf), Color_Blue, "Vf" );
  //    m_plotter.DRAW_POINT( BRep_Tool::Pnt(Vl), Color_Blue, "Vl" );
  //  }
  //  //
  //  MW.Add(E);
  //}
  ////
  //TopoDS_Wire W;
  //try
  //{
  //  W = MW.Wire();
  //}
  //catch ( ... )
  //{
  //  m_progress.SendLogMessage(LogErr(Normal) << "Wire reconstruction failed");
  //  return false;
  //}

  //if ( !W.Closed() )
  //{
  //  m_progress.SendLogMessage(LogErr(Normal) << "Wire is not closed");
  //  return false;
  //}

  // Build face
  BRepBuilderAPI_MakeFace MF(support, W, true);
  TopoDS_Face face = MF.Face();

  // Check orientation of the outer wire
  BRepTopAdaptor_FClass2d clas2d( face, Precision::Confusion() );
  if ( clas2d.PerformInfinitePoint() == TopAbs_IN )
  {
    W.Reverse();
    BRepBuilderAPI_MakeFace MF1(support, W, true);
    face = MF1.Face();
  }

  ShapeFix_Shape ShapeHealer(face);
  try
  {
    ShapeHealer.Perform();
  }
  catch ( ... )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Face healing failed");
    return false;
  }
  face = TopoDS::Face( ShapeHealer.Shape() );

  if ( !BRepAlgo::IsValid(face) )
    m_progress.SendLogMessage(LogWarn(Normal) << "Face is not valid");

  // Set result
  result = face;

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Construct topology")

  return true;
}
