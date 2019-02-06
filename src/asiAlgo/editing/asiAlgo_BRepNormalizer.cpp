//-----------------------------------------------------------------------------
// Created on: 10 November 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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

// Own include
#include <asiAlgo_BRepNormalizer.h>

// OCCT includes
#include <Message_ProgressIndicator.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NullObject.hxx>
#include <TColStd_ListIteratorOfListOfTransient.hxx>
#include <TColStd_ListOfTransient.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

#include <Geom2d_Line.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <BRepTools.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <gp_Pnt.hxx>

#include <gp.hxx>

#include <Standard_NullObject.hxx>
#include <gp_Trsf.hxx>
#include <BRepTools_TrsfModification.hxx>
#include <Message_ProgressSentry.hxx>
#include <Geom_Surface.hxx>

static void SetShapeFlags(const TopoDS_Shape& theInSh, TopoDS_Shape& theOutSh);

//-----------------------------------------------------------------------------

asiAlgo_BRepNormalizer::asiAlgo_BRepNormalizer(ActAPI_ProgressEntry progress,
                                               ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm (progress, plotter)
{}

//-----------------------------------------------------------------------------

asiAlgo_BRepNormalizer::asiAlgo_BRepNormalizer(const TopoDS_Shape&  S,
                                               ActAPI_ProgressEntry progress,
                                               ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm (progress, plotter),
  myShape           (S)
{
  this->put(S);
}

//-----------------------------------------------------------------------------

asiAlgo_BRepNormalizer::asiAlgo_BRepNormalizer(const TopoDS_Shape&                      S,
                                               const Handle(asiAlgo_BRepNormalization)& M,
                                               ActAPI_ProgressEntry                     progress,
                                               ActAPI_PlotterEntry                      plotter)
: ActAPI_IAlgorithm (progress, plotter),
  myShape           (S)
{
  this->put(S);
  this->Perform(M);
}

//-----------------------------------------------------------------------------

void asiAlgo_BRepNormalizer::Init(const TopoDS_Shape& S)
{
  myShape = S;
  this->put(S);
}

//-----------------------------------------------------------------------------

#ifdef DEBUG_Modifier
static TopTools_IndexedMapOfShape MapE, MapF;
#endif

bool asiAlgo_BRepNormalizer::Perform(const Handle(asiAlgo_BRepNormalization)& M)
{
  if ( myShape.IsNull() )
    return false;

#ifdef DEBUG_Modifier
  MapE.Clear(); MapF.Clear();
  TopExp::MapShapes(myShape, TopAbs_EDGE, MapE);
  TopExp::MapShapes(myShape, TopAbs_FACE, MapF);
#endif
  TopTools_DataMapIteratorOfDataMapOfShapeShape theIter(myMap);

  TopTools_IndexedDataMapOfShapeListOfShape aMVE, aMEF;
  TopExp::MapShapesAndAncestors(myShape, TopAbs_VERTEX, TopAbs_EDGE, aMVE);
  TopExp::MapShapesAndAncestors(myShape, TopAbs_EDGE, TopAbs_FACE, aMEF);

  if ( !this->createNewVertices(aMVE, M) )
    return false;

  if ( !this->fillNewCurveInfo(aMEF, M) )
    return false;

  if ( !this->fillNewSurfaceInfo(M) )
    return false;

  if ( !this->createOtherVertices(aMVE, aMEF, M) )
    return false;

  bool aNewGeom;
  this->rebuildRecursively(myShape, M, aNewGeom);

  if (myShape.ShapeType() == TopAbs_FACE)
  {
    if (myShape.Orientation() == TopAbs_REVERSED)
    {
      myMap(myShape).Reverse();
    }
    else
    {
      myMap(myShape).Orientation(myShape.Orientation());
    } 
  }
  else {
    myMap(myShape).Orientation(myShape.Orientation());
  }

  // Update the continuities
  BRep_Builder aBB;
  for (int ii = 1; ii <= aMEF.Extent(); ii++)
  {
    const TopoDS_Edge& CurE = TopoDS::Edge(aMEF.FindKey(ii));
    const TopoDS_Edge& NewE = TopoDS::Edge(myMap(CurE));
    if (!CurE.IsSame(NewE)) {
      TopTools_ListIteratorOfListOfShape it;
      it.Initialize(aMEF.FindFromKey(CurE));
      TopoDS_Face F1,F2;
      while (it.More() && F2.IsNull()) {
        if (F1.IsNull()) {
          F1 = TopoDS::Face(it.Value());
        }
        else {
          F2 = TopoDS::Face(it.Value());
        }
        it.Next();
      }
      if (!F2.IsNull()) {
        const TopoDS_Face& newf1 = TopoDS::Face(myMap(F1));
        const TopoDS_Face& newf2 = TopoDS::Face(myMap(F2));
        GeomAbs_Shape
          Newcont = M->Continuity(CurE,F1,F2,NewE,newf1,newf2);
        //
        if ( Newcont > GeomAbs_C0 )
        {
          aBB.Continuity(NewE,newf1,newf2,Newcont);
        }
      }
    }
    theIter.Next();
  }

  return true;
}

//-----------------------------------------------------------------------------

void asiAlgo_BRepNormalizer::put(const TopoDS_Shape& S)
{
  if (!myMap.IsBound(S)) {
    myMap.Bind(S,TopoDS_Shape());
    for(TopoDS_Iterator theIterator(S,false);theIterator.More();theIterator.Next()) {

      this->put(theIterator.Value());
    }
  }
}

//-----------------------------------------------------------------------------

bool asiAlgo_BRepNormalizer::rebuildRecursively(const TopoDS_Shape&                      S,
                                                const Handle(asiAlgo_BRepNormalization)& M,
                                                bool&                                    newGeom)
{
#ifdef DEBUG_Modifier
  int iF = MapF.Contains(S) ? MapF.FindIndex(S) : 0;
  int iE = MapE.Contains(S) ? MapE.FindIndex(S) : 0;
#endif

  TopAbs_ShapeEnum ts     = S.ShapeType();
  TopoDS_Shape&    result = myMap(S);
  //
  if ( !result.IsNull() )
  {
    newGeom = myHasNewGeom.Contains(S);
    return !S.IsSame(result);
  }

  bool rebuild = false, RevWires = false;
  TopAbs_Orientation ResOr = TopAbs_FORWARD;
  BRep_Builder B;
  double tol;
  bool No3DCurve = false; // en fait, si on n`a pas de 
  //modif geometry 3d , it is necessary to test the existence of a curve 3d.

  // new geometry ?

  switch (ts)
  {
  case TopAbs_FACE:
    {
      rebuild = myNSInfo.IsBound(TopoDS::Face(S));
      if (rebuild) 
      {
        const NewSurfaceInfo& aNSinfo = myNSInfo(TopoDS::Face(S));
        RevWires = aNSinfo.myRevWires;
        B.MakeFace(TopoDS::Face(result),aNSinfo.mySurface,
          aNSinfo.myLoc.Predivided(S.Location()),aNSinfo.myToler);
        result.Location(S.Location());
        if (aNSinfo.myRevFace) 
          ResOr = TopAbs_REVERSED;
        // set specifics flags of a Face
        B.NaturalRestriction(TopoDS::Face(result), BRep_Tool::NaturalRestriction(TopoDS::Face(S)));
      }

      // update triangulation on the copied face
      Handle(Poly_Triangulation) aTriangulation;
      if (M->NewTriangulation(TopoDS::Face(S), aTriangulation))
      {
        if (rebuild) // the copied face already exists => update it
          B.UpdateFace(TopoDS::Face(result), aTriangulation);
        else
        { // create new face with bare triangulation
          B.MakeFace(TopoDS::Face(result), aTriangulation);
          result.Location(S.Location());
        }
        rebuild = true;
      }
    }
    break;

  case TopAbs_EDGE:
    {
      rebuild = myNCInfo.IsBound(TopoDS::Edge(S));
      if (rebuild) 
      {
        const NewCurveInfo& aNCinfo = myNCInfo(TopoDS::Edge(S));
        if (aNCinfo.myCurve.IsNull()) { 
          B.MakeEdge(TopoDS::Edge(result));
          B.Degenerated(TopoDS::Edge(result),
                        BRep_Tool::Degenerated(TopoDS::Edge(S)));
          B.UpdateEdge(TopoDS::Edge(result),aNCinfo.myToler);  //OCC217
          No3DCurve = true;
        }
        else
        {
          B.MakeEdge(TopoDS::Edge(result),aNCinfo.myCurve,
            aNCinfo.myLoc.Predivided(S.Location()),aNCinfo.myToler);
          No3DCurve = false;
        }
        result.Location(S.Location());
//	result.Orientation(S.Orientation());

        // set specifics flags of an Edge
        B.SameParameter(TopoDS::Edge(result),
        BRep_Tool::SameParameter(TopoDS::Edge(S)));
        B.SameRange(TopoDS::Edge(result),
        BRep_Tool::SameRange(TopoDS::Edge(S)));
      }

      // update polygonal structure on the edge
      Handle(Poly_Polygon3D) aPolygon;
      if (M->NewPolygon(TopoDS::Edge(S), aPolygon))
      {
        if (rebuild) // the copied edge already exists => update it
          B.UpdateEdge(TopoDS::Edge(result), aPolygon, S.Location());
        else
        { // create new edge with bare polygon
          B.MakeEdge(TopoDS::Edge(result), aPolygon);
          result.Location(S.Location());
        }
        rebuild = true;
      }
    }
    break;
  default:;
  }

  // rebuild sub-shapes and test new sub-shape ?
  bool newgeom = rebuild;
  newGeom = rebuild;
  TopoDS_Iterator it;
  {
    int aShapeCount = 0;
    {
      for (it.Initialize(S, false); it.More(); it.Next()) ++aShapeCount;
    }

    for (it.Initialize(S, false); it.More(); it.Next())
    {
      // always call rebuild()
      bool isSubNewGeom = false;
      bool subrebuilt = this->rebuildRecursively(it.Value(), M, isSubNewGeom);
      //
      rebuild = subrebuilt || rebuild ;
      newGeom = newGeom || isSubNewGeom;
    }
  }
  if ( newGeom )
    myHasNewGeom.Add(S);

  // make an empty copy
  if (rebuild && !newgeom) {
    result = S.EmptyCopied();
    result.Orientation(TopAbs_FORWARD);
  }

  // Copy the sub-elements
  if (rebuild) {
    TopAbs_Orientation orient;
    for (it.Initialize(S,false); it.More(); it.Next()) {
      orient = it.Value().Orientation();
      if (RevWires || myMap(it.Value()).Orientation() == TopAbs_REVERSED) {
        orient = TopAbs::Reverse(orient);
      }
      B.Add(result,myMap(it.Value()).Oriented(orient));
    }


    if (ts == TopAbs_FACE) {
      // pcurves
      Handle(Geom2d_Curve) curve2d; //,curve2d1;
      TopoDS_Face face = TopoDS::Face(S);
      TopAbs_Orientation fcor = face.Orientation();
      if(fcor != TopAbs_REVERSED) fcor = TopAbs_FORWARD;

      TopExp_Explorer ex(face.Oriented(fcor),TopAbs_EDGE);
      for (;ex.More(); ex.Next()) 
      {
        const TopoDS_Edge& edge = TopoDS::Edge(ex.Current());

#ifdef DEBUG_Modifier
        iE = MapE.Contains(edge) ? MapE.FindIndex(edge) : 0;
#endif
        if ( newGeom &&
             M->NewCurve2d(edge, face, TopoDS::Edge(myMap(ex.Current())), TopoDS::Face(result), curve2d, tol) )
        {
          // rem dub 16/09/97 : Make constant topology or not make at all.
          // Do not make if CopySurface = 1
          // Atention, TRUE sewing edges (RealyClosed)  
          // stay even if  CopySurface is true.
    
          // check that edge contains two pcurves on this surface:
          // either it is true seam on the current face, or belongs to two faces
          // built on that same surface (see OCC21772)
          // Note: this check could be made separate method in BRepTools
          bool isClosed = false;
          if( BRep_Tool::IsClosed(edge,face) )
          {
            isClosed = ( ! newgeom || BRepTools::IsReallyClosed(edge,face) );
            if ( ! isClosed )
            {
              TopLoc_Location aLoc;
              TopoDS_Shape resface = (myMap.IsBound(face) ? myMap(face) : face);
              if(resface.IsNull())
                resface = face;
              Handle(Geom_Surface) aSurf = BRep_Tool::Surface(TopoDS::Face(resface), aLoc);
              // check other faces sharing the same surface
              TopExp_Explorer aExpF(myShape,TopAbs_FACE);
              for( ; aExpF.More() && !isClosed; aExpF.Next())
              {
                TopoDS_Face anOther = TopoDS::Face(aExpF.Current());
                if(anOther.IsSame(face))
                  continue;
                TopoDS_Shape resface2 = (myMap.IsBound(anOther) ? myMap(anOther) : anOther);
                if(resface2.IsNull())
                  resface2 = anOther;
                TopLoc_Location anOtherLoc;
                Handle(Geom_Surface) anOtherSurf = 
                  BRep_Tool::Surface(TopoDS::Face(resface2), anOtherLoc);
                if ( aSurf == anOtherSurf && aLoc.IsEqual (anOtherLoc) )
                {
                  TopExp_Explorer aExpE(anOther,TopAbs_EDGE);
                  for( ; aExpE.More() && !isClosed ; aExpE.Next())
                    isClosed = edge.IsSame(aExpE.Current());
                }
              }
            }
          }
          if (isClosed) 
          {
            TopoDS_Edge CurE = TopoDS::Edge(myMap(edge));
            TopoDS_Shape aLocalResult = result;
            aLocalResult.Orientation(TopAbs_FORWARD);
            TopoDS_Face CurF = TopoDS::Face(aLocalResult);
            Handle(Geom2d_Curve) curve2d1, currcurv;
            double f,l;
            if ((!RevWires && fcor != edge.Orientation()) ||
              ( RevWires && fcor == edge.Orientation())) {
                CurE.Orientation(TopAbs_FORWARD);
                curve2d1 = BRep_Tool::CurveOnSurface(CurE,CurF,f,l);
                if (curve2d1.IsNull()) curve2d1 = new Geom2d_Line(gp::OX2d());
                B.UpdateEdge (CurE, curve2d1, curve2d, CurF, 0.);
            }
            else {
              CurE.Orientation(TopAbs_REVERSED);
              curve2d1 = BRep_Tool::CurveOnSurface(CurE,CurF,f,l);
              if (curve2d1.IsNull()) curve2d1 = new Geom2d_Line(gp::OX2d());
              B.UpdateEdge (CurE, curve2d, curve2d1, CurF, 0.);
            }
            currcurv = BRep_Tool::CurveOnSurface(CurE,CurF,f,l);
            B.Range(CurE,f,l);
          }
          else {
            B.UpdateEdge(TopoDS::Edge(myMap(ex.Current())),
              curve2d,
              TopoDS::Face(result), 0.);
          }

          TopLoc_Location theLoc;
          double theF,theL;
          Handle(Geom_Curve) C3D = BRep_Tool::Curve(TopoDS::Edge(myMap(ex.Current())), theLoc, theF, theL);
          if (C3D.IsNull()) { // Update vertices
            double param;
            TopExp_Explorer ex2(edge,TopAbs_VERTEX);
            while (ex2.More()) {
              const TopoDS_Vertex& vertex = TopoDS::Vertex(ex2.Current());
              if (!M->NewParameter(vertex, edge, param, tol)) {
                tol = BRep_Tool::Tolerance(vertex);
                param = BRep_Tool::Parameter(vertex,edge);
              }

              TopAbs_Orientation vtxrelat = vertex.Orientation();
              if (edge.Orientation() == TopAbs_REVERSED) {
                // Update considere l'edge FORWARD, et le vertex en relatif
                vtxrelat= TopAbs::Reverse(vtxrelat);
              }
              //if (myMap(edge).Orientation() == TopAbs_REVERSED) {
              //  vtxrelat= TopAbs::Reverse(vtxrelat);
              //}

              TopoDS_Vertex aLocalVertex = TopoDS::Vertex(myMap(vertex));
              aLocalVertex.Orientation(vtxrelat);
              //B.UpdateVertex(TopoDS::Vertex
              //(myMap(vertex).Oriented(vtxrelat)),
              B.UpdateVertex(aLocalVertex, param, TopoDS::Edge(myMap(edge)), tol);
              ex2.Next();
            }
          }
        }

        // Copy polygon on triangulation
        Handle(Poly_PolygonOnTriangulation) aPolyOnTria_1, aPolyOnTria_2;
        bool aNewPonT = M->NewPolygonOnTriangulation(edge, face, aPolyOnTria_1);
        if (BRepTools::IsReallyClosed(edge, face))
        {
          // Obtain triangulation on reversed edge
          TopoDS_Edge anEdgeRev = edge;
          anEdgeRev.Reverse();
          aNewPonT = M->NewPolygonOnTriangulation(anEdgeRev, face, aPolyOnTria_2) || aNewPonT;
          // It there is only one polygon on triangulation, store it to aPolyOnTria_1
          if (aPolyOnTria_1.IsNull() && !aPolyOnTria_2.IsNull())
          {
            aPolyOnTria_1 = aPolyOnTria_2;
            aPolyOnTria_2 = Handle(Poly_PolygonOnTriangulation)();
          }
        }
        if (aNewPonT)
        {
          TopLoc_Location aLocation;
          Handle(Poly_Triangulation) aNewFaceTria =
              BRep_Tool::Triangulation(TopoDS::Face(myMap(face)), aLocation);
          TopoDS_Edge aNewEdge = TopoDS::Edge(myMap(edge));
          if (aPolyOnTria_2.IsNull())
            B.UpdateEdge(aNewEdge, aPolyOnTria_1, aNewFaceTria, aLocation);
          else
          {
            if (edge.Orientation() == TopAbs_FORWARD)
              B.UpdateEdge(aNewEdge, aPolyOnTria_1, aPolyOnTria_2, aNewFaceTria, aLocation);
            else
              B.UpdateEdge(aNewEdge, aPolyOnTria_2, aPolyOnTria_1, aNewFaceTria, aLocation);
          }
        }
      }

    }

//    else if (ts == TopAbs_EDGE) {
    else if (ts == TopAbs_EDGE && !No3DCurve) {
      // Vertices
      double param;
      const TopoDS_Edge& edge = TopoDS::Edge(S);
      TopAbs_Orientation edor = edge.Orientation();
      if(edor != TopAbs_REVERSED) edor = TopAbs_FORWARD;
      TopExp_Explorer ex(edge.Oriented(edor), TopAbs_VERTEX);
      while (ex.More()) {
        const TopoDS_Vertex& vertex = TopoDS::Vertex(ex.Current());

        if (!M->NewParameter(vertex, edge, param, tol)) {
          tol = BRep_Tool::Tolerance(vertex);
          param = BRep_Tool::Parameter(vertex,edge);
        }

        TopAbs_Orientation vtxrelat = vertex.Orientation();
        if (edor == TopAbs_REVERSED) {
          // Update considere l'edge FORWARD, et le vertex en relatif
          vtxrelat= TopAbs::Reverse(vtxrelat);
        }

        TopoDS_Vertex aLocalVertex = TopoDS::Vertex(myMap(vertex));
        aLocalVertex.Orientation(vtxrelat);

        if ( !aLocalVertex.IsSame(vertex))
          B.UpdateVertex(aLocalVertex, param, TopoDS::Edge(result), tol);

        ex.Next();
      }
    }

    // update flags
    result.Orientable(S.Orientable());
    result.Closed(S.Closed());
    result.Infinite(S.Infinite());
  }
  else
    result = S;

  // Set flag of the shape.
  result.Orientation(ResOr);

  SetShapeFlags(S, result);

  return rebuild;
}

//-----------------------------------------------------------------------------

bool asiAlgo_BRepNormalizer::createNewVertices(const TopTools_IndexedDataMapOfShapeListOfShape& MVE,
                                               const Handle(asiAlgo_BRepNormalization)&         M)
{
  double aToler;
  BRep_Builder aBB;
  gp_Pnt aPnt;  
  for (int i = 1; i <= MVE.Extent(); i++ )
  {
    //fill MyMap only with vertices with NewPoint == true
    const TopoDS_Vertex& aV = TopoDS::Vertex(MVE.FindKey(i));
    bool IsNewP = M->NewPoint(aV, aPnt, aToler);
    if (IsNewP)
    {
      TopoDS_Vertex aNewV;
      aBB.MakeVertex(aNewV, aPnt, aToler);
      SetShapeFlags(aV, aNewV);
      myMap(aV) = aNewV;
      myHasNewGeom.Add(aV);
    }

    if ( M->IsErrorState() )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Internal error in normalization tool.");
      return false;
    }
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_BRepNormalizer::fillNewCurveInfo(const TopTools_IndexedDataMapOfShapeListOfShape& MEF,
                                              const Handle(asiAlgo_BRepNormalization)&         M)
{
  Handle(Geom_Curve) aCurve;
  TopLoc_Location aLocation;
  asiAlgo_BRepNormalizer::NewCurveInfo aNCinfo;
  double aToler;
  for (int i = 1; i <= MEF.Extent(); i++ )
  {
    const TopoDS_Edge& anE = TopoDS::Edge(MEF.FindKey(i));
    bool IsNewCur = M->NewCurve(anE, aCurve, aLocation, aToler);
    if (IsNewCur)
    {
      aNCinfo.myCurve = aCurve;
      aNCinfo.myLoc = aLocation;
      aNCinfo.myToler = aToler;
      myNCInfo.Bind(anE, aNCinfo);
      myHasNewGeom.Add(anE);
    }

    if ( M->IsErrorState() )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Internal error in normalization tool.");
      return false;
    }
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_BRepNormalizer::fillNewSurfaceInfo(const Handle(asiAlgo_BRepNormalization)& M)
{
  TopTools_IndexedMapOfShape aMF;
  TopExp::MapShapes(myShape, TopAbs_FACE, aMF);
  asiAlgo_BRepNormalizer::NewSurfaceInfo aNSinfo;
  //
  for ( int i = 1; i <= aMF.Extent(); ++i )
  {
    const TopoDS_Face& aF = TopoDS::Face(aMF(i));
    bool RevFace;
    bool RevWires;
    Handle(Geom_Surface) aSurface;
    TopLoc_Location aLocation;
    double aToler1;
    bool IsNewSur = M->NewSurface(aF, aSurface, aLocation, aToler1, RevWires,RevFace);
    if (IsNewSur)
    {
      aNSinfo.mySurface = aSurface;
      aNSinfo.myLoc = aLocation;
      aNSinfo.myToler = aToler1;
      aNSinfo.myRevWires = RevWires;
      aNSinfo.myRevFace = RevFace;
      myNSInfo.Bind(aF, aNSinfo);
      myHasNewGeom.Add(aF);
    }
    else
    {
      //check if subshapes will be modified 
      bool notRebuilded = true;
      TopExp_Explorer exE(aF, TopAbs_EDGE);
      while (exE.More() && notRebuilded) 
      {
        const TopoDS_Edge& anEE = TopoDS::Edge(exE.Current());
        if (myNCInfo.IsBound(anEE))
        {
          notRebuilded = false;
          break;
        }
        TopExp_Explorer exV(anEE, TopAbs_VERTEX);
        while (exV.More() && notRebuilded) 
        {
          const TopoDS_Vertex& aVV = TopoDS::Vertex(exV.Current());
          if (!myMap(aVV).IsNull())
          {
            notRebuilded = false;
            break;
          }
          exV.Next();
        }
        exE.Next();
      }
      if (notRebuilded)
      {
        //subshapes is not going to be modified
        myNonUpdFace.Add(aF); 
      }
    }

    if ( M->IsErrorState() )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Internal error in normalization tool.");
      return false;
    }
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_BRepNormalizer::createOtherVertices(const TopTools_IndexedDataMapOfShapeListOfShape& MVE,
                                                 const TopTools_IndexedDataMapOfShapeListOfShape& MEF,
                                                 const Handle(asiAlgo_BRepNormalization)&         M)
{
  double aToler;

  //The following logic in some ways repeats the logic from the Rebuild() method.
  //If the face with its subshapes is not going to be modified 
  //(i.e. NewSurface() for this face and NewCurve(), NewPoint() for its edges/vertices returns false)
  //then the calling of NewCurve2d() for this face with its edges is not performed. 
  //Therefore, the updating of vertices will not present in such cases and 
  //the EmptyCopied() operation for vertices from this face is not needed. 

  for (int i = 1; i <= MVE.Extent(); i++ )
  {
    const TopoDS_Vertex& aV = TopoDS::Vertex(MVE.FindKey(i));
    TopoDS_Vertex aNewV = TopoDS::Vertex(myMap(aV));
    if ( aNewV.IsNull()) 
    {
       const TopTools_ListOfShape& aLEdges = MVE(i);
       bool toReplace = false;
       TopTools_ListIteratorOfListOfShape it(aLEdges);
       for (; it.More() && !toReplace; it.Next()) 
       {
         const TopoDS_Edge& anE = TopoDS::Edge(it.Value());
         if (myNCInfo.IsBound(anE) && !myNCInfo(anE).myCurve.IsNull())
            toReplace = true;

         if (!toReplace)
         {
           const TopTools_ListOfShape& aLFaces = MEF.FindFromKey(anE);
           TopTools_ListIteratorOfListOfShape it2(aLFaces);
           for (; it2.More(); it2.Next()) 
           {
             const TopoDS_Face& aF = TopoDS::Face(it2.Value());
             if (!myNonUpdFace.Contains(aF))
             {
               Handle(Geom2d_Curve) aCurve2d;

               //some NewCurve2d()s may use NewE arg internally, so the 
               //null TShape as an arg may lead to the exceptions
               TopoDS_Edge aDummyE = TopoDS::Edge(anE.EmptyCopied());

               if ( M->NewCurve2d(anE, aF, aDummyE, TopoDS_Face(), aCurve2d, aToler) )
               {
                 toReplace = true;
                 break;
               }
               //
               if ( M->IsErrorState() )
               {
                 m_progress.SendLogMessage(LogErr(Normal) << "Internal error in normalization tool.");
                 return false;
               }
             }
           }
         }
       }
       if (toReplace)
         aNewV = TopoDS::Vertex( aV.EmptyCopied() );
       else
         aNewV = aV;

       aNewV.Orientation(TopAbs_FORWARD);
       myMap(aV) = aNewV;
    }
  }

  return true;
}

//-----------------------------------------------------------------------------

static void SetShapeFlags(const TopoDS_Shape& inSh, TopoDS_Shape& outSh)
{
  outSh.Modified   ( inSh.Modified()   );
  outSh.Checked    ( inSh.Checked()    );
  outSh.Orientable ( inSh.Orientable() );
  outSh.Closed     ( inSh.Closed()     );
  outSh.Infinite   ( inSh.Infinite()   );
  outSh.Convex     ( inSh.Convex()     );
}
