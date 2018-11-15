//-----------------------------------------------------------------------------
// Created on: 17 February 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
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
#include <asiAlgo_ModBase.h>

// asiAlgo includes
#include <asiAlgo_IntersectCC.h>
#include <asiAlgo_IntersectSS.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Precision.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #pragma message("===== warning: DRAW_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiAlgo_ModBase::asiAlgo_ModBase(ActAPI_ProgressEntry notifier,
                                 ActAPI_PlotterEntry  plotter)
: asiAlgo_BRepNormalization (),
  m_progress                (notifier),
  m_plotter                 (plotter)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_ModBase::intersectSurfaces(const TopoDS_Face&                   F1,
                                        const TopoDS_Face&                   F2,
                                        Handle(asiAlgo_IntersectionCurveSS)& res) const
{
  // Get host surfaces to re-intersect
  Handle(Geom_Surface) S1 = BRep_Tool::Surface(F1);
  Handle(Geom_Surface) S2 = BRep_Tool::Surface(F2);

  // Choose tight precision to be as accurate as possible
  const double prec = Precision::Confusion();

  // Intersection curves
  asiAlgo_IntersectionCurvesSS icurves;

  // Intersect
  asiAlgo_IntersectSS interSS(m_progress, m_plotter);
  //
  if ( !interSS(S1, S2, prec, icurves) )
  {
    m_progress.SendLogMessage( LogErr(Normal) << "Cannot construct intersection curve for two faces." );
    return false;
  }

  if ( icurves.Length() != 1 )
  {
    m_progress.SendLogMessage( LogErr(Normal) << "Unexpected number of intersection curves: %1." << icurves.Length() );
    return false;
  }

  res = icurves(1);

#if defined DRAW_DEBUG
  m_plotter.DRAW_CURVE(res->C, Color_Violet, "intersectFaces:icurve");
#endif

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_ModBase::intersectCurves(const Handle(Geom_Curve)&            C1,
                                      const double                         f1,
                                      const double                         l1,
                                      const Handle(Geom_Curve)&            C2,
                                      const double                         f2,
                                      const double                         l2,
                                      Handle(asiAlgo_IntersectionPointCC)& res) const
{
#if defined DRAW_DEBUG
  m_plotter.DRAW_CURVE(new Geom_TrimmedCurve(C1, f1, l1), Color_Magenta, "intersectEdges:C1");
  m_plotter.DRAW_CURVE(new Geom_TrimmedCurve(C2, f2, l2), Color_Magenta, "intersectEdges:C2");
#endif

  // Choose tight precision to be as accurate as possible
  const double prec = Precision::Confusion();

  // Intersection points
  asiAlgo_IntersectionPointsCC ipoints;

  // Intersect
  asiAlgo_IntersectCC interCC(m_progress, m_plotter);
  //
  if ( !interCC(C1, f1, l1, C2, f2, l2, prec, ipoints) )
  {
    m_progress.SendLogMessage( LogErr(Normal) << "Cannot construct intersection point for edges." );
    return false;
  }

  if ( ipoints.Length() != 1 )
  {
    m_progress.SendLogMessage( LogErr(Normal) << "Unexpected number of intersection points: %1." << ipoints.Length() );
    return false;
  }

  res = ipoints(1);
  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_ModBase::correctOriC(const Handle(Geom_Curve)& newCurve,
                                  const TopoDS_Edge&        oldEdge,
                                  Handle(Geom_Curve)&       correctedCurve) const
{
  double f, l;
  Handle(Geom_Curve) oldCurve = BRep_Tool::Curve(oldEdge, f, l);

  // Check natural orientation at start point
  // ...

  gp_Pnt oldCurveP;
  gp_Vec oldCurveV1;
  oldCurve->D1(f, oldCurveP, oldCurveV1);

  gp_Pnt newCurveP;
  gp_Vec newCurveV1;
  newCurve->D1(f, newCurveP, newCurveV1);

  bool toReverse;
  if ( Abs( oldCurveV1.Angle(newCurveV1) ) > Abs( oldCurveV1.Angle( newCurveV1.Reversed() ) ) )
    toReverse = true;
  else
    toReverse = false;

  if ( !toReverse )
  {
    correctedCurve = newCurve;
    return true;
  }

  // Reverse geometrically
  correctedCurve = newCurve->Reversed();
  return true;
}
