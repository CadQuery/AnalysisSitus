//-----------------------------------------------------------------------------
// Created on: 20 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef geom_utils_h
#define geom_utils_h

// A-Situs includes
#include <analysis_situs.h>

// Active Data (API) includes
#include <ActAPI_ILogger.h>
#include <ActAPI_IProgressNotifier.h>

// OCCT includes
#include <gp_Trsf.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>

//! Auxiliary functions facilitating working with OCCT topological shapes.
class geom_utils
{
public:

  static TopoDS_Shape
    ApplyTransformation(const TopoDS_Shape& theShape,
                        const double        theXPos,
                        const double        theYPos,
                        const double        theZPos,
                        const double        theAngleA,
                        const double        theAngleB,
                        const double        theAngleC,
                        const bool          doCopy);

  static gp_Trsf
    Transformation(const double theXPos,
                   const double theYPos,
                   const double theZPos,
                   const double theAngleA,
                   const double theAngleB,
                   const double theAngleC);

  static TopoDS_Shape
    ApplyTransformation(const TopoDS_Shape& theShape,
                        const gp_Trsf&      theTransform,
                        const bool          doCopy);

  static TopoDS_Shape
    AssembleShapes(const TopTools_ListOfShape& theShapes);

  static void
    Bounds(const TopoDS_Shape& theShape,
           double& XMin, double& YMin, double& ZMin,
           double& XMax, double& YMax, double& ZMax);

  static bool
    CheckShape(const TopoDS_Shape&  theShape,
               ActAPI_ProgressEntry PEntry);

  static double
    MaxTolerance(const TopoDS_Shape& theShape);

  static bool
    ReadBRep(const TCollection_AsciiString& theFilename,
             TopoDS_Shape&                  theShape);

  static void
    ShapeSummary(const TopoDS_Shape&      shape,
                 TCollection_AsciiString& info);

  static TopoDS_Wire
    CreateCircularWire(const double radius);

  static TopoDS_Shape
    MakeSkin(const TopTools_SequenceOfShape& wires);

private:

  geom_utils(); //!< Prohibited.
  geom_utils(const geom_utils&); //!< Prohibited.

};

#endif
