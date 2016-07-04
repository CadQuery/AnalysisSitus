//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_classify_contour_shell.h>

// Geometry includes
#include <geom_classify_edge_face.h>

// OCCT includes
#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <NCollection_DataMap.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeFix_IntersectionTool.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Constructor.
//! \param S          [in] working shell.
//! \param inaccuracy [in] inaccuracy of the input geometry.
geom_classify_contour_shell::geom_classify_contour_shell(const TopoDS_Shell& S,
                                                         const double        inaccuracy)
: m_S(S), m_fInaccuracy(inaccuracy)
{}

//! Performs actual classification.
//! \param Probe     [in]     contour to classify.
//! \param Result    [out]    list of face slicers.
//! \param SlicerPts [out]    faces which are the suffer from interactions.
//! \param Journal   [in/out] journal instance.
//! \return true in case of success, false -- otherwise.
bool
  geom_classify_contour_shell::operator()(const TopoDS_Wire&              Probe,
                                          TSlicerMap&                     Result,
                                          geom_intersection_points_edges& SlicerPts,
                                          ActAPI_ProgressEntry            Journal) const
{
  // Map of faces and their slicer edges
  Result.Clear();
  SlicerPts.Clear();

  // Re-shape context
  Handle(ShapeBuild_ReShape) Ctx = new ShapeBuild_ReShape;

  // Pre-build bounding boxes for probe edges (this improves performance
  // of the underlying computations)
  NCollection_DataMap<TopoDS_Shape, Bnd_Box> ProbeBoxes;
  for ( TopExp_Explorer exp_probe(Probe, TopAbs_EDGE); exp_probe.More(); exp_probe.Next() )
  {
    Bnd_Box B;
    BRepBndLib::Add(exp_probe.Current(), B);
    B.Enlarge(m_fInaccuracy);

    // Save edge-box association
    ProbeBoxes.Bind(exp_probe.Current(), B);
  }

  // For each face classify each edge
  int f_idx = 1;
  for ( TopExp_Explorer exp_f(m_S, TopAbs_FACE); exp_f.More(); exp_f.Next(), ++f_idx )
  {
    const TopoDS_Face& F = TopoDS::Face( exp_f.Current() );

    // Prepare classifier
    geom_classify_edge_face ClassifyEF(F, m_fInaccuracy);

    // Slicer edges for each face
    Handle(TopTools_HSequenceOfShape) slicers = new TopTools_HSequenceOfShape;

    // Loop over the contour edges
    int e_idx = 1;
    for ( TopExp_Explorer exp_w(Probe, TopAbs_EDGE); exp_w.More(); exp_w.Next(), ++e_idx )
    {
      const TopoDS_Edge& E = TopoDS::Edge( exp_w.Current() );

      // ...
      // Now it is time for classification
      // ...

      geom_intersection_points_edges transitionSegments;
      const geom_membership res = ClassifyEF(E, ProbeBoxes.ChangeFind(E), transitionSegments, Journal);

      if ( res == Membership_Unknown )
      {
        Journal.SendLogMessage( LogWarn(Normal) << "geom_classify_contour_shell.CannotClassifyEdgeFace" << E << F );
        return false;
      }

      // Add slicer to the collection of edges
      if ( (res & Membership_InOn) && !E.IsNull() )
      {
        slicers->Append(E);

        for ( int k = 1; k <= transitionSegments.Length(); ++k )
          SlicerPts.Append( transitionSegments(k) );
      }

#if defined DRAW_DEBUG
      DRAW_INITGROUP(geom_classify_contour_shell)
      Draw_ColorKind color_noncomp = Draw_blanc;
      TCollection_AsciiString nam("slicer_"); nam += "f"; nam += f_idx; nam += "_e"; nam += e_idx;

      if ( res == Membership_In )
      {
        std::cout << "\tEdge-Face membership for face " << f_idx << " and edge " << e_idx << " : IN" << std::endl;
        color_noncomp = Draw_cyan;

        Handle(DBRep_DrawableShape)
          Dr = new DBRep_DrawableShape(E, Draw_vert, Draw_jaune,
                                       color_noncomp, Draw_bleu, 100., 2, 30);
        Draw::Set(nam.ToCString(), Dr);
      }
      else if ( res == Membership_On )
      {
        std::cout << "\tEdge-Face membership for face " << f_idx << " and edge " << e_idx << " : ON" << std::endl;
        color_noncomp = Draw_cyan;

        Handle(DBRep_DrawableShape)
          Dr = new DBRep_DrawableShape(E, Draw_vert, Draw_jaune,
                                       color_noncomp, Draw_bleu, 100., 2, 30);
        Draw::Set(nam.ToCString(), Dr);
      }
      else if ( res == Membership_Out )
      {
        color_noncomp = Draw_orange;
      }
#endif

      if ( res == Membership_Composite )
      {
#if defined DRAW_DEBUG
        DRAW_INITGROUP(transition)
        std::cout << "\tEdge-Face membership for face " << f_idx << " and edge " << e_idx << " : COMPOSITE" << std::endl;
#endif

        // Loop over the transition segments
        const int nTransitions = transitionSegments.Length();
        for ( int i = 1; i < nTransitions; ++i )
        {
          const geom_intersection_point_edges& p      = transitionSegments(i);
          const geom_intersection_point_edges& p_next = transitionSegments(i + 1);

          // Trim edge by transition parameters
          TopoDS_Edge E_res = this->trimmedCopy(E, p.W1, p_next.W1);

          if ( E_res.IsNull() )
            continue;

          if ( (p.Membership & Membership_InOn) && (p_next.Membership & Membership_InOnOut)  )
          /*// Add slicer: notice that on-on segments are filtered out as we
          // do not want to have slicer edges lying on the boundary edges
          // of a face. If we allow this, then in subsequent splitting it
          // definitely lead to necessity of singularities treatment. And we
          // want to escape from singularities as much as possible
          if ( (p.Membership & Membership_In) && (p_next.Membership & Membership_InOnOut) ||
               (p.Membership & Membership_On) && (p_next.Membership & Membership_In) )*/
          {
            slicers->Append(E_res);
            SlicerPts.Append(p);

            if ( (p.Membership & Membership_In) && (p_next.Membership & Membership_Out) )
              SlicerPts.Append(p_next);

#if defined DRAW_DEBUG
            DRAW_POINT(p.P, transition, Draw_jaune, Draw_Square)
            DRAW_POINT(p_next.P, transition, Draw_bleu, Draw_Square)
#endif
          }

#if defined DRAW_DEBUG
          // Draw segments with suitable colors
          Draw_ColorKind color = Draw_blanc;
          if ( p.Membership == Membership_Out && (p_next.Membership & Membership_InOn) )
          {
            color = Draw_orange;
          }
          else if ( (p.Membership & Membership_InOn) && (p_next.Membership & Membership_InOnOut) )
          {
            color = Draw_cyan;

            Handle(DBRep_DrawableShape)
              Dr = new DBRep_DrawableShape(E_res, Draw_vert, Draw_jaune,
                                           color, Draw_bleu, 100., 2, 30);

            nam += "_"; nam += i;
            Draw::Set(nam.ToCString(), Dr);
          }
          else if ( p.Membership == Membership_Out && p_next.Membership == Membership_Out )
          {
            color = Draw_or;
          }

          DRAW_POINT(p.P, geom_classify_contour_shell, color, Draw_X)
#endif
        }
      }
#if defined DRAW_DEBUG
      else // Non-composite membership
      {
        DRAW_POINT(BRep_Tool::Pnt( TopExp::FirstVertex(E) ), geom_classify_contour_shell, color_noncomp, Draw_X)
        DRAW_POINT(BRep_Tool::Pnt( TopExp::LastVertex(E) ), geom_classify_contour_shell, color_noncomp, Draw_X)
      }
#endif
    }

    // Register slicers for the face
    if ( !slicers->IsEmpty() )
      Result.Add(F, slicers);
  }

  return true;
}

//! Prepares the trimmed copy of the passed edge.
//! \param E [in] source edge.
//! \param f [in] first parameter.
//! \param l [in] last parameter.
//! \return trimmed edge.
TopoDS_Edge geom_classify_contour_shell::trimmedCopy(const TopoDS_Edge& E,
                                                     const double       f,
                                                     const double       l) const
{
  double dummy_f, dummy_l;
  TopoDS_Edge result;
  ShapeBuild_Edge().MakeEdge(result, BRep_Tool::Curve(E, dummy_f, dummy_l), E.Location(), f, l);
  return result;
}
