//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_contour_capture_validator.h>

// OCCT includes
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <BRepMesh_IncrementalMesh.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Default constructor.
geom_contour_capture_validator::geom_contour_capture_validator()
{}

//! Validates the passed body against the given contour simply by comparing
//! their bounding boxes with the given acceptable deviation.
//! \param Body                 [in]     captured body (shell).
//! \param Contour              [in]     contour to verify the captured body against.
//! \param deviation_percentage [in]     acceptable deviation in percents.
//! \param tess_precision       [in]     precision for meshing.
//! \param Journal              [in/out] journal instance.
//! \return true in case of success, false -- otherwise.
bool geom_contour_capture_validator::Compare_AABBs(const TopoDS_Shape&  Body,
                                                   const TopoDS_Shape&  Contour,
                                                   const int            deviation_percentage,
                                                   const double         tess_precision,
                                                   ActAPI_ProgressEntry Journal) const
{
#if defined DRAW_DEBUG
  std::cout << "\n[>>>] Validation stage" << std::endl;
  TIMER_NEW
  TIMER_GO
#endif

  // Tessellate body in order to obtain a precise bounding box
  try
  {
    BRepMesh_IncrementalMesh Mesh(Body, tess_precision);
  }
  catch ( ... )
  {
    Journal.SendLogMessage( LogErr(Normal) << "geom_contour_capture_validator.Compare_AABBs.CannotMeshBody" );
    return false;
  }

  // Tessellate contour in order to obtain a precise bounding box
  try
  {
    BRepMesh_IncrementalMesh Mesh(Contour, tess_precision);
  }
  catch ( ... )
  {
    Journal.SendLogMessage( LogErr(Normal) << "geom_contour_capture_validator.Compare_AABBs.CannotMeshContour" );
    return false;
  }

  // Build bounding boxes
  Bnd_Box BoxBody, BoxContour;
  BRepBndLib::Add(Body, BoxBody);
  BRepBndLib::Add(Contour, BoxContour);

  // Get min and max corners
  double xMin1, yMin1, zMin1, xMax1, yMax1, zMax1;
  double xMin2, yMin2, zMin2, xMax2, yMax2, zMax2;
  BoxContour.Get(xMin1, yMin1, zMin1, xMax1, yMax1, zMax1);
  BoxBody.Get(xMin2, yMin2, zMin2, xMax2, yMax2, zMax2);
  gp_Pnt min1(xMin1, yMin1, zMin1), max1(xMax1, yMax1, zMax1),
         min2(xMin2, yMin2, zMin2), max2(xMax2, yMax2, zMax2);

  // Get distances between corners
  const double dmin = min1.Distance(min2);
  const double dmax = max1.Distance(max2);

  // Deviation is taken with respect to the average bounding box diagonal
  const double ratio = deviation_percentage * 0.01;
  const double
    deviation = ( ( ( max1.XYZ() + max2.XYZ() )*0.5 - ( min1.XYZ() + min2.XYZ() )*0.5 )*ratio ).Modulus();

#if defined DRAW_DEBUG
  TIMER_FINISH
  TIMER_COUT_RESULT

  DRAW_INITGROUP(geom_contour_capture_validator_Compare_AABBs)
  DRAW_BOX(BoxContour, geom_contour_capture_validator_Compare_AABBs, Draw_rouge)
  DRAW_BOX(BoxBody,    geom_contour_capture_validator_Compare_AABBs, Draw_or)
#endif

  if ( dmin > deviation || dmax > deviation )
  {
    Journal.SendLogMessage( LogErr(Normal) << "geom_contour_capture_validator.Compare_AABBs.SignificantDifference" );
    return false;
  }

  return true;
}
