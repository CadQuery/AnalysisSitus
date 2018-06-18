// Created on: 2016-10-14
// Created by: Alexander MALYSHEV
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

#ifndef _BRepOffset_SimpleOffset1_HeaderFile
#define _BRepOffset_SimpleOffset1_HeaderFile

#include <BRepTools_Modification.hxx>
#include <GeomAbs_Shape.hxx>
#include <Geom2d_Curve.hxx>
#include <Standard_Real.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <NCollection_DataMap.hxx>

#include <ActAPI_IAlgorithm.h>

#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>

class Offset_HAdaptor3dCurveOnSurface;

class Offset_Adaptor3dCurveOnSurface : public Adaptor3d_CurveOnSurface
{
public:

  Offset_Adaptor3dCurveOnSurface() : Adaptor3d_CurveOnSurface() {}
  
  Offset_Adaptor3dCurveOnSurface(const Handle(Adaptor3d_HSurface)& S) : Adaptor3d_CurveOnSurface(S) {}
  
  //! Creates a CurveOnSurface from the 2d curve <C> and
  //! the surface <S>.
  Offset_Adaptor3dCurveOnSurface(const Handle(Adaptor2d_HCurve2d)& C, const Handle(Adaptor3d_HSurface)& S) : Adaptor3d_CurveOnSurface(C, S) {
    myCurve = C;
    mySurface = S;

    myUmin = mySurface->FirstUParameter();
    myUmax = mySurface->LastUParameter();
    myVmin = mySurface->FirstVParameter();
    myVmax = mySurface->LastVParameter();
  }

  void Load (const Handle(Adaptor2d_HCurve2d)& C, const Handle(Adaptor3d_HSurface)& S)
  {
    Adaptor3d_CurveOnSurface::Load(C, S);
    myCurve = C;
    mySurface = S;

    myUmin = mySurface->FirstUParameter();
    myUmax = mySurface->LastUParameter();
    myVmin = mySurface->FirstVParameter();
    myVmax = mySurface->LastVParameter();
  }

  Handle(Adaptor3d_HCurve) Trim (const Standard_Real First, const Standard_Real Last, const Standard_Real Tol) const Standard_OVERRIDE;

  //! Computes the point of parameter U on the curve.
  void D0 (const Standard_Real U, gp_Pnt& P) const Standard_OVERRIDE
  {
    gp_Pnt2d Puv;
  
    myCurve->D0(U,Puv);
    mySurface->D0(Puv.X() > myUmax ? myUmax : Puv.X(),
                  Puv.Y() > myVmax ? myVmax : Puv.Y(),
                  P);
  }
  //
  ////! Computes the point of parameter U on the curve with its
  ////! first derivative.
  ////! Raised if the continuity of the current interval
  ////! is not C1.
  //void D1 (const Standard_Real U, gp_Pnt& P, gp_Vec& V) const Standard_OVERRIDE
  //{
  //    gp_Pnt2d Puv;
  //  gp_Vec2d Duv;
  //  gp_Vec D1U,D1V;
  //
  //  Standard_Real FP = myCurve->FirstParameter();
  //  Standard_Real LP = myCurve->LastParameter();
  //
  //  Standard_Real Tol= Precision::PConfusion()/10; 
  //  if( ( Abs(U-FP)<Tol)&&(!myFirstSurf.IsNull()) )
  //    {
  //      myCurve->D1(U,Puv,Duv);
  //      myFirstSurf->D1(Puv.X(),Puv.Y(),P,D1U,D1V);
  //      V.SetLinearForm(Duv.X(),D1U,Duv.Y(),D1V);
  //    }
  //  else
  //    if( (Abs(U-LP)<Tol)&&(!myLastSurf.IsNull()) )
  //      {
	 // myCurve->D1(U,Puv,Duv);
	 // myLastSurf->D1(Puv.X(),Puv.Y(),P,D1U,D1V);
	 // V.SetLinearForm(Duv.X(),D1U,Duv.Y(),D1V);
  //      }
  //    else
  //      if      (myType == GeomAbs_Line  ) ElCLib::D1(U,myLin ,P,V);
  //      else if (myType == GeomAbs_Circle) ElCLib::D1(U,myCirc,P,V);
  //      else {
	 // myCurve->D1(U,Puv,Duv);
	 // mySurface->D1(Puv.X(),Puv.Y(),P,D1U,D1V);
	 // V.SetLinearForm(Duv.X(),D1U,Duv.Y(),D1V);
  //      }
  //}

protected:

  Handle(Adaptor3d_HSurface) mySurface;
  Handle(Adaptor2d_HCurve2d) myCurve;

  double myUmin, myUmax, myVmin, myVmax;

};

class Offset_HAdaptor3dCurveOnSurface : public Adaptor3d_HCurve
{

public:

  
  //! Creates an empty GenHCurve.
  Offset_HAdaptor3dCurveOnSurface() : Adaptor3d_HCurve() {}
  
  //! Creates a GenHCurve from a Curve
  Offset_HAdaptor3dCurveOnSurface(const Offset_Adaptor3dCurveOnSurface& C)
  {
    myCurve = C;
  }
  
  //! Sets the field of the GenHCurve.
  void Set (const Offset_Adaptor3dCurveOnSurface& C)
  {
    myCurve = C;
  }
  
  //! Returns the curve used to create the GenHCurve.
  //! This is redefined from HCurve, cannot be inline.
  const Adaptor3d_Curve& Curve() const Standard_OVERRIDE
  {
    return myCurve;
  }
  
  //! Returns the curve used to create the GenHCurve.
  //! This is redefined from HCurve, cannot be inline.
  Adaptor3d_Curve& GetCurve() Standard_OVERRIDE
  {
    return myCurve;
  }
  
  //! Returns the curve used to create the GenHCurve.
  Offset_Adaptor3dCurveOnSurface& ChangeCurve()
  {
    return myCurve;
  }




  DEFINE_STANDARD_RTTI_INLINE(Offset_HAdaptor3dCurveOnSurface,Adaptor3d_HCurve)

protected:


  Offset_Adaptor3dCurveOnSurface myCurve;


private:




};



class BRepOffset_SimpleOffset1;
DEFINE_STANDARD_HANDLE(BRepOffset_SimpleOffset1, BRepTools_Modification)

//! This class represents mechanism of simple offset algorithm i. e.
//! topology-preserve offset construction without intersection.
//!
//! The list below shows mapping scheme:
//! - Each surface is mapped to its geometric offset surface.
//! - For each edge, pcurves are mapped to the same pcurves on offset surfaces.
//! - For each edge, 3d curve is constructed by re-approximation of pcurve on the first offset face.
//! - Position of each vertex in a result shell is computed as average point of all ends of edges shared by that vertex.
//! - Tolerances are updated according to the resulting geometry.
class BRepOffset_SimpleOffset1 : public BRepTools_Modification
{
public:

  DEFINE_STANDARD_RTTI_INLINE(BRepOffset_SimpleOffset1, BRepTools_Modification)

  //! Constructor.
  //! @param theInputShape shape to be offset
  //! @param theOffsetValue offset distance (signed)
  //! @param theTolerance tolerance for handling singular points
  Standard_EXPORT BRepOffset_SimpleOffset1(ActAPI_ProgressEntry progress,
                                           ActAPI_PlotterEntry  plotter,
                                           const TopoDS_Shape& theInputShape,
                                           const Standard_Real theOffsetValue,
                                           const Standard_Real theTolerance);

  //! Returns Standard_True  if  the face  <F> has  been
  //! modified.  In this  case, <S> is the new geometric
  //! support of  the  face, <L> the  new location,<Tol>
  //! the new  tolerance.<RevWires> has  to  be set   to
  //! Standard_True   when the modification reverses the
  //! normal of  the   surface.(the wires   have  to  be
  //! reversed).   <RevFace>   has   to   be   set    to
  //! Standard_True if  the orientation  of the modified
  //! face changes in the  shells which contain  it.  --
  //! Here, <RevFace>  will  return Standard_True if the
  //! -- gp_Trsf is negative.
  Standard_EXPORT Standard_Boolean NewSurface (const TopoDS_Face& F,
                                               Handle(Geom_Surface)& S,
                                               TopLoc_Location& L,
                                               Standard_Real& Tol,
                                               Standard_Boolean& RevWires,
                                               Standard_Boolean& RevFace) Standard_OVERRIDE;

  //! Returns Standard_True  if  the edge  <E> has  been
  //! modified.  In this case,  <C> is the new geometric
  //! support of the  edge, <L> the  new location, <Tol>
  //! the         new    tolerance.   Otherwise, returns
  //! Standard_False,    and  <C>,  <L>,   <Tol> are not
  //! significant.
  Standard_EXPORT Standard_Boolean NewCurve (const TopoDS_Edge& E,
                                             Handle(Geom_Curve)& C,
                                             TopLoc_Location& L,
                                             Standard_Real& Tol) Standard_OVERRIDE;

  //! Returns  Standard_True if the  vertex <V> has been
  //! modified.  In this  case, <P> is the new geometric
  //! support of the vertex,   <Tol> the new  tolerance.
  //! Otherwise, returns Standard_False, and <P>,  <Tol>
  //! are not significant.
  Standard_EXPORT Standard_Boolean NewPoint (const TopoDS_Vertex& V,
                                             gp_Pnt& P,
                                             Standard_Real& Tol) Standard_OVERRIDE;

  //! Returns Standard_True if  the edge  <E> has a  new
  //! curve on surface on the face <F>.In this case, <C>
  //! is the new geometric support of  the edge, <L> the
  //! new location, <Tol> the new tolerance.
  //! Otherwise, returns  Standard_False, and <C>,  <L>,
  //! <Tol> are not significant.
  Standard_EXPORT Standard_Boolean NewCurve2d (const TopoDS_Edge& E,
                                               const TopoDS_Face& F,
                                               const TopoDS_Edge& NewE,
                                               const TopoDS_Face& NewF,
                                               Handle(Geom2d_Curve)& C,
                                               Standard_Real& Tol) Standard_OVERRIDE;

  //! Returns Standard_True if the Vertex  <V> has a new
  //! parameter on the  edge <E>. In  this case,  <P> is
  //! the parameter,    <Tol>  the     new    tolerance.
  //! Otherwise, returns Standard_False, and <P>,  <Tol>
  //! are not significant.
  Standard_EXPORT Standard_Boolean NewParameter (const TopoDS_Vertex& V,
                                                 const TopoDS_Edge& E,
                                                 Standard_Real& P,
                                                 Standard_Real& Tol) Standard_OVERRIDE;

  //! Returns the  continuity of  <NewE> between <NewF1>
  //! and <NewF2>.
  //!
  //! <NewE> is the new  edge created from <E>.  <NewF1>
  //! (resp. <NewF2>) is the new  face created from <F1>
  //! (resp. <F2>).
  Standard_EXPORT GeomAbs_Shape Continuity (const TopoDS_Edge& E,
                                            const TopoDS_Face& F1,
                                            const TopoDS_Face& F2,
                                            const TopoDS_Edge& NewE,
                                            const TopoDS_Face& NewF1,
                                            const TopoDS_Face& NewF2) Standard_OVERRIDE;

private:

  //! Method to fill new face data for single face.
  void FillFaceData(const TopoDS_Face& theFace);

  //! Method to fill new edge data for single edge.
  void FillEdgeData(const TopoDS_Edge& theEdge,
                    const TopTools_IndexedDataMapOfShapeListOfShape& theEdgeFaceMap,
                    const Standard_Integer theIdx);

  //! Method to fill new vertex data for single vertex.
  void FillVertexData(const TopoDS_Vertex& theVertex,
                      const TopTools_IndexedDataMapOfShapeListOfShape& theVertexEdgeMap,
                      const TopTools_IndexedDataMapOfShapeListOfShape& theVertexFaceMap,
                      const Standard_Integer theIdx);

  struct NewFaceData
  {
    Handle(Geom_Surface) myOffsetS;
    TopLoc_Location myL;
    Standard_Real myTol;
    Standard_Boolean myRevWires;
    Standard_Boolean myRevFace;
  };

  struct NewEdgeData
  {
    Handle(Geom_Curve) myOffsetC; // Resulting curve.
    TopLoc_Location myL;
    Standard_Real myTol;
  };

  struct NewVertexData
  {
    gp_Pnt myP;
    Standard_Real myTol;
  };

  //! Fills offset data.
  void FillOffsetData(const TopoDS_Shape& theInputShape);

  //! Copy-assignment constructor and copy constructor are not allowed.
  void operator=(const BRepOffset_SimpleOffset1&);
  BRepOffset_SimpleOffset1(const BRepOffset_SimpleOffset1&);

  //! Map of faces to new faces information.
  NCollection_DataMap<TopoDS_Face, NewFaceData> myFaceInfo;

  //! Map of edges to new edges information.
  NCollection_DataMap<TopoDS_Edge, NewEdgeData> myEdgeInfo;

  //! Map of vertices to new vertices information.
  NCollection_DataMap<TopoDS_Vertex, NewVertexData> myVertexInfo;

  //! Offset value.
  Standard_Real myOffsetValue;

  //! Tolerance.
  Standard_Real myTolerance;

  ActAPI_ProgressEntry m_progress;
  ActAPI_PlotterEntry  m_plotter;
};

#endif // _BRepOffset_SimpleOffset1_HeaderFile
