//-----------------------------------------------------------------------------
// Created on: 30 March 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_JoinEdges.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepTools_ReShape.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <GCE2d_MakeLine.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAPI_Interpolate.hxx>
#include <Precision.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeFix_Wire.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

static TopoDS_Wire FixWire(const TopoDS_Wire& theWire)
{
  ShapeFix_Wire aWireFixer;
  aWireFixer.Load(theWire);
  aWireFixer.FixClosed();
  aWireFixer.FixGaps2d();
  aWireFixer.FixConnected();
  return aWireFixer.Wire();
}

//! Constructor.
//! \param masterCAD [in] full CAD model.
asiAlgo_JoinEdges::asiAlgo_JoinEdges(const TopoDS_Shape& masterCAD)
{
  m_master = masterCAD;
}

//! Joins the given edges in the master model.
//! \param edges [in] edges to join.
//! \param face  [in] base face.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_JoinEdges::Perform(const TopTools_IndexedMapOfShape& edges,
                                const TopoDS_Face&                face)
{
  if ( edges.Extent() != 2 )
  {
    std::cout << "Error: only pair of edges is accepted" << std::endl;
    return false;
  }

  BRep_Builder BB;

  // Join edges
  TopoDS_Edge E1, E2, newE;
  this->chooseOrder(edges, E1, E2);
  //
  if ( !this->joinEdges(E1, E2, face, newE) )
  {
    std::cout << "Error: cannot join edges" << std::endl;
    return false;
  }

  // Build a new wire. It will be re-ordered properly by healing at the end
  Handle(ShapeExtend_WireData) WD = new ShapeExtend_WireData;
  for ( TopExp_Explorer exp(face, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    WD->Add( exp.Current() );
  }
  WD->Add( newE );
  //
  TopoDS_Wire W = FixWire( WD->Wire() );

  // Build another face
  BRepBuilderAPI_MakeFace mkFace( BRep_Tool::Surface(face), Precision::Confusion() );
  TopoDS_Face newFace = mkFace.Face();
  //
  if ( face.Orientation() == TopAbs_REVERSED )
    newFace.Reverse();
  //
  BB.Add(newFace, W);

  // Change old face with the reconstructed one
  Handle(BRepTools_ReShape) ReShape = new BRepTools_ReShape;
  ReShape->Replace(face, newFace);
  m_result = ReShape->Apply(m_master);

  return true; // Success
}

//! Finds a correct geometric order in which the edges should follow one
//! after another in order to be joined properly.
//! \param edges   [in] initial collection of edges.
//! \param eFirst  [in] first edge.
//! \param eSecond [in] second edge.
void asiAlgo_JoinEdges::chooseOrder(const TopTools_IndexedMapOfShape& edges,
                                    TopoDS_Edge&                      eFirst,
                                    TopoDS_Edge&                      eSecond) const
{
  const TopoDS_Edge& eCandidate1 = TopoDS::Edge( edges(1) );
  const TopoDS_Edge& eCandidate2 = TopoDS::Edge( edges(2) );

  double f1, l1, f2, l2;
  Handle(Geom_Curve) cCandidate1 = BRep_Tool::Curve(eCandidate1, f1, l1);
  Handle(Geom_Curve) cCandidate2 = BRep_Tool::Curve(eCandidate2, f2, l2);

  const double dist1 = cCandidate1->Value(l1).Distance( cCandidate2->Value(f2) );
  const double dist2 = cCandidate2->Value(l2).Distance( cCandidate1->Value(f1) );

  if ( dist1 < dist2 )
  {
    eFirst  = eCandidate1;
    eSecond = eCandidate2;
  }
  else
  {
    eFirst  = eCandidate2;
    eSecond = eCandidate1;
  }
}

//! Joins a couple of edges into a single edge.
//! \param edge_A  [in]  first edge to join.
//! \param edge_B  [in]  second edge to join.
//! \param face    [in]  base face.
//! \param eResult [out] result edge to fill gap.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_JoinEdges::joinEdges(const TopoDS_Edge& eFirst,
                                  const TopoDS_Edge& eSecond,
                                  const TopoDS_Face& face,
                                  TopoDS_Edge&       eResult) const
{
  // Get orientation of the edges on their host faces. We want to have
  // orientation irrelevant of face orientation, as we are going to
  // work in the parametric domain
  TopAbs_Orientation eFirstOri = TopAbs_EXTERNAL, eSecondOri = TopAbs_EXTERNAL;
  //
  for ( TopExp_Explorer exp(face, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    if ( exp.Current().IsSame(eFirst) )
      eFirstOri = exp.Current().Orientation();
    else if ( exp.Current().IsSame(eSecond) )
      eSecondOri = exp.Current().Orientation();
  }

  // Choose orientation of the bridge edge being built
  TopoDS_Edge eForward;
  if ( eFirstOri == TopAbs_FORWARD )
    eForward = eFirst;
  else if ( eSecondOri == TopAbs_FORWARD )
    eForward = eSecond;
  //
  TopAbs_Orientation bridgeOri = eFirst.Orientation();

  // Get p-curve for the first edge
  double first_f, first_l;
  Handle(Geom2d_Curve) cFirst = BRep_Tool::CurveOnSurface(eFirst, face, first_f, first_l);

  // Get p-curve for the second edge
  double second_f, second_l;
  Handle(Geom2d_Curve) cSecond = BRep_Tool::CurveOnSurface(eSecond, face, second_f, second_l);

  // Extract basic curve for the trimmed one
  if ( cFirst->IsKind( STANDARD_TYPE(Geom2d_TrimmedCurve) ) )
    cFirst = Handle(Geom2d_TrimmedCurve)::DownCast(cFirst)->BasisCurve();

  // Extract basic curve for the trimmed one
  if ( cSecond->IsKind( STANDARD_TYPE(Geom2d_TrimmedCurve) ) )
    cSecond = Handle(Geom2d_TrimmedCurve)::DownCast(cSecond)->BasisCurve();

  // Choose tolerance
  const double toler = Precision::Confusion();

  // Get host surface
  Handle(Geom_Surface) surf = BRep_Tool::Surface(face);

  // For straight lines we build a straight bridge
  if ( cFirst->IsKind( STANDARD_TYPE(Geom2d_Line) ) &&
       cSecond->IsKind( STANDARD_TYPE(Geom2d_Line) ) )
  {
    // Extremity points
    gp_Pnt2d P12d = cFirst->Value(first_l);
    gp_Pnt2d P22d = cSecond->Value(second_f);
    gp_Pnt   P1   = surf->Value( P12d.X(), P12d.Y() );
    gp_Pnt   P2   = surf->Value( P22d.X(), P22d.Y() );

    // Make vertices
    TopoDS_Vertex V1, V2;
    BRep_Builder BB;
    BB.MakeVertex(V1, P1, toler);
    BB.MakeVertex(V2, P2, toler);

    // Build planar edge
    BRepBuilderAPI_MakeEdge mkEdge(V1, V2);
    eResult = mkEdge.Edge();

    // Recover missing geometry
    ShapeFix_Edge SFE;
    SFE.FixAddPCurve(eResult, face, 0);
  }
  else
  {
    gp_Pnt2d P12d, P22d;
    gp_Vec2d T12d, T22d;
    cFirst->D1(first_l, P12d, T12d);
    cSecond->D1(second_f, P22d, T22d);

    Handle(TColgp_HArray1OfPnt2d) pts = new TColgp_HArray1OfPnt2d(1, 2);
    pts->ChangeValue(1) = P12d;
    pts->ChangeValue(2) = P22d;

    Geom2dAPI_Interpolate Interp(pts, 0, toler);
    Interp.Load(T12d, T22d);
    Interp.Perform();

    // Build edge
    Handle(Geom2d_Curve) crv2d = Interp.Curve();
    BRepBuilderAPI_MakeEdge mkEdge(crv2d, surf);
    eResult = mkEdge.Edge();

    // Recover missing geometry
    ShapeFix_Edge SFE;
    SFE.FixAddCurve3d(eResult);
  }

  // Set orientation
  eResult.Orientation(bridgeOri);

  return true;
}
