//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_split_edges.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <NCollection_QuickSort.hxx>
#include <ShapeFix_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_HSequenceOfShape.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Constructor.
//! \param inaccuracy [in] inaccuracy of the input geometry.
geom_split_edges::geom_split_edges(const double inaccuracy)
: geom_split_shape(inaccuracy)
{
}

//! Performs splitting of face boundaries.
//! \param InterPts        [in]     intersection points between interacting edges.
//! \param SlicerMap       [in]     old slicer map.
//! \param OriginalBody    [in]     original body.
//! \param ResultBody      [out]    result body.
//! \param Result          [out]    result as prepared faces.
//! \param ResultSlicerMap [out]    new slicer map.
//! \param Journal         [in/out] journal instance.
void geom_split_edges::operator()(const geom_intersection_points_edges& InterPts,
                                  const TSlicerMap&                     SlicerMap,
                                  const TopoDS_Shape&                   OriginalBody,
                                  TopoDS_Shape&                         ResultBody,
                                  TopoDS_Compound&                      Result,
                                  TSlicerMap&                           ResultSlicerMap,
                                  ActAPI_ProgressEntry                  Journal)
{
  // ...
  // For each boundary edge collect all its interaction points with other
  // edges. We first need to accumulate these points and only then we split
  // the boundary
  // ...

  TopoDS_Compound InteractingFaces;
  TopTools_IndexedMapOfShape InteractingFacesMap;
  BRep_Builder().MakeCompound(InteractingFaces);

  // Accumulate all faces whose boundaries must be split
  TEdgeInteractions edgeInters;
  for ( int p_idx = 1; p_idx <= InterPts.Length(); ++p_idx )
  {
    const geom_intersection_point_edges& p = InterPts(p_idx);
    TInterPt inter(p.P, p.W2, p.Face); // W2 is reserved for face boundary

    if ( p.E2.IsNull() )
    {
#if defined DRAW_DEBUG
      DRAW_INITGROUP(noedge_pt)
      DRAW_POINT(p.P, noedge_pt, Draw_vert, Draw_Circle)
#endif
      continue;
    }

    if ( !InteractingFacesMap.Contains(p.Face) )
    {
      BRep_Builder().Add(InteractingFaces, p.Face);
      InteractingFacesMap.Add(p.Face);
    }

    if ( !edgeInters.Contains(p.E2) )
    {
      edgeInters.Add( p.E2, TInterPts() );
    }
    TInterPts& inters = edgeInters.ChangeFromIndex( edgeInters.FindIndex(p.E2) );

    // Search for exactly the same point to avoid duplications
    bool isDuplicate = false;
    for ( int k = 1; k <= inters.Length(); ++k )
    {
      if ( inters(k).P.Distance(inter.P) < Precision::Confusion() )
      {
        isDuplicate = true;
        break;
      }
    }
    if ( !isDuplicate )
      inters.Append(inter);
  }

  // Prepare a compound for result
  TopoDS_Compound WireComp;
  BRep_Builder().MakeCompound(WireComp);

  // Split boundaries for the interacting faces
  Handle(ShapeBuild_ReShape) Ctx = new ShapeBuild_ReShape;
  for ( TEdgeInteractions::Iterator mit(edgeInters); mit.More(); mit.Next() )
  {
    TInterPts& inters = mit.ChangeValue();

    // Sort slicers by parameter values
    NCollection_Comparator<TInterPt> Comp;
    NCollection_QuickSort<TInterPts,
                          TInterPt>::Perform( inters, Comp, 1, inters.Length() );

    // ...
    // Now have a collection of its slicer points sorted in parametric
    // direction of the host curve -> we can split
    // ...

    TopoDS_Wire Wire = this->split(mit.Key(), inters, Ctx);
    if ( Wire.IsNull() )
    {
      Journal.SendLogMessage( LogWarn(Normal) << "CC_SplitEdge.CannotSplit" << mit.Key() );
      continue;
    }

    // Save modification
    Ctx->Replace(mit.Key(), Wire);

    // Add to result
    BRep_Builder().Add(WireComp, Wire);
  }

  // Compound for the new faces
  TopoDS_Compound NewFaces;
  BRep_Builder().MakeCompound(NewFaces);

  // Process each face and propagate its slicers
  Handle(ShapeBuild_ReShape) BodyCtx = new ShapeBuild_ReShape;
  for ( TSlicerMap::Iterator it(SlicerMap); it.More(); it.Next() )
  {
    const TopoDS_Face& F = it.Key();

    // Get slicers
    Handle(TopTools_HSequenceOfShape) slicer_edges;
    if ( SlicerMap.Contains(F) )
      slicer_edges = SlicerMap.FindFromKey(F);
    else
      continue;

    // Populate result map
    if ( InteractingFacesMap.Contains(F) )
    {
      // Apply the recorded modifications
      TopoDS_Face F_new = TopoDS::Face( Ctx->Apply(F) );

      // Run shape healing
      ShapeFix_Shape ShapeHealer(F_new);
      try
      {
        ShapeHealer.Perform();
      }
      catch ( ... )
      {
        Journal.SendLogMessage( LogErr(Normal) << "CC_SplitEdge.CannotFixShape" );
      }
      F_new = TopoDS::Face( ShapeHealer.Shape() );

      // Propagate slicers
      ResultSlicerMap.Add(F_new, slicer_edges);

      // Store new face in compound
      BRep_Builder().Add(NewFaces, F_new);

      // Record modification for body
      BodyCtx->Replace(F, F_new);
    }
    else
    {
      ResultSlicerMap.Add(F, slicer_edges);

      // Store new face in compound
      BRep_Builder().Add(NewFaces, F);
    }
  }

  ResultBody = BodyCtx->Apply(OriginalBody);

#if defined DRAW_DEBUG
  DBRep::Set("slice_pts", WireComp);
  DBRep::Set("slice_faces", NewFaces);
#endif

  // Set final result
  Result = NewFaces;
}
