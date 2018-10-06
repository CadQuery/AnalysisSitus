//-----------------------------------------------------------------------------
// Created on: 06 October 2018
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
#include <asiAlgo_CheckContour.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <Geom_Curve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_Wire.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #pragma message("===== warning: DRAW_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiAlgo_CheckContour::asiAlgo_CheckContour(ActAPI_ProgressEntry progress,
                                           ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_CheckContour::Check_isSingleWire(const TopoDS_Shape& contour) const
{
  int nWires = 0;
  for ( TopExp_Explorer exp(contour, TopAbs_WIRE); exp.More(); exp.Next() )
    nWires++;

  if ( nWires > 1 )
    return false;

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_CheckContour::Check_connectedWire(const TopoDS_Shape& contour,
                                               const bool          doCheckSharing,
                                               const double        prec) const
{
  // Extract wire.
  TopExp_Explorer exp(contour, TopAbs_WIRE);
  TopoDS_Wire W = TopoDS::Wire( exp.Current() );

  // Prepare wire data.
  Handle(ShapeExtend_WireData) WD = new ShapeExtend_WireData(W);
  const int nEdges = WD->NbEdges();
  if ( nEdges == 1 )
  {
    double f, l;
    Handle(Geom_Curve) C = BRep_Tool::Curve( WD->Edge(1), f, l );

    if ( C.IsNull() )
      return false;

    if ( C->IsPeriodic() )
      return true;

    gp_Pnt Pf = C->Value( C->FirstParameter() );
    gp_Pnt Pl = C->Value( C->LastParameter() );

    return Pf.Distance(Pl) < gp::Resolution();
  }
  else if ( !nEdges )
    return false;

  // Check connectivity.
  for ( int e = 1; e <= nEdges; ++e )
  {
    const int e_next = ( e == nEdges ? 1 : e + 1 );

    TopoDS_Edge E1 = WD->Edge(e);
    TopoDS_Edge E2 = WD->Edge(e_next);

    TopoDS_Vertex V1_f = ShapeAnalysis_Edge().FirstVertex (E1);
    TopoDS_Vertex V1_l = ShapeAnalysis_Edge().LastVertex  (E1);
    TopoDS_Vertex V2_f = ShapeAnalysis_Edge().FirstVertex (E2);
    TopoDS_Vertex V2_l = ShapeAnalysis_Edge().LastVertex  (E2);

    if ( doCheckSharing )
    {
      if ( !V1_f.IsSame(V2_l) && !V1_f.IsSame(V2_f) && !V1_l.IsSame(V2_f) && !V1_l.IsSame(V2_l) )
        return false;
    }
    else
    {
      gp_Pnt P1_f = BRep_Tool::Pnt(V1_f);
      gp_Pnt P1_l = BRep_Tool::Pnt(V1_l);
      gp_Pnt P2_f = BRep_Tool::Pnt(V2_f);
      gp_Pnt P2_l = BRep_Tool::Pnt(V2_l);

      bool isConnected = false;

      if ( P1_f.Distance(P2_f) < prec )
        isConnected = true;
      else if ( P1_f.Distance(P2_l) < prec )
        isConnected = true;
      else if ( P1_l.Distance(P2_f) < prec )
        isConnected = true;
      else if ( P1_l.Distance(P2_l) < prec )
        isConnected = true;

      if ( !isConnected )
        return false;
    }
  }

  return true;
}
