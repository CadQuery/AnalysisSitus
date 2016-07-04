//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_divide_by_continuity.h>

// OCCT includes
#include <ShapeUpgrade_ShapeDivideContinuity.hxx>

bool geom_divide_by_continuity::Divide_C0(TopoDS_Shape&        shape,
                                          const double         precision,
                                          ActAPI_ProgressEntry Journal)
{
  try
  {
    ShapeUpgrade_ShapeDivideContinuity DivideTool(shape);
    DivideTool.SetTolerance(precision);
    DivideTool.Perform();

    if ( DivideTool.Status(ShapeExtend_OK) )
      return true;

    if ( DivideTool.Status(ShapeExtend_FAIL) )
    {
      Journal.SendLogMessage( LogWarn(Normal) << "geom_divide_by_continuity.Divide_C0.Failed" << shape );
      return false;
    }

    if ( DivideTool.Status(ShapeExtend_DONE) )
    {
      shape = DivideTool.Result();
      return true;
    }
  }
  catch ( ... )
  {
    Journal.SendLogMessage( LogWarn(Normal) << "geom_divide_by_continuity.Divide_C0.Failed" << shape );
  }

  return false;
}
