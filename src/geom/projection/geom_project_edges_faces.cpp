//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_project_edges_faces.h>

// Geometry includes
#include <geom_project_edge_face.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Constructor.
geom_project_edges_faces::geom_project_edges_faces()
{
}

//! Performs edges-to-faces projection.
//! \param Slicers       [in]     face with all its slicer edges.
//! \param inaccuracy    [in]     inaccuracy of the input geometry.
//! \param ResultContour [out]    resulting wire(s).
//! \param ResultSlicers [out]    faces and their slicing edges.
//! \param Journal       [in/out] journal instance.
//! \return true in case of success, false -- otherwise.
bool geom_project_edges_faces::operator()(const TSlicerMap&    Slicers,
                                          const double         inaccuracy,
                                          TopoDS_Shape&        ResultContour,
                                          TopoDS_Shape&        ResultSlicers,
                                          ActAPI_ProgressEntry Journal)
{
  Handle(TopTools_HSequenceOfShape) proj_slicers = new TopTools_HSequenceOfShape;

  TopoDS_Compound CompObjectsProj;

#if defined DRAW_DEBUG
  TopoDS_Compound CompObjects;
  BRep_Builder().MakeCompound(CompObjects);
#endif

  BRep_Builder().MakeCompound(CompObjectsProj);

  // Loop over the tentative faces
  for ( TSlicerMap::Iterator it(Slicers); it.More(); it.Next() )
  {
    const TopoDS_Face& F = it.Key();
    Handle(TopTools_HSequenceOfShape)& slicers = it.ChangeValue();

    // Prepare projection tool
    geom_project_edge_face Proj(F);

#if defined DRAW_DEBUG
    TopoDS_Compound CompObject, CompEdges;
    BRep_Builder().MakeCompound(CompObject);
    BRep_Builder().MakeCompound(CompEdges);
    BRep_Builder().Add( CompObject, F );
#endif

    // Slicer compound
    TopoDS_Compound CompObjectProj, CompEdgesProj;
    BRep_Builder().MakeCompound(CompObjectProj);
    BRep_Builder().MakeCompound(CompEdgesProj);
    BRep_Builder().Add( CompObjectProj, F );

    // Loop over the slicer edges
    for ( int e_idx = 1; e_idx <= slicers->Length(); ++e_idx )
    {
      TopoDS_Edge E = TopoDS::Edge( slicers->Value(e_idx) );

      // Project edge on face
      TopoDS_Shape Res;
      gp_Lin projRay;

      // NOTICE: we make two attempts to project an edge onto a face. First
      //         attempt uses BOP-based projection and that is not by accident.
      //         Normal projection of OCCT has several advantages over the
      //         projection by direction (BOP-based projection):
      //
      //         1) You do not have to guess the projection direction, which
      //            is sometimes not trivial.
      //
      //         2) Normal projection behaves smarter than BOP-based one. We
      //            cannot say that it is more robust (it is not), but in some
      //            cases it may give much less complex geometry. E.g. consider
      //            a case when you have "almost planar" spline surface and
      //            "almost planar" spline curve to project. BOP-based
      //            projection may return very complicated projection result,
      //            while normal projection will more or less stick to the
      //            required complexity (at least you can control this
      //            complexity, which is not the case with BOP-based tool).
      //
      //         3) Finally, normal project mu(uuuu)ch faster than BOP-based one.
      //
      //         BUT: it is not as reliable as BOP-based one... Sometimes
      //              it may unreasonably shift a vertex or even produce empty
      //              result. That is why we use it only as a last-chance tool.

      if ( !Proj(E, 0, Res, projRay, Journal) )
      {
        Journal.SendLogMessage( LogWarn(Normal) << "geom_project_edges_faces.CannotBRepProj" << E << F );

        if ( !Proj(E, 1, Res, projRay, Journal) )
        {
          Journal.SendLogMessage( LogErr(Normal) << "geom_project_edges_faces.ProjectionFailed" << E << F );
          return false;
        }
      }

#if defined DRAW_DEBUG
      if ( !E.IsNull() )
      {
        BRep_Builder().Add( CompEdges, E );

        Handle(Geom_TrimmedCurve)
          projMarker = new Geom_TrimmedCurve( new Geom_Line(projRay), 0, 500 );

        BRep_Builder().Add( CompEdges, BRepBuilderAPI_MakeEdge(projMarker) );
      }
#endif

      if ( !Res.IsNull() )
      {
        BRep_Builder().Add( CompEdgesProj, Res );
      }

      // Projection is done, let's collect the results
      for ( TopExp_Explorer exp(Res, TopAbs_EDGE); exp.More(); exp.Next() )
      {
        TopoDS_Edge E_proj = TopoDS::Edge( exp.Current() );

        if ( E_proj.IsNull() )
          continue;

        proj_slicers->Append(E_proj);
      }
    }
    BRep_Builder().Add( CompObjectProj, CompEdgesProj );
    BRep_Builder().Add( CompObjectsProj, CompObjectProj );

#if defined DRAW_DEBUG
    BRep_Builder().Add( CompObjects, CompObject );
#endif
  }

#if defined DRAW_DEBUG
  DBRep::Set("obj", CompObjects);
  DBRep::Set("objproj", CompObjectsProj);
#endif

  // Connect the projected slicers to wires
  Handle(TopTools_HSequenceOfShape) proj_slicer_wires;
  ShapeAnalysis_FreeBounds::ConnectEdgesToWires(proj_slicers,
                                                inaccuracy,
                                                0,
                                                proj_slicer_wires);

  if ( proj_slicer_wires.IsNull() || proj_slicer_wires->IsEmpty() )
  {
    Journal.SendLogMessage( LogWarn(Normal) << "geom_project_edges_faces.NoWires" );
    return false;
  }

  // Make a single shape
  TopoDS_Compound ResC;
  BRep_Builder().MakeCompound(ResC);
  for ( int w_idx = 1; w_idx <= proj_slicer_wires->Length(); ++w_idx )
  {
    const TopoDS_Wire& W = TopoDS::Wire( proj_slicer_wires->Value(w_idx) );
    BRep_Builder().Add(ResC, W);
  }

  // Set results
  ResultContour = ResC;
  ResultSlicers = CompObjectsProj;
  return true;
}
