//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_pick_shell.h>

// Geometry includes
#include <geom_intersect_AABBs.h>
#include <geom_classify_point_face.h>
#include <geom_project_point_curve.h>

// OCCT includes
#include <BOPAlgo_CheckerSI.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_MapOfPassKey.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBndLib.hxx>
#include <BRepGProp.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//-----------------------------------------------------------------------------
// Class-level API
//-----------------------------------------------------------------------------

gp_Pnt geom_pick_shell::MidPoint(const TopoDS_Shape& Contour,
                                 const double        prec)
{
  // Compute contour's center using comprehensive series of points to
  // obtain a precise result
  gp_XYZ Pm_contour;
  int nContourVerts = 0;
  for ( TopExp_Explorer exp(Contour, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    const TopoDS_Edge& E = TopoDS::Edge( exp.Current() );
    BRepAdaptor_Curve bac(E);
    GCPnts_UniformAbscissa Absc(bac, prec);
    for ( int p = 1; p <= Absc.NbPoints(); ++p )
    {
      Pm_contour += bac.Value( Absc.Parameter(p) ).XYZ();
      ++nContourVerts;
    }
  }
  if ( nContourVerts )
    Pm_contour /= nContourVerts;

#if defined DRAW_DEBUG
  DRAW_INITGROUP(geom_pick_shell_P)
  DRAW_POINT(Pm_contour, geom_pick_shell_P, Draw_blanc, Draw_Plus)
#endif

  return Pm_contour;
}

//-----------------------------------------------------------------------------
// Object-level API
//-----------------------------------------------------------------------------

//! Constructor.
//! \param overlapping_prec [in] precision to check overlapping between
//!                              curves.
//! \param inaccuracy       [in] inaccuracy of the input geometry.
geom_pick_shell::geom_pick_shell(const double overlapping_prec,
                                 const double inaccuracy)
: m_fInaccuracy(inaccuracy)
{
  m_overlapping.prec       = overlapping_prec;
  m_overlapping.height_max = m_fInaccuracy*200;
  m_overlapping.laid_max   = m_overlapping.prec*10;

#if defined DRAW_DEBUG
  std::cout << "\t>>> Overlapping (precision, H max, L max) = ("
            << m_overlapping.prec << ", "
            << m_overlapping.height_max << ", "
            << m_overlapping.laid_max << ")" << std::endl;
#endif
}

//! Performs shell capturing.
//! \param Body    [in]     shell with immersed wire.
//! \param Contour [in]     wire representing the contour.
//! \param Result  [out]    resulting shell.
//! \param Journal [in/out] journal instance.
void geom_pick_shell::operator()(const TopoDS_Shape&  Body,
                                 const TopoDS_Shape&  Contour,
                                 TopoDS_Shell&        Result,
                                 ActAPI_ProgressEntry Journal)
{
  gp_Pnt Pm_contour = MidPoint(Contour, m_fInaccuracy*100);
  this->operator()(Body, Contour, Pm_contour, Result, Journal);
}

//! Performs shell capturing.
//! \param Body    [in]     shell with immersed wire.
//! \param Contour [in]     wire representing the contour.
//! \param MidPt   [in]     middle point to start from.
//! \param Result  [out]    resulting shell.
//! \param Journal [in/out] journal instance.
void geom_pick_shell::operator()(const TopoDS_Shape&  Body,
                                 const TopoDS_Shape&  Contour,
                                 const gp_Pnt&        MidPt,
                                 TopoDS_Shell&        Result,
                                 ActAPI_ProgressEntry Journal)
{
  /* =================================================
   *  Assemble minimal shell from the tentative faces
   * ================================================= */

  // Prepare structures and tools
  TopoDS_Shell MinShell;
  BRep_Builder().MakeShell(MinShell);
  geom_intersect_AABBs AABBx( Precision::Confusion() );
  Bnd_Box BndContour;
  NCollection_Sequence<TopoDS_Face> Faces;
  NCollection_Sequence<Bnd_Box> FaceBoxes;

  // Mesh body in order to have precise bounding boxes
  try
  {
    BRepMesh_IncrementalMesh Mesh(Body, m_fInaccuracy*1000);
  }
  catch ( ... )
  {
    Journal.SendLogMessage( LogErr(Normal) << "geom_pick_shell.AABB.CannotMeshBody" );
    return;
  }

  // Assemble minimal shell and precise boundary boxes for all tentative faces
  for ( TopExp_Explorer exp(Body, TopAbs_FACE); exp.More(); exp.Next() )
  {
    const TopoDS_Face& F = TopoDS::Face( exp.Current() );

    Bnd_Box BndFace;
    if ( AABBx(F, Contour, BndFace, BndContour) )
    {
      BRep_Builder().Add(MinShell, F);
      Faces.Append(F);
      FaceBoxes.Append(BndFace);
    }
  }

#if defined DRAW_DEBUG
  DBRep::Set("pick_min_shell", MinShell);
#endif

  /* =================================
   *  Find "most nested" bounding box
   * ================================= */

#if defined DRAW_DEBUG
  DRAW_INITGROUP(geom_pick_shell_ContourBox)
  if ( !BndContour.IsVoid() )
  {
    DRAW_BOX(BndContour, geom_pick_shell_ContourBox, Draw_kaki)
  }
#endif

  // ...
  // Choose the face which "contains" (accurate to distance) the contour's
  // middle point
  // ...

  // Find the closest "containing" face
  TopoDS_Face Fm;
  NCollection_Sequence<TopoDS_Face> FacesInOn;
  NCollection_Sequence<Bnd_Box> FaceBoxesInOn;
  for ( int k = 1; k <= Faces.Length(); ++k )
  {
    geom_classify_point_face ClassifyPF(Faces(k), m_overlapping.prec, m_overlapping.prec);
    const geom_membership mbs = ClassifyPF(MidPt, false, NULL);

    if ( mbs & Membership_InOn )
    {
      FacesInOn.Append( Faces(k) );
      FaceBoxesInOn.Append( FaceBoxes(k) );
    }
  }
  if ( FacesInOn.Length() )
    Fm = this->chooseByBoundingMiddle(FacesInOn, FaceBoxesInOn, MidPt);

  // If there is nothing "containing", just find the closest face
  if ( Fm.IsNull() )
    Fm = this->chooseByBoundingMiddle(Faces, FaceBoxes, MidPt);

  if ( Fm.IsNull() )
  {
    Journal.SendLogMessage( LogWarn(Normal) << "geom_pick_shell.CannotFindSeedFace" );
    return;
  }

#if defined DRAW_DEBUG
  DBRep::Set("Fm", Fm);
#endif

  /* =============================
   *  Traverse all neighbor faces
   * ============================= */

  // Get initial collection of blocked edges
  TopTools_IndexedMapOfShape BlockedEdges, BlockedFaces;
  int e_idx = 0;
  for ( TopExp_Explorer exp(Contour, TopAbs_EDGE); exp.More(); exp.Next(), ++e_idx )
  {
    const TopoDS_Edge& E = TopoDS::Edge( exp.Current() );

    // Populate map
    BlockedEdges.Add(E);

#if defined DRAW_DEBUG
    std::cout << "Contour edge #" << e_idx << " ("
              << geom_utils::ShapeAddr( exp.Current() ).ToCString()
              << ") is bounding"
              << std::endl;
#endif
  }

  // Get parent-child relationships
  TopTools_IndexedDataMapOfShapeListOfShape M;
  TopExp::MapShapesAndAncestors(MinShell, TopAbs_EDGE, TopAbs_FACE, M);

  // Gather unblocked faces
  int f_idx = 1;
  TopTools_IndexedMapOfShape ResultFaces;
  NCollection_DataMap<TopoDS_Edge, geom_edge_edge_dist> ExtremaTools;
  this->traverse(Fm, M, f_idx, BlockedEdges, BlockedFaces, ResultFaces, ExtremaTools);

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

  Result = ResultShell;
}

//! Traverses over the neighbor faces of the given face (the "seed" one)
//! accumulating those lying inside the active contour. Such filtering is
//! organized simply by not visiting those faces sharing the blocked edges
//! passed as an argument.
//! \param SeedFace     [in]     face to start from.
//! \param M            [in]     parent-child relationship.
//! \param faceIdx      [in/out] face traversal index.
//! \param BlockedEdges [in/out] edges to skip when deciding which face to go next.
//! \param BlockedFaces [in/out] already traversed faces.
//! \param Result       [out]    traversed faces.
//! \param ExtremaTools [in/out] acceleration structure for extrema.
void geom_pick_shell::traverse(const TopoDS_Face&                                  SeedFace,
                            const TopTools_IndexedDataMapOfShapeListOfShape&       M,
                            int&                                                   faceIdx,
                            TopTools_IndexedMapOfShape&                            BlockedEdges,
                            TopTools_IndexedMapOfShape&                            BlockedFaces,
                            TopTools_IndexedMapOfShape&                            Result,
                            NCollection_DataMap<TopoDS_Edge, geom_edge_edge_dist>& ExtremaTools)
{
  BlockedFaces.Add(SeedFace);
  Result.Add(SeedFace);

#if defined DRAW_DEBUG
  const int seed_face_idx = faceIdx;
  TCollection_AsciiString nam("Fn_"); nam += faceIdx;
  DBRep::Set(nam.ToCString(), SeedFace);
#endif

  // Traverse
  int e_idx = 1;
  for ( TopExp_Explorer exp(SeedFace, TopAbs_EDGE); exp.More(); exp.Next(), ++e_idx )
  {
    const TopoDS_Edge& E = TopoDS::Edge( exp.Current() );

    if ( BlockedEdges.Contains(E) )
    {
      continue;
    }
    else
    {
      // One "hack" is to prohibit walking by very small edges which may be
      // covered by tolerance and thus not necessarily present in the list
      // of bounding edges

      GProp_GProps props;
      BRepGProp::LinearProperties(E, props);
      if ( props.Mass() < m_fInaccuracy*10 )
      {
        BlockedEdges.Add(E); // Do not allow such edge again
        continue;
      }

      // Another "hack" is to check that the allowed (not blocked) edge
      // occupies nearly the same position in 3D. The latter singularity may
      // happen if the source body was not really split by the passed contour.
      // Such situation looks abnormal but sometimes it may happen in absolutely
      // legal way. E.g. if the contour wire was originally overlapping
      // with some input edge of the shell, the corresponding piece of that
      // contour wire may not be present in the immersed shape

      geom_edge_edge_dist EEDist;
      if ( ExtremaTools.IsBound(E) )
        EEDist = ExtremaTools.Find(E);
      else
      {
        EEDist.Init(E, m_overlapping.prec);
        ExtremaTools.Bind(E, EEDist);
      }

      // Compare discrete representations
      bool isOverlapping = false;
      for ( int e_idx = 1; e_idx <= BlockedEdges.Extent(); ++e_idx )
      {
        const TopoDS_Edge& E_blocked = TopoDS::Edge( BlockedEdges.FindKey(e_idx) );

        // Check distance between edges
        bool hasOverlapping = false;
        double ee_dist = RealLast();
        if ( !EEDist(E_blocked, true, ee_dist, m_overlapping.height_max, m_overlapping.laid_max, hasOverlapping, NULL) )
          continue; // Something gone wrong, e.g. the blocking edge does not possess curve

        if ( hasOverlapping && ee_dist < m_overlapping.height_max )
        {
#if defined DRAW_DEBUG
          std::cout << "Overlapping edges have been detected (distance = " << ee_dist << ")" << std::endl;
          TCollection_AsciiString ovl_nam("overlap_f"); ovl_nam += seed_face_idx; ovl_nam += "_e"; ovl_nam += e_idx;
          TopoDS_Compound OvlComp;
          BRep_Builder().MakeCompound(OvlComp);
          BRep_Builder().Add(OvlComp, E_blocked);
          BRep_Builder().Add(OvlComp, E);
          DBRep::Set(ovl_nam.ToCString(), OvlComp);
#endif

          isOverlapping = true;
          break;
        }
      }

      // Overlapping with blocking means blocking
      if ( isOverlapping )
      {
#if defined DRAW_DEBUG
        std::cout << "Traversed edge ("
                  << geom_utils::ShapeAddr(E).ToCString()
                  << ") is overlapping -> set as bounding"
                  << std::endl;
#endif
        BlockedEdges.Add(E);
        continue;
      }

    }

    if ( !BlockedEdges.Contains(E) )
    {
      const TopTools_ListOfShape& FaceList = M.FindFromKey(E);
      for ( TopTools_ListIteratorOfListOfShape lit(FaceList); lit.More(); lit.Next() )
      {
        const TopoDS_Face& Fnext = TopoDS::Face( lit.Value() );
        if ( BlockedFaces.Contains(Fnext) )
          continue;

        faceIdx++;

#if defined DRAW_DEBUG
        std::cout << "*** Face " << faceIdx << " generated from edge "
                  << geom_utils::ShapeAddr(E).ToCString() << std::endl;
#endif

        // Continue recursively
        this->traverse(Fnext, M, faceIdx, BlockedEdges, BlockedFaces, Result, ExtremaTools);
      }

      BlockedEdges.Add(E);
    }
  }
}

//! Returns max tolerance per vertices. It is enough to limit ourselves
//! with vertices as according to the tolerance rule, we cannot have greater
//! values on edges and faces.
//! \param shape [in] input shape.
//! \return max tolerance.
double geom_pick_shell::maxTol(const TopoDS_Shape& shape) const
{
  double aResult = 0.0;
  for ( TopExp_Explorer exp(shape, TopAbs_VERTEX); exp.More(); exp.Next() )
  {
    aResult = Max(BRep_Tool::Tolerance( TopoDS::Vertex( exp.Current() ) ), aResult);
  }
  return aResult;
}

//! Selects a face by criterion of bounding box midpoint closeness to the
//! passed point.
//! \param Faces     [in] source faces.
//! \param FaceBoxes [in] boundary boxes of the faces.
//! \param MidPt     [in] midpoint.
//! \return selected face.
TopoDS_Face
  geom_pick_shell::chooseByBoundingMiddle(const NCollection_Sequence<TopoDS_Face>& Faces,
                                          const NCollection_Sequence<Bnd_Box>&     FaceBoxes,
                                          const gp_Pnt&                            MidPt) const
{
  TopoDS_Face Fm;

  // If no tentative face found, let's choose the one whose middle point
  // is closest to the middle of the contour
  double min_dist = RealLast();
  for ( int k = 1; k <= Faces.Length(); ++k )
  {
    const Bnd_Box& BF = FaceBoxes(k);
    gp_Pnt F_midPt = 0.5*( BF.CornerMin().XYZ() + BF.CornerMax().XYZ() );
    const double d = F_midPt.Distance(MidPt);

#if defined DRAW_DEBUG
    DRAW_INITGROUP(F_midPt)
    DRAW_POINT(F_midPt, F_midPt, Draw_vert, Draw_Square)
#endif

    if ( d < min_dist )
    {
      min_dist = d;
      Fm       = Faces(k);
    }
  }

  return Fm;
}
