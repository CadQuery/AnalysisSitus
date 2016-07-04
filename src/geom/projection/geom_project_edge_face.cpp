//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_project_edge_face.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAlgo_NormalProjection.hxx>
#include <BRepOffsetAPI_NormalProjection.hxx>
#include <BRepProj_Projection.hxx>
#include <Geom_Surface.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeFix_Edge.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Constructor.
//! \param Face [in] face to project the edges to.
geom_project_edge_face::geom_project_edge_face(const TopoDS_Face& Face)
: m_face(Face)
{
}

//! Performs edge-to-face projection.
//! \param Edge         [in]     edge to project.
//! \param doNormalProj [in]     indicates whether to perform normal projection
//!                              instead of trying to predict projection
//!                              direction by surface normal at the middle
//!                              point of the edge. The second approach seems
//!                              to be more robust.
//! \param Result       [out]    resulting edge(s).
//! \param ProjRay      [out]    projection direction together with origin.
//!                              Initialized if the normal projection mode
//!                              is disabled.
//! \param Journal      [in/out] journal instance.
//! \return true in case of success, false -- otherwise.
bool geom_project_edge_face::operator()(const TopoDS_Edge&   Edge,
                                        const bool           doNormalProj,
                                        TopoDS_Shape&        Result,
                                        gp_Lin&              ProjRay,
                                        ActAPI_ProgressEntry Journal)
{
  if ( !doNormalProj )
  {
    double f, l;
    BRep_Tool::Range(Edge, f, l);
    BRepAdaptor_Curve BAC(Edge);
    gp_Pnt Pm = BAC.Value( (f + l)*0.5 );

    Handle(Geom_Surface) S = BRep_Tool::Surface(m_face);
    ShapeAnalysis_Surface sas(S);
    gp_Pnt2d UV = sas.ValueOfUV( Pm, Precision::Confusion() );

    gp_Pnt S_P;
    gp_Vec S_Du, S_Dv;
    S->D1(UV.X(), UV.Y(), S_P, S_Du, S_Dv);
    gp_Dir S_Norm = S_Du.Crossed(S_Dv);

    // Run projection
    TopTools_ListOfShape projResult;
    try
    {
      BRepProj_Projection Proj(Edge, m_face, S_Norm);

      // Accumulate projection results
      if ( Proj.IsDone() )
      {
        while ( Proj.More() )
        {
          projResult.Append( Proj.Current() );
          Proj.Next();
        }
      }
      else
        return false;
    }
    catch ( ... )
    {
#if defined DRAW_DEBUG
      DBRep::Set("proj_fail_f", m_face);
      DBRep::Set("proj_fail_e", Edge);
#endif

      Journal.SendLogMessage( LogWarn(Normal) << "geom_project_edge_face.ProjectionFailed" << Edge << m_face );
      return false;
    }

    if ( projResult.Extent() > 1 )
    {
      TopoDS_Compound Comp;
      BRep_Builder().MakeCompound(Comp);
      for ( TopTools_ListIteratorOfListOfShape lit(projResult); lit.More(); lit.Next() )
      {
        BRep_Builder().Add( Comp, lit.Value() );
      }
      Result = Comp;
    }
    else
      Result = projResult.First();

    ProjRay = gp_Lin(Pm, S_Norm);
  }
  else
  {
    // Default parameters as proposed by DRAW
    const double        Tol         = 1.e-4;
    const double        Tol2d       = Pow(Tol, 2./3);
    const GeomAbs_Shape Continuity  = GeomAbs_C2;
    const int           MaxDeg      = 14;
    const int           MaxSeg      = 16;

    // Prepare and build projection
    BRepOffsetAPI_NormalProjection OrtProj(m_face);
    OrtProj.Add(Edge);
    OrtProj.SetParams(Tol, Tol2d, Continuity, MaxDeg, MaxSeg);
    OrtProj.Build();
    Result = OrtProj.Shape();
  }

#if defined DRAW_DEBUG
  DRAW_INITGROUP(geom_project_edge_face)
  DRAW_EDGE(Result, geom_project_edge_face, Draw_cyan)

  double __f, __l;

  DRAW_INITGROUP(geom_project_edge_face_PCurve)
  for ( TopExp_Explorer exp(Result, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    const TopoDS_Edge& E = TopoDS::Edge( exp.Current() );
    DRAW_CURVE2D(BRep_Tool::CurveOnSurface(E, m_face, __f, __l), geom_project_edge_face_PCurve, Draw_blanc)
  }
#endif

  return true;
}
