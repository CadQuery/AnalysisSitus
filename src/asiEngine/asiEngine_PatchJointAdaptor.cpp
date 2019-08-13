//-----------------------------------------------------------------------------
// Created on: 28 June 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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
//    * Neither the name of the copyright holder(s) nor the
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
#include <asiEngine_PatchJointAdaptor.h>

// asiEngine includes
#include <asiEngine_RE.h>

// OpenCascade includes
#include <Geom_TrimmedCurve.hxx>

// Mobius includes
#include <mobius/cascade.h>
#include <mobius/core_Precision.h>
#include <mobius/geom_BSplineCurve.h>

using namespace mobius;

//-----------------------------------------------------------------------------

asiEngine_PatchJointAdaptor::asiEngine_PatchJointAdaptor(const Handle(asiEngine_Model)& model,
                                                         ActAPI_ProgressEntry           progress,
                                                         ActAPI_PlotterEntry            plotter)
: asiAlgo_PatchJointAdaptor (progress, plotter),
  m_model                   (model)
{}

//-----------------------------------------------------------------------------

bool asiEngine_PatchJointAdaptor::Init(const Handle(asiData_ReEdgeNode)& edgeNode)
{
  if ( edgeNode.IsNull() || !edgeNode->IsWellFormed() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Null or inconsistent edge.");
    return false;
  }

  m_edge = edgeNode;

  // Prepare RE API.
  asiEngine_RE reApi(m_model, m_progress);

  // Get owner patches.
  if ( !reApi.GetPatchesByEdge(m_edge,
                               m_coedgeLeft, m_coedgeRight,
                               m_patchLeft, m_patchRight) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot get patches by edge.");
    return false;
  }

  if ( !m_patchLeft.IsNull() )
    m_progress.SendLogMessage( LogInfo(Normal) << "Left patch: %1." << m_patchLeft->GetName() );

  if ( !m_patchRight.IsNull() )
    m_progress.SendLogMessage( LogInfo(Normal) << "Right patch: %1." << m_patchRight->GetName() );

  if ( m_patchRight.IsNull() || m_patchRight.IsNull() )
  {
    m_progress.SendLogMessage( LogWarn(Normal) << "Naked edge: there is no left or/and right patch." );
    return false; // Naked edge.
  }

  if ( m_coedgeLeft.IsNull() || m_coedgeRight.IsNull() )
  {
    m_progress.SendLogMessage( LogWarn(Normal) << "Incomplete topology: there is no left or/and right coedge." );
    return false; // Incomplete topology.
  }

  // Get next and previous coedges.
  m_coedgeLeftTop  = reApi.GetNext     (m_coedgeLeft);
  m_coedgeLeftBot  = reApi.GetPrevious (m_coedgeLeft);
  m_coedgeRightTop = reApi.GetPrevious (m_coedgeRight);
  m_coedgeRightBot = reApi.GetNext     (m_coedgeRight);
  //
  if ( m_coedgeLeftTop.IsNull() )
  {
    m_progress.SendLogMessage( LogWarn(Normal) << "Incomplete topology: there is no left-top coedge." );
    return false; // Incomplete topology.
  }
  //
  if ( m_coedgeLeftBot.IsNull() )
  {
    m_progress.SendLogMessage( LogWarn(Normal) << "Incomplete topology: there is no left-bottom coedge." );
    return false; // Incomplete topology.
  }
  //
  if ( m_coedgeRightTop.IsNull() )
  {
    m_progress.SendLogMessage( LogWarn(Normal) << "Incomplete topology: there is no right-top coedge." );
    return false; // Incomplete topology.
  }
  //
  if ( m_coedgeRightBot.IsNull() )
  {
    m_progress.SendLogMessage( LogWarn(Normal) << "Incomplete topology: there is no right-bottom coedge." );
    return false; // Incomplete topology.
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiEngine_PatchJointAdaptor::AlignEdges()
{
  /* Align top edges. */

  Handle(Geom_BSplineCurve)
    ltCurve = Handle(Geom_BSplineCurve)::DownCast( m_coedgeLeftTop->GetEdge()->GetCurve() );

  Handle(Geom_BSplineCurve)
    rtCurve = Handle(Geom_BSplineCurve)::DownCast( m_coedgeRightTop->GetEdge()->GetCurve() );

  if ( !this->alignControlPoles( ltCurve, m_coedgeLeftTop->IsSameSense(),
                                 rtCurve, m_coedgeRightTop->IsSameSense() ) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Failed to align top curves.");
    return false;
  }

  // Set curves back to the edges.
  m_coedgeLeftTop->GetEdge()->SetCurve(ltCurve);
  m_coedgeRightTop->GetEdge()->SetCurve(rtCurve);

  /* Align bottom edges. */

  Handle(Geom_BSplineCurve)
    lbCurve = Handle(Geom_BSplineCurve)::DownCast( m_coedgeLeftBot->GetEdge()->GetCurve() );

  Handle(Geom_BSplineCurve)
    rbCurve = Handle(Geom_BSplineCurve)::DownCast( m_coedgeRightBot->GetEdge()->GetCurve() );

  if ( !this->alignControlPoles( lbCurve, !m_coedgeLeftBot->IsSameSense(),
                                 rbCurve, !m_coedgeRightBot->IsSameSense() ) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Failed to align bottom curves.");
    return false;
  }

  // Set curves back to the edges.
  m_coedgeLeftBot->GetEdge()->SetCurve(lbCurve);
  m_coedgeRightBot->GetEdge()->SetCurve(rbCurve);

  return true; // Success.
}

//-----------------------------------------------------------------------------

void asiEngine_PatchJointAdaptor::DumpGraphically()
{
  ActAPI_Color revsColor = Color_Blue,
               forwColor = Color_Red;

  m_plotter.REDRAW_SURFACE ("patch-left",  m_patchLeft->GetSurface(),  Color_Default);
  m_plotter.REDRAW_SURFACE ("patch-right", m_patchRight->GetSurface(), Color_Default);
  //
  m_plotter.REDRAW_CURVE   ("coedge-left",      this->getCoedgeCurveRepr(m_coedgeLeft),     m_coedgeLeft     ->IsSameSense() ? forwColor : revsColor, false );
  m_plotter.REDRAW_CURVE   ("coedge-left-top",  this->getCoedgeCurveRepr(m_coedgeLeftTop),  m_coedgeLeftTop  ->IsSameSense() ? forwColor : revsColor, false );
  m_plotter.REDRAW_CURVE   ("coedge-left-bot",  this->getCoedgeCurveRepr(m_coedgeLeftBot),  m_coedgeLeftBot  ->IsSameSense() ? forwColor : revsColor, false );
  m_plotter.REDRAW_CURVE   ("coedge-right",     this->getCoedgeCurveRepr(m_coedgeRight),    m_coedgeRight    ->IsSameSense() ? forwColor : revsColor, false );
  m_plotter.REDRAW_CURVE   ("coedge-right-top", this->getCoedgeCurveRepr(m_coedgeRightTop), m_coedgeRightTop ->IsSameSense() ? forwColor : revsColor, false );
  m_plotter.REDRAW_CURVE   ("coedge-right-bot", this->getCoedgeCurveRepr(m_coedgeRightBot), m_coedgeRightBot ->IsSameSense() ? forwColor : revsColor, false );
}

//-----------------------------------------------------------------------------

bool
  asiEngine_PatchJointAdaptor::alignControlPoles(Handle(Geom_BSplineCurve)& curveLeft,
                                                 const bool                 sameSenseLeft,
                                                 Handle(Geom_BSplineCurve)& curveRight,
                                                 const bool                 sameSenseRight)
{
  // Convert curves to Mobius structures.
  t_ptr<t_bcurve> mbCurveLeft  = cascade::GetMobiusBCurve(curveLeft);
  t_ptr<t_bcurve> mbCurveRight = cascade::GetMobiusBCurve(curveRight);

  const int numPolesLeft  = mbCurveLeft->GetNumOfPoles();
  const int numPolesRight = mbCurveRight->GetNumOfPoles();

  // Get poles to change.
  t_xyz* pPoleLeft       = &mbCurveLeft  ->ChangePole ( sameSenseLeft  ? 1                 : numPolesLeft - 2 );
  t_xyz* pPoleRight      = &mbCurveRight ->ChangePole ( sameSenseRight ? numPolesRight - 2 : 1 );
  t_xyz  poleCornerLeft  = mbCurveLeft   ->GetPole    ( sameSenseLeft  ? 0                 : numPolesLeft - 1);
  t_xyz  poleCornerRight = mbCurveRight  ->GetPole    ( sameSenseRight ? numPolesRight - 1 : 0);
  t_xyz  poleCorner      = (poleCornerLeft + poleCornerRight)*0.5;

  // Check if the curves are already aligned.
  t_xyz vLeft2Corner  = (*pPoleLeft - poleCorner).Normalized();
  t_xyz vCorner2Right = (poleCorner - *pPoleRight).Normalized();
  //
  const double dot = vLeft2Corner.Dot(vCorner2Right);
  //
  m_progress.SendLogMessage(LogInfo(Normal) << "dot = %1" << dot);

  m_plotter.DRAW_POINT(cascade::GetOpenCascadePnt(*pPoleLeft),  Color_Red,   "pleft");
  m_plotter.DRAW_POINT(cascade::GetOpenCascadePnt(*pPoleRight), Color_Green, "pright");
  m_plotter.DRAW_POINT(cascade::GetOpenCascadePnt(poleCorner),  Color_Blue,  "pcorner");

  t_xyz vLeft2Right = (*pPoleRight - *pPoleLeft).Normalized();

  const double dr = (*pPoleRight - poleCorner).Modulus();
  const double dl = (*pPoleLeft  - poleCorner).Modulus();

  t_xyz newPoleRight = poleCorner + vLeft2Right*dr;
  t_xyz newPoleLeft  = poleCorner - vLeft2Right*dl;

  // Update control points.
  pPoleRight->SetXYZ(newPoleRight);
  pPoleLeft->SetXYZ(newPoleLeft);

  // Override curves.
  curveLeft  = cascade::GetOpenCascadeBCurve(mbCurveLeft);
  curveRight = cascade::GetOpenCascadeBCurve(mbCurveRight);

  return true; // Success.
}

//-----------------------------------------------------------------------------

Handle(Geom_Curve)
  asiEngine_PatchJointAdaptor::getCoedgeCurveRepr(const Handle(asiData_ReCoedgeNode)& coedgeNode) const
{
  // Get edge.
  Handle(asiData_ReEdgeNode)
    edgeNode = coedgeNode->GetEdge();

  // Get patch.
  Handle(asiData_RePatchNode)
    patchNode = Handle(asiData_RePatchNode)::DownCast( coedgeNode->GetParentNode() );

  // Get host surface of the patch.
  Handle(Geom_Surface) surf = patchNode->GetSurface();

  // Analyze basic local properties at coedge.
  bool isSurfGoesU, isLeftBound;
  double uMin, uMax, vMin, vMax;
  //
  if ( !this->AnalyzeJoint(edgeNode->GetCurve(), surf,
                           isSurfGoesU, isLeftBound,
                           uMin, uMax, vMin, vMax) )
    return NULL;

  // Apply small shift to have a nice margin for graphical representation.
  const double uDelta = (uMax - uMin)*0.1;
  const double vDelta = (vMax - vMin)*0.1;
  const double delta  = isSurfGoesU ? vDelta : uDelta;

  // Get the corresponding isoline.
  Handle(Geom_Curve)
    iso = ( isSurfGoesU ? surf->VIso(isLeftBound ? vMin + delta : vMax - delta)
                        : surf->UIso(isLeftBound ? uMin + delta : uMax - delta) );

  // Trim for better visualization.
  iso = new Geom_TrimmedCurve(iso, iso->FirstParameter() + delta, iso->LastParameter() - delta);

  // Reverse for non-samesense coedges.
  if ( !coedgeNode->IsSameSense() )
    iso->Reverse();

  return iso;
}
