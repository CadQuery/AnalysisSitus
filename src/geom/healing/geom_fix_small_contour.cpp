//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_fix_small_contour.h>

// OCCT includes
#include <ShapeBuild_ReShape.hxx>
#include <ShapeFix_Wireframe.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Constructor.
geom_fix_small_contour::geom_fix_small_contour()
{
}

//! Eliminates unreasonably small edges from the given contour.
//! \param Contour   [in]     contour to heal.
//! \param precision [in]     healing precision.
//! \param Result    [out]    resulting contour.
//! \param Journal   [in/out] journal instance.
//! \return healed contour.
bool geom_fix_small_contour::operator()(const TopoDS_Shape&  Contour,
                                        const double         precision,
                                        TopoDS_Wire&         Result,
                                        ActAPI_ProgressEntry Journal)
{
  const double tol_ang = M_PI/6;
  Handle(ShapeFix_Wireframe) FixTool = new ShapeFix_Wireframe;
  Handle(ShapeBuild_ReShape) ReShape = new ShapeBuild_ReShape;
  FixTool->SetContext(ReShape);
  FixTool->Load(Contour);
  FixTool->SetPrecision(precision);

  const int mode_drop = 0;
  TopTools_MapOfShape SmallEdges, MultyEdges;
  TopTools_DataMapOfShapeListOfShape EdgeToFaces, FaceWithSmall;
  FixTool->CheckSmallEdges(SmallEdges, EdgeToFaces, FaceWithSmall, MultyEdges);
  FixTool->MergeSmallEdges(SmallEdges, EdgeToFaces, FaceWithSmall, MultyEdges, mode_drop, tol_ang);

  TopoDS_Shape ResSh = FixTool->Shape();
  TopExp_Explorer exp(ResSh, TopAbs_WIRE);
  while ( exp.More() )
  {
    Result = TopoDS::Wire( exp.Current() );

    exp.Next();
    if ( exp.More() )
      return false;
  }

  return true;
}
