//-----------------------------------------------------------------------------
// Created on: 20 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <geom_utils.h>

// OCCT includes
#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRepBndLib.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepCheck_Status.hxx>
#include <BRepTools.hxx>
#include <gp_Quaternion.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

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
  //! \param PEntry   [in] Progress Entry.
  //! \param theAna   [in] checker tool.
  //! \param theShape [in] shape to check.
  void StructuralDump(ActAPI_ProgressEntry      PEntry,
                      const BRepCheck_Analyzer& theAna,
                      const TopoDS_Shape&       theShape)
  {
    if ( PEntry.Access().IsNull() )
      return; // Cannot dump to NULL Logger

    PEntry.SendLogMessage(LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_HAS_PROBLEMS");

    // Initialize array storing the number of problems
    Handle(TColStd_HArray1OfInteger) NbProblems = new TColStd_HArray1OfInteger(1, 32);
    for ( int i = 1; i <= 32; i++ )
      NbProblems->SetValue(i, 0);

    Handle(TopTools_HSequenceOfShape) sl = new TopTools_HSequenceOfShape();

    TopTools_DataMapOfShapeListOfShape MAP;
    GetProblemShapes(theAna, theShape, sl, NbProblems, MAP);

    if ( NbProblems->Value(1) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_POINT_ON_CURVE"
                                             << NbProblems->Value(1) );
    if ( NbProblems->Value(2) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_POINT_ON_CURVE_ON_SURFACE"
                                             << NbProblems->Value(2) );
    if ( NbProblems->Value(3) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_POINT_ON_SURFACE"
                                             << NbProblems->Value(3) );
    if ( NbProblems->Value(4) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_NO_3D_CURVE"
                                             << NbProblems->Value(4) );
    if ( NbProblems->Value(5) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_MULTIPLE_3D_CURVE"
                                             << NbProblems->Value(5) );
    if ( NbProblems->Value(6) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_3D_CURVE"
                                             << NbProblems->Value(6) );
    if ( NbProblems->Value(7) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_NO_CURVE_ON_SURFACE"
                                             << NbProblems->Value(7) );
    if ( NbProblems->Value(8) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_CURVE_ON_SURFACE"
                                             << NbProblems->Value(8) );
    if ( NbProblems->Value(9) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_CURVE_ON_CLOSED_SURFACE"
                                             << NbProblems->Value(9) );
    if ( NbProblems->Value(10) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_SAME_RANGE_FLAG"
                                             << NbProblems->Value(10) );
    if ( NbProblems->Value(11) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_SAME_PARAMETER_FLAG"
                                             << NbProblems->Value(11) );
    if ( NbProblems->Value(12) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_DEGEN_FLAG"
                                             << NbProblems->Value(12) );
    if ( NbProblems->Value(13) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_FREE_EDGE"
                                             << NbProblems->Value(13) );
    if ( NbProblems->Value(14) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_MULTI_CONNEXITY"
                                             << NbProblems->Value(14) );
    if ( NbProblems->Value(15) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_RANGE"
                                             << NbProblems->Value(15) );
    if ( NbProblems->Value(16) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_EMPTY_WIRE"
                                             << NbProblems->Value(16) );
    if ( NbProblems->Value(17) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_REDUNDANT_EDGE"
                                             << NbProblems->Value(17) );
    if ( NbProblems->Value(18) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_SELF_INTER_WIRE"
                                             << NbProblems->Value(18) );
    if ( NbProblems->Value(19) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_NO_SURFACE"
                                             << NbProblems->Value(19) );
    if ( NbProblems->Value(20) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_WIRE"
                                             << NbProblems->Value(20) );
    if ( NbProblems->Value(21) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_REDUNDANT_WIRE"
                                             << NbProblems->Value(21) );
    if ( NbProblems->Value(22) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INTER_WIRES"
                                             << NbProblems->Value(22) );
    if ( NbProblems->Value(23) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_INV_IMBRICATION_OF_WIRES"
                                             << NbProblems->Value(23) );
    if ( NbProblems->Value(24) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_EMPTY_SHELL"
                                             << NbProblems->Value(24) );
    if ( NbProblems->Value(25) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_REDUNDANT_FACE"
                                             << NbProblems->Value(25) );
    if ( NbProblems->Value(26) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_UNORIENTABLE_SHAPE"
                                             << NbProblems->Value(26) );
    if ( NbProblems->Value(27) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_NOT_CLOSED"
                                             << NbProblems->Value(27) );
    if ( NbProblems->Value(28) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_NOT_CONNECTED"
                                             << NbProblems->Value(28) );
    if ( NbProblems->Value(29) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_SUBSHAPE_NOT_IN_SHAPE"
                                             << NbProblems->Value(29) );
    if ( NbProblems->Value(30) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_BAD_ORIENTATION"
                                             << NbProblems->Value(30) );
    if ( NbProblems->Value(31) > 0 )
      PEntry.SendLogMessage( LogWarn(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_BAD_ORIENTATION_OF_SUBSHAPE"
                                             << NbProblems->Value(31) );
    if ( NbProblems->Value(32) > 0 )
      PEntry.SendLogMessage( LogErr(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_FAILED"
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
                                             const double        theAngleC)
{
  gp_Trsf aTrsf = Transformation(theXPos, theYPos, theZPos,
                                 theAngleA, theAngleB, theAngleC);

  return theShape.Moved(aTrsf);
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

//! Applies the passed transformation to the given shape. Returns another shape
//! as a result (no deep copy of geometry is performed, only location is
//! changed).
//! \param theShape [in] shape to transform.
//! \param theTransform [in] transformation to apply.
//! \return transformed shape.
TopoDS_Shape
  geom_utils::ApplyTransformation(const TopoDS_Shape& theShape,
                                  const gp_Trsf&      theTransform)
{
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
//! \param PEntry   [in] Logger instance to cumulate all meaningful messages.
//! \return true if shape is valid, false -- otherwise.
bool
  geom_utils::CheckShape(const TopoDS_Shape&  theShape,
                         ActAPI_ProgressEntry PEntry)
{
  BRepCheck_Analyzer Checker(theShape);

  // Check validity
  if ( Checker.IsValid() )
  {
    PEntry.SendLogMessage(LogInfo(Normal) << "ALGO_SHAPEUTILS_CHECKSHAPE_GOOD");
    return true;
  }

  // ...
  // Shape is known to be invalid at this stage
  // ...

  // Perform structural dump
  CheckShapeAux::StructuralDump(PEntry, Checker, theShape);

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
      nbEdges      = 0,
      nbVertexes   = 0;

  // The following map is used to skip already traversed TShapes
  TopTools_MapOfShape M;
  M.Add(shape);

  // Traverse all sub-shapes
  TopTools_ListOfShape shapeList;
  shapeList.Append(shape);
  //
  for ( TopTools_ListIteratorOfListOfShape itL(shapeList); itL.More(); itL.Next() )
  {
    const TopoDS_Shape& currentShape = itL.Value();

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
      nbWires++;
    else if ( currentShape.ShapeType() == TopAbs_EDGE )
      nbEdges++;
    else if ( currentShape.ShapeType() == TopAbs_VERTEX )
      nbVertexes++;

    // Iterate over the direct children of a shape
    for ( TopoDS_Iterator sub_it(currentShape); sub_it.More(); sub_it.Next() )
    {
      const TopoDS_Shape& subShape = sub_it.Value();

      if ( !M.Add(subShape) )
        continue;

      // Add sub-shape to iterate over
      shapeList.Append(subShape);
    }
  }

  // Prepare output string with the gathered summary
  info += "PART SUMMARY:\n";
  info += "- nb compsolids: "; info += nbCompsolids; info += "\n";
  info += "- nb compounds: ";  info += nbCompounds;  info += "\n";
  info += "- nb solids: ";     info += nbSolids;     info += "\n";
  info += "- nb shells: ";     info += nbShells;     info += "\n";
  info += "- nb faces: ";      info += nbFaces;      info += "\n";
  info += "- nb wires: ";      info += nbWires;      info += "\n";
  info += "- nb edges: ";      info += nbEdges;      info += "\n";
  info += "- nb vertices: ";   info += nbVertexes;   info += "\n";
}
