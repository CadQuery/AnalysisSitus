//-----------------------------------------------------------------------------
// Created on: 20 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sergey Slyadnev nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiAlgo_Utils_h
#define asiAlgo_Utils_h

// asiAlgo includes
#include <asiAlgo_BaseCloud.h>
#include <asiAlgo_Naming.h>

// Active Data (API) includes
#include <ActAPI_ILogger.h>
#include <ActAPI_IProgressNotifier.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Cut.hxx>
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
#include <Geom2d_BSplineCurve.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
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

  //! Splits the passed string by the given delimiter. Note that the
  //! passed output vector is not cleaned up beforehand.
  //! \param source_str [in]  input string to split.
  //! \param delim_str  [in]  delimiter string.
  //! \param result     [out] resulting collection of tokens after split.
  static void SplitStr(const std::string&        source_str,
                       const std::string&        delim_str,
                       std::vector<std::string>& result)
  {
    // Initialize collection of strings to split
    std::vector<std::string> chunks;
    chunks.push_back(source_str);

    // Split by each delimiter consequently
    for ( size_t delim_idx = 0; delim_idx < delim_str.length(); ++delim_idx )
    {
      std::vector<std::string> new_chunks;
      const char delim = delim_str[delim_idx];

      // Split each chunk
      for ( size_t chunk_idx = 0; chunk_idx < chunks.size(); ++chunk_idx )
      {
        const std::string& source = chunks[chunk_idx];
        std::string::size_type currPos = 0, prevPos = 0;
        while ( (currPos = source.find(delim, prevPos) ) != std::string::npos )
        {
          std::string item = source.substr(prevPos, currPos - prevPos);
          if ( item.size() > 0 )
          {
            new_chunks.push_back(item);
          }
          prevPos = currPos + 1;
        }
        new_chunks.push_back( source.substr(prevPos) );
      }

      // Set new collection of chunks for splitting by the next delimiter
      chunks = new_chunks;
    }

    // Set result
    result = chunks;
  }

  //! Extracts substring from the passed source.
  //! \param source [in] input string to extract substring from.
  //! \param idx_F  [in] 0-based index to start extraction from (inclusively).
  //! \param length [in] number of characters to extract.
  //! \return resulting substring.
  static std::string SubStr(const std::string& source,
                            const int          idx_F,
                            const int          length)
  {
    return source.substr(idx_F, length);
  }

  //! Converts the passed value to string. This function is used to
  //! substitute std::to_string() for compilers incompatible with
  //! C++ 11.
  //! \param value [in] value to convert.
  //! \return string.
  template <typename T>
  static std::string ToString(T value)
  {
    std::ostringstream os;
    os << value;
    return os.str();
  }

  //! Returns geometry of a face as a string label.
  //! \param face [in] face to inspect.
  //! \return label.
  static std::string FaceGeometryName(const TopoDS_Face& face)
  {
    Handle(Geom_Surface) surf = BRep_Tool::Surface(face);
    return SurfaceName(surf);
  }

  //! Converts the passed named shape to string.
  static TCollection_AsciiString
    NamedShapeToString(const TopoDS_Shape&           subShape,
                       const int                     globalId,
                       const Handle(asiAlgo_Naming)& naming)
  {
    TCollection_AsciiString msg("Sub-shape: ");
    msg += ShapeTypeStr(subShape).c_str();
    msg += "\n\t Global ID: "; msg += globalId;
    msg += "\n\t Address: ";   msg += ShapeAddr(subShape).c_str();
    //
    if ( !naming.IsNull() )
    {
      msg += "\n\t Name: ";

      TCollection_AsciiString label;
      naming->FindName(subShape, label);
      //
      if ( !label.IsEmpty() )
        msg += label;
      else
        msg += "<empty>";
    }

    return msg;
  }

  //! Converts orientation of the given shape to string.
  //! \param shape [in] shape to access orientation.
  //! \return string representation of orientation.
  static TCollection_AsciiString
    OrientationToString(const TopoDS_Shape& shape)
  {
    return OrientationToString( shape.Orientation() );
  }

  //! Converts the passed orientation to string.
  //! \param ori [in] orientation to convert.
  //! \return string representation of orientation.
  static TCollection_AsciiString
    OrientationToString(const TopAbs_Orientation ori)
  {
    TCollection_AsciiString oriStr;

    // Check orientation.
    if ( ori == TopAbs_FORWARD )
      oriStr = "forward";
    else if ( ori == TopAbs_REVERSED )
      oriStr = "reversed";
    else if ( ori == TopAbs_INTERNAL )
      oriStr = "internal";
    else if ( ori == TopAbs_EXTERNAL )
      oriStr = "external";

    return oriStr;
  }

  //! Converts the passed continuity to string.
  //! \param[in] cont continuity.
  //! \return string representation of continuity.
  static TCollection_AsciiString
    ContinuityToString(const GeomAbs_Shape cont)
  {
    TCollection_AsciiString contStr;

    // Check continuity.
    switch ( cont )
    {
      case GeomAbs_C0 : contStr = "C0"; break;
      case GeomAbs_C1 : contStr = "C1"; break;
      case GeomAbs_C2 : contStr = "C2"; break;
      case GeomAbs_C3 : contStr = "C3"; break;
      case GeomAbs_CN : contStr = "CN"; break;
      case GeomAbs_G1 : contStr = "G1"; break;
      case GeomAbs_G2 : contStr = "G2"; break;
      default: break;
    }

    return contStr;
  }

  //! Converts the passed location to string.
  //! \param loc [in] location to convert.
  //! \return string representation of location.
  static TCollection_AsciiString
    LocationToString(const TopLoc_Location& loc)
  {
    const gp_Trsf& T      = loc.Transformation();
    gp_Mat         T_roto = T.VectorialPart();
    const gp_XYZ&  T_move = T.TranslationPart();
    gp_TrsfForm    T_form = T.Form();

    TCollection_AsciiString result;
    result += "\n---\nTransformation: ";
    if ( T_form == gp_Identity )
      result += "identity";
    else if ( T_form == gp_Rotation )
      result += "rotation";
    else if ( T_form == gp_Translation )
      result += "translation";
    else if ( T_form == gp_PntMirror )
      result += "point mirror (central symmetry)";
    else if ( T_form == gp_Ax1Mirror )
      result += "axis mirror (rotational symmetry)";
    else if ( T_form == gp_Ax2Mirror )
      result += "plane mirror (bilateral symmetry)";
    else if ( T_form == gp_Scale )
      result += "scaling";
    else if ( T_form == gp_CompoundTrsf )
      result += "combination of orthogonal transformations";
    else
      result += "non-orthogonal transformation";
    //
    result += "\n---\n";
    result += T_roto(1, 1); result += " "; result += T_roto(1, 2); result += " "; result += T_roto(1, 3); result += "\n";
    result += T_roto(2, 1); result += " "; result += T_roto(2, 2); result += " "; result += T_roto(2, 3); result += "\n";
    result += T_roto(3, 1); result += " "; result += T_roto(3, 2); result += " "; result += T_roto(3, 3);
    result += "\n---\n";
    result += T_move.X(); result += " "; result += T_move.Y(); result += " "; result += T_move.Z();

    return result;
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

  //! Returns shape type as string.
  //! \param shapeType [in] shape type in question.
  //! \return type as string.
  static std::string ShapeTypeStr(const TopAbs_ShapeEnum& shapeType)
  {
    std::string name;
    if ( shapeType == TopAbs_COMPOUND )
      name = "COMPOUND";
    else if ( shapeType == TopAbs_COMPSOLID )
      name = "COMPSOLID";
    else if ( shapeType == TopAbs_SOLID )
      name = "SOLID";
    else if ( shapeType == TopAbs_SHELL )
      name = "SHELL";
    else if ( shapeType == TopAbs_FACE )
      name = "FACE";
    else if ( shapeType == TopAbs_WIRE )
      name = "WIRE";
    else if ( shapeType == TopAbs_EDGE )
      name = "EDGE";
    else if ( shapeType == TopAbs_VERTEX )
      name = "VERTEX";
    else
      name = "SHAPE";

    return name;
  }

  //! Returns shape type as string.
  //! \param shape [in] shape to get type string for.
  //! \return type string.
  static std::string ShapeTypeStr(const TopoDS_Shape& shape)
  {
    return ShapeTypeStr( shape.ShapeType() );
  }

  //! Returns shape address as string with a prefix indicating its type,
  //! e.g. "e05FB0" is an edge and "f780A1" is a face.
  //! \param shape [in] shape to extract the address for.
  //! \return address of TShape as string prefixed by type indicator.
  static std::string ShapeAddrWithPrefix(const TopoDS_Shape& shape)
  {
    std::string addr_str = ShapeAddr    ( shape.TShape() );
    std::string prefix   = ShapeTypeStr ( shape );

    // Notice extra spacing for better visualization
    return prefix + " [" + addr_str + "]";
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
      addr_str = SubStr( addr_str, (int) pos, (int) (addr_str.size() - pos) );

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

  static bool IsEmptyShape(const TopoDS_Shape& shape)
  {
    if ( shape.IsNull() )
      return true;

    if ( shape.ShapeType() >= TopAbs_FACE )
      return false;

    int numSubShapes = 0;
    for ( TopoDS_Iterator it(shape); it.More(); it.Next() )
      numSubShapes++;

    return numSubShapes == 0;
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
    Bounds(const Handle(Poly_Triangulation)& tris,
           double& XMin, double& YMin, double& ZMin,
           double& XMax, double& YMax, double& ZMax,
           const double tolerance = 0.0);

  asiAlgo_EXPORT static bool
    CheckShape(const TopoDS_Shape&  shape,
               ActAPI_ProgressEntry Journal);

  asiAlgo_EXPORT static bool
    HasAllClosedWires(const TopoDS_Face& face,
                      const double       coincConfusion3d);

  asiAlgo_EXPORT static bool
    IsClosedGeometrically(const TopoDS_Wire& wire,
                          const TopoDS_Face& face,
                          const double       coincConfusion3d);

  asiAlgo_EXPORT static bool
    HasEdgesWithoutVertices(const TopoDS_Face& face);

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
               const double        fuzz = 0.0);

  asiAlgo_EXPORT static TopoDS_Shape
    BooleanCut(const TopoDS_Shape&         Object,
               const TopTools_ListOfShape& Tools,
               const bool                  isParallel,
               const double                fuzz = 0.0);

  asiAlgo_EXPORT static TopoDS_Shape
    BooleanCut(const TopoDS_Shape&         Object,
               const TopTools_ListOfShape& Tools,
               const bool                  isParallel,
               const double                fuzz,
               BRepAlgoAPI_Cut&            API);

  asiAlgo_EXPORT static TopoDS_Shape
    BooleanFuse(const TopTools_ListOfShape& objects);

  asiAlgo_EXPORT static TopoDS_Shape
    BooleanGeneralFuse(const TopTools_ListOfShape& objects,
                       const double                fuzz,
                       BOPAlgo_Builder&            API);

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

  asiAlgo_EXPORT static Handle(Geom_BSplineCurve)
    PolylineAsSpline(const std::vector<gp_XYZ>& trace);

  asiAlgo_EXPORT static Handle(Geom2d_BSplineCurve)
    PolylineAsSpline(const std::vector<gp_XY>& trace);

  asiAlgo_EXPORT static bool
    Contains(const TopoDS_Shape& shape,
             const TopoDS_Shape& subShape);

  asiAlgo_EXPORT static TopoDS_Shape
    GetImage(const TopoDS_Shape&       source,
             BRepBuilderAPI_MakeShape& API);

  asiAlgo_EXPORT static Handle(Poly_Triangulation)
    CreateTriangle(const gp_Pnt& P0,
                   const gp_Pnt& P1,
                   const gp_Pnt& P2);

  asiAlgo_EXPORT static void
    HexagonPoles(const gp_XY& center,
                 const double dist2Pole,
                 gp_XY&       P1,
                 gp_XY&       P2,
                 gp_XY&       P3,
                 gp_XY&       P4,
                 gp_XY&       P5,
                 gp_XY&       P6);

  //! Calculates normal vectors at interior face points. This method uses
  //! point membership classification to ensure that inner points only are
  //! used.
  //!
  //! CAUTION: Face orientation is not taken into account. The returned
  //!          normal vectors represent geometric orientation of
  //!          the face (i.e., the cross product of its partials).
  //!
  //! \param[in]  face    topological face in question.
  //! \param[out] test    points (internal or lying on the boundary).
  //! \param[out] vectors computed normal field.
  //!
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT static bool
    CalculateFaceNormals(const TopoDS_Face&                face,
                         Handle(asiAlgo_BaseCloud<float>)& points,
                         Handle(asiAlgo_BaseCloud<float>)& vectors);

  //! This function takes "just any" interior point on a face.
  //!
  //! CAUTION: This function is not super-fast as it follows generate-and-test
  //!          approach, i.e., it samples many points and selects the one lying
  //!          INSIDE the face. Therefore, do not use this method in heavy loops.
  //!
  //! \param[in]  face topological face in question.
  //! \param[out] uv   UV coordinates of the selected point.
  //! \param[out] xyz  Cartesian coordinates of the selected point in the 
  //!                  modeling space.
  //!
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT static bool
    GetFaceAnyInteriorPoint(const TopoDS_Face& face,
                            gp_Pnt2d&          uv,
                            gp_Pnt&            xyz);

  //! Prints some useful analytics for the passed surface to the given
  //! output stream.
  //! \param[in]  surf parametric surface in question.
  //! \param[out] out  output stream.
  asiAlgo_EXPORT static void
    PrintSurfaceDetails(const Handle(Geom_Surface)& surf,
                        Standard_OStream&           out);

  //! Calculates curvature comb for the passed curve at the given parameter
  //! value. The method follows the paper "Farin, G. 2016. Curvature combs
  //! and curvature plots. CAD Computer Aided Design 80, 6–8."
  //!
  //! \param[in]  curve    parametric curve to analyze.
  //! \param[in]  u        parameter value.
  //! \param[in]  curvAmpl curvature amplification factor used to bring out
  //!                      salient features of the comb.
  //! \param[out] p        point on curve at the given parameter value.
  //! \param[out] k        curvature at the given parameter value.
  //! \param[out] c        curvature comb.
  //!
  //! \return false if a comb cannot be calculated. NOTE: the output argument
  //!         <p> will be calculated anyway.
  asiAlgo_EXPORT static bool
    CalculateCurvatureComb(const Handle(Geom_Curve)& curve,
                           const double              u,
                           const double              curvAmpl,
                           gp_Pnt&                   p,
                           double&                   k,
                           gp_Vec&                   c);

  //! Calculates curvature combs along the given curve.
  //!
  //! \param[in]  curve      parametric curve to analyze.
  //! \param[in]  f          first parameter of the range of interest.
  //! \param[in]  l          last parameter of the range of interest.
  //! \param[in]  numPts     number of discretization points.
  //! \param[in]  curvAmpl   curvature amplification factor used to bring out
  //!                        salient features of the comb.
  //! \param[out] points     discretization points.
  //! \param[out] params     discretization parameters.
  //! \param[out] curvatures curvatures.
  //! \param[out] combs      curvature combs.
  //! \param[out] combsOk    statuses of combs calculation.
  //!
  //! \return false if combs calculation is not possible on the passed data.
  asiAlgo_EXPORT static bool
    CalculateCurvatureCombs(const Handle(Geom_Curve)& curve,
                            const double              f,
                            const double              l,
                            const int                 numPts,
                            const double              curvAmpl,
                            std::vector<gp_Pnt>&      points,
                            std::vector<double>&      params,
                            std::vector<double>&      curvatures,
                            std::vector<gp_Vec>&      combs,
                            std::vector<bool>&        combsOk);

  //! Reparametrizes the given B-spline curve applying the passed new range
  //! for its knot vector.
  //!
  //! \param[in]  curve    B-spline curve to reparametrize.
  //! \param[in]  newFirst new first parameter value.
  //! \param[in]  newLast  new last parameter value.
  //! \param[in]  toCopy   indicates whether to prepare a copy of the
  //!                      input curve instead of directly modifying it.
  //! \param[out] result   reparametrized curve.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT static bool
    ReparametrizeBSpl(const Handle(Geom2d_Curve)&  curve,
                      const double                 newFirst,
                      const double                 newLast,
                      const bool                   toCopy,
                      Handle(Geom2d_BSplineCurve)& result);

private:

  asiAlgo_Utils() = delete;
  asiAlgo_Utils(const asiAlgo_Utils&) = delete;

};

#endif
