//-----------------------------------------------------------------------------
// Created on: 20 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef modeling_shape_utils_h
#define modeling_shape_utils_h

// A-Situs includes
#include <analysis_situs.h>

// Active Data (API) includes
#include <ActAPI_ILogger.h>
#include <ActAPI_IProgressNotifier.h>

// OCCT includes
#include <gp_Trsf.hxx>
#include <TopoDS_Compound.hxx>
#include <TopTools_ListOfShape.hxx>

//! Auxiliary functions facilitating working with OCCT topological shapes.
class modeling_shape_utils
{
public:

  ASitus_EXPORT static TopoDS_Shape
    ApplyTransformation(const TopoDS_Shape& theShape,
                        const double theXPos,
                        const double theYPos,
                        const double theZPos,
                        const double theAngleA,
                        const double theAngleB,
                        const double theAngleC);

  ASitus_EXPORT static gp_Trsf
    Transformation(const double theXPos,
                   const double theYPos,
                   const double theZPos,
                   const double theAngleA,
                   const double theAngleB,
                   const double theAngleC);

  ASitus_EXPORT static TopoDS_Shape
    ApplyTransformation(const TopoDS_Shape& theShape,
                        const gp_Trsf& theTransform);

  ASitus_EXPORT static TopoDS_Shape
    AssembleShapes(const TopTools_ListOfShape& theShapes);

  ASitus_EXPORT static void
    Bounds(const TopoDS_Shape& theShape,
           double& XMin, double& YMin, double& ZMin,
           double& XMax, double& YMax, double& ZMax);

  ASitus_EXPORT static bool
    CheckShape(const TopoDS_Shape& theShape,
               ActAPI_ProgressEntry PEntry);

  ASitus_EXPORT static double
    MaxTolerance(const TopoDS_Shape& theShape);

private:

  modeling_shape_utils(); //!< Prohibited.
  modeling_shape_utils(const modeling_shape_utils&); //!< Prohibited.

};

#endif
