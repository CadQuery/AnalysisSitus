//-----------------------------------------------------------------------------
// Created on: 03 August 2018
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
#include <asiAlgo_CheckValidity.h>

// OCCT includes
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <IntRes2d_Domain.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_ShapeTolerance.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

//-----------------------------------------------------------------------------
// Auxiliary functions
//-----------------------------------------------------------------------------

//! Auxiliary toolkit for shape validity checker.
namespace CheckShapeAux
{
  //! Evaluates 3D point as `S(c2d(t))`.
  //! \param[in] surf  host surface.
  //! \param[in] c2d   two-dimensional parametric curve.
  //! \param[in] param parameter to evaluate the host geometry in.
  //! \return three-dimensional point.
  gp_Pnt GetPointOnSurf(const Handle(Geom_Surface)& surf,
                        const Geom2dAdaptor_Curve&  c2d,
                        const double                param)
  {
    gp_Pnt2d aP2d = c2d.Value(param);
    return surf->Value( aP2d.X(), aP2d.Y() );
  }

  //! Checks whether the given list of shapes contains the passed shape.
  //! \param L [in] list to check.
  //! \param S [in] entry to find.
  //! \return true if shape was found, false -- otherwise.
  bool Contains(const TopTools_ListOfShape& L,
                const TopoDS_Shape&         S)
  {
    TopTools_ListIteratorOfListOfShape it;
    for ( it.Initialize(L); it.More(); it.Next() )
    {
      if ( it.Value().IsSame(S) )
        return true;
    }
    return false;
  }

  //! Extracted from OCCT Draw Test Harness for "checkshape" command. This
  //! function is used for preparation of output for invalid shapes.
  //! \param stat       [in]  OCCT topological check status.
  //! \param NbProblems [out] output array.
  void FillProblems(const BRepCheck_Status            stat,
                    Handle(TColStd_HArray1OfInteger)& NbProblems)
  {
    // Increment the number of anomalies of the given type
    switch ( stat )
    {
      case BRepCheck_InvalidPointOnCurve:
        NbProblems->ChangeValue(BRepCheck_InvalidPointOnCurve)++; break;
        //
      case BRepCheck_InvalidPointOnCurveOnSurface:
        NbProblems->ChangeValue(BRepCheck_InvalidPointOnCurveOnSurface)++; break;
        //
      case BRepCheck_InvalidPointOnSurface:
        NbProblems->ChangeValue(BRepCheck_InvalidPointOnSurface)++; break;
        //
      case BRepCheck_No3DCurve:
        NbProblems->ChangeValue(BRepCheck_No3DCurve)++; break;
        //
      case BRepCheck_Multiple3DCurve:
        NbProblems->ChangeValue(BRepCheck_Multiple3DCurve)++; break;
        //
      case BRepCheck_Invalid3DCurve:
        NbProblems->ChangeValue(BRepCheck_Invalid3DCurve)++; break;
        //
      case BRepCheck_NoCurveOnSurface:
        NbProblems->ChangeValue(BRepCheck_NoCurveOnSurface)++; break;
        //
      case BRepCheck_InvalidCurveOnSurface:
        NbProblems->ChangeValue(BRepCheck_InvalidCurveOnSurface)++; break;
        //
      case BRepCheck_InvalidCurveOnClosedSurface:
        NbProblems->ChangeValue(BRepCheck_InvalidCurveOnClosedSurface)++; break;
        //
      case BRepCheck_InvalidSameRangeFlag:
        NbProblems->ChangeValue(BRepCheck_InvalidSameRangeFlag)++; break;
        //
      case BRepCheck_InvalidSameParameterFlag:
        NbProblems->ChangeValue(BRepCheck_InvalidSameParameterFlag)++; break;
        //
      case BRepCheck_InvalidDegeneratedFlag:
        NbProblems->ChangeValue(BRepCheck_InvalidDegeneratedFlag)++; break;
        //
      case BRepCheck_FreeEdge:
        NbProblems->ChangeValue(BRepCheck_FreeEdge)++; break;
        //
      case BRepCheck_InvalidMultiConnexity:
        NbProblems->ChangeValue(BRepCheck_InvalidMultiConnexity)++; break;
        //
      case BRepCheck_InvalidRange:
        NbProblems->ChangeValue(BRepCheck_InvalidRange)++; break;
        //
      case BRepCheck_EmptyWire:
        NbProblems->ChangeValue(BRepCheck_EmptyWire)++; break;
        //
      case BRepCheck_RedundantEdge:
        NbProblems->ChangeValue(BRepCheck_RedundantEdge)++; break;
        //
      case BRepCheck_SelfIntersectingWire:
        NbProblems->ChangeValue(BRepCheck_SelfIntersectingWire)++; break;
        //
      case BRepCheck_NoSurface:
        NbProblems->ChangeValue(BRepCheck_NoSurface)++; break;
        //
      case BRepCheck_InvalidWire:
        NbProblems->ChangeValue(BRepCheck_InvalidWire)++; break;
        //
      case BRepCheck_RedundantWire:
        NbProblems->ChangeValue(BRepCheck_RedundantWire)++; break;
        //
      case BRepCheck_IntersectingWires:
        NbProblems->ChangeValue(BRepCheck_IntersectingWires)++; break;
        //
      case BRepCheck_InvalidImbricationOfWires:
        NbProblems->ChangeValue(BRepCheck_InvalidImbricationOfWires)++; break;
        //
      case BRepCheck_EmptyShell:
        NbProblems->ChangeValue(BRepCheck_EmptyShell)++; break;
        //
      case BRepCheck_RedundantFace:
        NbProblems->ChangeValue(BRepCheck_RedundantFace)++; break;
        //
      case BRepCheck_InvalidImbricationOfShells:
        NbProblems->ChangeValue(BRepCheck_InvalidImbricationOfShells)++; break;
        //
      case BRepCheck_UnorientableShape:
        NbProblems->ChangeValue(BRepCheck_UnorientableShape)++; break;
        //
      case BRepCheck_NotClosed:
        NbProblems->ChangeValue(BRepCheck_NotClosed)++; break;
        //
      case BRepCheck_NotConnected:
        NbProblems->ChangeValue(BRepCheck_NotConnected)++; break;
        //
      case BRepCheck_SubshapeNotInShape:
        NbProblems->ChangeValue(BRepCheck_SubshapeNotInShape)++; break;
        //
      case BRepCheck_BadOrientation:
        NbProblems->ChangeValue(BRepCheck_BadOrientation)++; break;
        //
      case BRepCheck_BadOrientationOfSubshape:
        NbProblems->ChangeValue(BRepCheck_BadOrientationOfSubshape)++; break;
        //
      case BRepCheck_InvalidPolygonOnTriangulation:
        NbProblems->ChangeValue(BRepCheck_InvalidPolygonOnTriangulation)++; break;
        //
      case BRepCheck_InvalidToleranceValue:
        NbProblems->ChangeValue(BRepCheck_InvalidToleranceValue)++; break;
        //
      case BRepCheck_EnclosedRegion:
        NbProblems->ChangeValue(BRepCheck_EnclosedRegion)++; break;
        //
      case BRepCheck_CheckFail:
        NbProblems->ChangeValue(BRepCheck_CheckFail)++; break;
        //
      default: break;
    }
  }

  //! \todo provide description here.
  void GetProblemSub(const BRepCheck_Analyzer&           Ana,
                     const TopoDS_Shape&                 Shape,
                     Handle(TopTools_HSequenceOfShape)&  sl,
                     Handle(TColStd_HArray1OfInteger)&   NbProblems,
                     const TopAbs_ShapeEnum              Subtype,
                     TopTools_DataMapOfShapeListOfShape& Map)
  {
    BRepCheck_ListIteratorOfListOfStatus itl;
    TopExp_Explorer exp;
    for ( exp.Init(Shape, Subtype); exp.More(); exp.Next() )
    {
      // Get current shape
      const TopoDS_Shape& sub = exp.Current();

      // Get result for the current shape
      const Handle(BRepCheck_Result)& res = Ana.Result(sub);

      // Iterate over the result
      for ( res->InitContextIterator(); res->MoreShapeInContext(); res->NextShapeInContext() )
      {
        if ( res->ContextualShape().IsSame(Shape) && !Contains(Map(sub), Shape) )
        {
          Map(sub).Append(Shape);
          itl.Initialize( res->StatusOnShape() );

          if ( itl.Value() != BRepCheck_NoError )
          {
            int ii = 0;
            for ( ii = 1; ii <= sl->Length(); ii++ )
              if ( sl->Value(ii).IsSame(sub) )
                break;

            if ( ii > sl->Length() )
            {
              sl->Append(sub);
              FillProblems(itl.Value(), NbProblems);
            }

            for ( ii = 1; ii <= sl->Length(); ii++ )
              if ( sl->Value(ii).IsSame(Shape) )
                break;

            if ( ii > sl->Length() )
            {
              sl->Append(Shape);
              FillProblems(itl.Value(), NbProblems);
            }
          }
          break;
        }
      }
    }
  }

  //! \todo provide description here.
  static void GetProblemShapes(const BRepCheck_Analyzer&           Ana,
                               const TopoDS_Shape&                 Shape,
                               Handle(TopTools_HSequenceOfShape)&  sl,
                               Handle(TColStd_HArray1OfInteger)&   NbProblems,
                               TopTools_DataMapOfShapeListOfShape& theMap)
  {
    for ( TopoDS_Iterator iter(Shape); iter.More(); iter.Next() )
    {
      GetProblemShapes(Ana, iter.Value(), sl,  NbProblems, theMap);
    }

    TopAbs_ShapeEnum styp = Shape.ShapeType();
    BRepCheck_ListIteratorOfListOfStatus itl;

    if ( !Ana.Result(Shape).IsNull() && !theMap.IsBound(Shape) )
    {
      itl.Initialize( Ana.Result(Shape)->Status() );

      if ( itl.Value() != BRepCheck_NoError )
      {
        sl->Append(Shape);
        FillProblems(itl.Value(), NbProblems);
      }
    }

    if ( !theMap.IsBound(Shape) )
    {
      TopTools_ListOfShape thelist;
      theMap.Bind(Shape, thelist);
    }

    switch ( styp )
    {
      case TopAbs_EDGE:
        GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_VERTEX, theMap);
        break;
      case TopAbs_FACE:
        GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_WIRE,   theMap);
        GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_EDGE,   theMap);
        GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_VERTEX, theMap);
        break;
      case TopAbs_SHELL:
        break;
      case TopAbs_SOLID:
        GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_SHELL, theMap);
        break;
      default:
        break;
    }
  }

  //! Dumps all detected invalidities to Logger.
  //! \param[in] progress progress entry.
  //! \param[in] plotter  imperative plotter.
  //! \param[in] analyzer checker tool.
  //! \param[in] shape    shape to check.
  void StructuralDump(ActAPI_ProgressEntry      progress,
                      ActAPI_PlotterEntry       plotter,
                      const BRepCheck_Analyzer& analyzer,
                      const TopoDS_Shape&       shape)
  {
    if ( progress.Access().IsNull() )
      return; // Cannot dump to null Logger

    progress.SendLogMessage(LogErr(Normal) << "Shape is invalid.");

    // Initialize array storing the number of problems
    Handle(TColStd_HArray1OfInteger)
      NbProblems = new TColStd_HArray1OfInteger(1, BRepCheck_CheckFail);
    //
    for ( int i = 1; i <= BRepCheck_CheckFail; i++ )
      NbProblems->SetValue(i, 0);

    Handle(TopTools_HSequenceOfShape) sl = new TopTools_HSequenceOfShape();
    Handle(TopTools_HSequenceOfShape) slv, sle, slw, slf, sls, slo;

    TopTools_DataMapOfShapeListOfShape MAP;
    GetProblemShapes(analyzer, shape, sl, NbProblems, MAP);

    if ( NbProblems->Value(BRepCheck_InvalidPointOnCurve) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Invalid point on curve (count: %1)."
                                               << NbProblems->Value(BRepCheck_InvalidPointOnCurve) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidPointOnCurveOnSurface) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Invalid point on CONS (count: %1)."
                                               << NbProblems->Value(BRepCheck_InvalidPointOnCurveOnSurface) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidPointOnSurface) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Invalid point on surface (count: %1)."
                                               << NbProblems->Value(BRepCheck_InvalidPointOnSurface) );
    //
    if ( NbProblems->Value(BRepCheck_No3DCurve) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Missing 3D curve (count: %1)."
                                               << NbProblems->Value(BRepCheck_No3DCurve) );
    //
    if ( NbProblems->Value(BRepCheck_Multiple3DCurve) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Multiple 3D curve (count: %1)."
                                               << NbProblems->Value(BRepCheck_Multiple3DCurve) );
    //
    if ( NbProblems->Value(BRepCheck_Invalid3DCurve) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Invalid 3D curve (count: %1)."
                                               << NbProblems->Value(BRepCheck_Invalid3DCurve) );
    //
    if ( NbProblems->Value(BRepCheck_NoCurveOnSurface) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Missing p-curve (count: %1)."
                                               << NbProblems->Value(BRepCheck_NoCurveOnSurface) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidCurveOnSurface) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Invalid p-curve (count: %1)."
                                               << NbProblems->Value(BRepCheck_InvalidCurveOnSurface) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidCurveOnClosedSurface) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Invalid p-curve on closed surface (count: %1)."
                                               << NbProblems->Value(BRepCheck_InvalidCurveOnClosedSurface) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidSameRangeFlag) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Same-range flag is turned off (count: %1)."
                                               << NbProblems->Value(BRepCheck_InvalidSameRangeFlag) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidSameParameterFlag) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Same-parameter flag is turned off (count: %1)."
                                               << NbProblems->Value(BRepCheck_InvalidSameParameterFlag) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidDegeneratedFlag) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Invalid degenerated flag (count: %1)."
                                               << NbProblems->Value(BRepCheck_InvalidDegeneratedFlag) );
    //
    if ( NbProblems->Value(BRepCheck_FreeEdge) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Unexpected free edge (count: %1)."
                                               << NbProblems->Value(BRepCheck_FreeEdge) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidMultiConnexity) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Multi-connexity problem (count: %1)."
                                               << NbProblems->Value(BRepCheck_InvalidMultiConnexity) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidRange) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Invalid range (count: %1)."
                                               << NbProblems->Value(BRepCheck_InvalidRange) );
    //
    if ( NbProblems->Value(BRepCheck_EmptyWire) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Empty wire (count: %1)."
                                               << NbProblems->Value(BRepCheck_EmptyWire) );
    //
    if ( NbProblems->Value(BRepCheck_RedundantEdge) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Redundant edge (count: %1)."
                                              << NbProblems->Value(BRepCheck_RedundantEdge) );
    //
    if ( NbProblems->Value(BRepCheck_SelfIntersectingWire) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Self-intersecting wire (count: %1)."
                                               << NbProblems->Value(BRepCheck_SelfIntersectingWire) );
    //
    if ( NbProblems->Value(BRepCheck_NoSurface) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Missing surface (count: %1)."
                                               << NbProblems->Value(BRepCheck_NoSurface) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidWire) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Invalid wire (count: %1)."
                                               << NbProblems->Value(BRepCheck_InvalidWire) );
    //
    if ( NbProblems->Value(BRepCheck_RedundantWire) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Redundant wire (count: %1)."
                                               << NbProblems->Value(BRepCheck_RedundantWire) );
    //
    if ( NbProblems->Value(BRepCheck_IntersectingWires) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Intersecting wires (count: %1)."
                                               << NbProblems->Value(BRepCheck_IntersectingWires) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidImbricationOfWires) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Invalid imbrication of wires (count: %1)."
                                               << NbProblems->Value(BRepCheck_InvalidImbricationOfWires) );
    //
    if ( NbProblems->Value(BRepCheck_EmptyShell) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Empty shell (count: %1)."
                                               << NbProblems->Value(BRepCheck_EmptyShell) );
    //
    if ( NbProblems->Value(BRepCheck_RedundantFace) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Redundant face (count: %1)."
                                               << NbProblems->Value(BRepCheck_RedundantFace) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidImbricationOfShells) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Invalid imbrication of shells (count: %1)."
                                               << NbProblems->Value(BRepCheck_InvalidImbricationOfShells) );
    //
    if ( NbProblems->Value(BRepCheck_UnorientableShape) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Unorientable shape (count: %1)."
                                               << NbProblems->Value(BRepCheck_UnorientableShape) );
    //
    if ( NbProblems->Value(BRepCheck_NotClosed) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Not closed (count: %1)."
                                               << NbProblems->Value(BRepCheck_NotClosed) );
    //
    if ( NbProblems->Value(BRepCheck_NotConnected) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Not connected (count: %1)."
                                               << NbProblems->Value(BRepCheck_NotConnected) );
    //
    if ( NbProblems->Value(BRepCheck_SubshapeNotInShape) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Sub-shape is not in shape (count: %1)."
                                               << NbProblems->Value(BRepCheck_SubshapeNotInShape) );
    //
    if ( NbProblems->Value(BRepCheck_BadOrientation) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Bad orientation (count: %1)."
                                              << NbProblems->Value(BRepCheck_BadOrientation) );
    //
    if ( NbProblems->Value(BRepCheck_BadOrientationOfSubshape) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Bad orientation of sub-shape (count: %1)."
                                               << NbProblems->Value(BRepCheck_BadOrientationOfSubshape) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidPolygonOnTriangulation) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Invalid polygon on triangulation (count: %1)."
                                               << NbProblems->Value(BRepCheck_InvalidPolygonOnTriangulation) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidToleranceValue) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Invalid tolerance value (count: %1)."
                                               << NbProblems->Value(BRepCheck_InvalidToleranceValue) );
    //
    if ( NbProblems->Value(BRepCheck_EnclosedRegion) > 0 )
      progress.SendLogMessage( LogWarn(Normal) << "Enclosed region (count: %1)."
                                               << NbProblems->Value(BRepCheck_EnclosedRegion) );
    //
    if ( NbProblems->Value(BRepCheck_CheckFail) > 0 )
      progress.SendLogMessage( LogErr(Normal) << "Shape checker failed (count: %1)."
                                              << NbProblems->Value(BRepCheck_CheckFail) );

    /* =====================
     *  Dump invalid shapes
     * ===================== */

    slv = new TopTools_HSequenceOfShape();
    sle = new TopTools_HSequenceOfShape();
    slw = new TopTools_HSequenceOfShape();
    slf = new TopTools_HSequenceOfShape();
    sls = new TopTools_HSequenceOfShape();
    slo = new TopTools_HSequenceOfShape();

    // Populate collections.
    for ( int i = 1; i <= sl->Length(); ++i )
    {
      const TopoDS_Shape& shi = sl->Value(i);
      TopAbs_ShapeEnum    sti = shi.ShapeType();
      //
      switch (sti)
      {
        case TopAbs_VERTEX : slv->Append(shi); break;
        case TopAbs_EDGE   : sle->Append(shi); break;
        case TopAbs_WIRE   : slw->Append(shi); break;
        case TopAbs_FACE   : slf->Append(shi); break;
        case TopAbs_SHELL  : sls->Append(shi); break;
        case TopAbs_SOLID  : slo->Append(shi); break;
        default            : break;
      }
    }

    BRep_Builder B;

    // Vertices.
    if ( slv->Length() )
    {
      TopoDS_Compound comp;
      B.MakeCompound(comp);
      const int nb = slv->Length();

      for ( int i = 1; i <= nb; ++i )
        B.Add( comp, slv->Value(i) );

      plotter.DRAW_SHAPE(comp, Color_Red, 1.0, true, "faulty_vertices");
    }

    // Edges.
    if ( sle->Length() )
    {
      TopoDS_Compound comp;
      B.MakeCompound(comp);
      const int nb = sle->Length();

      for( int i = 1; i <= nb; ++i )
        B.Add( comp, sle->Value(i) );

      plotter.DRAW_SHAPE(comp, Color_Red, 1.0, true, "faulty_edges");
    }

    // Wires.
    if ( slw->Length() )
    {
      TopoDS_Compound comp;
      B.MakeCompound(comp);
      const int nb = slw->Length();

      for ( int i = 1; i <= nb; ++i )
        B.Add( comp, slw->Value(i) );

      plotter.DRAW_SHAPE(comp, Color_Red, 1.0, true, "faulty_wires");
    }

    // Faces.
    if ( slf->Length() )
    {
      TopoDS_Compound comp;
      B.MakeCompound(comp);
      const int nb = slf->Length();

      for ( int i = 1; i <= nb; ++i )
        B.Add( comp, slf->Value(i) );

      plotter.DRAW_SHAPE(comp, Color_Red, "faulty_faces");
    }

    // Shells.
    if ( sls->Length() )
    {
      TopoDS_Compound comp;
      B.MakeCompound(comp);
      const int nb = sls->Length();

      for ( int i = 1; i <= nb; ++i )
        B.Add( comp, sls->Value(i) );

      plotter.DRAW_SHAPE(comp, Color_Red, "faulty_shells");
    }

    // Solids.
    if ( slo->Length() )
    {
      TopoDS_Compound comp;
      B.MakeCompound(comp);
      const int nb = slo->Length();

      for ( int i = 1; i <= nb; ++i )
        B.Add( comp, slo->Value(i) );

      plotter.DRAW_SHAPE(comp, Color_Red, "faulty_solids");
    }
  }

} // CheckShapeAux namespace.

//-----------------------------------------------------------------------------

asiAlgo_CheckValidity::asiAlgo_CheckValidity(ActAPI_ProgressEntry progress,
                                             ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_CheckValidity::CheckBasic(const TopoDS_Shape& shape)
{
  BRepCheck_Analyzer Checker(shape);

  // Check validity
  if ( Checker.IsValid() )
  {
    m_progress.SendLogMessage(LogInfo(Normal) << "Part is topologically and geometrically sound.");
    return true;
  }

  // ...
  // Shape is known to be invalid at this stage
  // ...

  // Perform structural dump
  CheckShapeAux::StructuralDump(m_progress, m_plotter, Checker, shape);

  return false;
}

//-----------------------------------------------------------------------------

bool asiAlgo_CheckValidity::IsFinite(const TopoDS_Solid& solid)
{
  BRepClass3d_SolidClassifier clas3d(solid);
  clas3d.PerformInfinitePoint( Precision::Confusion() );
  //
  if ( clas3d.State() == TopAbs_IN )
    return false;

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_CheckValidity::HasAllClosedWires(const TopoDS_Face& face)
{
  // Set default tolerance.
  const double tolerance = MaxTolerance(face)*5.0;

  // Check closeness.
  return HasAllClosedWires(face, tolerance);
}

//-----------------------------------------------------------------------------

bool asiAlgo_CheckValidity::HasAllClosedWires(const TopoDS_Face& face,
                                              const double       coincConfusion3d)
{
  // Loop over the wires
  for ( TopoDS_Iterator it(face); it.More(); it.Next() )
  {
    const TopoDS_Shape& subshape = it.Value();
    //
    if ( subshape.ShapeType() != TopAbs_WIRE )
      continue;

    const TopoDS_Wire& wire = TopoDS::Wire(subshape);

    // Check each wire individually.
    if ( !IsClosedGeometrically(wire, face, coincConfusion3d) )
      return false;
  }
  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_CheckValidity::IsClosedGeometrically(const TopoDS_Wire& wire,
                                                  const TopoDS_Face& face,
                                                  const double       coincConfusion3d)
{
  // Convert spatial tolerance to UV-parametric tolerance using resolutions.
  // This is actually a weak check as sometimes correction according to
  // resolution gives unreliable 2D tolerances. However, we think that using
  // such resolutions is still better than application of three-dimensional
  // tolerances in two-dimensional space.
  GeomAdaptor_Surface bas( BRep_Tool::Surface(face) );
  //
  const double uToler      = bas.UResolution(coincConfusion3d);
  const double vToler      = bas.VResolution(coincConfusion3d);
  const double uvConfusion = Max(uToler, vToler);

  // This method takes into account topological orientation of the wire
  // in question. However, it does not check any orientation flags. It simply
  // tests all extremities of one edge with all extremities of another edge.

  // It should be noted that in OpenCascade there is no restriction for a
  // wire to contain all edges in the order of traverse. That is, a wire
  // traversed as E1->E2->E3->E4 may physically contain those edges in
  // the order like E1, E2, E4, E3. Therefore, simple sequential iteration
  // from previous to next edge is not enough to check that all extremities
  // of all edges are 1-manifold. To check this, we use an auxiliary map of
  // edges together with valences of their extremities. For example:
  //
  //  Edge | <Extremity 1; Extremity 2> <-> <Valence 1; Valence 2>
  // ------+------------------------------------------------------
  //   E1  | <(0,0); (0,1)>             <-> <2, 2>
  //   E2  | <(1,1); (0,1)>             <-> <1, 2>
  //  ...
  //
  // This map allows making verdict on closeness of the wire in question.
  // Apparently, in a well-defined contour, all edges should have all
  // extremities with valence 2.
  //
  // NOTICE: to avoid degeneracies in 3D for special cases like cylinders,
  //         cones, spheres, tori, etc., we use (u,v) parametric space
  //         for extremity coordinates. We make use of the fact, that a valid
  //         parametric domain is a topological rectangle. Our reasoning of
  //         valences is valid only in this assumption.

  // Check number of edges
  TopTools_IndexedMapOfShape edges;
  TopExp::MapShapes(wire, TopAbs_EDGE, edges);
  const int nEdges = edges.Extent();
  //
  if ( !nEdges )
    return false;
  //
  if ( nEdges == 1 )
  {
    double f, l;
    Handle(Geom_Curve) C = BRep_Tool::Curve( TopoDS::Edge( edges(1) ), f, l );

    if ( C.IsNull() )
      return false;

    // In case of sole edge, the boundary will be closed if two conditions
    // are met:
    //
    // - 3D curve is closed
    // - 2D curve is closed

    gp_Pnt Pf = C->Value( C->FirstParameter() );
    gp_Pnt Pl = C->Value( C->LastParameter() );

    // The following condition is Ok for closed curves
    const bool isOk3d = ( Pf.Distance(Pl) < coincConfusion3d );

    // Check 2d
    gp_Pnt2d Pf2d, Pl2d;
    BRep_Tool::UVPoints(TopoDS::Edge( edges(1) ), face, Pf2d, Pl2d);

    // The following condition is Ok for closed curves
    const bool isOk2d = ( Pf2d.Distance(Pl2d) < uvConfusion );

    return isOk3d && isOk2d;
  }

  struct t_extremities
  {
    gp_Pnt2d P1, P2;
  };
  struct t_valences
  {
    int V1, V2;
  };
  typedef NCollection_DataMap< TopoDS_Edge, std::pair<t_extremities, t_valences> > t_valence_map;

  // Prepare valence map
  t_valence_map valenceMap;

  // Check connectivity. We use explorer to loop over the wire, because we need
  // all coedges to participate in that loop. E.g. a single seam edge should be
  // iterated twice as it has two p-curves.
  for ( TopExp_Explorer exp(wire, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    TopoDS_Edge E = TopoDS::Edge( exp.Current() );

    // Get 2D points. Notice that we get 2D points as extermities of CONS
    // (Curve ON Surface) and not with UVPoints() method of BRep_Tool. This
    // is because the latter method returns pre-evaluated UV coordinates
    // which suffer from instability when it comes to periodic surfaces.
    // E.g., U coordinate may be either 0 or 2*PI depending on how to evaluate
    // the corresponding UV pair. However, if we use pcurve explicitly, such
    // instability does not happen as we essentially use geometry which we see
    // in parametric domain of a face (always a finite boundedregion without
    // any weird periodicity anomalies).
    gp_Pnt2d             Pf, Pl;
    double               f, l;
    Handle(Geom2d_Curve) c2d;
    //
    try
    {
      c2d = BRep_Tool::CurveOnSurface(E, face, f, l);
    }
    catch ( ... ) { continue; }
    //
    if ( !c2d.IsNull() )
    {
      Pf = c2d->Value(f);
      Pl = c2d->Value(l);
    }
    else
    {
      // Use cached points if p-curve is not available...
      BRep_Tool::UVPoints(E, face, Pf, Pl);
    }

    // Add initial valence record for the new edge
    valenceMap.Bind( E, std::pair<t_extremities, t_valences>() );
    valenceMap(E).first.P1  = Pf;
    valenceMap(E).first.P2  = Pl;
    valenceMap(E).second.V1 = 1;
    valenceMap(E).second.V2 = 1;

    // Update valence map wrt new record
    for ( t_valence_map::Iterator it2(valenceMap); it2.More(); it2.Next() )
    {
      const TopoDS_Edge& otherEdge = it2.Key();
      //
      if ( otherEdge.IsPartner(E) )
        continue;

      std::pair<t_extremities, t_valences>& otherRecord = it2.ChangeValue();

      const gp_Pnt2d& otherPf = otherRecord.first.P1;
      const gp_Pnt2d& otherPl = otherRecord.first.P2;

      if ( Pf.Distance(otherPf) < uvConfusion )
      {
        valenceMap(E).second.V1++;
        otherRecord.second.V1++;
      }
      if ( Pf.Distance(otherPl) < uvConfusion )
      {
        valenceMap(E).second.V1++;
        otherRecord.second.V2++;
      }
      if ( Pl.Distance(otherPf) < uvConfusion )
      {
        valenceMap(E).second.V2++;
        otherRecord.second.V1++;
      }
      if ( Pl.Distance(otherPl) < uvConfusion )
      {
        valenceMap(E).second.V2++;
        otherRecord.second.V2++;
      }
    } // end update valence map
  } // end accumulating of valence map

#if defined COUT_DEBUG
  for ( t_valence_map::Iterator it(valenceMap); it.More(); it.Next() )
  {
    const TopoDS_Edge&                          E      = it.Key();
    const std::pair<t_extremities, t_valences>& record = it.Value();

    std::cout << ShapeAddrWithPrefix(E).c_str() << std::endl;
    std::cout << "\t(" << record.first.P1.X() << ", "
                       << record.first.P1.Y() << ") ~ "
                       << record.second.V1 << std::endl;
    std::cout << "\t(" << record.first.P2.X() << ", "
                       << record.first.P2.Y() << ") ~ "
                       << record.second.V2 << std::endl;
  }
  std::cout << "***" << std::endl;
#endif

  // Now check valence map. If any record with valence < 2 exists, return false.
  for ( t_valence_map::Iterator it(valenceMap); it.More(); it.Next() )
  {
    const std::pair<t_extremities, t_valences>& record = it.Value();
    //
    if ( record.second.V1 < 2 || record.second.V2 < 2 )
      return false;
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_CheckValidity::HasEdgesWithoutVertices(const TopoDS_Face& face)
{
  for ( TopExp_Explorer exp(face, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    TopTools_IndexedMapOfShape vertices;
    TopExp::MapShapes(exp.Current(), TopAbs_VERTEX, vertices);

    if ( vertices.IsEmpty() )
      return true;
  }
  return false;
}

//-----------------------------------------------------------------------------

double asiAlgo_CheckValidity::MaxTolerance(const TopoDS_Shape& shape)
{
  ShapeAnalysis_ShapeTolerance TolerChecker;
  const double maxToler = TolerChecker.Tolerance(shape, 1); // 1 means max

  return maxToler;
}

//-----------------------------------------------------------------------------

bool asiAlgo_CheckValidity::HasDistinctVertexOrientations(const TopoDS_Edge& edge)
{
  bool isForwardFound = false, isReversedFound = false;

  for ( TopoDS_Iterator it(edge); it.More(); it.Next() )
  {
    const TopoDS_Shape& subsh = it.Value();
    //
    if ( subsh.Orientation() == TopAbs_FORWARD )
      isForwardFound = true;
    //
    if ( subsh.Orientation() == TopAbs_REVERSED )
      isReversedFound = true;
  }

  return isForwardFound && isReversedFound;
}

//-----------------------------------------------------------------------------

bool asiAlgo_CheckValidity::HasDomainSelfIntersections(const TopoDS_Face& face,
                                                       const bool         stopAtFirst)
{
  const double         conf = Precision::Confusion();
  const double         tol  = this->MaxTolerance(face);
  Handle(Geom_Surface) surf = BRep_Tool::Surface(face);

  // Get all edges.
  TopTools_IndexedMapOfShape edgesMap;
  TopExp::MapShapes(face, TopAbs_EDGE, edgesMap);

  // Get all pcurves and build their bounding boxes.
  std::vector<TopoDS_Edge>               edges;
  std::vector<Handle(Geom2d_Curve)>      pcurves;
  std::vector<Bnd_Box2d>                 aabbs;
  std::vector<std::pair<double, double>> paramLimits; // (first, last) curve parameters.
  //
  for ( int k = 1; k <= edgesMap.Extent(); ++k )
  {
    const TopoDS_Edge& E = TopoDS::Edge( edgesMap(k) );

    // Get pcurve.
    double f, l;
    Handle(Geom2d_Curve) c2d = BRep_Tool::CurveOnSurface(E, face, f, l);
    //
    if ( c2d.IsNull() )
      continue;

    // Add edge.
    edges.push_back(E);

    // Add pcurve.
    pcurves.push_back(c2d);

    // Compute bounding box.
    Bnd_Box2d aabb;
    BndLib_Add2dCurve::Add(c2d, conf, aabb);
    //
    aabbs.push_back(aabb);

    // Add parameter pair.
    paramLimits.push_back( std::pair<double, double>(f, l) );
  }

  const int numCurves = int( pcurves.size() );

  // Check for intersections.
  int numHits = 0;
  TColStd_PackedMapOfInteger checked;
  //
  for ( int i = 0; i < numCurves; ++i )
  {
    if ( checked.Contains(i) ) continue;

    TopoDS_Vertex V1f = ShapeAnalysis_Edge().FirstVertex(edges[i]);
    TopoDS_Vertex V1l = ShapeAnalysis_Edge().LastVertex(edges[i]);

    for ( int j = 0; j < numCurves; ++j )
    {
      if ( i == j ) continue;

      if ( aabbs[i].IsOut(aabbs[j]) ) continue; // Fast intersection test.

      /* Precise check */

      TopoDS_Vertex V2f = ShapeAnalysis_Edge().FirstVertex(edges[j]);
      TopoDS_Vertex V2l = ShapeAnalysis_Edge().LastVertex(edges[j]);

      // Perform intersection.
      Geom2dAdaptor_Curve c1(pcurves[i], paramLimits[i].first, paramLimits[i].second);
      Geom2dAdaptor_Curve c2(pcurves[j], paramLimits[j].first, paramLimits[j].second);
      //
      Geom2dInt_GInter inter(c1, c2, conf, tol);
      //
      if ( !inter.IsDone() ) continue;

      // Consult intersection results.
      const int numInter = inter.NbPoints();
      for ( int k = 1; k <= numInter; ++k )
      {
        const IntRes2d_IntersectionPoint& IP  = inter.Point(k);
        const IntRes2d_Transition&        tr1 = IP.TransitionOfFirst();
        const IntRes2d_Transition&        tr2 = IP.TransitionOfSecond();
        //
        if ( tr1.PositionOnCurve() != IntRes2d_Middle &&
             tr2.PositionOnCurve() != IntRes2d_Middle ) continue;

        const double pOn1 = IP.ParamOnFirst();
        const double pOn2 = IP.ParamOnSecond();

        /* Check in 3D */

        // Get intersection points.
        gp_Pnt P1 = CheckShapeAux::GetPointOnSurf(surf, c1, pOn1);
        gp_Pnt P2 = CheckShapeAux::GetPointOnSurf(surf, c2, pOn2);

        const bool hitExtremityC1 = ( P1.Distance( BRep_Tool::Pnt(V1f) ) < tol || 
                                      P1.Distance( BRep_Tool::Pnt(V1l) ) < tol );
        const bool hitExtremityC2 = ( P2.Distance( BRep_Tool::Pnt(V2f) ) < tol || 
                                      P2.Distance( BRep_Tool::Pnt(V2l) ) < tol );

        if ( hitExtremityC1 && hitExtremityC2 ) continue;

        ++numHits;

        // Graphical dump.
        m_plotter.DRAW_POINT(P1,         Color_Yellow, "ip3d1");
        m_plotter.DRAW_POINT(P2,         Color_Yellow, "ip3d2");
        m_plotter.DRAW_POINT(IP.Value(), Color_Yellow, "ip2d");
      }

      if ( stopAtFirst && numHits > 0 )
        return true;
    }

    checked.Add(i);
  }

  return numHits > 0;
}
