//-----------------------------------------------------------------------------
// Created on: 09 March 2017
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

// Own include
#include <asiAlgo_PlateOnEdges.h>

// asiAlgo includes
#include <asiAlgo_Timer.h>

// Mobius includes
#if defined USE_MOBIUS
  #include <mobius/cascade.h>
  #include <mobius/geom_FairBSurf.h>
#endif

// OCCT includes
#include <Adaptor3d_HCurveOnSurface.hxx>
#include <BRep_Builder.hxx>
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
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomPlate_BuildPlateSurface.hxx>
#include <GeomPlate_HArray1OfHCurve.hxx>
#include <GeomPlate_MakeApprox.hxx>
#include <GeomPlate_PlateG0Criterion.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_Shape.hxx>
#include <TColGeom2d_HArray1OfCurve.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiAlgo_PlateOnEdges::asiAlgo_PlateOnEdges(ActAPI_ProgressEntry progress,
                                           ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm ( progress, plotter ),
  m_iNumDiscrPts    ( 10 ),
  m_fFairCoeff      ( 0. )
{}

//-----------------------------------------------------------------------------

asiAlgo_PlateOnEdges::asiAlgo_PlateOnEdges(const Handle(asiAlgo_AAG)& aag,
                                           ActAPI_ProgressEntry       progress,
                                           ActAPI_PlotterEntry        plotter)
: ActAPI_IAlgorithm ( progress, plotter ),
  m_shape           ( aag->GetMasterCAD() ),
  m_aag             ( aag),
  m_iNumDiscrPts    ( 10 ),
  m_fFairCoeff      ( 0. )
{}

//-----------------------------------------------------------------------------

asiAlgo_PlateOnEdges::asiAlgo_PlateOnEdges(const TopoDS_Shape&  shape,
                                           ActAPI_ProgressEntry progress,
                                           ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm ( progress, plotter ),
  m_shape           ( shape ),
  m_iNumDiscrPts    ( 10 ),
  m_fFairCoeff      ( 0. )
{}

//-----------------------------------------------------------------------------

bool asiAlgo_PlateOnEdges::Build(Handle(TopTools_HSequenceOfShape)& edges,
                                 const unsigned int                 continuity,
                                 Handle(Geom_BSplineSurface)&       support,
                                 TopoDS_Face&                       result)
{
  // Build surface.
  if ( !this->BuildSurf(edges, continuity, support) )
    return false;

  // Build face.
  if ( !this->BuildFace(edges, support, result) )
    return false;

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_PlateOnEdges::Build(const TColStd_PackedMapOfInteger&  edgeIndices,
                                 const unsigned int                 continuity,
                                 Handle(TopTools_HSequenceOfShape)& edges,
                                 Handle(Geom_BSplineSurface)&       support,
                                 TopoDS_Face&                       result)
{
  // Build surface.
  if ( !this->BuildSurf(edgeIndices, continuity, edges, support) )
    return false;

  // Build face.
  if ( !this->BuildFace(edges, support, result) )
    return false;

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_PlateOnEdges::Build(const TColStd_PackedMapOfInteger& edgeIndices,
                                 const unsigned int                continuity,
                                 Handle(Geom_BSplineSurface)&      support,
                                 TopoDS_Face&                      result)
{
  Handle(TopTools_HSequenceOfShape) edges;
  //
  return this->Build(edgeIndices, continuity, edges, support, result);
}

//-----------------------------------------------------------------------------

bool asiAlgo_PlateOnEdges::BuildSurf(const TColStd_PackedMapOfInteger&  edgeIndices,
                                     const unsigned int                 continuity,
                                     Handle(TopTools_HSequenceOfShape)& edges,
                                     Handle(Geom_BSplineSurface)&       support)
{
  if ( edges.IsNull() )
    edges = new TopTools_HSequenceOfShape;

  for ( TColStd_MapIteratorOfPackedMapOfInteger eit(edgeIndices); eit.More(); eit.Next() )
  {
    const int          eidx = eit.Key();
    const TopoDS_Edge& E    = TopoDS::Edge( m_aag->RequestMapOfEdges().FindKey(eidx) );
    //
    edges->Append(E);
  }

  return this->BuildSurf(edges, continuity, support);
}

//-----------------------------------------------------------------------------

bool asiAlgo_PlateOnEdges::BuildSurf(const Handle(TopTools_HSequenceOfShape)& edges,
                                     const unsigned int                       continuity,
                                     Handle(Geom_BSplineSurface)&             support)
{
  /* ==============================
   *  STAGE 1: prepare constraints
   * ============================== */

  const int nbedges = int( edges->Length() );

  // Prepare working collection for support curves (used to create
  // pinpoint constraints).
  Handle(GeomPlate_HArray1OfHCurve)
    fronts = new GeomPlate_HArray1OfHCurve(1, nbedges);

  // Prepare working collection for smoothness values associated with
  // each support curve.
  Handle(TColStd_HArray1OfInteger)
    tang = new TColStd_HArray1OfInteger(1, nbedges);

  // Prepare working collection for discretization numbers associated with
  // each support curve.
  Handle(TColStd_HArray1OfInteger)
    nbPtsCur = new TColStd_HArray1OfInteger(1, nbedges);

  if ( m_shape.IsNull() )
  {
    /* Master shape is null -> we cannot use CONS constraints */

    // Loop over the curves to prepare constraints.
    int i = 0;
    for ( int eidx = 0; eidx < nbedges; ++eidx )
    {
      i++;
      tang->SetValue(i, continuity);
      nbPtsCur->SetValue(i, m_iNumDiscrPts); // Number of discretization points.

      // Get edge and its host curve.
      double f, l;
      const TopoDS_Edge& E = TopoDS::Edge( edges->Value(eidx + 1) );
      Handle(Geom_Curve) C = BRep_Tool::Curve(E, f, l);

      Handle(GeomAdaptor_HCurve)
        curveAdt = new GeomAdaptor_HCurve(C);

      fronts->SetValue(i, curveAdt);
    }
  }
  else
  {
    /* Master shape is not null -> we can use CONS constraints */

    // Get model.
    const TopoDS_Shape& model = m_shape;

    // Build edge-face map.
    TopTools_IndexedDataMapOfShapeListOfShape M;
    TopExp::MapShapesAndAncestors(model, TopAbs_EDGE, TopAbs_FACE, M);

    // Loop over the edges to prepare constraints.
    int i = 0;
    for ( int eidx = 0; eidx < nbedges; ++eidx )
    {
      i++;
      tang->SetValue(i, continuity);
      nbPtsCur->SetValue(i, m_iNumDiscrPts); // Number of discretization points.

      const TopoDS_Edge& E = TopoDS::Edge( edges->Value(eidx + 1) );
      const TopoDS_Face& F = TopoDS::Face( M.FindFromKey(E).First() );

      m_plotter.DRAW_SHAPE(E, Color_Red, 1.0, true, "asiAlgo_PlateOnEdges_E");

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
  }

  /* ======================
   *  STAGE 2: build plate
   * ====================== */

  TIMER_NEW
  TIMER_GO

  // Build plate.
  GeomPlate_BuildPlateSurface BuildPlate(nbPtsCur, fronts, tang, 3);
  //
  try
  {
    BuildPlate.Perform();
  }
  catch ( ... )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Exception in OCCT plate surface builder");
    return false;
  }
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

  /* ================================
   *  STAGE 4: fair surface if asked
   * ================================ */

   if ( m_fFairCoeff )
   {
#if defined USE_MOBIUS
     TIMER_RESET
     TIMER_GO

     mobius::t_ptr<mobius::t_bsurf>
       mobSurf = mobius::cascade::GetMobiusBSurface(support);

     // Perform fairing.
     mobius::geom_FairBSurf fairing(mobSurf, m_fFairCoeff, nullptr, nullptr);
     //
     const int nPolesU = int( mobSurf->GetPoles().size() );
     const int nPolesV = int( mobSurf->GetPoles()[0].size() );
     //
     for ( int i = 0; i < nPolesU; ++i )
     {
       fairing.AddPinnedPole( i, 0 );
       fairing.AddPinnedPole( i, nPolesV - 1 );
     }
     //
     for ( int j = 0; j < nPolesV; ++j )
     {
       fairing.AddPinnedPole( 0, j );
       fairing.AddPinnedPole( nPolesU - 1, j );
     }
     //
     if ( !fairing.Perform() )
     {
       m_progress.SendLogMessage(LogErr(Normal) << "Fairing failed.");
       return false;
     }

     // Get the faired surface.
     const mobius::t_ptr<mobius::t_bsurf>& mobResult = fairing.GetResult();

     // Convert to OCCT B-surface.
     support = mobius::cascade::GetOpenCascadeBSurface(mobResult);

     TIMER_FINISH
     TIMER_COUT_RESULT_MSG("Fair B-surface")
#else
     m_progress.SendLogMessage(LogWarn(Normal) << "Fairing feature is not available.");
#endif
   }

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_PlateOnEdges::BuildFace(Handle(TopTools_HSequenceOfShape)& edges,
                                     const Handle(Geom_BSplineSurface)& support,
                                     TopoDS_Face&                       result)
{
  // Compose a new wire.
  Handle(TopTools_HSequenceOfShape) freeWires;
  ShapeAnalysis_FreeBounds::ConnectEdgesToWires(edges, 1e-3, 0, freeWires);
  //
  const TopoDS_Wire& repatchW = TopoDS::Wire( freeWires->First() );

  // Build new face.
  TopoDS_Face newF = BRepBuilderAPI_MakeFace(support, repatchW, false);

  // Heal defects.
  ShapeFix_Shape shapeHealer(newF);
  shapeHealer.Perform();
  newF = TopoDS::Face( shapeHealer.Shape() );

  // Classify point to invert wire if necessary.
  BRepTopAdaptor_FClass2d FClass(newF, 0.0);
  if ( FClass.PerformInfinitePoint() == TopAbs_IN )
  {
    BRep_Builder B;
    TopoDS_Shape S = newF.EmptyCopied();
    TopoDS_Iterator it(newF);
    while ( it.More() )
    {
      B.Add( S, it.Value().Reversed() );
      it.Next();
    }
    newF = TopoDS::Face(S);
  }

  result = newF;
  return true;
}
