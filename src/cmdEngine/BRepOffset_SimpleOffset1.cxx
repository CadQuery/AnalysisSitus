// Created on: 2016-10-14
// Created by: Alexander MALYSHEV
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2016 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// Include self.
#include <BRepOffset_SimpleOffset1.hxx>

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepLib.hxx>
#include <BRep_Tool.hxx>
#include <BRepOffset.hxx>
#include <Geom_OffsetSurface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <NCollection_Vector.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

#include <Geom_BSplineCurve.hxx>

static const Standard_Integer NCONTROL=22;


//=============================================================================
//function : BRepOffset_SimpleOffset1
//purpose  : Constructor
//=============================================================================
BRepOffset_SimpleOffset1::BRepOffset_SimpleOffset1(ActAPI_ProgressEntry progress,
                                                   ActAPI_PlotterEntry  plotter,
                                                   const TopoDS_Shape& theInputShape,
                                                   const Standard_Real theOffsetValue,
                                                   const Standard_Real theTolerance)
: m_progress(progress),
  m_plotter(plotter),
  myOffsetValue(theOffsetValue),
  myTolerance(theTolerance)
{
  FillOffsetData(theInputShape);
}

//=============================================================================
//function : NewSurface
//purpose  :
//=============================================================================
Standard_Boolean BRepOffset_SimpleOffset1::NewSurface(const TopoDS_Face& F,
                                                     Handle(Geom_Surface)& S,
                                                     TopLoc_Location& L,
                                                     Standard_Real& Tol,
                                                     Standard_Boolean& RevWires,
                                                     Standard_Boolean& RevFace)
{
  if (!myFaceInfo.IsBound(F))
    return Standard_False;

  const NewFaceData& aNFD = myFaceInfo.Find(F);

  S = aNFD.myOffsetS;
  L = aNFD.myL;
  Tol = aNFD.myTol;
  RevWires = aNFD.myRevWires;
  RevFace = aNFD.myRevFace;

  return Standard_True;
}

//=============================================================================
//function : NewCurve
//purpose  :
//=============================================================================
Standard_Boolean BRepOffset_SimpleOffset1::NewCurve(const TopoDS_Edge& E,
                                                   Handle(Geom_Curve)& C,
                                                   TopLoc_Location& L,
                                                   Standard_Real& Tol)
{
  if (!myEdgeInfo.IsBound(E))
    return Standard_False;

  const NewEdgeData& aNED = myEdgeInfo.Find(E);

  C = aNED.myOffsetC;
  L = aNED.myL;
  Tol = aNED.myTol;

  return Standard_True;
}

//=============================================================================
//function : NewPoint
//purpose  :
//=============================================================================
Standard_Boolean BRepOffset_SimpleOffset1::NewPoint (const TopoDS_Vertex& V,
                                                    gp_Pnt& P,
                                                    Standard_Real& Tol)
{
  if (!myVertexInfo.IsBound(V))
    return Standard_False;

  static int count = 0;
  count++;

  const NewVertexData& aNVD = myVertexInfo.Find(V);

  gp_Pnt oldPnt = BRep_Tool::Pnt(V);

  P = aNVD.myP;
  Tol = aNVD.myTol;

  /*m_plotter.DRAW_POINT(oldPnt, Color_Red,   "oldPnt");
  m_plotter.DRAW_POINT(P,      Color_Green, "newPnt");*/

  return Standard_True;
}

//=============================================================================
//function : NewCurve2d
//purpose  :
//=============================================================================
Standard_Boolean BRepOffset_SimpleOffset1::NewCurve2d (const TopoDS_Edge& E,
                                                      const TopoDS_Face& F,
                                                      const TopoDS_Edge& /*NewE*/,
                                                      const TopoDS_Face& /*NewF*/,
                                                      Handle(Geom2d_Curve)& C,
                                                      Standard_Real& Tol)
{
  // Use original pcurve.
  Standard_Real aF, aL;
  C = BRep_Tool::CurveOnSurface(E, F, aF, aL);
  Tol = BRep_Tool::Tolerance(E);

  if (myEdgeInfo.IsBound(E))
    Tol = myEdgeInfo.Find(E).myTol;

  return Standard_True;
}

//=============================================================================
//function : NewParameter
//purpose  :
//=============================================================================
Standard_Boolean BRepOffset_SimpleOffset1::NewParameter (const TopoDS_Vertex& V,
                                                        const TopoDS_Edge& E,
                                                        Standard_Real& P,
                                                        Standard_Real& Tol)
{
  // Use original parameter.
  P = BRep_Tool::Parameter(V, E);
  Tol = BRep_Tool::Tolerance(V);

  if (myVertexInfo.IsBound(V))
    Tol = myVertexInfo.Find(V).myTol;

  return Standard_True;
}

//=============================================================================
//function : NewParameter
//purpose  :
//=============================================================================
GeomAbs_Shape BRepOffset_SimpleOffset1::Continuity (const TopoDS_Edge& E,
                                                   const TopoDS_Face& F1,
                                                   const TopoDS_Face& F2,
                                                   const TopoDS_Edge& /*NewE*/,
                                                   const TopoDS_Face& /*NewF1*/,
                                                   const TopoDS_Face& /*NewF2*/)
{
  // Compute result using original continuity.
  return BRep_Tool::Continuity(E, F1, F2);
}

//=============================================================================
//function : FillOffsetData
//purpose  : 
//=============================================================================
void BRepOffset_SimpleOffset1::FillOffsetData(const TopoDS_Shape& theShape)
{
  // Clears old data.
  myFaceInfo.Clear();
  myEdgeInfo.Clear();
  myVertexInfo.Clear();

  // Faces loop. Compute offset surface for each face.
  TopExp_Explorer anExpSF(theShape, TopAbs_FACE);
  for(; anExpSF.More(); anExpSF.Next())
  {
    const TopoDS_Face& aCurrFace = TopoDS::Face(anExpSF.Current());
    FillFaceData(aCurrFace);
  }

  // Iterate over edges to compute 3d curve.
  TopTools_IndexedDataMapOfShapeListOfShape aEdgeFaceMap;
  TopExp::MapShapesAndAncestors(theShape, TopAbs_EDGE, TopAbs_FACE, aEdgeFaceMap);
  for (Standard_Integer anIdx = 1; anIdx <= aEdgeFaceMap.Size(); ++anIdx)
  {
    const TopoDS_Edge& aCurrEdge = TopoDS::Edge(aEdgeFaceMap.FindKey(anIdx));
    FillEdgeData(aCurrEdge, aEdgeFaceMap, anIdx);
  }

  // Iterate over vertices to compute new vertex.
  TopTools_IndexedDataMapOfShapeListOfShape aVertexEdgeMap, aVertexFaceMap;
  TopExp::MapShapesAndAncestors(theShape, TopAbs_VERTEX, TopAbs_EDGE, aVertexEdgeMap);
  TopExp::MapShapesAndUniqueAncestors(theShape, TopAbs_VERTEX, TopAbs_FACE, aVertexFaceMap);
  for (Standard_Integer anIdx = 1; anIdx <= aVertexEdgeMap.Size(); ++anIdx)
  {
    const TopoDS_Vertex & aCurrVertex = TopoDS::Vertex(aVertexEdgeMap.FindKey(anIdx));
    FillVertexData(aCurrVertex, aVertexEdgeMap, aVertexFaceMap, anIdx);
  }
}

//=============================================================================
//function : FillFaceData
//purpose  : 
//=============================================================================
void BRepOffset_SimpleOffset1::FillFaceData(const TopoDS_Face& theFace)
{
  NewFaceData aNFD;
  aNFD.myRevWires = Standard_False;
  aNFD.myRevFace = Standard_False;
  aNFD.myTol = BRep_Tool::Tolerance(theFace);

  // Create offset surface.

  // Any existing transformation is applied to the surface.
  // New face will have null transformation.
  Handle(Geom_Surface) aS = BRep_Tool::Surface(theFace);
  aS = BRepOffset::CollapseSingularities (aS, theFace, myTolerance);

  // Take into account face orientation.
  Standard_Real aMult = 1.0;
  if (theFace.Orientation() == TopAbs_REVERSED)
    aMult = -1.0;

  aNFD.myOffsetS = new Geom_OffsetSurface(aS, aMult * myOffsetValue, Standard_True);
  aNFD.myL = TopLoc_Location(); // Null transformation.

  // Save offset surface in map.
  myFaceInfo.Bind(theFace, aNFD);
}

//=============================================================================
//function : FillEdgeData
//purpose  : 
//=============================================================================
void BRepOffset_SimpleOffset1::FillEdgeData(const TopoDS_Edge& theEdge,
                                           const TopTools_IndexedDataMapOfShapeListOfShape& theEdgeFaceMap,
                                           const Standard_Integer theIdx)
{
  const TopTools_ListOfShape& aFacesList = theEdgeFaceMap(theIdx);

  if (aFacesList.Size() == 0)
    return; // Free edges are skipped.

  // Get offset surface.
  const TopoDS_Face& aCurrFace = TopoDS::Face(aFacesList.First());

  if (!myFaceInfo.IsBound(aCurrFace))
    return;

  // No need to deal with transformation - it is applied in fill faces data method.
  const NewFaceData & aNFD = myFaceInfo.Find(aCurrFace);
  Handle(Geom_Surface) anOffsetSurf = aNFD.myOffsetS;

  // Compute offset 3d curve.
  Standard_Real aF, aL;
  Handle(Geom2d_Curve) aC2d = BRep_Tool::CurveOnSurface(theEdge, aCurrFace, aF, aL);

  if ( theIdx == 13 )
    m_plotter.DRAW_CURVE2D(aC2d, Color_Green, "aC2d_13");

  BRepBuilderAPI_MakeEdge anEdgeMaker(aC2d, anOffsetSurf, aF, aL);
  TopoDS_Edge aNewEdge = anEdgeMaker.Edge();

  // Compute max tolerance. Vertex tolerance usage is taken from existing offset computation algorithm.
  // This piece of code significantly influences resulting performance.
  Standard_Real aTol = BRep_Tool::MaxTolerance(theEdge, TopAbs_VERTEX);
  BRepLib::BuildCurves3d(aNewEdge, aTol);

  NewEdgeData aNED;
  aNED.myOffsetC = BRep_Tool::Curve(aNewEdge, aNED.myL, aF, aL);

  Handle(Geom_BSplineCurve)::DownCast(aNED.myOffsetC)->Degree();

  /*if ( theIdx == 14 )
  {
    m_plotter.DRAW_SURFACE(anOffsetSurf, Color_Green, "anOffsetSurf");
  }*/
  if ( !aNED.myOffsetC.IsNull() )
  {
    TCollection_AsciiString name("aNED.myOffsetC_");
    name += theIdx;
    m_plotter.REDRAW_CURVE(name, aNED.myOffsetC, Color_Green);
  }
  /*}*/

  // Iterate over adjacent faces for the current edge and compute max deviation.
  Standard_Real anEdgeTol = 0.0;
  TopTools_ListOfShape::Iterator anIter(aFacesList);
  for ( ; !aNED.myOffsetC.IsNull() && anIter.More() ; anIter.Next())
  {
    const TopoDS_Face& aCurFace = TopoDS::Face(anIter.Value());

    if (!myFaceInfo.IsBound(aCurFace))
      continue;

    // Create offset curve on surface.
    const Handle(Geom2d_Curve) aC2dNew = BRep_Tool::CurveOnSurface(theEdge, aCurFace, aF, aL);
    const Handle(Adaptor2d_HCurve2d) aHCurve2d = new Geom2dAdaptor_HCurve(aC2dNew, aF, aL);
    const Handle(Adaptor3d_HSurface) aHSurface = new GeomAdaptor_HSurface(myFaceInfo.Find(aCurFace).myOffsetS);
    Adaptor3d_CurveOnSurface aCurveOnSurf(aHCurve2d, aHSurface);

    // Extract 3d-curve (it is not null).
    const GeomAdaptor_Curve aCurve3d(aNED.myOffsetC, aF, aL);

    // It is necessary to compute maximal deviation (tolerance).
    Standard_Real aMaxTol = 0.0;
    ShapeAnalysis_Edge::ComputeDeviation(aCurve3d, aCurveOnSurf, Standard_True, aMaxTol, NCONTROL);
    anEdgeTol = Max (anEdgeTol, aMaxTol);
  }
  aNED.myTol = Max(BRep_Tool::Tolerance(aNewEdge), anEdgeTol);

  // Save computed 3d curve in map.
  myEdgeInfo.Bind(theEdge, aNED);
}

//=============================================================================
//function : FillVertexData
//purpose  : 
//=============================================================================
void BRepOffset_SimpleOffset1::FillVertexData(const TopoDS_Vertex& theVertex,
                                             const TopTools_IndexedDataMapOfShapeListOfShape& theVertexEdgeMap,
                                             const TopTools_IndexedDataMapOfShapeListOfShape& theVertexFaceMap,
                                             const Standard_Integer theIdx)
{
  // Algorithm:
  // Find adjacent edges for the given vertex.
  // Find corresponding end on the each adjacent edge.
  // Get offset points for founded end.
  // Set result vertex position as barycenter of founded points.

  gp_Pnt aCurrPnt = BRep_Tool::Pnt(theVertex);

  const TopTools_ListOfShape& aEdgesList = theVertexEdgeMap(theIdx);
  const TopTools_ListOfShape& aFacesList = theVertexFaceMap(theIdx);

  if (aEdgesList.Size() == 0)
    return; // Free verices are skipped.

  // Array to store offset points.
  NCollection_Vector<gp_Pnt> anOffsetPointVec;

  Standard_Real aMaxEdgeTol = 0.0;

  // Iterate over adjacent edges.
  TopTools_ListOfShape::Iterator anIterEdges(aEdgesList);
  for (; anIterEdges.More() ; anIterEdges.Next() )
  {
    const TopoDS_Edge& aCurrEdge = TopoDS::Edge(anIterEdges.Value());

    if (!myEdgeInfo.IsBound(aCurrEdge))
      continue; // Skip shared edges with wrong orientation.

    // Find the closest bound.
    Standard_Real aF, aL;
    Handle(Geom_Curve) aC3d = BRep_Tool::Curve(aCurrEdge, aF, aL);

    // Protection from degenerated edges.
    if (aC3d.IsNull())
      continue;

    const gp_Pnt aPntF = aC3d->Value(aF);
    const gp_Pnt aPntL = aC3d->Value(aL);

    if ( theIdx == 14 )
    {
      m_plotter.DRAW_SHAPE(aCurrEdge, Color_Red, "aCurrEdge");
      m_plotter.DRAW_POINT(aPntF, Color_Red, "aPntF");
      m_plotter.DRAW_POINT(aPntL, Color_Red, "aPntL");
    }

    const Standard_Real aSqDistF = aPntF.SquareDistance(aCurrPnt);
    const Standard_Real aSqDistL = aPntL.SquareDistance(aCurrPnt);

    Standard_Real aMinParam = aF, aMaxParam = aL;
    if (aSqDistL < aSqDistF)
    {
      // Square distance to last point is closer.
      aMinParam = aL; aMaxParam = aF;
    }

    // Compute point on offset edge.
    const NewEdgeData& aNED = myEdgeInfo.Find(aCurrEdge);

    const Handle(Geom_Curve) &anOffsetCurve = aNED.myOffsetC;

    double fff = anOffsetCurve->FirstParameter();
    double lll = anOffsetCurve->LastParameter();

    gp_Pnt anOriginalPoint = aC3d->Value(aMinParam);
    /*const*/ gp_Pnt anOffsetPoint = anOffsetCurve->Value(aMinParam);

    std::cout << "Distance: " << anOriginalPoint.Distance(anOffsetPoint) << std::endl;

    gp_XYZ anOffsetPointOnFaces;
    int aNumFaces = 0;
    for ( TopTools_ListIteratorOfListOfShape aFaceIter(aFacesList); aFaceIter.More(); aFaceIter.Next() )
    {
      aNumFaces++;
      const TopoDS_Face& aFace = TopoDS::Face( aFaceIter.Value() );

      gp_Pnt2d aUV = BRep_Tool::Parameters(theVertex, aFace);

      if ( theIdx == 14 )
      {
        std::cout << "U = " << aUV.X() << std::endl;
        std::cout << "V = " << aUV.Y() << std::endl;
      }

      double uMin, uMax, vMin, vMax;
      myFaceInfo.Find(aFace).myOffsetS->Bounds(uMin, uMax, vMin, vMax);

      gp_XYZ anOffsetPointOnSurf = myFaceInfo.Find(aFace).myOffsetS->Value( aUV.X() > uMax ? uMax : aUV.X(),
                                                                            aUV.Y() > vMax ? vMax : aUV.Y() ).XYZ();

      // 5.2866714434994027
      // 5.2866714434994000
      // (5219.35, -1079.95, 619.74)

      

      anOffsetPointOnFaces += anOffsetPointOnSurf;
    }
    if ( aNumFaces )
      anOffsetPointOnFaces /= aNumFaces;

    if ( theIdx == 14 )
    {
      m_plotter.DRAW_CURVE(anOffsetCurve, Color_Green, "anOffsetCurve");
      m_plotter.DRAW_POINT(anOffsetPoint, Color_Green, "anOffsetPoint");
      m_plotter.DRAW_POINT(anOffsetPointOnFaces, Color_Green, "anOffsetPointOnFaces");
    }

    anOffsetPoint = anOffsetPointOnFaces;

    anOffsetPointVec.Append(anOffsetPoint);

    // Handle situation when edge is closed.
    TopoDS_Vertex aV1, aV2;
    TopExp::Vertices(aCurrEdge, aV1, aV2);
    if (aV1.IsSame(aV2))
    {
      const gp_Pnt anOffsetPointLast = anOffsetCurve->Value(aMaxParam);
      anOffsetPointVec.Append(anOffsetPointLast);
    }

    aMaxEdgeTol = Max(aMaxEdgeTol, aNED.myTol);
  }

  // NCollection_Vector starts from 0 by default.
  // It's better to use lower() and upper() in this case instead of direct indexes range.
  gp_Pnt aCenter(0.0, 0.0, 0.0);
  for(Standard_Integer i  = anOffsetPointVec.Lower();
                       i <= anOffsetPointVec.Upper();
                       ++i)
  {
    aCenter.SetXYZ(aCenter.XYZ() + anOffsetPointVec.Value(i).XYZ());
  }
  aCenter.SetXYZ(aCenter.XYZ() / anOffsetPointVec.Size());

  // Compute max distance.
  Standard_Real aSqMaxDist = 0.0;
  for(Standard_Integer i  = anOffsetPointVec.Lower();
                       i <= anOffsetPointVec.Upper();
                       ++i)
  {
    const Standard_Real aSqDist = aCenter.SquareDistance(anOffsetPointVec.Value(i));
    if (aSqDist > aSqMaxDist)
      aSqMaxDist = aSqDist;
  }

  const Standard_Real aResTol = Max(aMaxEdgeTol, Sqrt(aSqMaxDist));

  const Standard_Real aMultCoeff = 1.001; // Avoid tolernace problems.
  NewVertexData aNVD;
  aNVD.myP = aCenter;
  aNVD.myTol = aResTol * aMultCoeff;

  // Save computed vertex info.
  myVertexInfo.Bind(theVertex, aNVD);
}
