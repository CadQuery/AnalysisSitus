//-----------------------------------------------------------------------------
// Created on: 26 December 2018
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
#include <asiUI_PickCurveCallback.h>

// asiEngine includes
#include <asiEngine_IV.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

//-----------------------------------------------------------------------------

asiUI_PickCurveCallback* asiUI_PickCurveCallback::New()
{
  return new asiUI_PickCurveCallback(NULL);
}

//-----------------------------------------------------------------------------

asiUI_PickCurveCallback::asiUI_PickCurveCallback(asiUI_Viewer* pViewer)
//
: asiUI_ViewerCallback(pViewer)
{}

//-----------------------------------------------------------------------------

asiUI_PickCurveCallback::~asiUI_PickCurveCallback()
{}

//-----------------------------------------------------------------------------

void asiUI_PickCurveCallback::Execute(vtkObject*    pCaller,
                                      unsigned long eventId,
                                      void*         pCallData)
{
  asiUI_NotUsed(pCaller);

  // Custom point on the mesh was clicked.
  if ( eventId == EVENT_SELECT_WORLD_POINT )
  {
    gp_XYZ hit;
    if ( !this->getPickedPoint(pCallData, hit) )
      return;

    // Find Curve Node with the name in question.
    asiEngine_IV IV(m_model, m_notifier, m_plotter);
    //
    Handle(asiData_IVCurveNode) curveNode = IV.Find_Curve(m_name);

    // Modify Data Model.
    m_model->OpenCommand();
    {
      if ( curveNode.IsNull() )
        curveNode = IV.Create_Curve(m_name, false);

      // Add reper point.
      curveNode->AddReperPoint(hit);

      // Get all available reper points.
      std::vector<gp_XYZ> pts;
      curveNode->GetReperPoints(pts);

      // Build curve.
      if ( pts.size() > 1 )
      {
        Handle(Geom_BSplineCurve) curve;
        //
        if ( !asiAlgo_Utils::ApproximatePoints(pts, 3, 3, Precision::Confusion(), curve) )
        {
          m_model->AbortCommand();
          //
          m_notifier.SendLogMessage(LogErr(Normal) << "Curve approximation failed.");
          return;
        }

        // Update Node.
        curveNode->SetCurve(curve);
      }
    }
    m_model->CommitCommand();

    // Update Curve Node.
    m_pViewer->PrsMgr()->Actualize(curveNode);
    m_pBrowser->Populate();
  }
}
