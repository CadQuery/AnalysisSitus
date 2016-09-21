//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_refine_tolerance.h>

// OCCT includes
#include <BRepCheck_Analyzer.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_ShapeTolerance.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Constructor.
geom_refine_tolerance::geom_refine_tolerance()
{
}

//! Attempts to rebuild shape boundaries.
//! \param Body       [in]     shape to refine.
//! \param Journal    [in/out] journal instance.
bool geom_refine_tolerance::RebuildBounds(TopoDS_Shape&        Body,
                                          ActAPI_ProgressEntry Journal) const
{
  for ( TopExp_Explorer exp(Body, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    const TopoDS_Edge& E = TopoDS::Edge( exp.Current() );

    ShapeBuild_Edge().RemoveCurve3d(E);
    ShapeFix_Edge().FixAddCurve3d(E);
  }
  return true;
}

//! Attempts to refine tolerance.
//! \param Body       [in]     shape to refine.
//! \param inaccuracy [in]     min allowed inaccuracy value.
//! \param Journal    [in/out] journal instance.
//! \return healed contour.
bool geom_refine_tolerance::Degrade(TopoDS_Shape&        Body,
                                    const double         inaccuracy,
                                    ActAPI_ProgressEntry Journal) const
{
  // Get max tolerance
  ShapeAnalysis_ShapeTolerance TolAnalysis;
  TolAnalysis.InitTolerance();
  TolAnalysis.AddTolerance(Body);
  const double maxTol = TolAnalysis.GlobalTolerance(1);

  // Attempt to refine tolerance
  ShapeFix_ShapeTolerance FixTol;
  double refinedTol = maxTol, prevTol = maxTol;
  bool isValid = true;
  do
  {
    prevTol    = refinedTol;
    refinedTol = 0.5*refinedTol;
    FixTol.SetTolerance(Body, refinedTol);

    BRepCheck_Analyzer Check(Body, 1);
    isValid = Check.IsValid();
  }
  while ( isValid && refinedTol > inaccuracy );

  // Set last valid tolerance
  FixTol.SetTolerance(Body, prevTol);

  return true;
}
