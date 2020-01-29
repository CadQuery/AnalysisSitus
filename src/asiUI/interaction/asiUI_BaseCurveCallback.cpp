//-----------------------------------------------------------------------------
// Created on: 21 December (*) 2018
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
#include <asiUI_BaseCurveCallback.h>

// asiUI includes
#include <asiUI_Viewer.h>

// asiVisu includes
#include <asiVisu_NodeInfo.h>
#include <asiVisu_PrsManager.h>

// OCCT includes
#include <gp_Lin.hxx>
#include <ShapeAnalysis_Curve.hxx>

//-----------------------------------------------------------------------------

asiUI_BaseCurveCallback::asiUI_BaseCurveCallback(asiUI_Viewer* pViewer)
//
: asiUI_ViewerCallback(pViewer)
{}

//-----------------------------------------------------------------------------

asiUI_BaseCurveCallback::~asiUI_BaseCurveCallback()
{}

//-----------------------------------------------------------------------------

bool asiUI_BaseCurveCallback::getPickedPointOnCurve(void*                        pCallData,
                                                    Handle(asiData_IVCurveNode)& curveNode,
                                                    gp_Pnt&                      resultPt,
                                                    double&                      resultParam) const
{
  // Convert passed data to cell picker result.
  asiVisu_CellPickerResult* pPickRes = (asiVisu_CellPickerResult*) pCallData;

  // Get picked position.
  gp_XYZ pos = pPickRes->GetPickedPos();

  // Get picked curve.
  Handle(Geom_Curve) curve = this->getPickedCurve(pCallData, curveNode);
  //
  if ( curve.IsNull() )
    return false;

  // Invert point on curve.
  ShapeAnalysis_Curve sac;
  sac.Project(curve, pos, 1.0e-3, resultPt, resultParam);

  return true;
}

//-----------------------------------------------------------------------------

Handle(Geom_Curve)
  asiUI_BaseCurveCallback::getPickedCurve(void*                        pCallData,
                                          Handle(asiData_IVCurveNode)& curveNode) const
{
  curveNode = this->getPickedCurveNode(pCallData);
  //
  if ( curveNode.IsNull() || !curveNode->IsWellFormed() )
    return nullptr;

  // Get Curve.
  return curveNode->GetCurve();
}

//-----------------------------------------------------------------------------

Handle(asiData_IVCurveNode)
  asiUI_BaseCurveCallback::getPickedCurveNode(void* pCallData) const
{
  // Convert passed data to cell picker result.
  asiVisu_CellPickerResult* pPickRes = (asiVisu_CellPickerResult*) pCallData;

  // Get Node Info from Actor.
  asiVisu_NodeInfo* nodeInfo = asiVisu_NodeInfo::Retrieve( pPickRes->GetPickedActor() );
  //
  if ( !nodeInfo )
    return nullptr;

  // Get Curve Node.
  Handle(ActAPI_INode) N = m_model->FindNode( nodeInfo->GetNodeId() );
  //
  if ( !N->IsInstance( STANDARD_TYPE(asiData_IVCurveNode) ) )
    return nullptr;

  return Handle(asiData_IVCurveNode)::DownCast(N);
}
