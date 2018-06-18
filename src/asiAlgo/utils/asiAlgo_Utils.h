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
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SweptSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>

//-----------------------------------------------------------------------------

#define asiAlgo_TooSmallValue 1.0e-4
#define asiAlgo_SlashStr      "/"

//-----------------------------------------------------------------------------

//! Auxiliary functions facilitating working with OCCT topological shapes.
namespace asiAlgo_Utils
{
  //! Functions to work with strings.
  namespace Str
  {
    //! Splits the passed string by the given delimiter. Note that the
    //! passed output vector is not cleaned up beforehand.
    //! \param source_str [in]  input string to split.
    //! \param delim_str  [in]  delimiter string.
    //! \param result     [out] resulting collection of tokens after split.
    asiAlgo_EXPORT void
      Split(const std::string&        source_str,
            const std::string&        delim_str,
            std::vector<std::string>& result);

    //! Replaces all occurrences of {what} with {with} in string {str}.
    //! \param str [in/out] target string.
    //! \param what [in] sub-string to replace.
    //! \param with [in] string to replace with.
    asiAlgo_EXPORT void
      ReplaceAll(std::string&       str,
                 const std::string& what,
                 const std::string& with);

    //! Extracts substring from the passed source.
    //! \param source [in] input string to extract substring from.
    //! \param idx_F  [in] 0-based index to start extraction from (inclusively).
    //! \param length [in] number of characters to extract.
    //! \return resulting substring.
    asiAlgo_EXPORT std::string
      SubStr(const std::string& source,
             const int          idx_F,
             const int          length);

    //! Returns the passed string ensuring that it has a trailing slash.
    asiAlgo_EXPORT std::string
      Slashed(const std::string& strIN);

    //! Converts the passed value to string. This function is used to
    //! substitute std::to_string() for compilers incompatible with
    //! C++ 11.
    //! \param value [in] value to convert.
    //! \return string.
    template <typename T>
    std::string ToString(T value)
    {
      std::ostringstream os;
      os << value;
      return os.str();
    }
  };

  //! Functions for working with environment.
  namespace Env
  {
    asiAlgo_EXPORT
      std::string AsiTestData();

    asiAlgo_EXPORT
      std::string AsiTestDumping();

    asiAlgo_EXPORT
      std::string AsiTestDescr();

    asiAlgo_EXPORT
      std::string GetVariable(const char* varName);
  }

  //! Functions providing JSON representations of primitives.
  namespace JSON
  {
    asiAlgo_EXPORT void
      DumpCurve(const Handle(Geom_Curve)& curve,
                Standard_OStream&         out);

    asiAlgo_EXPORT void
      DumpSurface(const Handle(Geom_Surface)& surface,
                  Standard_OStream&           out);
  }

  //! Converts the passed double value to string with max precision.
  //! \param[in] val value to convert.
  //! \return string representation.
  asiAlgo_EXPORT std::string
    DoubleToString(const double val);

  //! Returns geometry of a face as a string label.
  //! \param face [in] face to inspect.
  //! \return label.
  asiAlgo_EXPORT std::string
    FaceGeometryName(const TopoDS_Face& face);

  //! Converts the passed named shape to string.
  asiAlgo_EXPORT TCollection_AsciiString
    NamedShapeToString(const TopoDS_Shape&           subShape,
                       const int                     globalId,
                       const Handle(asiAlgo_Naming)& naming);

  //! Converts orientation of the given shape to string.
  //! \param shape [in] shape to access orientation.
  //! \return string representation of orientation.
  asiAlgo_EXPORT TCollection_AsciiString
    OrientationToString(const TopoDS_Shape& shape);

  //! Converts the passed orientation to string.
  //! \param ori [in] orientation to convert.
  //! \return string representation of orientation.
  asiAlgo_EXPORT TCollection_AsciiString
    OrientationToString(const TopAbs_Orientation ori);

  //! Converts the passed continuity to string.
  //! \param[in] cont continuity.
  //! \return string representation of continuity.
  asiAlgo_EXPORT TCollection_AsciiString
    ContinuityToString(const GeomAbs_Shape cont);

  //! Converts the passed location to string.
  //! \param loc [in] location to convert.
  //! \return string representation of location.
  asiAlgo_EXPORT TCollection_AsciiString
    LocationToString(const TopLoc_Location& loc);

  //! Returns human-readable curve name.
  //! \param curve [in] curve to inspect.
  //! \return curve name.
  asiAlgo_EXPORT std::string
    CurveName(const Handle(Geom_Curve)& curve);

  //! Returns human-readable surface name.
  //! \param surf [in] surface to inspect.
  //! \return surface name.
  asiAlgo_EXPORT std::string
    SurfaceName(const Handle(Geom_Surface)& surf);

  //! Returns shape type as string.
  //! \param shapeType [in] shape type in question.
  //! \return type as string.
  asiAlgo_EXPORT std::string
    ShapeTypeStr(const TopAbs_ShapeEnum& shapeType);

  //! Returns shape type as string.
  //! \param shape [in] shape to get type string for.
  //! \return type string.
  asiAlgo_EXPORT std::string
    ShapeTypeStr(const TopoDS_Shape& shape);

  //! Returns shape address as string with a prefix indicating its type,
  //! e.g. "e05FB0" is an edge and "f780A1" is a face.
  //! \param shape [in] shape to extract the address for.
  //! \return address of TShape as string prefixed by type indicator.
  asiAlgo_EXPORT std::string
    ShapeAddrWithPrefix(const TopoDS_Shape& shape);

  //! Returns TShape address as string.
  //! \param tshape [in] TShape pointer to extract the address for.
  //! \return address of TShape as string.
  asiAlgo_EXPORT std::string
    ShapeAddr(const Handle(TopoDS_TShape)& tshape);

  //! Returns TShape address as string.
  //! \param shape [in] shape to extract the address for.
  //! \return address of TShape as string.
  asiAlgo_EXPORT std::string
    ShapeAddr(const TopoDS_Shape& shape);

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

  //! Checks whether the passed shape is empty.
  asiAlgo_EXPORT bool
    IsEmptyShape(const TopoDS_Shape& shape);

  //! Checks whether the curve or its basic curve is circular.
  //! \param[in] curve curve to check.
  //! \return true/false.
  asiAlgo_EXPORT bool
    IsBasisCircular(const Handle(Geom_Curve)& curve);

  //! Applies the passed transformation to the given shape. Returns another shape
  //! as a result (no deep copy of geometry is performed, only location is
  //! changed).
  //! \param theShape  [in] shape to apply transformation to.
  //! \param theXPos   [in] X position.
  //! \param theYPos   [in] Y position.
  //! \param theZPos   [in] Z position.
  //! \param theAngleA [in] rotation angle A.
  //! \param theAngleB [in] rotation angle B.
  //! \param theAngleC [in] rotation angle C.
  //! \return relocated shape.
  asiAlgo_EXPORT TopoDS_Shape
    ApplyTransformation(const TopoDS_Shape& theShape,
                        const double        theXPos,
                        const double        theYPos,
                        const double        theZPos,
                        const double        theAngleA,
                        const double        theAngleB,
                        const double        theAngleC,
                        const bool          doCopy);

  //! Returns OCCT transformation structure for the given elemental
  //! transformations.
  //! \param theXPos   [in] X position.
  //! \param theYPos   [in] Y position.
  //! \param theZPos   [in] Z position.
  //! \param theAngleA [in] rotation angle A.
  //! \param theAngleB [in] rotation angle B.
  //! \param theAngleC [in] rotation angle C.
  //! \return OCCT transformation structure.
  asiAlgo_EXPORT gp_Trsf
    Transformation(const double theXPos,
                   const double theYPos,
                   const double theZPos,
                   const double theAngleA,
                   const double theAngleB,
                   const double theAngleC);

  //! Applies the passed transformation to the given shape.
  //! \param theShape     [in] shape to transform.
  //! \param theTransform [in] transformation to apply.
  //! \param doCopy       [in] indicates whether to construct a deep copy.
  //! \return transformed shape.
  asiAlgo_EXPORT TopoDS_Shape
    ApplyTransformation(const TopoDS_Shape& theShape,
                        const gp_Trsf&      theTransform,
                        const bool          doCopy);

  //! Creates a compound from the given list of shapes. If the list contains
  //! only one not-null shape, this single shape is returned as-is.
  //! \param[in] shapes source shapes.
  //! \return resulting compound.
  asiAlgo_EXPORT TopoDS_Shape
    AssembleShapes(const TopTools_ListOfShape& shapes);

  //! Creates a compound from the given sequence of shapes. If the sequence
  //! contains only one not-null shape, this single shape is returned as-is.
  //! If the sequence is NULL or empty, then null shape is returned.
  //! \param[in] shapes source shapes.
  //! \return resulting compound.
  asiAlgo_EXPORT TopoDS_Shape
    AssembleShapes(const Handle(TopTools_HSequenceOfShape)& shapes);

  //! Calculates bounding box for the given shape.
  //! \param shape     [in]  input shape.
  //! \param XMin      [out] min X.
  //! \param YMin      [out] min Y.
  //! \param ZMin      [out] min Z.
  //! \param XMax      [out] max X.
  //! \param YMax      [out] max Y.
  //! \param ZMax      [out] max Z.
  //! \param tolerance [in]  tolerance to enlarge the bounding box with.
  //! \return false if bounding box is void.
  asiAlgo_EXPORT bool
    Bounds(const TopoDS_Shape& shape,
           double& XMin, double& YMin, double& ZMin,
           double& XMax, double& YMax, double& ZMax,
           const double tolerance = 0.0);

  //! Calculates bounding box for the given triangulation.
  //! \param tris      [in]  input triangulation.
  //! \param XMin      [out] min X.
  //! \param YMin      [out] min Y.
  //! \param ZMin      [out] min Z.
  //! \param XMax      [out] max X.
  //! \param YMax      [out] max Y.
  //! \param ZMax      [out] max Z.
  //! \param tolerance [in]  tolerance to enlarge the bounding box with.
  //! \return false if bounding box is void.
  asiAlgo_EXPORT bool
    Bounds(const Handle(Poly_Triangulation)& tris,
           double& XMin, double& YMin, double& ZMin,
           double& XMax, double& YMax, double& ZMax,
           const double tolerance = 0.0);

  //! Checks OCCT validity rules of the given shape.
  //! \param shape   [in] shape to check.
  //! \param Journal [in] Logger instance to cumulate all meaningful messages.
  //! \return true if shape is valid, false -- otherwise.
  asiAlgo_EXPORT bool
    CheckShape(const TopoDS_Shape&  shape,
               ActAPI_ProgressEntry Journal);

  //! Checks whether the passed face has all contours (wires)
  //! geometrically closed.
  //! \param face             [in] face to check.
  //! \param coincConfusion3d [in] coincidence confusion tolerance. This value
  //!                              is used to recognize points as coincident.
  //! \return true if face is Ok, false -- otherwise.
  asiAlgo_EXPORT bool
    HasAllClosedWires(const TopoDS_Face& face,
                      const double       coincConfusion3d);

  //! Checks whether the passed wire is closed.
  //! \param wire             [in] wire to check.
  //! \param face             [in] face owning the wire.
  //! \param coincConfusion3d [in] coincidence confusion tolerance. This value
  //!                              is used to recognize points as coincident.
  //! \return true if wire is Ok, false -- otherwise.
  asiAlgo_EXPORT bool
    IsClosedGeometrically(const TopoDS_Wire& wire,
                          const TopoDS_Face& face,
                          const double       coincConfusion3d);

  //! Checks whether the passed face contains any edges without vertices.
  //! \param face [in] face to check.
  asiAlgo_EXPORT bool
    HasEdgesWithoutVertices(const TopoDS_Face& face);

  //! Returns maximum tolerance value bound to the passed shape.
  //! \param shape [in] shape to check.
  //! \return maximum tolerance value.
  asiAlgo_EXPORT double
    MaxTolerance(const TopoDS_Shape& shape);

  //! Reads CAD model from native OCCT b-rep file.
  //! \param filename [in]  filename.
  //! \param shape    [out] CAD model retrieved from file.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    ReadBRep(const TCollection_AsciiString& filename,
             TopoDS_Shape&                  shape);

  //! Writes shape to B-Rep format.
  //! \param theShape    [in] shape to write.
  //! \param theFilename [in] filename.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    WriteBRep(const TopoDS_Shape&            shape,
              const TCollection_AsciiString& filename);

  //! Collects summary information for the given shape.
  //! \param shape        [in]  input shape.
  //! \param nbCompsolids [out] number of compsolids.
  //! \param nbCompounds  [out] number of compounds.
  //! \param nbSolids     [out] number of solids.
  //! \param nbShells     [out] number of shells.
  //! \param nbFaces      [out] number of faces.
  //! \param nbWires      [out] number of wires.
  //! \param nbOuterWires [out] number of outer wires.
  //! \param nbInnerWires [out] number of inner wires.
  //! \param nbEdges      [out] number of edges.
  //! \param nbVertexes   [out] number of vertices.
  asiAlgo_EXPORT void
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

  //! Collects summary information of the given shape: returns the number
  //! of sub-shapes of each type.
  //! \param shape [in]  shape to analyze.
  //! \param info  [out] shape summary as string.
  asiAlgo_EXPORT void
    ShapeSummary(const TopoDS_Shape&      shape,
                 TCollection_AsciiString& info);

  //! Creates a circular wire with the given radius.
  //! \param radius [in] radius of the host circle.
  //! \return created wire.
  asiAlgo_EXPORT TopoDS_Wire
    CreateCircularWire(const double radius);

  //! Skins a surface through the passed sections.
  //! \param wires [in] sections to skin.
  //! \return skinning result.
  asiAlgo_EXPORT TopoDS_Shape
    MakeSkin(const TopTools_SequenceOfShape& wires);

  //! Performs sewing.
  //! \param[in] shape     shape to sew.
  //! \param[in] tolerance sewing tolerance.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Sew(const TopoDS_Shape& shape,
        const double        tolerance,
        TopoDS_Shape&       result);

  //! Performs "same domain" expansion on faces and edges.
  //! \param shape [in/out] shape to modify.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    MaximizeFaces(TopoDS_Shape& shape);

  //! Interpolates the given collection of points with B-curve of the
  //! desired degree.
  //! \param points [in]  points to interpolate.
  //! \param p      [in]  degree to use.
  //! \param result [out] interpolant.
  //! \return true om case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    InterpolatePoints(const std::vector<gp_Pnt>& points,
                      const int                  p,
                      Handle(Geom_BSplineCurve)& result);

  asiAlgo_EXPORT bool
    InterpolatePoints(const Handle(asiAlgo_BaseCloud<double>)& points,
                      const int                                p,
                      Handle(Geom_BSplineCurve)&               result);

  //! Performs Boolean Cut of a tool shape from the given object shape.
  //! \param Object [in] object to cut from.
  //! \param Tool   [in] tool to cut out.
  //! \param fuzzy  [in] fuzzy tolerance.
  //! \return result shape.
  asiAlgo_EXPORT TopoDS_Shape
    BooleanCut(const TopoDS_Shape& Object,
               const TopoDS_Shape& Tool,
               const double        fuzz = 0.0);

  asiAlgo_EXPORT TopoDS_Shape
    BooleanCut(const TopoDS_Shape&         Object,
               const TopTools_ListOfShape& Tools,
               const bool                  isParallel,
               const double                fuzz = 0.0);

  asiAlgo_EXPORT TopoDS_Shape
    BooleanCut(const TopoDS_Shape&         Object,
               const TopTools_ListOfShape& Tools,
               const bool                  isParallel,
               const double                fuzz,
               BRepAlgoAPI_Cut&            API);

  asiAlgo_EXPORT TopoDS_Shape
    BooleanFuse(const TopTools_ListOfShape& objects);

  asiAlgo_EXPORT TopoDS_Shape
    BooleanIntersect(const TopTools_ListOfShape& objects);

  asiAlgo_EXPORT TopoDS_Shape
    BooleanGeneralFuse(const TopTools_ListOfShape& objects,
                       const double                fuzz,
                       BOPAlgo_Builder&            API);

  asiAlgo_EXPORT TopoDS_Shape
    BooleanGeneralFuse(const TopTools_ListOfShape& objects,
                       const double                fuzz);

  asiAlgo_EXPORT bool
    BooleanRemoveFaces(const TopoDS_Shape&  shape,
                       const TopoDS_Shape&  face2Remove,
                       const bool           runParallel,
                       const bool           trackHistory,
                       TopoDS_Shape&        result,
                       ActAPI_ProgressEntry progress);

  asiAlgo_EXPORT bool
    BooleanRemoveFaces(const TopoDS_Shape&         shape,
                       const TopTools_ListOfShape& faces2Remove,
                       const bool                  runParallel,
                       const bool                  trackHistory,
                       TopoDS_Shape&               result,
                       ActAPI_ProgressEntry        progress);

  asiAlgo_EXPORT void
    ExplodeBySolids(const TopoDS_Shape&   model,
                    TopTools_ListOfShape& solids);

  asiAlgo_EXPORT bool
    InvertFace(const TopoDS_Face&    face,
               TopTools_ListOfShape& inverted);

  asiAlgo_EXPORT double
    AutoSelectLinearDeflection(const TopoDS_Shape& model);

  asiAlgo_EXPORT double
    AutoSelectAngularDeflection(const TopoDS_Shape& model);

  //! Constructs B-spline curve of degree 1 from the passed poles.
  //! \param[in] trace poles of the curve to construct.
  //! \return 1-degree spline curve.
  asiAlgo_EXPORT Handle(Geom_BSplineCurve)
    PolylineAsSpline(const std::vector<gp_XYZ>& trace);

  //! Constructs 2D B-spline curve of degree 1 from the passed poles.
  //! \param[in] trace poles of the curve to construct.
  //! \return 1-degree 2D spline curve.
  asiAlgo_EXPORT Handle(Geom2d_BSplineCurve)
    PolylineAsSpline(const std::vector<gp_XY>& trace);

  //! Checks whether the given shape contains the given sub-shape. This method
  //! builds a map of sub-shapes, so it is quite slow.
  //! \param[in] shape    master shape to check.
  //! \param[in] subShape sub-shape in question.
  //! \return true/false.
  asiAlgo_EXPORT bool
    Contains(const TopoDS_Shape& shape,
             const TopoDS_Shape& subShape);

  asiAlgo_EXPORT TopoDS_Shape
    GetImage(const TopoDS_Shape&       source,
             BRepBuilderAPI_MakeShape& API);

  asiAlgo_EXPORT Handle(Poly_Triangulation)
    CreateTriangle(const gp_Pnt& P0,
                   const gp_Pnt& P1,
                   const gp_Pnt& P2);

  asiAlgo_EXPORT void
    HexagonPoles(const gp_XY& center,
                 const double dist2Pole,
                 gp_XY&       P1,
                 gp_XY&       P2,
                 gp_XY&       P3,
                 gp_XY&       P4,
                 gp_XY&       P5,
                 gp_XY&       P6);

  //! Constructs a two-dimensional regular polygon.
  //! \param[in]  center    center point.
  //! \param[in]  dist2Pole radius of circumcircle.
  //! \param[in]  numPoles  number of poles in the resulting regular polygon.
  //! \param[out] poles     calculated poles.
  asiAlgo_EXPORT void
    PolygonPoles(const gp_XY&        center,
                 const double        dist2Pole,
                 const int           numPoles,
                 std::vector<gp_XY>& poles);

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
  asiAlgo_EXPORT bool
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
  asiAlgo_EXPORT bool
    GetFaceAnyInteriorPoint(const TopoDS_Face& face,
                            gp_Pnt2d&          uv,
                            gp_Pnt&            xyz);

  //! Prints some useful analytics for the passed surface to the given
  //! output stream.
  //! \param[in]  surf parametric surface in question.
  //! \param[out] out  output stream.
  asiAlgo_EXPORT void
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
  asiAlgo_EXPORT bool
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
  asiAlgo_EXPORT bool
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
  asiAlgo_EXPORT bool
    ReparametrizeBSpl(const Handle(Geom2d_Curve)&  curve,
                      const double                 newFirst,
                      const double                 newLast,
                      const bool                   toCopy,
                      Handle(Geom2d_BSplineCurve)& result);

  //! This function is designed to evaluate curvature of the surface along the
  //! the given edge. It takes the tangency direction of the edge's curve for
  //! the passed parameter value and calculates the normal curvature at the
  //! corresponding point on surface.
  //! \param[in]  face face of interest.
  //! \param[in]  edge edge of interest.
  //! \param[in]  t    parameter on the edge's curve to evaluate curvature for.
  //! \param[out] k    evaluated curvature.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    EvaluateAlongCurvature(const TopoDS_Face& face,
                           const TopoDS_Edge& edge,
                           const double       t,
                           double&            k);

  //! This function is designed to evaluate curvature of the surface along the
  //! the given edge. It takes the tangency direction of the edge's curve for
  //! the middle parameter value and calculates the normal curvature at the
  //! corresponding point on surface.
  //! \param[in]  face face of interest.
  //! \param[in]  edge edge of interest.
  //! \param[out] k    evaluated curvature.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    EvaluateAlongCurvature(const TopoDS_Face& face,
                           const TopoDS_Edge& edge,
                           double&            k);

  //! Calculates curvature in the middle of the passed curve.
  //! \param[in]  curve  curve in question.
  //! \param[out] P      point on curve.
  //! \param[out] T      tangent vector on curve.
  //! \param[out] k      evaluated curvature.
  //! \param[out] r      equivalent curvature radius.
  //! \param[out] center center of curvature.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    CalculateMidCurvature(const Handle(Geom_Curve)& curve,
                          gp_Pnt&                   P,
                          gp_Dir&                   T,
                          double&                   k,
                          double&                   r,
                          gp_Pnt&                   center);

  //! Calculates curvature in the middle of the passed curve.
  //! \param[in]  curve  curve in question.
  //! \param[out] k      evaluated curvature.
  //! \param[out] r      equivalent curvature radius.
  //! \param[out] center center of curvature.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    CalculateMidCurvature(const Handle(Geom_Curve)& curve,
                          double&                   k,
                          double&                   r,
                          gp_Pnt&                   center);

  //! Calculates curvature in the middle of the passed curve.
  //! \param[in]  curve curve in question.
  //! \param[out] k     evaluated curvature.
  //! \param[out] r     equivalent curvature radius.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    CalculateMidCurvature(const Handle(Geom_Curve)& curve,
                          double&                   k,
                          double&                   r);

};

#endif
