//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_project_wire_shell.h>

// Geometry includes
#include <geom_intersect_AABBs.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgo_NormalProjection.hxx>
#include <BRepOffsetAPI_NormalProjection.hxx>
#include <BRepProj_Projection.hxx>
#include <Geom_Surface.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <NCollection_Sequence.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeFix_Edge.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Constructor.
//! \param Shell      [in] shell to project the wire to.
//! \param inaccuracy [in] inaccuracy of the input geometry.
geom_project_wire_shell::geom_project_wire_shell(const TopoDS_Shell& Shell,
                                                 const double        inaccuracy)
: m_shell(Shell), m_fInaccuracy(inaccuracy)
{
}

//! Performs wire-to-shell projection.
//! \param Wire    [in]     wire to project.
//! \param Result  [out]    resulting edge(s).
//! \param Journal [in/out] journal instance.
void geom_project_wire_shell::operator()(const TopoDS_Wire&   Wire,
                                         TopoDS_Shape&        Result,
                                         ActAPI_ProgressEntry Journal)
{
  /* =================================================
   *  Assemble minimal shell from the tentative faces
   * ================================================= */

  TopoDS_Shell MinShell;
  BRep_Builder().MakeShell(MinShell);
  geom_intersect_AABBs AABBx( Precision::Confusion() );
  for ( TopExp_Explorer exp(m_shell, TopAbs_FACE); exp.More(); exp.Next() )
  {
    const TopoDS_Face& F = TopoDS::Face( exp.Current() );
    if ( AABBx(F, Wire) )
    {
      BRep_Builder().UpdateFace(F, m_fInaccuracy*0.1);
      BRep_Builder().Add(MinShell, F);
    }
  }

#if defined DRAW_DEBUG
  DBRep::Set("proj_min_shell", MinShell);
#endif

  /* ====================
   *  Perform projection
   * ==================== */

  // Predict projection direction
  gp_XYZ Pm;
  int nNodes = 0;
  NCollection_Sequence<gp_XYZ> nodes;
  for ( TopExp_Explorer exp(Wire, TopAbs_VERTEX); exp.More(); exp.Next(), ++nNodes )
  {
    gp_XYZ node = BRep_Tool::Pnt( TopoDS::Vertex( exp.Current() ) ).XYZ();
    nodes.Append(node);
    Pm += node;
  }
  Pm /= nNodes;

  // Predict normal
  gp_XYZ Nm;
  for ( int k = 1; k < nodes.Length(); ++k )
  {
    const gp_XYZ& P_curr = nodes(k);
    const gp_XYZ& P_next = nodes(k + 1);

    Nm += (P_curr - Pm).Crossed(P_next - Pm);
  }
  Nm /= nNodes;

#if defined DRAW_DEBUG
  DRAW_INITGROUP(geom_project_wire_shell)
  DRAW_LINE3D(gp_Pnt(Pm), gp_Pnt(Pm + gp_Dir(Nm).XYZ()*1000), geom_project_wire_shell, Draw_rouge)
#endif

  // Project
  BRepProj_Projection Proj(Wire, MinShell, gp_Dir(Nm));
  TopoDS_Shape ProjResult = Proj.Shape();

  // Add wires to a collection
  Handle(TopTools_HSequenceOfShape) proj_edges = new TopTools_HSequenceOfShape;
  for ( TopExp_Explorer exp(ProjResult, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    proj_edges->Append( exp.Current() );
  }

  // Try to connect
  Handle(TopTools_HSequenceOfShape) all_wires;
  ShapeAnalysis_FreeBounds::ConnectEdgesToWires(proj_edges, m_fInaccuracy, 0, all_wires);

  // Process result
  if ( all_wires->Length() != 1 )
  {
    TopoDS_Compound ResComp;
    BRep_Builder().MakeCompound(ResComp);
    for ( int k = 1; k <= all_wires->Length(); ++k )
    {
      BRep_Builder().Add( ResComp, all_wires->Value(k) );
    }

    Result = ResComp;
    Journal.SendLogMessage( LogWarn(Normal) << "geom_project_wire_shell.NotSingleWire" << ResComp );
  }
  else
    Result = all_wires->Value(1);
}
