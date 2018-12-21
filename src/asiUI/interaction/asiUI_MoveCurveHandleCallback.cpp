//-----------------------------------------------------------------------------
// Created on: 21 December 2018
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
#include <asiUI_MoveCurveHandleCallback.h>

// asiUI includes
#include <asiUI_Viewer.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// OCCT includes
#include <Geom_BSplineCurve.hxx>

//-----------------------------------------------------------------------------

asiUI_MoveCurveHandleCallback* asiUI_MoveCurveHandleCallback::New()
{
  return new asiUI_MoveCurveHandleCallback(NULL);
}

//-----------------------------------------------------------------------------

asiUI_MoveCurveHandleCallback::asiUI_MoveCurveHandleCallback(asiUI_Viewer* pViewer)
//
: asiUI_CurveHandleCallback(pViewer)
{}

//-----------------------------------------------------------------------------

asiUI_MoveCurveHandleCallback::~asiUI_MoveCurveHandleCallback()
{}

//-----------------------------------------------------------------------------

void asiUI_MoveCurveHandleCallback::Execute(vtkObject*    pCaller,
                                            unsigned long eventId,
                                            void*         pCallData)
{
  asiUI_NotUsed(pCaller);

  // Process event.
  if ( eventId == EVENT_SELECT_CELL )
  {
    // Convert passed data to cell picker result.
    asiVisu_CellPickerResult* pPickRes = (asiVisu_CellPickerResult*) pCallData;

    // Get picked Curve Node.
    Handle(asiData_IVCurveNode) curveNode = this->getPickedCurveNode(pCallData);
    //
    if ( curveNode.IsNull() )
      return;

    // Store in the Data Model.
    m_model->OpenCommand();
    {
      // Get ID of the picked handle.
      const int
        activeHandle = pPickRes->GetPickedElementIds().GetMinimalMapped();

      // Set index.
      curveNode->SetActiveHandle(activeHandle);
    }
    m_model->CommitCommand();

    // Get ready for moving points.
    m_currentCurveNode = curveNode;
  }
  else if ( eventId == EVENT_SELECT_WORLD_POINT )
  {
    if ( m_currentCurveNode.IsNull() || m_currentCurveNode->GetActiveHandle() == -1 )
      return;

    // Get picked point.
    gp_XYZ hit;
    if ( !this->getPickedPoint(pCallData, hit) )
      return;

    // Get curve.
    Handle(Geom_BSplineCurve)
      curve = Handle(Geom_BSplineCurve)::DownCast( m_currentCurveNode->GetCurve() );

    // Make a copy.
    Handle(Geom_BSplineCurve)
      resCurve = Handle(Geom_BSplineCurve)::DownCast( curve->Copy() );

    // Move point.
    const int index1 = 2, index2 = resCurve->Poles().Length() - 1;
    int resIndex1, resIndex2;
    const double u = m_currentCurveNode->GetActiveHandleParam();
    //
    resCurve->MovePoint(u, hit, index1, index2, resIndex1, resIndex2);

    // Store in the data model.
    m_model->OpenCommand();
    {
      m_currentCurveNode->SetCurve(resCurve);
      m_currentCurveNode->SetActiveHandle(-1);
    }
    m_model->CommitCommand();

    // Actualize.
    m_pViewer->PrsMgr()->Actualize(m_currentCurveNode);

    // Nullify curve.
    m_currentCurveNode.Nullify();
  }
}
