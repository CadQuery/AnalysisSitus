//-----------------------------------------------------------------------------
// Created on: 20 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_utils.h>

// Common includes
#include <common_draw_test_suite.h>

// Feature includes
#include <feature_merge_faces.h>

// OCCT includes
#include <Bnd_Box.hxx>
#include <BOPAlgo_BOP.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BRep_Builder.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepCheck_Status.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepTools.hxx>
#include <GC_MakeCircle.hxx>
#include <GeomConvert.hxx>
#include <gp_Circ.hxx>
#include <gp_Quaternion.hxx>
#include <gp_Vec.hxx>
#include <math_Matrix.hxx>
#include <NCollection_IncAllocator.hxx>
#include <Precision.hxx>
#include <ShapeUpgrade_UnifySameDomain.hxx>
#include <StlAPI_Reader.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

// Eigen includes
#include <Eigen/Dense>

//-----------------------------------------------------------------------------

#undef DUMP_COUT
#undef DUMP_FILES

#define dump_filename_N  "D:\\N_interp_log_OCCT.log"
#define dump_filename_Bx "D:\\Bx_interp_log_OCCT.log"
#define dump_filename_By "D:\\By_interp_log_OCCT.log"
#define dump_filename_Bz "D:\\Bz_interp_log_OCCT.log"

//-----------------------------------------------------------------------------
// Auxiliary functions
//-----------------------------------------------------------------------------

//! Auxiliary toolkit for shape validity checker.
namespace CheckShapeAux
{

  //! Checks whether the given list of shapes contains the passed shape.
  //! \param L [in] list to check.
  //! \param S [in] entry to find.
  //! \return true if shape was found, false -- otherwise.
  bool Contains(const TopTools_ListOfShape& L,
                const TopoDS_Shape&         S)
  {
    TopTools_ListIteratorOfListOfShape it;
    for ( it.Initialize(L); it.More(); it.Next() )
    {
      if ( it.Value().IsSame(S) )
        return true;
    }
    return false;
  }

  //! Extracted from OCCT Draw Test Harness for "checkshape" command. This
  //! function is used for preparation of output for invalid shapes.
  //! \param stat       [in]  OCCT topological check status.
  //! \param NbProblems [out] output array.
  void FillProblems(const BRepCheck_Status            stat,
                    Handle(TColStd_HArray1OfInteger)& NbProblems)
  {
    switch ( stat )
    {
    case BRepCheck_InvalidPointOnCurve:
      NbProblems->SetValue(1, NbProblems->Value(1) + 1); break;
    case BRepCheck_InvalidPointOnCurveOnSurface:
      NbProblems->SetValue(2, NbProblems->Value(2) + 1); break;
    case BRepCheck_InvalidPointOnSurface:
      NbProblems->SetValue(3, NbProblems->Value(3) + 1); break;
    case BRepCheck_No3DCurve:
      NbProblems->SetValue(4, NbProblems->Value(4) + 1); break;
    case BRepCheck_Multiple3DCurve:
      NbProblems->SetValue(5, NbProblems->Value(5) + 1); break;
    case BRepCheck_Invalid3DCurve:
      NbProblems->SetValue(6, NbProblems->Value(6) + 1); break;
    case BRepCheck_NoCurveOnSurface:
      NbProblems->SetValue(7, NbProblems->Value(7) + 1); break;
    case BRepCheck_InvalidCurveOnSurface:
      NbProblems->SetValue(8, NbProblems->Value(8) + 1); break;
    case BRepCheck_InvalidCurveOnClosedSurface:
      NbProblems->SetValue(9, NbProblems->Value(9) + 1); break;
    case BRepCheck_InvalidSameRangeFlag:
      NbProblems->SetValue(10, NbProblems->Value(10) + 1); break;
    case BRepCheck_InvalidSameParameterFlag:
      NbProblems->SetValue(11, NbProblems->Value(11) + 1); break;
    case BRepCheck_InvalidDegeneratedFlag:
      NbProblems->SetValue(12, NbProblems->Value(12) + 1); break;
    case BRepCheck_FreeEdge:
      NbProblems->SetValue(13, NbProblems->Value(13) + 1); break;
    case BRepCheck_InvalidMultiConnexity:
      NbProblems->SetValue(14, NbProblems->Value(14) + 1); break;
    case BRepCheck_InvalidRange:
      NbProblems->SetValue(15, NbProblems->Value(15) + 1); break;
    case BRepCheck_EmptyWire:
      NbProblems->SetValue(16, NbProblems->Value(16) + 1); break;
    case BRepCheck_RedundantEdge:
      NbProblems->SetValue(17, NbProblems->Value(17) + 1); break;
    case BRepCheck_SelfIntersectingWire:
      NbProblems->SetValue(18, NbProblems->Value(18) + 1); break;
    case BRepCheck_NoSurface:
      NbProblems->SetValue(19, NbProblems->Value(19) + 1); break;
    case BRepCheck_InvalidWire:
      NbProblems->SetValue(20, NbProblems->Value(20) + 1); break;
    case BRepCheck_RedundantWire:
      NbProblems->SetValue(21, NbProblems->Value(21) + 1); break;
    case BRepCheck_IntersectingWires:
      NbProblems->SetValue(22, NbProblems->Value(22) + 1); break;
    case BRepCheck_InvalidImbricationOfWires:
      NbProblems->SetValue(23, NbProblems->Value(23) + 1); break;
    case BRepCheck_EmptyShell:
      NbProblems->SetValue(24, NbProblems->Value(24) + 1); break;
    case BRepCheck_RedundantFace:
      NbProblems->SetValue(25, NbProblems->Value(25) + 1); break;
    case BRepCheck_UnorientableShape:
      NbProblems->SetValue(26, NbProblems->Value(26) + 1); break;
    case BRepCheck_NotClosed:
      NbProblems->SetValue(27, NbProblems->Value(27) + 1); break;
    case BRepCheck_NotConnected:
      NbProblems->SetValue(28, NbProblems->Value(28) + 1); break;
    case BRepCheck_SubshapeNotInShape:
      NbProblems->SetValue(29, NbProblems->Value(29) + 1); break;
    case BRepCheck_BadOrientation:
      NbProblems->SetValue(30, NbProblems->Value(30) + 1); break;
    case BRepCheck_BadOrientationOfSubshape:
      NbProblems->SetValue(31, NbProblems->Value(31) + 1); break;
    case BRepCheck_CheckFail:
      NbProblems->SetValue(32, NbProblems->Value(32) + 1); break;
    default:
      break;
    }
  }

  //! \todo provide description here.
  void GetProblemSub(const BRepCheck_Analyzer&           Ana,
                     const TopoDS_Shape&                 Shape,
                     Handle(TopTools_HSequenceOfShape)&  sl,
                     Handle(TColStd_HArray1OfInteger)&   NbProblems,
                     const TopAbs_ShapeEnum              Subtype,
                     TopTools_DataMapOfShapeListOfShape& Map)
  {
    BRepCheck_ListIteratorOfListOfStatus itl;
    TopExp_Explorer exp;
    for ( exp.Init(Shape, Subtype); exp.More(); exp.Next() )
    {
      // Get current shape
      const TopoDS_Shape& sub = exp.Current();

      // Get result for the current shape
      const Handle(BRepCheck_Result)& res = Ana.Result(sub);

      // Iterate over the result
      for ( res->InitContextIterator(); res->MoreShapeInContext(); res->NextShapeInContext() )
      {
        if ( res->ContextualShape().IsSame(Shape) && !Contains(Map(sub), Shape) )
        {
          Map(sub).Append(Shape);
          itl.Initialize( res->StatusOnShape() );

          if ( itl.Value() != BRepCheck_NoError )
          {
            int ii = 0;
            for ( ii = 1; ii <= sl->Length(); ii++ )
              if ( sl->Value(ii).IsSame(sub) )
                break;

            if ( ii > sl->Length() )
            {
              sl->Append(sub);
              FillProblems(itl.Value(), NbProblems);
            }

            for ( ii = 1; ii <= sl->Length(); ii++ )
              if ( sl->Value(ii).IsSame(Shape) )
                break;

            if ( ii > sl->Length() )
            {
              sl->Append(Shape);
              FillProblems(itl.Value(), NbProblems);
            }
          }
          break;
        }
      }
    }
  }

  //! \todo provide description here.
  static void GetProblemShapes(const BRepCheck_Analyzer&           Ana,
                               const TopoDS_Shape&                 Shape,
                               Handle(TopTools_HSequenceOfShape)&  sl,
                               Handle(TColStd_HArray1OfInteger)&   NbProblems,
                               TopTools_DataMapOfShapeListOfShape& theMap)
  {
    for ( TopoDS_Iterator iter(Shape); iter.More(); iter.Next() )
    {
      GetProblemShapes(Ana, iter.Value(), sl,  NbProblems, theMap);
    }

    TopAbs_ShapeEnum styp = Shape.ShapeType();
    BRepCheck_ListIteratorOfListOfStatus itl;

    if ( !Ana.Result(Shape).IsNull() && !theMap.IsBound(Shape) )
    {
      itl.Initialize( Ana.Result(Shape)->Status() );

      if ( itl.Value() != BRepCheck_NoError )
      {
        sl->Append(Shape);
        FillProblems(itl.Value(), NbProblems);
      }
    }

    if ( !theMap.IsBound(Shape) )
    {
      TopTools_ListOfShape thelist;
      theMap.Bind(Shape, thelist);
    }

    switch ( styp )
    {
      case TopAbs_EDGE:
        GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_VERTEX, theMap);
        break;
      case TopAbs_FACE:
        GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_WIRE,   theMap);
        GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_EDGE,   theMap);
        GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_VERTEX, theMap);
        break;
      case TopAbs_SHELL:
        break;
      case TopAbs_SOLID:
        GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_SHELL, theMap);
        break;
      default:
        break;
    }
  }

  //! Dumps all detected invalidities to Logger.
  //! \param Journal   [in] Progress Entry.
  //! \param theAna   [in] checker tool.
  //! \param theShape [in] shape to check.
  void StructuralDump(ActAPI_ProgressEntry      Journal,
                      const BRepCheck_Analyzer& theAna,
                      const TopoDS_Shape&       theShape)
  {
    if ( Journal.Access().IsNull() )
      return; // Cannot dump to NULL Logger

    Journal.SendLogMessage(LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_HAS_PROBLEMS");

    // Initialize array storing the number of problems
    Handle(TColStd_HArray1OfInteger) NbProblems = new TColStd_HArray1OfInteger(1, 32);
    for ( int i = 1; i <= 32; i++ )
      NbProblems->SetValue(i, 0);

    Handle(TopTools_HSequenceOfShape) sl = new TopTools_HSequenceOfShape();

    TopTools_DataMapOfShapeListOfShape MAP;
    GetProblemShapes(theAna, theShape, sl, NbProblems, MAP);

    if ( NbProblems->Value(1) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_POINT_ON_CURVE"
                                             << NbProblems->Value(1) );
    if ( NbProblems->Value(2) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_POINT_ON_CURVE_ON_SURFACE"
                                             << NbProblems->Value(2) );
    if ( NbProblems->Value(3) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_POINT_ON_SURFACE"
                                             << NbProblems->Value(3) );
    if ( NbProblems->Value(4) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_NO_3D_CURVE"
                                             << NbProblems->Value(4) );
    if ( NbProblems->Value(5) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_MULTIPLE_3D_CURVE"
                                             << NbProblems->Value(5) );
    if ( NbProblems->Value(6) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_3D_CURVE"
                                             << NbProblems->Value(6) );
    if ( NbProblems->Value(7) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_NO_CURVE_ON_SURFACE"
                                             << NbProblems->Value(7) );
    if ( NbProblems->Value(8) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_CURVE_ON_SURFACE"
                                             << NbProblems->Value(8) );
    if ( NbProblems->Value(9) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_CURVE_ON_CLOSED_SURFACE"
                                             << NbProblems->Value(9) );
    if ( NbProblems->Value(10) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_SAME_RANGE_FLAG"
                                             << NbProblems->Value(10) );
    if ( NbProblems->Value(11) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_SAME_PARAMETER_FLAG"
                                             << NbProblems->Value(11) );
    if ( NbProblems->Value(12) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_DEGEN_FLAG"
                                             << NbProblems->Value(12) );
    if ( NbProblems->Value(13) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_FREE_EDGE"
                                             << NbProblems->Value(13) );
    if ( NbProblems->Value(14) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_MULTI_CONNEXITY"
                                             << NbProblems->Value(14) );
    if ( NbProblems->Value(15) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_RANGE"
                                             << NbProblems->Value(15) );
    if ( NbProblems->Value(16) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_EMPTY_WIRE"
                                             << NbProblems->Value(16) );
    if ( NbProblems->Value(17) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_REDUNDANT_EDGE"
                                             << NbProblems->Value(17) );
    if ( NbProblems->Value(18) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_SELF_INTER_WIRE"
                                             << NbProblems->Value(18) );
    if ( NbProblems->Value(19) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_NO_SURFACE"
                                             << NbProblems->Value(19) );
    if ( NbProblems->Value(20) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_WIRE"
                                             << NbProblems->Value(20) );
    if ( NbProblems->Value(21) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_REDUNDANT_WIRE"
                                             << NbProblems->Value(21) );
    if ( NbProblems->Value(22) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INTER_WIRES"
                                             << NbProblems->Value(22) );
    if ( NbProblems->Value(23) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_IMBRICATION_OF_WIRES"
                                             << NbProblems->Value(23) );
    if ( NbProblems->Value(24) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_EMPTY_SHELL"
                                             << NbProblems->Value(24) );
    if ( NbProblems->Value(25) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_REDUNDANT_FACE"
                                             << NbProblems->Value(25) );
    if ( NbProblems->Value(26) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_UNORIENTABLE_SHAPE"
                                             << NbProblems->Value(26) );
    if ( NbProblems->Value(27) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_NOT_CLOSED"
                                             << NbProblems->Value(27) );
    if ( NbProblems->Value(28) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_NOT_CONNECTED"
                                             << NbProblems->Value(28) );
    if ( NbProblems->Value(29) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_SUBSHAPE_NOT_IN_SHAPE"
                                             << NbProblems->Value(29) );
    if ( NbProblems->Value(30) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_BAD_ORIENTATION"
                                             << NbProblems->Value(30) );
    if ( NbProblems->Value(31) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_BAD_ORIENTATION_OF_SUBSHAPE"
                                             << NbProblems->Value(31) );
    if ( NbProblems->Value(32) > 0 )
      Journal.SendLogMessage( LogErr(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_FAILED"
                                            << NbProblems->Value(32) );
  }

};

//-----------------------------------------------------------------------------
// Shape Utilities
//-----------------------------------------------------------------------------

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
TopoDS_Shape geom_utils::ApplyTransformation(const TopoDS_Shape& theShape,
                                             const double        theXPos,
                                             const double        theYPos,
                                             const double        theZPos,
                                             const double        theAngleA,
                                             const double        theAngleB,
                                             const double        theAngleC,
                                             const bool          doCopy)
{
  gp_Trsf T = Transformation(theXPos, theYPos, theZPos,
                             theAngleA, theAngleB, theAngleC);

  return ApplyTransformation(theShape, T, doCopy);
}

//! Returns OCCT transformation structure for the given elemental
//! transformations.
//! \param theXPos   [in] X position.
//! \param theYPos   [in] Y position.
//! \param theZPos   [in] Z position.
//! \param theAngleA [in] rotation angle A.
//! \param theAngleB [in] rotation angle B.
//! \param theAngleC [in] rotation angle C.
//! \return OCCT transformation structure.
gp_Trsf geom_utils::Transformation(const double theXPos,
                                   const double theYPos,
                                   const double theZPos,
                                   const double theAngleA,
                                   const double theAngleB,
                                   const double theAngleC)
{
  gp_Vec aTranslation(theXPos, theYPos, theZPos);

  gp_Quaternion aRotationX(gp::DX(), theAngleA);
  gp_Quaternion aRotationY(gp::DY(), theAngleB);
  gp_Quaternion aRotationZ(gp::DZ(), theAngleC);

  gp_Trsf aTrsf;
  aTrsf.SetRotation(aRotationZ * aRotationY * aRotationX);
  aTrsf.SetTranslationPart(aTranslation);

  return aTrsf;
}

//! Applies the passed transformation to the given shape.
//! \param theShape     [in] shape to transform.
//! \param theTransform [in] transformation to apply.
//! \param doCopy       [in] indicates whether to construct a deep copy.
//! \return transformed shape.
TopoDS_Shape
  geom_utils::ApplyTransformation(const TopoDS_Shape& theShape,
                                  const gp_Trsf&      theTransform,
                                  const bool          doCopy)
{
  if ( doCopy )
  {
    TopoDS_Shape copy = BRepBuilderAPI_Copy(theShape, 1).Shape();
    return copy.Moved(theTransform);
  }

  return theShape.Moved(theTransform);
}

//! Creates a compound from the given list of shapes. If the list contains
//! only one not-null shape, this single shape is returned as-is.
//! \param theShapes [in] source shapes.
//! \return resulting compound.
TopoDS_Shape
  geom_utils::AssembleShapes(const TopTools_ListOfShape& theShapes)
{
  TopoDS_Shape aResult;

  TopoDS_Shape aShape;
  int aNbShapes = 0;
  for ( TopTools_ListIteratorOfListOfShape it(theShapes); it.More(); it.Next() )
    if ( !it.Value().IsNull() )
    {
      ++aNbShapes;
      aShape = it.Value();
    }

  if ( aNbShapes > 1)
  {
    TopoDS_Compound aCmp;
    BRep_Builder B;
    B.MakeCompound(aCmp);

    for ( TopTools_ListIteratorOfListOfShape it(theShapes); it.More(); it.Next() )
    {
      const TopoDS_Shape& aShape = it.Value();
      if ( aShape.IsNull() )
        continue;

      ++aNbShapes;
      B.Add( aCmp, it.Value() );
    }

    aResult = aCmp;
  }
  else
    aResult = aShape;

  return aResult;
}

//! Calculates bounding box for the given shape.
//! \param theShape [in]  input shape.
//! \param XMin     [out] min X.
//! \param YMin     [out] min Y.
//! \param ZMin     [out] min Z.
//! \param XMax     [out] max X.
//! \param YMax     [out] max Y.
//! \param ZMax     [out] max Z.
void geom_utils::Bounds(const TopoDS_Shape& theShape,
                        double& XMin, double& YMin, double& ZMin,
                        double& XMax, double& YMax, double& ZMax)
{
  Bnd_Box aBndBox;
  BRepBndLib::Add(theShape, aBndBox);
  aBndBox.Get(XMin, YMin, ZMin, XMax, YMax, ZMax);
}

//! Checks OCCT validity rules of the given shape.
//! \param theShape [in] shape to check.
//! \param Journal   [in] Logger instance to cumulate all meaningful messages.
//! \return true if shape is valid, false -- otherwise.
bool
  geom_utils::CheckShape(const TopoDS_Shape&  theShape,
                         ActAPI_ProgressEntry Journal)
{
  BRepCheck_Analyzer Checker(theShape);

  // Check validity
  if ( Checker.IsValid() )
  {
    Journal.SendLogMessage(LogInfo(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_GOOD");
    return true;
  }

  // ...
  // Shape is known to be invalid at this stage
  // ...

  // Perform structural dump
  CheckShapeAux::StructuralDump(Journal, Checker, theShape);

  return false;
}

//! Returns maximum tolerance value bound to the passed shape.
//! \param theShape [in] shape to check.
//! \return maximum tolerance value.
double geom_utils::MaxTolerance(const TopoDS_Shape& theShape)
{
  double aMaxToler = Precision::Confusion();

  // Tolerance by faces
  for ( TopExp_Explorer exp(theShape, TopAbs_FACE); exp.More(); exp.Next() )
  {
    double aTol = BRep_Tool::Tolerance( TopoDS::Face( exp.Current() ) );
    if ( aTol > aMaxToler )
      aMaxToler = aTol;
  }

  // Tolerance by edges
  for ( TopExp_Explorer exp(theShape, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    double aTol = BRep_Tool::Tolerance( TopoDS::Edge( exp.Current() ) );
    if ( aTol > aMaxToler )
      aMaxToler = aTol;
  }

  // Tolerance by vertices
  for ( TopExp_Explorer exp(theShape, TopAbs_VERTEX); exp.More(); exp.Next() )
  {
    double aTol = BRep_Tool::Tolerance( TopoDS::Vertex( exp.Current() ) );
    if ( aTol > aMaxToler )
      aMaxToler = aTol;
  }

  return aMaxToler;
}

//! Reads CAD model from native OCCT b-rep file.
//! \param theFilename [in]  filename.
//! \param theShape    [out] CAD model retrieved from file.
//! \return true in case of success, false -- otherwise.
bool geom_utils::ReadBRep(const TCollection_AsciiString& theFilename,
                          TopoDS_Shape&                  theShape)
{
  BRep_Builder BB;
  return BRepTools::Read(theShape, theFilename.ToCString(), BB) > 0;
}

//! Reads CAD model from native STL file.
//! \param theFilename [in]  filename.
//! \param theShape    [out] CAD model retrieved from file.
//! \return true in case of success, false -- otherwise.
bool geom_utils::ReadSTL(const TCollection_AsciiString& theFilename,
                         TopoDS_Shape&                  theShape)
{
  StlAPI_Reader stlReader;
  stlReader.Read( theShape, theFilename.ToCString() );
  return !theShape.IsNull();
}

//! Writes shape to B-Rep format.
//! \param theShape    [in] shape to write.
//! \param theFilename [in] filename.
//! \return true in case of success, false -- otherwise.
bool geom_utils::WriteBRep(const TopoDS_Shape&            theShape,
                           const TCollection_AsciiString& theFilename)
{
  return BRepTools::Write( theShape, theFilename.ToCString() ) > 0;
}

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
void geom_utils::ShapeSummary(const TopoDS_Shape& shape,
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
                              int&                nbVertexes)
{
  if ( shape.IsNull() )
    return;

  // Get all outer wires in the model
  TopTools_IndexedMapOfShape outerWires;
  for ( TopExp_Explorer exp(shape, TopAbs_FACE); exp.More(); exp.Next() )
  {
    const TopoDS_Face& F = TopoDS::Face( exp.Current() );
    outerWires.Add( BRepTools::OuterWire(F) );
  }

  // The following map is used to skip already traversed TShapes
  TopTools_IndexedMapOfShape M;

  // Traverse all sub-shapes
  TopTools_ListOfShape shapeList;
  shapeList.Append(shape);
  //
  for ( TopTools_ListIteratorOfListOfShape itL(shapeList); itL.More(); itL.Next() )
  {
    const TopoDS_Shape& currentShape = itL.Value();
    //
    if ( M.Contains( currentShape.Located( TopLoc_Location() ) ) )
      continue;
    else
      M.Add( currentShape.Located( TopLoc_Location() ) );

    if ( currentShape.ShapeType() == TopAbs_COMPSOLID )
      nbCompsolids++;
    else if ( currentShape.ShapeType() == TopAbs_COMPOUND )
      nbCompounds++;
    else if ( currentShape.ShapeType() == TopAbs_SOLID )
      nbSolids++;
    else if ( currentShape.ShapeType() == TopAbs_SHELL )
      nbShells++;
    else if ( currentShape.ShapeType() == TopAbs_FACE )
      nbFaces++;
    else if ( currentShape.ShapeType() == TopAbs_WIRE )
    {
      nbWires++;
      if ( outerWires.Contains(currentShape) )
        nbOuterWires++;
      else
        nbInnerWires++;
    }
    else if ( currentShape.ShapeType() == TopAbs_EDGE )
    {
      nbEdges++;
      if ( BRep_Tool::Degenerated( TopoDS::Edge(currentShape) ) )
        nbDegenEdges++;
    }
    else if ( currentShape.ShapeType() == TopAbs_VERTEX )
      nbVertexes++;

    // Iterate over the direct children of a shape
    for ( TopoDS_Iterator sub_it(currentShape); sub_it.More(); sub_it.Next() )
    {
      const TopoDS_Shape& subShape = sub_it.Value();

      // Add sub-shape to iterate over
      shapeList.Append(subShape);
    }
  }
}

//! Collects summary information of the given shape: returns the number
//! of sub-shapes of each type.
//! \param shape [in]  shape to analyze.
//! \param info  [out] shape summary as string.
void geom_utils::ShapeSummary(const TopoDS_Shape&      shape,
                              TCollection_AsciiString& info)
{
  // Summary
  int nbCompsolids = 0,
      nbCompounds  = 0,
      nbSolids     = 0,
      nbShells     = 0,
      nbFaces      = 0,
      nbWires      = 0,
      nbOuterWires = 0,
      nbInnerWires = 0,
      nbEdges      = 0,
      nbDegenEdges = 0,
      nbVertexes   = 0;

  // Extract summary
  ShapeSummary(shape,
               nbCompsolids,
               nbCompounds,
               nbSolids,
               nbShells,
               nbFaces,
               nbWires,
               nbOuterWires,
               nbInnerWires,
               nbEdges,
               nbDegenEdges,
               nbVertexes);

  // Prepare output string with the gathered summary
  info += "PART SUMMARY (EQ TSHAPE, ANY TRSF, ANY ORI):\n";
  info += "- nb compsolids: "; info += nbCompsolids; info += "\n";
  info += "- nb compounds: ";  info += nbCompounds;  info += "\n";
  info += "- nb solids: ";     info += nbSolids;     info += "\n";
  info += "- nb shells: ";     info += nbShells;     info += "\n";
  info += "- nb faces: ";      info += nbFaces;      info += "\n";
  info += "- nb wires: ";      info += nbWires;      info += "\n";
  info += "- nb edges: ";      info += nbEdges;      info += "\n";
  info += "- nb vertices: ";   info += nbVertexes;   info += "\n";
}

//! Creates a circular wire with the given radius.
//! \param radius [in] radius of the host circle.
//! \return created wire.
TopoDS_Wire geom_utils::CreateCircularWire(const double radius)
{
  Handle(Geom_Circle) C = GC_MakeCircle(gp_Ax1( gp::Origin(), gp::DZ() ), radius);

  // Let's convert our circle to b-curve
  Handle(Geom_BSplineCurve) BC = GeomConvert::CurveToBSplineCurve(C, Convert_QuasiAngular);

  // Build a wire
  return BRepBuilderAPI_MakeWire( BRepBuilderAPI_MakeEdge(BC) );
}

//! Skins a surface through the passed sections.
//! \param wires [in] sections to skin.
//! \return skinning result.
TopoDS_Shape geom_utils::MakeSkin(const TopTools_SequenceOfShape& wires)
{
  if ( wires.Length() < 2 )
  {
    std::cout << "Error: not enough sections" << std::endl;
    return TopoDS_Shape();
  }

  // Initialize and build
  BRepOffsetAPI_ThruSections ThruSections(0, 0, 1.0e-2);
  ThruSections.SetSmoothing(1);
  ThruSections.SetMaxDegree(2);
  //
  for ( int k = 1; k <= wires.Length(); ++k )
  {
    if ( wires(k).ShapeType() != TopAbs_WIRE )
    {
      std::cout << "Warning: section " << k << " is not a wire" << std::endl;
      continue;
    }
    //
    ThruSections.AddWire( TopoDS::Wire( wires(k) ) );
  }
  try
  {
    ThruSections.Build();
  }
  catch ( ... )
  {
    std::cout << "Error: crash in BRepOffsetAPI_ThruSections" << std::endl;
    return TopoDS_Shape();
  }
  //
  if ( !ThruSections.IsDone() )
  {
    std::cout << "Error: IsDone() false in BRepOffsetAPI_ThruSections" << std::endl;
    return TopoDS_Shape();
  }

  // Get the result
  TopExp_Explorer exp(ThruSections.Shape(), TopAbs_SHELL);
  if ( !exp.More() )
    return TopoDS_Shape();
  //
  TopoDS_Shell ResultShell = TopoDS::Shell( exp.Current() );
  //
  std::cout << "Skinning was done successfully" << std::endl;
  return ResultShell;
}

//! Performs sewing.
//! \param shape     [in/out] shape to sew.
//! \param tolerance [in]     sewing tolerance.
//! \return true in case of success, false -- otherwise.
bool geom_utils::Sew(TopoDS_Shape& shape,
                     const double  tolerance)
{
  BRepBuilderAPI_Sewing Sewer(tolerance);
  Sewer.Load(shape);

  // Perform sewing
  Sewer.Perform();
  shape = Sewer.SewedShape();
  return true;
}

//! Performs "same domain" expansion on faces and edges.
//! \param shape [in/out] shape to modify.
//! \return true in case of success, false -- otherwise.
bool geom_utils::MaximizeFaces(TopoDS_Shape& shape)
{
  feature_merge_faces merger(shape, NULL, NULL);
  return merger.Perform_OpenSource();
}

//! Interpolates the given collection of points with B-curve of the
//! desired degree.
//! \param points [in]  points to interpolate.
//! \param p      [in]  degree to use.
//! \param result [out] interpolant.
//! \return true om case of success, false -- otherwise.
bool geom_utils::InterpolatePoints(const std::vector<gp_Pnt>& points,
                                   const int                  p,
                                   Handle(Geom_BSplineCurve)& result)
{
  // Number of unknown control points
  const int numPoles = (int) points.size();
  const int n        = numPoles - 1;
  const int m        = n + p + 1;

  // Calculate total chord length
  double chordTotal = 0.0;
  for ( size_t k = 1; k < numPoles; ++k )
    chordTotal += Sqrt( ( points[k].XYZ() - points[k-1].XYZ() ).Modulus() );

  //-------------------------------------------------------------------------
  // Choose parameters
  double* pParams = new double[numPoles];
  pParams[0] = 0.0;
  for ( int k = 1; k < numPoles - 1; ++k )
  {
    pParams[k] = pParams[k - 1] + Sqrt( ( points[k].XYZ() - points[k-1].XYZ() ).Modulus() ) / chordTotal;
  }
  pParams[numPoles - 1] = 1.0;

  //-------------------------------------------------------------------------
  // Choose knots
  double* pKnots = new double[m + 1];
  //
  for ( int k = 0; k < p + 1; ++k )
    pKnots[k] = 0.0;
  for ( int k = m; k > m - p - 1; --k )
    pKnots[k] = 1.0;
  for ( int j = 1; j <= n - p; ++j )
  {
    pKnots[j + p] = 0.0;
    for ( int i = j; i <= j + p - 1; ++i )
    {
      pKnots[j + p] += pParams[i];
    }
    pKnots[j + p] /= p;
  }

  // Convert to flat knots
  TColStd_Array1OfReal flatKnots(1, m + 1);
  for ( int k = 0; k <= m; ++k )
    flatKnots(k + 1) = pKnots[k];

  // Initialize matrix from the passed row pointer
  const int dim = n + 1;
  Eigen::MatrixXd eigen_N_mx(dim, dim);

  // Calculate non-zero B-splines for each parameter value
  for ( int k = 0; k <= n; ++k )
  {
    int firstNonZeroIdx;
    math_Matrix N_mx(1, 1, 1, p + 1);
    BSplCLib::EvalBsplineBasis(0, p + 1, flatKnots, pParams[k], firstNonZeroIdx, N_mx);

#if defined DUMP_COUT
    // Dump
    std::cout << "\tFirst Non-Zero Index for " << pParams[k] << " is " << firstNonZeroIdx << std::endl;
    for ( int kk = 1; kk <= p + 1; ++kk )
    {
      std::cout << "\t" << N_mx(1, kk);
    }
    std::cout << std::endl;
#endif

    // Fill Eigen's matrix
    for ( int c = 0; c < firstNonZeroIdx - 1; ++c )
    {
      eigen_N_mx(k, c) = 0.0;
    }
    //
    for ( int c = firstNonZeroIdx - 1, kk = 1; c < firstNonZeroIdx + p; ++c, ++kk )
    {
      eigen_N_mx(k, c) = N_mx(1, kk);
    }
    //
    for ( int c = firstNonZeroIdx + p; c < dim; ++c )
    {
      eigen_N_mx(k, c) = 0.0;
    }
  }

#if defined DUMP_FILES
  // Dump Eigen matrix
  {
    std::ofstream FILE;
    FILE.open(dump_filename_N, std::ios::out | std::ios::trunc);
    //
    if ( !FILE.is_open() )
    {
      std::cout << "Error: cannot open file for dumping" << std::endl;
      return false;
    }
    //
    for ( int i = 0; i < dim; ++i )
    {
      for ( int j = 0; j < dim; ++j )
      {
        FILE << eigen_N_mx(i, j);
        if ( (i + j + 2) < (dim + dim) )
          FILE << "\t";
      }
      FILE << "\n";
    }
  }
#endif

  // Initialize vector of right hand side
  Eigen::MatrixXd eigen_B_mx(dim, 3);
  for ( int r = 0; r < dim; ++r )
  {
    eigen_B_mx(r, 0) = points[r].X();
    eigen_B_mx(r, 1) = points[r].Y();
    eigen_B_mx(r, 2) = points[r].Z();
  }

#if defined DUMP_FILES
  // Dump Eigen B [X]
  {
    std::ofstream FILE;
    FILE.open(dump_filename_Bx, std::ios::out | std::ios::trunc);
    //
    if ( !FILE.is_open() )
    {
      std::cout << "Error: cannot open file for dumping" << std::endl;
      return false;
    }
    //
    for ( int i = 0; i < dim; ++i )
    {
      FILE << eigen_B_mx(i, 0) << "\n";
    }
  }
#endif

  //---------------------------------------------------------------------------
  // BEGIN: solve linear system
  //---------------------------------------------------------------------------

  TIMER_NEW
  TIMER_GO

  // Solve
  Eigen::ColPivHouseholderQR<Eigen::MatrixXd> QR(eigen_N_mx);
  Eigen::MatrixXd eigen_X_mx = QR.solve(eigen_B_mx);

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("B-curve interpolation")

  //---------------------------------------------------------------------------
  // END: solve linear system
  //---------------------------------------------------------------------------

  // Fill poles
  TColgp_Array1OfPnt poles(1, dim);
  for ( int i = 0; i < dim; ++i )
  {
    gp_Pnt P( eigen_X_mx(i, 0), eigen_X_mx(i, 1), eigen_X_mx(i, 2) );
    poles(i + 1) = P;
  }

  // Fill knots
  TColStd_Array1OfReal knots(1, m + 1 - 2*p);
  TColStd_Array1OfInteger mults(1, m + 1 - 2*p);
  //
  knots(1) = 0; mults(1) = p + 1;
  //
  for ( int j = 2, jj = p + 1; j <= m - 2*p; ++j, ++jj )
  {
    knots(j) = pKnots[jj]; mults(j) = 1;
  }
  //
  knots( knots.Upper() ) = 1; mults( mults.Upper() ) = p + 1;

#if defined DUMP_COUT
  // Dump knots
  for ( int k = knots.Lower(); k <= knots.Upper(); ++k )
  {
    std::cout << "knots(" << k << ") = " << knots(k) << "\t" << mults(k) << std::endl;
  }
#endif

  // Create B-spline curve
  Handle(Geom_BSplineCurve) bcurve = new Geom_BSplineCurve(poles, knots, mults, p);

  // Save result
  result = bcurve;

  // Delete parameters
  delete[] pParams;
  delete[] pKnots;

  return true;
}

//! Calculates angle between two edges at their joint.
//! \param E1 [in] first edge.
//! \param E2 [in] second edge.
//! \param F  [in] base face.
//! \return calculated angle.
feature_angle geom_utils::AngleBetweenEdges(const TopoDS_Edge& E1,
                                         const TopoDS_Edge& E2,
                                         const TopoDS_Face& F)
{
  // TODO find convex edges

  // Decide which edge is first and which is second
  // ...

  /*TopoDS_Edge First, Second;

  TopoDS_Vertex E1_Vf, E1_Vl, E2_Vf, E2_Vl;
  TopExp::FirstVertex(E1*/
  return Angle_Undefined;
}

//! Performs Boolean Cut of a tool shape from the given object shape.
//! \param Object [in] object to cut from.
//! \param Tool   [in] tool to cut out.
//! \param fuzzy  [in] fuzzy tolerance.
//! \return result shape.
TopoDS_Shape geom_utils::BooleanCut(const TopoDS_Shape& Object,
                                    const TopoDS_Shape& Tool,
                                    const double        fuzzy)
{
  // Prepare the arguments
  TopTools_ListOfShape BOP_args;
  BOP_args.Append(Object);
  BOP_args.Append(Tool);

  // Prepare data structure (calculate interferences)
  Handle(NCollection_BaseAllocator) Alloc = new NCollection_IncAllocator;
  //
  BOPAlgo_PaveFiller DSFiller(Alloc);
  DSFiller.SetArguments(BOP_args);
  DSFiller.SetRunParallel(0);
  DSFiller.SetFuzzyValue(fuzzy);
  DSFiller.Perform();

  // Check data structure
  int iErr = DSFiller.ErrorStatus();
  if ( iErr )
  {
    std::cout << "Error: cannot cut" << std::endl;
    return TopoDS_Shape();
  }

  // Run BOP
  BOPAlgo_BOP BOP(Alloc);
  BOP.AddArgument(Object);
  BOP.AddTool(Tool);
  BOP.SetRunParallel(0);
  BOP.SetOperation(BOPAlgo_CUT);
  BOP.PerformWithFiller(DSFiller);
  iErr = BOP.ErrorStatus();
  if ( iErr )
  {
    std::cout << "Error: cannot cut the part model from the stock model" << std::endl;
    return TopoDS_Shape();
  }

  return BOP.Shape();
}
