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
#include <asiUI_AddCurveHandleCallback.h>

// asiUI includes
#include <asiUI_Viewer.h>

// asiVisu includes
#include <asiVisu_Utils.h>

//-----------------------------------------------------------------------------

asiUI_AddCurveHandleCallback* asiUI_AddCurveHandleCallback::New()
{
  return new asiUI_AddCurveHandleCallback(NULL);
}

//-----------------------------------------------------------------------------

asiUI_AddCurveHandleCallback::asiUI_AddCurveHandleCallback(asiUI_Viewer* pViewer)
//
: asiUI_BaseCurveCallback(pViewer)
{}

//-----------------------------------------------------------------------------

asiUI_AddCurveHandleCallback::~asiUI_AddCurveHandleCallback()
{}

//-----------------------------------------------------------------------------

void asiUI_AddCurveHandleCallback::Execute(vtkObject*    pCaller,
                                           unsigned long eventId,
                                           void*         pCallData)
{
  asiUI_NotUsed(pCaller);

  if ( eventId == EVENT_SELECT_CELL )
  {
    // Get picked point.
    Handle(asiData_IVCurveNode) curveNode;
    gp_Pnt                      hitPt;
    double                      hitParam;
    //
    if ( !this->getPickedPointOnCurve(pCallData, curveNode, hitPt, hitParam) )
      return;

    // Store the obtained parameter as a handle of curve.
    m_model->OpenCommand();
    {
      curveNode->AddHandle(hitParam);
    }
    m_model->CommitCommand();

    // Actualize Presentation.
    m_pViewer->PrsMgr()->Actualize(curveNode);
  }
}
