//-----------------------------------------------------------------------------
// Created on: 21 November 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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
#include <asiUI_PickContourCallback.h>

// asiAlgo includes
#include <asiAlgo_HitFacet.h>
#include <asiAlgo_ProjectPointOnMesh.h>

// asiUI includes
#include <asiUI_ViewerPart.h>

// asiVisu includes
#include <asiVisu_PrsManager.h>
#include <asiVisu_Utils.h>

// OCCT includes
#include <gp_Lin.hxx>

//-----------------------------------------------------------------------------

void hitMidPoint(asiAlgo_ProjectPointOnMesh&        HitFacet,
                 const Handle(asiData_ContourNode)& contourNode,
                 const gp_XYZ&                      prevPt,
                 const gp_XYZ&                      nextPt)
{
  if ( (nextPt - prevPt).Modulus() < 1e-2 )
    return;

  // Get midpoint.
  const gp_XYZ midPt = 0.5*(prevPt + nextPt);

  // Hit facet for the midpoint.
  gp_Pnt hitMidPt = HitFacet.Perform(midPt);
  int midPtFacetIndex = (HitFacet.GetTriIdx().size() ? HitFacet.GetTriIdx()[0] : -1);
  //
  const bool isProjected = (midPtFacetIndex != -1);
  //if ( !HitFacet(midPt, 1.0e-3, hitMidPt, midPtFacetIndex) )
  //
    hitMidPoint(HitFacet, contourNode, prevPt, hitMidPt.XYZ());
  //}
  //else
  //{
  if ( isProjected )
  {
    gp_XYZ prevPersistent = contourNode->GetPoint(contourNode->GetNumPoints() - 1);

    if ( (prevPersistent - hitMidPt.XYZ()).Modulus() > 1.0 )
      contourNode->AddPoint(hitMidPt.XYZ(), midPtFacetIndex);
  }

  hitMidPoint(HitFacet, contourNode, hitMidPt.XYZ(), nextPt);
  //}
}

//-----------------------------------------------------------------------------

//! Instantiation routine.
//! \return instance of the callback class.
asiUI_PickContourCallback* asiUI_PickContourCallback::New()
{
  return new asiUI_PickContourCallback(NULL);
}

//-----------------------------------------------------------------------------

//! Constructor accepting owning viewer as a parameter.
//! \param theViewer [in] owning viewer.
asiUI_PickContourCallback::asiUI_PickContourCallback(asiUI_Viewer* theViewer)
: asiUI_ViewerCallback(theViewer)
{}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_PickContourCallback::~asiUI_PickContourCallback()
{}

//-----------------------------------------------------------------------------

//! Listens to a dedicated event. Performs all useful operations.
//! \param theCaller   [in] caller instance.
//! \param theEventId  [in] ID of the event triggered this listener.
//! \param theCallData [in] invocation context.
void asiUI_PickContourCallback::Execute(vtkObject*    vtkNotUsed(theCaller),
                                        unsigned long vtkNotUsed(theEventId),
                                        void*         theCallData)
{
  const vtkSmartPointer<asiVisu_PrsManager>& mgr = this->GetViewer()->PrsMgr();

  // Get picking ray
  gp_Lin pickRay = *( (gp_Lin*) theCallData );

  for ( size_t k = 0; k < m_bvhs.size(); ++k )
  {
    // Prepare a tool to find the intersected facet
    asiAlgo_HitFacet HitFacet(m_bvhs[k], m_notifier, m_plotter);
    asiAlgo_ProjectPointOnMesh HitFacet2(m_bvhs[k], NULL, NULL);

    // Find intersection
    gp_XYZ hit;
    int facet_idx;
    if ( !HitFacet(pickRay, facet_idx, hit) )
    {
      m_notifier.SendLogMessage(LogWarn(Normal) << "Cannot find the intersected facet.");
      return;
    }

    // Get object to store the contour
    Handle(asiData_ContourNode)
      contour_n = m_model->GetPartNode()->GetContour();
    //
    if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
    {
      m_notifier.SendLogMessage(LogWarn(Normal) << "No persistent data for contour available.");
      return;
    }

    // Get active face index
    const int fidx = m_bvhs[k]->GetFacet(facet_idx).FaceIndex;
    //
    m_notifier.SendLogMessage(LogInfo(Normal) << "Picked point (%1, %2, %3) on face %4"
                                              << hit.X()
                                              << hit.Y()
                                              << hit.Z()
                                              << fidx);

    // Modify data
    m_model->OpenCommand();
    {
      const int numContourPts = contour_n->GetNumPoints();

      // Experiment with middle point
      if ( numContourPts > 0 )
      {
        // Get previous point
        const gp_XYZ prevPt = contour_n->GetPoint(numContourPts - 1);

        // Add midpoints
        hitMidPoint(HitFacet2, contour_n, prevPt, hit);
      }

      // Add hitted point
      contour_n->AddPoint(hit, fidx);
    }
    m_model->CommitCommand();
    //
    mgr->Actualize( contour_n.get() );
  }
}
