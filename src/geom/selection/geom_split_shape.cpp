//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_split_shape.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <ShapeFix_IntersectionTool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Constructor accepting inaccuracy of the input geometry.
//! \param inaccuracy [in] inaccuracy to set.
geom_split_shape::geom_split_shape(const double inaccuracy)
: m_fInaccuracy(inaccuracy)
{}

//! Splits the passed edge by the given interaction points.
//! \param E   [in]     edge to split.
//! \param pts [in]     slicer points.
//! \param ctx [in/out] re-shaping context.
//! \return wire of pieces.
TopoDS_Wire geom_split_shape::split(const TopoDS_Edge&                E,
                                    const TInterPts&                  pts,
                                    const Handle(ShapeBuild_ReShape)& ctx) const
{
  // Prepare tools
  const double prec = 1.0e-3;

#if defined DRAW_DEBUG
  static int eslice_count = 0; eslice_count++;
  TCollection_AsciiString __debug_draw_BaseName("eslice");
  __debug_draw_BaseName += eslice_count;

  TopoDS_Compound SlicePts;
  BRep_Builder().MakeCompound(SlicePts);
  BRep_Builder().Add(SlicePts, E);

  for ( int k = 1; k <= pts.Length(); ++k )
  {
    BRep_Builder().Add( SlicePts, BRepBuilderAPI_MakeVertex(pts(k).P) );
  }

  DBRep::Set(__debug_draw_BaseName.ToCString(), SlicePts);
#endif

  // Splitting tool
  ShapeFix_IntersectionTool InterTool(ctx, prec, m_fInaccuracy);

  // Collection of edges slices
  Handle(TopTools_HSequenceOfShape) edges = new TopTools_HSequenceOfShape;

  // Loop over the sorted collection of interaction points
  TopoDS_Edge next2Split = E;
  for ( int p_idx = 1; p_idx <= pts.Length(); ++p_idx )
  {
    const TInterPt& inter = pts(p_idx);

    TopoDS_Edge E1;
    TopoDS_Vertex V = BRepBuilderAPI_MakeVertex(inter.P);

    // Split
    if ( next2Split.Orientation() == TopAbs_FORWARD )
    {
      if ( !InterTool.SplitEdge(next2Split, inter.W, V, inter.F, E1, next2Split, prec) )
        continue; // Let's try another vertex
    }
    else
    {
      if ( !InterTool.SplitEdge(next2Split, inter.W, V, inter.F, next2Split, E1, prec) )
        continue; // Let's try another vertex
    }

    // Add next ready-to-use slice
    edges->Append(E1);
  }

  if ( edges->Length() )
    edges->Append(next2Split);
  else
    return TopoDS_Wire();

  // Create a wire
  Handle(TopTools_HSequenceOfShape) wires;
  ShapeAnalysis_FreeBounds::ConnectEdgesToWires(edges, prec, 0, wires);
  if ( wires.IsNull() || wires->Length() != 1 )
    Standard_ProgramError::Raise("Unexpected connectivity"); // TODO: be exception-safe

  // Return the resulting wire
  return TopoDS::Wire( wires->Value(1) );
}

//! This routine gets rid of geometrically coincident but topologically
//! distinct extremities in the given wire.
//! \param mosaic     [in/out] contour to process.
//! \param face       [in/out] face to process.
//! \param inaccuracy [in]     inaccuracy to use.
//! \param ctx        [in/out] re-shaping context.
void geom_split_shape::mergeCoincidentVertices(TopoDS_Shape&                     mosaic,
                                               TopoDS_Face&                      face,
                                               const double                      inaccuracy,
                                               const Handle(ShapeBuild_ReShape)& ctx) const
{
  t_interactions interactions;

  // Accumulate all interactions
  for ( TopExp_Explorer exp(mosaic, TopAbs_VERTEX); exp.More(); exp.Next() )
  {
    const TopoDS_Vertex& V_curr = TopoDS::Vertex( exp.Current() );
    interactions.__add(V_curr, inaccuracy);
  }

  // Now collect requests for merging interactions
  for ( int tuple_idx = 1; tuple_idx <= interactions.List.Length(); ++tuple_idx )
  {
    const NCollection_Sequence<TopoDS_Vertex>& tuple = interactions.List(tuple_idx);

    const int nVertices = tuple.Length();
    if ( nVertices == 1 )
      continue;

    // Calculate average vertex
    gp_XYZ Pm;
    for ( int k = 1; k <= nVertices; ++k )
    {
      gp_XYZ Pv = BRep_Tool::Pnt( tuple(k) ).XYZ();
      Pm += Pv;
    }
    Pm /= nVertices;

    // Build vertex
    TopoDS_Vertex Vm = BRepBuilderAPI_MakeVertex(Pm);
    BRep_Builder().UpdateVertex(Vm, inaccuracy);

    // Replace
    for ( int k = 1; k <= nVertices; ++k )
    {
      const TopoDS_Vertex& Old = tuple(k);
      ctx->Replace( Old, Vm.Oriented( Old.Orientation() ) );
    }
  }

  // Apply merging requests
  mosaic = ctx->Apply(mosaic);
  face   = TopoDS::Face( ctx->Apply(face) );
}
