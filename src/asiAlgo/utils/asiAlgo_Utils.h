//-----------------------------------------------------------------------------
// Created on: 20 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_Utils_h
#define asiAlgo_Utils_h

// A-Situs includes
#include <asiAlgo.h>

// Active Data (API) includes
#include <ActAPI_ILogger.h>
#include <ActAPI_IProgressNotifier.h>

// Qr includes
#include <QrCore.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SweptSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>

//-----------------------------------------------------------------------------

#define asiAlgo_TooSmallValue 1.0e-4

//-----------------------------------------------------------------------------

//! Auxiliary functions facilitating working with OCCT topological shapes.
class asiAlgo_Utils
{
public:

  //! Returns geometry of a face as a string label.
  //! \param face [in] face to inspect.
  //! \return label.
  static std::string FaceGeometryName(const TopoDS_Face& face)
  {
    Handle(Geom_Surface) surf = BRep_Tool::Surface(face);
    return SurfaceName(surf);
  }

  //! Converts orientation of the given shape to string.
  //! \param shape [in] shape to access orientation.
  static TCollection_AsciiString
    OrientationToString(const TopoDS_Shape& shape)
  {
    TCollection_AsciiString oriStr;

    // Check orientation
    if ( shape.Orientation() == TopAbs_FORWARD )
      oriStr = "FORWARD";
    else if ( shape.Orientation() == TopAbs_REVERSED )
      oriStr = "REVERSED";
    else if ( shape.Orientation() == TopAbs_INTERNAL )
      oriStr = "INTERNAL";
    else if ( shape.Orientation() == TopAbs_EXTERNAL )
      oriStr = "EXTERNAL";

    return oriStr;
  }

  //! Returns human-readable surface name.
  //! \param surf [in] surface to inspect.
  //! \return surface name.
  static std::string SurfaceName(const Handle(Geom_Surface)& surf)
  {
    if ( surf->IsInstance( STANDARD_TYPE(Geom_Plane) ) )
      return "plane";
    //
    if ( surf->IsInstance( STANDARD_TYPE(Geom_CylindricalSurface) ) )
      return "cylinder";
    //
    if ( surf->IsInstance( STANDARD_TYPE(Geom_BezierSurface) ) )
      return "bezier";
    //
    if ( surf->IsInstance( STANDARD_TYPE(Geom_BSplineSurface) ) )
      return "b-surface";
    //
    if ( surf->IsInstance( STANDARD_TYPE(Geom_ConicalSurface) ) )
      return "cone";
    //
    if ( surf->IsInstance( STANDARD_TYPE(Geom_OffsetSurface) ) )
    {
      Handle(Geom_OffsetSurface) os = Handle(Geom_OffsetSurface)::DownCast(surf);
      return std::string("offset on ") + SurfaceName( os->BasisSurface() );
    }
    //
    if ( surf->IsInstance( STANDARD_TYPE(Geom_RectangularTrimmedSurface) ) )
    {
      Handle(Geom_RectangularTrimmedSurface) ts = Handle(Geom_RectangularTrimmedSurface)::DownCast(surf);
      return std::string("rect trimmed on ") + SurfaceName( ts->BasisSurface() );
    }
    //
    if ( surf->IsInstance( STANDARD_TYPE(Geom_SphericalSurface) ) )
      return "sphere";
    //
    if ( surf->IsInstance( STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion) ) )
      return "ruled";
    //
    if ( surf->IsInstance( STANDARD_TYPE(Geom_SurfaceOfRevolution) ) )
      return "revolution";
    //
    if ( surf->IsInstance( STANDARD_TYPE(Geom_SweptSurface) ) )
      return "sweep";
    //
    if ( surf->IsInstance( STANDARD_TYPE(Geom_ToroidalSurface) ) )
      return "torus";

    return "unknown";
  }

  //! Returns shape address as string with a prefix indicating its type,
  //! e.g. "e05FB0" is an edge and "f780A1" is a face.
  //! \param shape [in] shape to extract the address for.
  //! \return address of TShape as string prefixed by type indicator.
  static std::string ShapeAddrWithPrefix(const TopoDS_Shape& shape)
  {
    std::string addr_str = ShapeAddr( shape.TShape() );

    std::string prefix;
    const TopAbs_ShapeEnum type = shape.ShapeType();
    if ( type == TopAbs_COMPOUND )
      prefix = "COMPOUND";
    else if ( type == TopAbs_COMPSOLID )
      prefix = "COMPSOLID";
    else if ( type == TopAbs_SOLID )
      prefix = "SOLID";
    else if ( type == TopAbs_SHELL )
    {
      if ( shape.Closed() )
        prefix = "CLOSED";
      else
        prefix = "OPEN";

      prefix += " SHELL";
    }
    else if ( type == TopAbs_FACE )
      prefix = "FACE";
    else if ( type == TopAbs_WIRE )
      prefix = "WIRE";
    else if ( type == TopAbs_EDGE )
      prefix = "EDGE";
    else if ( type == TopAbs_VERTEX )
      prefix = "VERTEX";
    else
      prefix = "SHAPE";

    // Notice extra spacing for better visualization
    return "    " + prefix + " [" + addr_str + "]";
  }

  //! Returns TShape address as string.
  //! \param tshape [in] TShape pointer to extract the address for.
  //! \return address of TShape as string.
  static std::string ShapeAddr(const Handle(TopoDS_TShape)& tshape)
  {
    std::string addr_str;
    std::ostringstream ost;
    ost << tshape.get();
    addr_str = ost.str();

    size_t pos = 0;
    while ( addr_str[pos] == '0' )
      pos++;

    if ( pos )
      addr_str = QrCore::substr( addr_str, (int) pos, (int) (addr_str.size() - pos) );

    return addr_str;
  }

  //! Returns TShape address as string.
  //! \param shape [in] shape to extract the address for.
  //! \return address of TShape as string.
  static std::string ShapeAddr(const TopoDS_Shape& shape)
  {
    return ShapeAddr( shape.TShape() );
  }

  //! Checks face type.
  //! \param[in] face face to check.
  //! \return true/false.
  template<typename TSurf>
  static bool IsTypeOf(const TopoDS_Face& face)
  {
    Handle(Geom_Surface) surf = BRep_Tool::Surface(face);
    if ( surf->IsInstance( STANDARD_TYPE(TSurf) ) )
      return true;

    return false;
  }

//-----------------------------------------------------------------------------

  asiAlgo_EXPORT static TopoDS_Shape
    ApplyTransformation(const TopoDS_Shape& theShape,
                        const double        theXPos,
                        const double        theYPos,
                        const double        theZPos,
                        const double        theAngleA,
                        const double        theAngleB,
                        const double        theAngleC,
                        const bool          doCopy);

  asiAlgo_EXPORT static gp_Trsf
    Transformation(const double theXPos,
                   const double theYPos,
                   const double theZPos,
                   const double theAngleA,
                   const double theAngleB,
                   const double theAngleC);

  asiAlgo_EXPORT static TopoDS_Shape
    ApplyTransformation(const TopoDS_Shape& theShape,
                        const gp_Trsf&      theTransform,
                        const bool          doCopy);

  asiAlgo_EXPORT static TopoDS_Shape
    AssembleShapes(const TopTools_ListOfShape& theShapes);

  asiAlgo_EXPORT static bool
    Bounds(const TopoDS_Shape& shape,
           double& XMin, double& YMin, double& ZMin,
           double& XMax, double& YMax, double& ZMax,
           const double tolerance = 0.0);

  asiAlgo_EXPORT static bool
    CheckShape(const TopoDS_Shape&  shape,
               ActAPI_ProgressEntry Journal);

  asiAlgo_EXPORT static double
    MaxTolerance(const TopoDS_Shape& shape);

  asiAlgo_EXPORT static bool
    ReadBRep(const TCollection_AsciiString& theFilename,
             TopoDS_Shape&                  theShape);

  asiAlgo_EXPORT static bool
    WriteBRep(const TopoDS_Shape&            theShape,
              const TCollection_AsciiString& theFilename);

  asiAlgo_EXPORT static void
    ShapeSummary(const TopoDS_Shape& shape,
                 int&                nbCompsolids,
                 int&                nbCompounds,
                 int&                nbSolids,
                 int&                nbShells,
                 int&                nbFaces,
                 int&                nbWires,
                 int&                nbOuterWires,
                 int&                nbInnerWires,
                 int&                nbEdges,
                 int&                nbDegenEdges,
                 int&                nbVertexes);

  asiAlgo_EXPORT static void
    ShapeSummary(const TopoDS_Shape&      shape,
                 TCollection_AsciiString& info);

  asiAlgo_EXPORT static TopoDS_Wire
    CreateCircularWire(const double radius);

  asiAlgo_EXPORT static TopoDS_Shape
    MakeSkin(const TopTools_SequenceOfShape& wires);

  asiAlgo_EXPORT static bool
    Sew(TopoDS_Shape& shape,
        const double  tolerance);

  asiAlgo_EXPORT static bool
    MaximizeFaces(TopoDS_Shape& shape);

  asiAlgo_EXPORT static bool
    InterpolatePoints(const std::vector<gp_Pnt>& points,
                      const int                  p,
                      Handle(Geom_BSplineCurve)& result);

  asiAlgo_EXPORT static TopoDS_Shape
    BooleanCut(const TopoDS_Shape& Object,
               const TopoDS_Shape& Tool,
               const double        fuzzy = 0.0);

  asiAlgo_EXPORT static TopoDS_Shape
    BooleanFuse(const TopTools_ListOfShape& objects);

  asiAlgo_EXPORT static void
    ExplodeBySolids(const TopoDS_Shape&   model,
                    TopTools_ListOfShape& solids);

  asiAlgo_EXPORT static bool
    InvertFace(const TopoDS_Face&    face,
               TopTools_ListOfShape& inverted);

  asiAlgo_EXPORT static double
    AutoSelectLinearDeflection(const TopoDS_Shape& model);

  asiAlgo_EXPORT static double
    AutoSelectAngularDeflection(const TopoDS_Shape& model);

private:

  asiAlgo_Utils(); //!< Prohibited.
  asiAlgo_Utils(const asiAlgo_Utils&); //!< Prohibited.

};

#endif
