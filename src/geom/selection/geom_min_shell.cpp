//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_min_shell.h>

// Geometry includes
#include <geom_intersect_AABBs.h>
#include <geom_classify_edge_face.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Constructor.
//! \param inaccuracy [in] input inaccuracy.
geom_min_shell::geom_min_shell(const double inaccuracy)
: m_fInaccuracy(inaccuracy)
{}

//! Extracts min shell which is tentative for the passed contour on the
//! given body.
//! \param Body    [in]     shell with immersed wire.
//! \param Contour [in]     wire representing the contour.
//! \param Result  [out]    resulting shell.
//! \param Journal [in/out] journal instance.
//! \return true in case of success, false -- otherwise.
bool geom_min_shell::operator()(const TopoDS_Shape&  Body,
                                const TopoDS_Shape&  Contour,
                                TopoDS_Shell&        Result,
                                ActAPI_ProgressEntry Journal)
{
  /* ================================================
   *  The first filtering is coarse: make up a shell
   *  basing on criterion of intersecting AABBs
   * ================================================ */

  // Prepare structures and tools
  TopoDS_Shell CoarseShell;
  BRep_Builder().MakeShell(CoarseShell);
  geom_intersect_AABBs AABBx(m_fInaccuracy);
  Bnd_Box BndContour;
  NCollection_Sequence<TopoDS_Face> Faces;

  // Assemble minimal shell and precise boundary boxes for all tentative faces
  for ( TopExp_Explorer exp(Body, TopAbs_FACE); exp.More(); exp.Next() )
  {
    const TopoDS_Face& F = TopoDS::Face( exp.Current() );

    Bnd_Box BndFace;
    if ( AABBx(F, Contour, BndFace, BndContour) )
    {
      BRep_Builder().Add(CoarseShell, F);
      Faces.Append(F);
    }
  }

#if defined DRAW_DEBUG
  DBRep::Set("coarse_shell", CoarseShell);
#endif

  /* ===================================================
   *  Now find just one face which is tentative for ANY
   *  edge of the contour
   * =================================================== */

  // Take any edge
  TopExp_Explorer c_exp(Contour, TopAbs_EDGE);
  const TopoDS_Edge& anyEdge = TopoDS::Edge( c_exp.Current() );

#if defined DRAW_DEBUG
  DBRep::Set("MS_seed_edge", anyEdge);
#endif

  // Loop over the tentative faces to find one which contains the edge
  TopoDS_Face startF;
  for ( TopExp_Explorer exp(CoarseShell, TopAbs_FACE); exp.More(); exp.Next() )
  {
    const TopoDS_Face& F = TopoDS::Face( exp.Current() );

    // Prepare classifier
    geom_classify_edge_face ClassifyEF(F, m_fInaccuracy);

    // Classify
    geom_intersection_points_edges transitions;
    const geom_membership msp = ClassifyEF(anyEdge, transitions, Journal);
    if ( msp & Membership_InOnComp )
    {
      startF = F;
      break;
    }
  }

  // Check if we have a seed face now
  if ( startF.IsNull() )
  {
    Journal.SendLogMessage( LogWarn(Normal) << "geom_min_shell.CannotFindSeedFace" );
    Result = CoarseShell; // Let us set the best min shell we have as a result
    return false;
  }

#if defined DRAW_DEBUG
  DBRep::Set("MS_seed_face", startF);
#endif

  /* ====================================================
   *  Now traverse by neighbor faces forming a min shell
   * ==================================================== */

  // Get initial collection of blocked edges
  TopTools_IndexedMapOfShape BlockedEdges, BlockedFaces;

  // Get parent-child relationships
  TopTools_IndexedDataMapOfShapeListOfShape M;
  TopExp::MapShapesAndAncestors(CoarseShell, TopAbs_EDGE, TopAbs_FACE, M);

  // Gather unblocked faces
  int f_idx = 1;
  TopTools_IndexedMapOfShape ResultFaces;
  this->traverse(startF, M, f_idx, BlockedEdges, BlockedFaces, ResultFaces);

  /* ========================================
   *  Build a shell from the traversed faces
   * ======================================== */

  TopoDS_Shell ResultShell;
  BRep_Builder().MakeShell(ResultShell);
  ResultShell.Closed(0);

  for ( int i = 1; i <= ResultFaces.Extent(); ++i )
  {
    TopoDS_Face F = TopoDS::Face( ResultFaces.FindKey(i) );
    BRep_Builder().Add(ResultShell, F);
  }

#if defined DRAW_DEBUG
  DBRep::Set("MS_result_shell", ResultShell);
#endif

  // Make a deep copy in order to detach ourselves from the input model
  Result = TopoDS::Shell( BRepBuilderAPI_Copy(ResultShell) );

#if defined DRAW_DEBUG
  DBRep::Set("MS_result_shell_copy", Result);
#endif

  return true;
}

//! Traverses over the neighbor faces of the given face (the "seed" one)
//! accumulating those sharing some edges..
//! \param SeedFace          [in]     face to start from.
//! \param M                 [in]     parent-child relationship.
//! \param faceIdx           [in/out] face traversal index.
//! \param BlockedEdges      [in/out] edges to skip when deciding which face to go next.
//! \param BlockedFaces      [in/out] already traversed faces.
//! \param Result            [out]    traversed faces.
void geom_min_shell::traverse(const TopoDS_Face&                               SeedFace,
                              const TopTools_IndexedDataMapOfShapeListOfShape& M,
                              int&                                             faceIdx,
                              TopTools_IndexedMapOfShape&                      BlockedEdges,
                              TopTools_IndexedMapOfShape&                      BlockedFaces,
                              TopTools_IndexedMapOfShape&                      Result)
{
  Result.Add(SeedFace);
  BlockedFaces.Add(SeedFace);

#if defined DRAW_DEBUG
  TCollection_AsciiString nam("MS_"); nam += faceIdx;
  DBRep::Set(nam.ToCString(), SeedFace);
#endif

  // Traverse
  int e_idx = 1;
  for ( TopExp_Explorer exp(SeedFace, TopAbs_EDGE); exp.More(); exp.Next(), ++e_idx )
  {
    const TopoDS_Edge& E = TopoDS::Edge( exp.Current() );

    if ( BlockedEdges.Contains(E) )
      continue;

    BlockedEdges.Add(E);

    const TopTools_ListOfShape& FaceList = M.FindFromKey(E);
    for ( TopTools_ListIteratorOfListOfShape lit(FaceList); lit.More(); lit.Next() )
    {
      const TopoDS_Face& Fnext = TopoDS::Face( lit.Value() );
      if ( BlockedFaces.Contains(Fnext) )
        continue;

      BlockedFaces.Add(Fnext);
      ++faceIdx;

      // Continue recursively
      this->traverse(Fnext, M, faceIdx, BlockedEdges, BlockedFaces, Result);
    }
  }
}
