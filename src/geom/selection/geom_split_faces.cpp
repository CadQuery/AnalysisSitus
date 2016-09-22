//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_split_faces.h>

// Geometry includes
#include <geom_check_contour.h>
#include <geom_classify_point_face_N_IP.h>
#include <geom_epsilon_empty.h>
#include <geom_epsilon_face_interior_full.h>
#include <geom_epsilon_face_interior_hemi.h>
#include <geom_epsilon_face_interior_sector.h>
#include <geom_project_vertex_edge.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepFeat_SplitShape.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeFix_Shape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//-----------------------------------------------------------------------------
// Class-level API
//-----------------------------------------------------------------------------

//! Extracts the image of the given slicer edges which is already immersed to
//! the working body.
//! \param Body       [in] immersed body.
//! \param Slicers    [in] slicer edges to find images for.
//! \param inaccuracy [in] driving inaccuracy.
//! \return immersed contour.
TopoDS_Shape
  geom_split_faces::ExtractImmersedContour(const TopoDS_Shape&                      Body,
                                           const Handle(TopTools_HSequenceOfShape)& Slicers,
                                           const double                             inaccuracy)
{
  TopoDS_Compound res;
  BRep_Builder().MakeCompound(res);

  for ( TopExp_Explorer exp(Body, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    const TopoDS_Edge& E_body = TopoDS::Edge( exp.Current() );

    for ( int slicer_idx = 1; slicer_idx <= Slicers->Length(); ++slicer_idx )
    {
      const TopoDS_Edge& E_slicer = TopoDS::Edge( Slicers->Value(slicer_idx) );

      if ( t_interactions::__areCoincident(E_body, E_slicer, inaccuracy) )
        BRep_Builder().Add(res, E_body);
    }
  }

  return res;
}

//-----------------------------------------------------------------------------
// Object-level API
//-----------------------------------------------------------------------------

//! Constructor.
//! \param inaccuracy [in] inaccuracy of the input geometry.
geom_split_faces::geom_split_faces(const double inaccuracy)
: geom_split_shape(inaccuracy)
{
}

//! Performs splitting of faces.
//! \param Slicers         [in]     faces with slice edges.
//! \param PreparedFaces   [in]     faces with pre-split boundaries.
//! \param OriginalBody    [in]     original body.
//! \param ImmersedBody    [out]    result body.
//! \param ImmersedContour [out]    result contour.
//! \param Journal         [in/out] journal instance.
//! \return true in case of success, false -- otherwise.
bool geom_split_faces::operator()(const TSlicerMap&      Slicers,
                                  const TopoDS_Compound& PreparedFaces,
                                  const TopoDS_Shape&    OriginalBody,
                                  TopoDS_Shape&          ImmersedBody,
                                  TopoDS_Shape&          ImmersedContour,
                                  ActAPI_ProgressEntry   Journal) const
{
  // Modification tool
  Handle(ShapeBuild_ReShape) ctx = new ShapeBuild_ReShape;

  // Slicer edges
  Handle(TopTools_HSequenceOfShape) SlicerEdges = new TopTools_HSequenceOfShape;

  // Loop over the target faces
  int fidx = 0;
  for ( TopoDS_Iterator fit(PreparedFaces); fit.More(); fit.Next(), ++fidx )
  {
    TopoDS_Face F      = TopoDS::Face( fit.Value() );
    TopoDS_Face F_copy = TopoDS::Face( BRepBuilderAPI_Copy(F) );

    if ( !Slicers.Contains(F) )
    {
      Journal.SendLogMessage( LogWarn(Normal) << "geom_split_faces.NoSlicersForFace" << F );
      continue;
    }

    // Split copy of the face
    TopoDS_Shell part_body;
    bool isNested = false;
    if ( !this->Split(F_copy, Slicers.FindFromKey(F), part_body, SlicerEdges, isNested, Journal) )
    {
      Journal.SendLogMessage( LogWarn(Normal) << "geom_split_faces.CannotSplitFace" << F );
      return false;
    }

    // Record modification
    ctx->Replace(F, part_body);
  }

  // Apply modification
  TopoDS_Shape newBody = ctx->Apply(OriginalBody);

#if defined DRAW_DEBUG
  DBRep::Set("presplit", newBody);
#endif

  // Get number of faces
  TopTools_IndexedMapOfShape newBodyFaces;
  TopExp::MapShapes(newBody, TopAbs_FACE, newBodyFaces);
  const int nFaces = newBodyFaces.Extent();

  // Apply sewing
  if ( nFaces > 1 )
    this->sew(newBody);

  // Run shape healing
  ShapeFix_Shape ShapeHealer(newBody);
  ShapeHealer.FixWireTool()->FixDegeneratedMode() = 0;
  try
  {
    ShapeHealer.Perform();
  }
  catch ( ... )
  {
    Journal.SendLogMessage( LogWarn(Normal) << "geom_split_faces.CannotFixShape" );
  }
  newBody = ShapeHealer.Shape();

  // Apply sewing again (after SHG)
  if ( nFaces > 1 )
    this->sew(newBody);

#if defined DRAW_DEBUG
  DBRep::Set("split", newBody);

  TopoDS_Compound SlicerEdgesComp;
  BRep_Builder().MakeCompound(SlicerEdgesComp);
  for ( int i = 1; i <= SlicerEdges->Length(); ++i )
  {
    BRep_Builder().Add( SlicerEdgesComp, SlicerEdges->Value(i) );
  }
  DBRep::Set("slicer_edges_comp", SlicerEdgesComp);
#endif

  ImmersedBody = newBody;
  ImmersedContour = ExtractImmersedContour(ImmersedBody, SlicerEdges, m_fInaccuracy);

  BRepCheck_Analyzer Check(ImmersedBody, 1);
  const bool isValid = Check.IsValid();
  if ( !isValid )
  {
    Journal.SendLogMessage( LogWarn(Normal) << "geom_split_faces.ResultIsInvalidShell" << ImmersedBody );
  }

  return isValid;
}

//! Splits the passed face by the given slice edges.
//! \param F               [in/out] face to split.
//! \param edges           [in]     slicer edges.
//! \param ImmersedBody    [out]    resulting faces.
//! \param ImmersedEdges   [out]    immersed edges.
//! \param isNested        [out]    indicates whether the split function
//!                                 results with a pure nested contour.
//! \param Journal         [in/out] journal instance.
//! \param pInterrupt      [in]     auxiliary callback.
//! \return true in case of success, false -- otherwise.
bool geom_split_faces::Split(TopoDS_Face&                             F,
                             const Handle(TopTools_HSequenceOfShape)& edges,
                             TopoDS_Shell&                            ImmersedBody,
                             Handle(TopTools_HSequenceOfShape)&       ImmersedEdges,
                             bool&                                    isNested,
                             ActAPI_ProgressEntry                     Journal,
                             geom_split_faces_interrupt               pInterrupt) const
{
#if defined DRAW_DEBUG
  static int split_shape_count = 0; ++split_shape_count;
  TCollection_AsciiString nam_obj("split_obj_"); nam_obj += split_shape_count;

  TopoDS_Compound Obj;
  BRep_Builder().MakeCompound(Obj);
  BRep_Builder().Add(Obj, F);

  for ( int e_idx = 1; e_idx <= edges->Length(); ++e_idx )
  {
    BRep_Builder().Add( Obj, edges->Value(e_idx) );
  }

  DBRep::Set(nam_obj.ToCString(), Obj);
#endif

  /* =================================
   *  Get p-curves and assemble wires
   * ================================= */

  Handle(TopTools_HSequenceOfShape) healed_edges = new TopTools_HSequenceOfShape;
  for ( int e_idx = 1; e_idx <= edges->Length(); ++e_idx )
  {
    TopoDS_Edge E = TopoDS::Edge( edges->Value(e_idx) );

    ShapeFix_Edge().FixAddPCurve(E, F, 0);

    healed_edges->Append(E);
  }

  // Connect healed edges to wires
  Handle(TopTools_HSequenceOfShape) slicer_wires;
  ShapeAnalysis_FreeBounds::ConnectEdgesToWires(healed_edges, m_fInaccuracy, 0, slicer_wires);

  // Create compound for the resulting contour
  TopoDS_Compound ResContour;
  BRep_Builder().MakeCompound(ResContour);

  /* ===================================================================
   *  Prepare "mosaic": a well-formed set of slice-edges and self-edges
   * =================================================================== */

  Handle(TopTools_HSequenceOfShape) all_edges = new TopTools_HSequenceOfShape;

  // Add slicer-edges
  for ( int w_idx = 1; w_idx <= slicer_wires->Length(); ++w_idx )
  {
    TopoDS_Wire wire = TopoDS::Wire( slicer_wires->Value(w_idx) );
    Handle(ShapeExtend_WireData) local_wd = new ShapeExtend_WireData(wire);

    for ( int k = 1; k <= local_wd->NbEdges(); ++k )
    {
      ImmersedEdges->Append( local_wd->Edge(k) );
      all_edges->Append( local_wd->Edge(k) );
    }
  }

  // Add self-edges
  for ( TopExp_Explorer f_exp(F, TopAbs_EDGE); f_exp.More(); f_exp.Next() )
  {
    all_edges->Append( f_exp.Current() );
  }

  // Assemble wires from the soup of edges
  Handle(TopTools_HSequenceOfShape) pre_wires;
  ShapeAnalysis_FreeBounds::ConnectEdgesToWires(all_edges, m_fInaccuracy, 0, pre_wires);

  // Assemble wires to wires
  Handle(TopTools_HSequenceOfShape) all_wires;
  ShapeAnalysis_FreeBounds::ConnectWiresToWires(pre_wires, m_fInaccuracy, 0, all_wires);

  // Put all edges into a compound thus obtaining one soup of edges
  TopoDS_Compound mosaic;
  BRep_Builder().MakeCompound(mosaic);
  for ( int w_idx = 1; w_idx <= all_wires->Length(); ++w_idx )
  {
    TopoDS_Wire wire = TopoDS::Wire( all_wires->Value(w_idx) );
    BRep_Builder().Add(mosaic, wire);
  }

  // Re-shaper
  Handle(ShapeBuild_ReShape) ctx = new ShapeBuild_ReShape;

  // Get rid of possible coincident but distinct vertices
  this->mergeCoincidentVertices(mosaic, F, m_fInaccuracy, ctx);

  /* ======================================================
   *  Check whether there are some dead ends in the mosaic
   * ====================================================== */

  // Soup of all vertices by edges in mosaic
  TopTools_IndexedDataMapOfShapeListOfShape M_all;
  TopExp::MapShapesAndAncestors(mosaic, TopAbs_VERTEX, TopAbs_EDGE, M_all);

#if defined DRAW_DEBUG
  TCollection_AsciiString nam_wire_pre("smosaic_pre_"); nam_wire_pre += split_shape_count;
  TCollection_AsciiString nam_face_pre("sface_pre_");   nam_face_pre += split_shape_count;

  DBRep::Set(nam_wire_pre.ToCString(), mosaic);
  DBRep::Set(nam_face_pre.ToCString(), F);
#endif

  // Traverse for contour
  double tolerGap = m_fInaccuracy;
  bool isMosaicOk = true;
  TopTools_ListOfShape dead_ends;
  //
  this->WalkLeft(F, M_all, M_all, true, dead_ends, Journal, pInterrupt);
  //
  if ( dead_ends.Extent() )
  {
    Journal.SendLogMessage( LogWarn(Normal) << "geom_split_faces.DeadEnds" );
    //
    if ( this->resolveDeadEnds(mosaic, F,dead_ends, m_fInaccuracy*50, tolerGap, Journal) )
    {
      Journal.SendLogMessage( LogInfo(Normal) << "geom_split_faces.DeadEndsResolved" );

      // Rebuild child-parent map
      M_all.Clear();
      TopExp::MapShapesAndAncestors(mosaic, TopAbs_VERTEX, TopAbs_EDGE, M_all);

      // Check dead ends again
      dead_ends.Clear();
      this->WalkLeft(F, M_all, M_all, true, dead_ends, Journal, pInterrupt);
      if ( dead_ends.Extent() )
      {
        Journal.SendLogMessage( LogWarn(Normal) << "geom_split_faces.CannotResolveDeadEnds" );
        isMosaicOk = false;
      }
    }
    else
    {
      Journal.SendLogMessage( LogWarn(Normal) << "geom_split_faces.CannotResolveDeadEnds" );
      isMosaicOk = false;
    }
  }

#if defined DRAW_DEBUG
  TCollection_AsciiString nam_wire("smosaic_"); nam_wire += split_shape_count;
  TCollection_AsciiString nam_face("sface_");   nam_face += split_shape_count;

  DBRep::Set(nam_wire.ToCString(), mosaic);
  DBRep::Set(nam_face.ToCString(), F);
#endif

  if ( !isMosaicOk )
    return false;

  /* ===============================================================
   *  Check whether there contour wire is nested. If so, extract it
   * =============================================================== */

  TopTools_IndexedDataMapOfShapeListOfShape M_nested;
  isNested = this->isImbrication(M_all, M_nested);
  if ( isNested )
  {
    Journal.SendLogMessage( LogInfo(Normal) << "geom_split_faces.IsolatedContour" );
    M_all = M_nested;

#if defined DRAW_DEBUG
    DRAW_INITGROUP(isolated_contour)
    for ( int i = 1; i <= M_all.Extent(); ++i )
    {
      DRAW_POINT( BRep_Tool::Pnt( TopoDS::Vertex( M_all.FindKey(i) ) ), isolated_contour, Draw_rouge, Draw_Square )
    }
#endif
  }

  /* =================================
   *  Split face by well-formed wires
   * ================================= */

  // Traverse for contour
  TopTools_ListOfShape contours;
  if ( !this->WalkLeft(F, M_all, M_all, false, contours, Journal, pInterrupt) )
  {
    Journal.SendLogMessage( LogWarn(Normal) << "geom_split_faces.NoResultAfterWalking" << mosaic << F );
    return false;
  }

  /* ========================
   *  Create resulting shell
   * ======================== */

  // Create new faces for each sub-contour
  int sc = 1;
  Handle(Geom_Surface) S = BRep_Tool::Surface(F);
  TopTools_ListOfShape faces;
  for ( TopTools_ListIteratorOfListOfShape lit(contours); lit.More(); lit.Next(), ++sc )
  {
    const TopoDS_Shape& sc_shape = lit.Value();

#if defined DRAW_DEBUG
    TCollection_AsciiString nam_subc("spavew_"); nam_subc += split_shape_count; nam_subc += "_"; nam_subc += sc;
    DBRep::Set( nam_subc.ToCString(), sc_shape );
#endif

    if ( !this->isClosedContour(sc_shape) )
      continue;

    // Make face with the new contour on the host surface
    TopoDS_Face sub_F = BRepBuilderAPI_MakeFace( S, TopoDS::Wire(sc_shape), 0 );

    // Reverse derived face if the host one was reversed
    if ( F.Orientation() == TopAbs_REVERSED )
      sub_F.Orientation(TopAbs_REVERSED);

#if defined DRAW_DEBUG
    TCollection_AsciiString nam_subf_o("spavef_original_"); nam_subf_o += split_shape_count; nam_subf_o += "_"; nam_subf_o += sc;
    DBRep::Set( nam_subf_o.ToCString(), sub_F );
#endif

    // Fix possible anomalies
    ShapeFix_Face ShapeHealer(sub_F);
    ShapeHealer.SetContext(ctx);
    ShapeHealer.FixWireTool()->FixDegeneratedMode() = 0;
    try
    {
      ShapeHealer.Perform();
    }
    catch ( ... )
    {
      Journal.SendLogMessage( LogWarn(Normal) << "geom_split_faces.CannotFixShape" );
      return false;
    }
    sub_F = ShapeHealer.Face();

    // Get rid of possible coincident but distinct vertices. In some cases
    // shape healing may insert new vertices thus breaking validity of the
    // shape instead of tolerance increase which will keep the shape valid.
    // Yes, it is valid at a price of worse quality, but still... it is valid!
    this->mergeCoincidentVertices(mosaic, sub_F, tolerGap, ctx);

    // Add face to resulting collection
    faces.Append(sub_F);

#if defined DRAW_DEBUG
    TCollection_AsciiString nam_subf("spavef_"); nam_subf += split_shape_count; nam_subf += "_"; nam_subf += sc;
    DBRep::Set( nam_subf.ToCString(), sub_F );
#endif
  }

  // Prepare resulting shell
  TopoDS_Shell res_Shell;
  BRep_Builder().MakeShell(res_Shell);

  // Keep only those faces which are not containing other ones
  for ( TopTools_ListIteratorOfListOfShape fit1(faces); fit1.More(); fit1.Next() )
  {
    const TopoDS_Face& Checked = TopoDS::Face( fit1.Value() );
    BRep_Builder().Add(res_Shell, Checked);
  }

  // Set result
  ImmersedBody = res_Shell;
  return true;
}

//! With left-turn rule and starting from each non-traversed vertex, this
//! routine walks over the passed contour which contains both slice-edges
//! and self-edges. During such walking it extracts all distinct closed
//! contours which can be then used for capturing sub-faces from the single
//! host face.
//!
//! \param F              [in]     host face.
//! \param Mosaic         [in]     mosaic of contour edges.
//! \param Contours       [out]    extracted closed contours.
//! \param Journal        [in/out] journal instance.
//! \param pInterrupt     [in]     auxiliary callback.
//! \return true in case of success, false -- otherwise.
bool geom_split_faces::WalkLeft(const TopoDS_Face&         F,
                                const TopoDS_Shape&        Mosaic,
                                TopTools_ListOfShape&      Contours,
                                ActAPI_ProgressEntry       Journal,
                                geom_split_faces_interrupt pInterrupt) const
{
  // Soup of all vertices by edges in host face (i.e. on boundary)
  TopTools_IndexedDataMapOfShapeListOfShape M_bnd;
  TopExp::MapShapesAndAncestors(F, TopAbs_VERTEX, TopAbs_EDGE, M_bnd);

  // Soup of all vertices by edges in host face (i.e. on boundary)
  TopTools_IndexedDataMapOfShapeListOfShape M_all;
  TopExp::MapShapesAndAncestors(Mosaic, TopAbs_VERTEX, TopAbs_EDGE, M_all);

  // Walk
  return this->WalkLeft(F, M_all, M_bnd, false, Contours, Journal, pInterrupt);
}

//! With left-turn rule and starting from each non-traversed vertex, this
//! routine walks over the passed contour which contains both slice-edges
//! and self-edges. During such walking it extracts all distinct closed
//! contours which can be then used for capturing sub-faces from the single
//! host face.
//!
//! \param F          [in]     host face.
//! \param Mosaic     [in]     mosaic of contour edges.
//! \param Seeds      [in]     restricted set of potential seed vertices.
//! \param Contours   [out]    extracted closed contours.
//! \param Journal    [in/out] journal instance.
//! \param pInterrupt [in]     auxiliary callback.
//! \return true in case of success, false -- otherwise.
bool geom_split_faces::WalkLeft(const TopoDS_Face&                               F,
                                const TopoDS_Shape&                              Mosaic,
                                const TopTools_IndexedDataMapOfShapeListOfShape& Seeds,
                                TopTools_ListOfShape&                            Contours,
                                ActAPI_ProgressEntry                             Journal,
                                geom_split_faces_interrupt                       pInterrupt) const
{
  // Soup of all vertices by edges in host face (i.e. on boundary)
  TopTools_IndexedDataMapOfShapeListOfShape M_all;
  TopExp::MapShapesAndAncestors(Mosaic, TopAbs_VERTEX, TopAbs_EDGE, M_all);

  // Walk
  return this->WalkLeft(F, M_all, Seeds, false, Contours, Journal, pInterrupt);
}

//! With left-turn rule and starting from each non-traversed vertex, this
//! routine walks over the passed contour which contains both slice-edges
//! and self-edges. During such walking it extracts all distinct closed
//! contours which can be then used for capturing sub-faces from the single
//! host face.
//!
//! \param F             [in]     host face.
//! \param All           [in]     all vertices and their parent edges.
//! \param Seeds         [in]     restricted set of potential seed vertices.
//! \param checkDeadEnds [in]     checks dead ends in the mosaic.
//! \param Results       [out]    extracted closed contours or dead ends.
//! \param Journal       [in/out] journal instance.
//! \param pInterrupt    [in]     auxiliary callback.
//! \return true in case of success, false -- otherwise.
bool geom_split_faces::WalkLeft(const TopoDS_Face&                               F,
                                const TopTools_IndexedDataMapOfShapeListOfShape& All,
                                const TopTools_IndexedDataMapOfShapeListOfShape& Seeds,
                                const bool                                       checkDeadEnds,
                                TopTools_ListOfShape&                            Results,
                                ActAPI_ProgressEntry                             Journal,
                                geom_split_faces_interrupt                       pInterrupt) const
{
#if defined DRAW_DEBUG
  DRAW_INITGROUP(seedv)
  DRAW_INITGROUP(seede)
#endif

  // Working variables
  t_node V_seed;
  Handle(geom_epsilon_base) eps_base;

  // Traversed edges
  TopTools_IndexedMapOfShape TraversedSeeds, TraversedEdges;

  // Start traversing from the seed which is chosen on face boundary
  // (this is a limitation, but we can compute boundary neighborhood
  // only on boundary points)
  while ( this->nextSeed(F, Seeds, TraversedSeeds, TraversedEdges, V_seed, eps_base, Journal) )
  {
    if ( pInterrupt )
      pInterrupt();

    // Add next seed
    TraversedSeeds.Add(V_seed.Opp.V);

#if defined DRAW_DEBUG
    DRAW_POINT(BRep_Tool::Pnt(V_seed.Opp.V), seedv, Draw_vert, Draw_Circle)
#endif

    // Tools and working variables
    Handle(ShapeExtend_WireData) wd = new ShapeExtend_WireData;
    t_node V = V_seed, V_prev, V_next;
    TopoDS_Edge E_next, E_seed = V_seed.Opp.E;
    bool stop = false;
    int iter = 0;
    const int max_iters = 1000;
    gp_Dir ref_norm;

    // Path composed of the vertices traversed when accumulating each sub-contour
    TopTools_IndexedMapOfShape path;

    // Traverse
    do
    {
      iter++;

      // Take previous direction
      gp_Dir prev_dir;
      bool pickNearest;
      if ( V_prev.IsNull() )
      {
        if ( eps_base.IsNull() || eps_base->IsKind( STANDARD_TYPE(geom_epsilon_empty) ) )
        {
          Journal.SendLogMessage( LogWarn(Normal) << "geom_split_faces.CannotEvaluateNeighborhoodAtSeed" << V_seed.Opp.V );
          return false;
        }

        // Interior of the edge
        if ( eps_base->IsKind( STANDARD_TYPE(geom_epsilon_face_interior_hemi) ) )
        {
          Handle(geom_epsilon_face_interior_hemi)
            eps = Handle(geom_epsilon_face_interior_hemi)::DownCast(eps_base);

          ref_norm = eps->n;
          prev_dir = eps->n.Crossed(eps->N);
        }

        // Extremity of the edge
        if ( eps_base->IsKind( STANDARD_TYPE(geom_epsilon_face_interior_sector) ) )
        {
          Handle(geom_epsilon_face_interior_sector)
            eps = Handle(geom_epsilon_face_interior_sector)::DownCast(eps_base);

          ref_norm = eps->n;
          prev_dir = eps->T2;
        }

        // Interior of the face
        if ( eps_base->IsKind( STANDARD_TYPE(geom_epsilon_face_interior_full) ) )
        {
          Handle(geom_epsilon_face_interior_full)
            eps = Handle(geom_epsilon_face_interior_full)::DownCast(eps_base);

          ref_norm = eps->n;

          // Take any direction
          prev_dir = gp::DX();
        }

        pickNearest = true;

#if defined DRAW_DEBUG
        DRAW_INITGROUP(geom_split_faces_SeedT)
        DRAW_LINE3D(BRep_Tool::Pnt(V.Opp.V), gp_Pnt(BRep_Tool::Pnt(V.Opp.V).XYZ() + prev_dir.XYZ()*100),
                    geom_split_faces_SeedT, Draw_magenta)
#endif
      }
      else
      {
        gp_XYZ v_prev_XYZ = BRep_Tool::Pnt(V_prev.Opp.V).XYZ();
        gp_XYZ v_XYZ      = BRep_Tool::Pnt(V.Opp.V).XYZ();

        prev_dir = v_XYZ - v_prev_XYZ;
        pickNearest = false;

#if defined DRAW_DEBUG
        DRAW_INITGROUP(geom_split_faces_SeedT)
        DRAW_LINE3D(BRep_Tool::Pnt(V.Opp.V), gp_Pnt(BRep_Tool::Pnt(V.Opp.V).XYZ() + prev_dir.XYZ()*100),
                    geom_split_faces_SeedT, Draw_magenta)
#endif
      }

      // Pick up the next edge
      if ( !this->nextEdge(F, V, V_prev, prev_dir, ref_norm, All, pickNearest, E_next, V_next) )
      {
        if ( checkDeadEnds )
        {
          Results.Append(V.Opp.V);
        }

        stop = true;
        Journal.SendLogMessage( LogWarn(Normal) << "geom_split_faces.CannotCompleteContour" );
        continue; // Let it finish the loop
      }

      // Set seed to the first edge for further check
      // (we should not return by the same edge)
      if ( E_seed.IsNull() )
        E_seed = E_next;

      // Add next edge to the sub-contour
      wd->Add(E_next);
      TraversedEdges.Add(E_next);

#if defined DRAW_DEBUG
      DRAW_EDGE(E_next, seede, Draw_vert)
#endif

      // Check if we have reached the beginning
      if ( t_interactions::__areCoincident(V_next.Opp.V, V_seed.Opp.V, m_fInaccuracy) )
      {
        if ( checkDeadEnds && !path.IsEmpty() && !E_seed.IsNull() &&
             t_interactions::__areCoincident(E_next, E_seed, m_fInaccuracy) )
        {
          Results.Append(V_seed.Opp.V);
        }

        stop = true;
      }
      else if ( !V_prev.IsNull() && !E_seed.IsNull() &&
                t_interactions::__areCoincident(E_next, E_seed, m_fInaccuracy) )
      {
        // Seed edge is not always initialized. If we are here, it normally
        // means that the starting vertex was the alien one. If so, then there
        // is no sense to check that we arrive into the same extremity as
        // it will not ever happen (alien vertex does not live in the contour)

        stop = true;
      }
      else
      {
        // Continue traverse
        path.Add(V_next.Opp.V);
        V_prev = V;
        V      = V_next;

#if defined DRAW_DEBUG
        DRAW_POINT(BRep_Tool::Pnt(V_next.Opp.V), seedv, Draw_blanc, Draw_Square)
#endif
      }

      // Check if we exceed the max number of iterations
      if ( iter > max_iters )
      {
        stop = true;
        Journal.SendLogMessage( LogWarn(Normal) << "geom_split_faces.MaxNumIterationsExceeded" );
        continue;
      }
    }
    while ( !stop );

    // We do not want to have successor seeds selected from the
    // traversed paths, so let's propagate blocking property
    for ( int path_i = 1; path_i <= path.Extent(); ++path_i )
      TraversedSeeds.Add( path.FindKey(path_i) );

    // Make a wire from the sub-contour edges
    if ( !checkDeadEnds && !wd.IsNull() )
    {
      TopoDS_Wire subContour = wd->Wire();
      Results.Append(subContour);
    }
  }

  // Return true if we ended up at least with one sub-contour
  return (Results.Extent() > 0);
}

//! There are several paths to follow which meet at the given vertex. This
//! routine tests each possibility and takes the one giving the most left
//! turn to the next unvisited vertex.
//!
//! \param F             [in]     host face.
//! \param V             [in]     vertex to decide the next path.
//! \param V_prev        [in]     previous traversed vertex used to calculate
//!                               angle of turn.
//! \param prev_dir      [in]     previous direction.
//! \param ori_ref_dir   [in]     original reference direction to measure angles.
//! \param M             [in]     child-parent relationships.
//! \param pickNearest   [in]     indicates whether we want to pick an edge which
//!                               is nearest to the seed direction in terms of
//!                               the absolute angle. Otherwise the most left one
//!                               with respect to the reference normal will be chosen.
//! \param NextEdge      [out]    next edge to traverse.
//! \param NextVertex    [out]    next vertex to traverse.
//! \return true if there is still some opportunity to traverse, false -- otherwise.
bool geom_split_faces::nextEdge(const TopoDS_Face&                               F,
                                const t_node&                                    V,
                                const t_node&                                    V_prev,
                                const gp_Dir&                                    prev_dir,
                                const gp_Dir&                                    ori_ref_dir,
                                const TopTools_IndexedDataMapOfShapeListOfShape& M,
                                const bool                                       pickNearest,
                                TopoDS_Edge&                                     NextEdge,
                                t_node&                                          NextVertex) const
{
  // ...
  // Take candidate vertices where we can go
  // ...

  t_opportunities opportunities;

  // Reference direction
  gp_Dir ref_dir = ori_ref_dir;

  // Get candidates
  TopTools_ListOfShape candidates;
  if ( V.isAlien )
    candidates.Append(V.Opp.E);
  else
  {
    candidates = M.FindFromKey(V.Opp.V);

    // Evaluate new referential norm
    Handle(Geom_Surface) S = BRep_Tool::Surface(F);
    ShapeAnalysis_Surface sas(S);
    gp_Pnt2d UV = sas.ValueOfUV( BRep_Tool::Pnt(V.Opp.V), Precision::Confusion() );
    gp_Pnt S_P;
    gp_Vec S_Du, S_Dv;
    S->D1(UV.X(), UV.Y(), S_P, S_Du, S_Dv);
    gp_Dir S_Norm = S_Du.Crossed(S_Dv);

    // Override reference direction
    ref_dir = S_Norm;

#if defined DRAW_DEBUG
    DRAW_INITGROUP(ref_norm)
    DRAW_LINE3D(BRep_Tool::Pnt(V.Opp.V), gp_Pnt(BRep_Tool::Pnt(V.Opp.V).XYZ() + S_Norm.XYZ()*500), ref_norm, Draw_bleu)
#endif
  }

  // Traverse over the candidate edges
  for ( TopTools_ListIteratorOfListOfShape it(candidates); it.More(); it.Next() )
  {
    const TopoDS_Edge& E = TopoDS::Edge( it.Value() );

    // Ensure that we are not going to traverse the same path as previously
    if ( !V_prev.Opp.E.IsNull() && E.IsSame(V_prev.Opp.E) )
      continue;

    TopoDS_Vertex V_f = ShapeAnalysis_Edge().FirstVertex(E);
    TopoDS_Vertex V_l = ShapeAnalysis_Edge().LastVertex(E);

    if ( V.isAlien )
    {
      // Both extremities are to be checked
      opportunities.add( t_opportunity(V_f, E) );
      opportunities.add( t_opportunity(V_l, E) );
    }
    else
    {
      TopoDS_Vertex V_probe;

      if ( V_f.IsPartner(V.Opp.V) )
        V_probe = V_l;

      if ( V_l.IsPartner(V.Opp.V) )
        V_probe = V_f;

      if ( V_probe.IsNull() || V_probe.IsSame(V_prev.Opp.V) )
        continue; // Null probes and probes corresponding to the reverse
                  // movement are forbidden

      // Now we have a probe vertex which is not yet traversed, so it lies
      // in the terra incognita. Let's put this vertex into the list of
      // opportunities

      t_opportunity o;
      o.V = V_probe;
      o.E = E;
      opportunities.add(o);
    }
  }

  if ( !opportunities.any() )
    return false; // No way more

  if ( opportunities.size() == 1 )
  {
    NextEdge   = opportunities.take(1).E;
    NextVertex = t_node(opportunities.take(1).V, false);
    return true; // One way only
  }

  // ...
  // Now we have got one or several opportunities to continue traversal.
  // Let's choose the one giving the most left turn
  // ...

  gp_XYZ v_XYZ = BRep_Tool::Pnt(V.Opp.V).XYZ();

  // Traverse over the opportunities
  t_opportunity o_next;
  double ang = pickNearest ? RealLast() : -RealLast();
  for ( int o_idx = 1; o_idx <= opportunities.size(); ++o_idx )
  {
    const t_opportunity& o = opportunities.take(o_idx);

    // Opportunity point
    gp_XYZ o_XYZ;

    // ...
    // Angle for left-turn rule should be checked near the probe point
    // in order to perform local (!) sampling. It is generally not safe
    // to use opportunity vertices as probe points as these vertices are
    // too distant. However, it is at the same time not easy to predict
    // the parametric shift to pick up the probe point: at vertex where
    // two edges meet it is very important to take into account that host
    // curves of these edges may have completely different parameterizations,
    // so (let's say) t = 0.1 may be a near point for E1 but a very distant
    // point for E2. Needless to say that our current approach for picking
    // up the probe point is completely driven by the non-regression test
    // database
    // ...

    double dist_o = 0.0, param_o = 0.0, dist_seed = 0.0, param_seed = 0.0;
    gp_Pnt P_proj_o, P_proj_seed;
    geom_project_vertex_edge ProjVert( o.E, Precision::Confusion() );

    bool isProj = ProjVert(o.V, dist_o, param_o, P_proj_o, NULL) &&
                  ProjVert(V.Opp.V, dist_seed, param_seed, P_proj_seed, NULL);

    if ( isProj )
    {
      if ( P_proj_o.Distance(P_proj_seed) < m_fInaccuracy )
        isProj = false;
    }

    if ( isProj )
    {
      BRepAdaptor_Curve bac(o.E);

      // TODO: generally it is not enough just to take half-parameter.
      //       This is a subject for further investigation
      o_XYZ = bac.Value(param_seed + (param_o - param_seed)*0.1).XYZ();
    }
    else
    {
      // No way, let's use vertex as a last chance
      o_XYZ = BRep_Tool::Pnt(o.V).XYZ();
    }

    gp_XYZ delta = o_XYZ - v_XYZ;

    // Check if we are able to calculate angle
    if ( delta.Modulus() < Precision::Confusion() )
      continue;

#if defined DRAW_DEBUG
    DRAW_INITGROUP(opportunity)
    DRAW_INITGROUP(opportunity_dir)
    DRAW_POINT(o_XYZ, opportunity, Draw_corail, Draw_Plus)
    DRAW_LINE3D(gp_Pnt(v_XYZ), gp_Pnt(o_XYZ), opportunity_dir, Draw_corail)
#endif

    const gp_Dir o_dir(delta);
    double o_ang = prev_dir.AngleWithRef(o_dir, ref_dir);

    if ( pickNearest )
      o_ang = Abs(o_ang);

    if ( pickNearest && (o_ang < ang) || !pickNearest && (o_ang > ang) )
    {
      ang    = o_ang;
      o_next = o;
    }
  }

  // Set result
  NextEdge   = o_next.E;
  NextVertex = t_node(o_next.V, false);
  return true;
}

//! Attempts to find next non traversed vertex in the original soup of
//! vertices. If nothing remains non-traversed, this method returns false.
//! \param F                 [in]     host face.
//! \param M                 [in]     original soup of vertices and their edges.
//! \param TraversedVertices [in]     already traversed vertices.
//! \param TraversedEdges    [in]     already traversed edges.
//! \param Node              [out]    found non-traversed vertex (if any).
//! \param eps               [out]    neighborhood at a point.
//! \param Journal           [in/out] journal instance.
//! \return true/false.
bool geom_split_faces::nextSeed(const TopoDS_Face&                               F,
                                const TopTools_IndexedDataMapOfShapeListOfShape& M,
                                const TopTools_IndexedMapOfShape&                TraversedVertices,
                                const TopTools_IndexedMapOfShape&                TraversedEdges,
                                t_node&                                          Node,
                                Handle(geom_epsilon_base)&                       eps,
                                ActAPI_ProgressEntry                             Journal) const
{
  // Find non-traversed vertex
  TopoDS_Shape V_shape;
  for ( int key_idx = 1; key_idx <= M.Extent(); ++key_idx )
  {
    const TopoDS_Shape& V_candidate = M.FindKey(key_idx);

    if ( !TraversedVertices.Contains(V_candidate) )
    {
      V_shape = V_candidate;
      break;
    }
  }

  if ( V_shape.IsNull() )
  {
    // ...
    // If we are here, it means that all potential seed vertices have been
    // already traversed. If so, it does not mean that there are no more
    // paths to consider. Let's take non-traversed edges and attempt to
    // get new paths from them
    // ...

    // Try to find any unvisited edge
    TopoDS_Edge E_unvisited;
    for ( int i = 1; i <= M.Extent(); ++i )
    {
      const TopTools_ListOfShape& edges = M.FindFromIndex(i);

      bool stop = false;
      for ( TopTools_ListIteratorOfListOfShape lit(edges); lit.More(); lit.Next() )
      {
        if ( !TraversedEdges.Contains( lit.Value() ) )
        {
          E_unvisited = TopoDS::Edge( lit.Value() );
          stop = true;
          break;
        }
      }

      if ( stop )
        break;
    }

    if ( E_unvisited.IsNull() )
      return false; // Finally, no unvisited edge remains

    // ...
    // Make a fictive vertex at the middle in order to pick up the edge
    // ...

    double f, l;
    BRep_Tool::Range(E_unvisited, f, l);

    // Evaluate point
    BRepAdaptor_Curve bac(E_unvisited);
    gp_Pnt Pm = bac.Value( 0.5*(f + l) );

    // Create alien node
    Node = t_node(BRepBuilderAPI_MakeVertex(Pm), E_unvisited, true);
  }
  else
    Node = t_node(TopoDS::Vertex(V_shape), false);

#if defined DRAW_DEBUG
  DRAW_INITGROUP(geom_split_faces_seedv)
  DRAW_POINT( BRep_Tool::Pnt(Node.Opp.V), geom_split_faces_seedv, Draw_blanc, Draw_Plus )
#endif

  // Extract neighborhood for the vertex on the face
  geom_classify_point_face_N_IP ClassPointFaceN( F, m_fInaccuracy, Precision::Confusion() );
  if ( ClassPointFaceN(BRep_Tool::Pnt(Node.Opp.V), eps, NULL) & Membership_InOn )
  {
    return true;
  }

  Journal.SendLogMessage( LogWarn(Normal) << "geom_split_faces.CannotEvaluateNeighborhoodPointFace" << Node.Opp.V << F );
  return false; // Strange
}

//! Checks that the passed contour is closed and can be used for constructing
//! face patch so.
//! \param Contour [in] contour to check.
//! \return true/false.
bool geom_split_faces::isClosedContour(const TopoDS_Shape& Contour) const
{
  return geom_check_contour().Check_connectedWire(Contour, false, m_fInaccuracy, NULL);
}

//! Performs sewing.
//! \param Body [in] shape to sew.
void geom_split_faces::sew(TopoDS_Shape& Body) const
{
  // Apply sewing
  BRepBuilderAPI_Sewing Sewer(m_fInaccuracy);
  Sewer.Load(Body);
  Sewer.Perform();
  Body = Sewer.SewedShape();
}

//! Given the list of blind vertices which are not perspective to follow,
//! this method attempts to find a possible continuation on an non-split edge
//! and modify the input mosaic contour respectively.
//! \param Mosaic   [in/out] working mosaic contour.
//! \param Face     [in/out] working face.
//! \param DeadEnds [in]     dead-end vertices.
//! \param maxGap   [in]     max allowed gap between the dead-end vertex and
//!                          its continuation.
//! \param resGap   [in]     resulting gap.
//! \param Journal  [in/out] journal instance.
//! \return true if all dead-ends were processed successfully, false -- otherwise.
bool geom_split_faces::resolveDeadEnds(TopoDS_Shape&               Mosaic,
                                       TopoDS_Face&                Face,
                                       const TopTools_ListOfShape& DeadEnds,
                                       const double                maxGap,
                                       double&                     resGap,
                                       ActAPI_ProgressEntry        Journal) const
{
  Handle(ShapeBuild_ReShape) Ctx = new ShapeBuild_ReShape;

  // Process each dead end one by one
  resGap = m_fInaccuracy;
  for ( TopTools_ListIteratorOfListOfShape v_it(DeadEnds); v_it.More(); v_it.Next() )
  {
    const TopoDS_Vertex& current_dead_end = TopoDS::Vertex( v_it.Value() );

    // Search for a candidate continuation
    for ( TopExp_Explorer e_exp(Mosaic, TopAbs_EDGE); e_exp.More(); e_exp.Next() )
    {
      const TopoDS_Edge& E = TopoDS::Edge( e_exp.Current() );

      // Check extremities
      TopoDS_Vertex Vf = ShapeAnalysis_Edge().FirstVertex(E);
      TopoDS_Vertex Vl = ShapeAnalysis_Edge().LastVertex(E);
      if ( t_interactions::__areCoincident(Vf, current_dead_end, 0.0) ||
           t_interactions::__areCoincident(Vl, current_dead_end, 0.0) ) // Pure sharing is checked
        continue;

      // Check gap
      double gap = RealLast(), param = RealLast();
      gp_Pnt proj;
      geom_project_vertex_edge ProjVertexEdge( E, Precision::Confusion() );
      if ( !ProjVertexEdge(current_dead_end, gap, param, proj, NULL) )
        continue;

      if ( gap < maxGap )
      {
#if defined DRAW_DEBUG
        DRAW_INITGROUP(dead_end_solution)
        DRAW_POINT(proj, dead_end_solution, Draw_vert, Draw_Losange)
        DRAW_EDGE(E, dead_end_solution, Draw_vert)
#endif
        // Update tolerance
        resGap = gap;

        // Check if the projection falls into singularity. If not, we are
        // ready to split. If yes, we just get satisfied with the projection
        // gap in order to merge distinct vertices
        TopoDS_Vertex V_proj = BRepBuilderAPI_MakeVertex(proj);
        if ( !t_interactions::__areCoincident(Vf, V_proj, resGap) &&
             !t_interactions::__areCoincident(Vl, V_proj, resGap) ) // Pure sharing is checked
        {
          // Split edge
          TInterPts pts;
          pts.Append( TInterPt(proj, param, Face) );
          TopoDS_Wire W = this->split(E, pts, Ctx);

          if ( W.IsNull() )
          {
            Journal.SendLogMessage( LogWarn(Normal) << "geom_split_faces.CannotResolveDeadEnd" << current_dead_end << E );
            return false;
          }

          // Save modification
          Ctx->Replace(E, W);
        }
      }
    }
  }

  // Apply modifications
  Mosaic = Ctx->Apply(Mosaic);
  Face   = TopoDS::Face( Ctx->Apply(Face) );

  // Get rid of possible coincident but distinct vertices
  this->mergeCoincidentVertices(Mosaic, Face, resGap, Ctx);

#if defined DRAW_DEBUG
  DBRep::Set("de_f", Face);
#endif

  return true;
}

//! Checks whether the passed mosaic contour contains a disconnected nested
//! sub-contour. Check is done be expansion against child-parent relationships.
//! \param M_all    [in]  vertex-edge relationships.
//! \param M_nested [out] vertex-edge relationships for the detected
//!                       isolated sub-contour (if any).
//! \return true/false.
bool geom_split_faces::isImbrication(const TopTools_IndexedDataMapOfShapeListOfShape& M_all,
                                     TopTools_IndexedDataMapOfShapeListOfShape&       M_nested) const
{
  // Take any vertex to expand edges from
  const TopoDS_Vertex& V_seed = TopoDS::Vertex( M_all.FindKey(1) );

  // Loop over adjacency graph to check whether we traverse all the graph or
  // if anything remains. If so, the graph is not connected and we may
  // extract the isolated sub-contour
  TopTools_IndexedDataMapOfShapeListOfShape Visited;
  this->traverseByExpansion(V_seed, M_all, Visited);

  // Check if all vertices have been visited
  if ( Visited.Extent() == M_all.Extent() )
    return false; // If so, there is no isolated sub-contour

  // Gather the rest of vertices which remain unvisited
  TopTools_IndexedDataMapOfShapeListOfShape Unvisited;
  for ( int v_idx = 1; v_idx <= M_all.Extent(); ++v_idx )
  {
    const TopoDS_Vertex& V = TopoDS::Vertex( M_all.FindKey(v_idx) );
    if ( Visited.Contains(V) )
      continue;

    Unvisited.Add( V, M_all.FindFromKey(V) );
  }

  // Now we have two collections: visited and unvisited. One collection is
  // geometrically nested to another one and we have to choose the nested one
  // ...

  Bnd_Box B_visited;
  for ( int i_visited = 1; i_visited <= Visited.Extent(); ++i_visited )
    B_visited.Add( BRep_Tool::Pnt( TopoDS::Vertex( Visited.FindKey(i_visited) ) ) );

  Bnd_Box B_unvisited;
  for ( int i_unvisited = 1; i_unvisited <= Unvisited.Extent(); ++i_unvisited )
    B_unvisited.Add( BRep_Tool::Pnt( TopoDS::Vertex( Unvisited.FindKey(i_unvisited) ) ) );

  B_unvisited.Enlarge(m_fInaccuracy*1000);
  gp_Pnt vMin = B_visited.CornerMin();
  gp_Pnt vMax = B_visited.CornerMax();

  if ( B_unvisited.IsOut(vMin) || B_unvisited.IsOut(vMax) )
    M_nested = Unvisited;
  else
    M_nested = Visited;

#if defined DRAW_DEBUG
  DRAW_INITGROUP(box_visited)
  DRAW_BOX(B_visited, box_visited, Draw_vert)

  DRAW_INITGROUP(box_unvisited)
  DRAW_BOX(B_unvisited, box_unvisited, Draw_bleu)
#endif

  // Whatever the nested collection, we have a nested contour here
  return true;
}

//! Starting from the passed seed (current) vertex, this routine travels
//! over all adjacent edges recursively, thus gathering all vertices which
//! can be reached from the seed one.
//! \param V_seed  [in]     vertex to start traveling from.
//! \param M_all   [in]     child-parent relationship.
//! \param Visited [in/out] visited vertices.
void geom_split_faces::traverseByExpansion(const TopoDS_Vertex&                             V_current,
                                           const TopTools_IndexedDataMapOfShapeListOfShape& M_all,
                                           TopTools_IndexedDataMapOfShapeListOfShape&       Visited) const
{
  const TopTools_ListOfShape& Edges = M_all.FindFromKey(V_current);
  Visited.Add( V_current, Edges );

  // Continue expansion
  for ( TopTools_ListIteratorOfListOfShape lit(Edges); lit.More(); lit.Next() )
  {
    const TopoDS_Edge& E = TopoDS::Edge( lit.Value() );

    TopoDS_Vertex V_first = ShapeAnalysis_Edge().FirstVertex(E);
    TopoDS_Vertex V_last  = ShapeAnalysis_Edge().LastVertex(E);
    TopoDS_Vertex V_next;

    if ( !Visited.Contains(V_first) )
      V_next = V_first;
    else if ( !Visited.Contains(V_last) )
      V_next = V_last;
    else
      continue;

    this->traverseByExpansion(V_next, M_all, Visited);
  }
}
