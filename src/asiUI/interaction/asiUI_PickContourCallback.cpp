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
#include <asiAlgo_MeshProjectLine.h>

// asiUI includes
#include <asiUI_ViewerPart.h>

// asiVisu includes
#include <asiVisu_NodeInfo.h>
#include <asiVisu_PrsManager.h>
#include <asiVisu_Utils.h>

// asiEngine includes
#include <asiEngine_RE.h>

// OCCT includes
#include <gp_Lin.hxx>

//-----------------------------------------------------------------------------

//! Instantiation routine.
//! \return instance of the callback class.
asiUI_PickContourCallback* asiUI_PickContourCallback::New()
{
  return new asiUI_PickContourCallback(NULL);
}

//-----------------------------------------------------------------------------

//! Constructor accepting owning viewer as a parameter.
//! \param[in] pViewer owning viewer.
asiUI_PickContourCallback::asiUI_PickContourCallback(asiUI_Viewer* pViewer)
//
: asiUI_ViewerCallback (pViewer),
  m_pBrowser           (nullptr)
{}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_PickContourCallback::~asiUI_PickContourCallback()
{}

//-----------------------------------------------------------------------------

//! Listens to a dedicated event. Performs all useful operations.
//! \param[in] pCaller   caller instance.
//! \param[in] eventId   ID of the event triggered this listener.
//! \param[in] pCallData data passed from the caller.
void asiUI_PickContourCallback::Execute(vtkObject*    pCaller,
                                        unsigned long eventId,
                                        void*         pCallData)
{
  asiUI_NotUsed(pCaller);

  const vtkSmartPointer<asiVisu_PrsManager>& mgr = this->GetViewer()->PrsMgr();

  if ( eventId == EVENT_SELECT_CELL )
  {
    asiVisu_CellPickerResult* pPickRes = (asiVisu_CellPickerResult*) pCallData;

    // Retrieve Node information.
    asiVisu_NodeInfo* pNodeInfo = asiVisu_NodeInfo::Retrieve( pPickRes->GetPickedActor() );
    //
    if ( !pNodeInfo )
      return;

    // Get Vertex Node.
    Handle(ActAPI_INode) node = m_model->FindNode( pNodeInfo->GetNodeId() );
    //
    if ( !node->IsInstance( STANDARD_TYPE(asiData_ReVertexNode) )
      || !node->IsWellFormed() )
      return;

    Handle(asiData_ReVertexNode)
      vertexNode = Handle(asiData_ReVertexNode)::DownCast(node);

    double vx, vy, vz;
    if ( !vertexNode->GetPoint(vx, vy, vz) )
    {
      m_notifier.SendLogMessage(LogErr(Normal) << "Cannot access vertex coordinates.");
      return;
    }
    //
    gp_XYZ hit(vx, vy, vz);

    if ( m_patch.IsNull() )
    {
      m_notifier.SendLogMessage(LogErr(Normal) << "There is no patch to complete contour for.");
      return;
    }

    // Business logic of reconstruction.
    asiEngine_RE reApi(m_model, m_notifier, m_plotter);

    // Tool for line projection.
    asiAlgo_MeshProjectLine ProjectLineMesh(m_bvh, m_notifier, m_plotter);

    // Modify data.
    m_model->OpenCommand();
    {
      // Prepare Active Contour.
      m_contour = reApi.GetOrCreate_ActiveContour();

      // Connect two points by a projected polyline.
      const int numContourPts = m_contour->GetNumPoints();
      //
      if ( numContourPts > 0 )
      {
        // Get previous point.
        const gp_XYZ prevPt = m_contour->GetPoint(numContourPts - 1);

        // Add midpoints.
        std::vector<gp_XYZ> projPts;
        if ( !ProjectLineMesh.Perform(prevPt, hit, projPts, 0.001) )
        {
          m_notifier.SendLogMessage(LogErr(Normal) << "Cannot project line to mesh.");
          m_model->AbortCommand();
          return;
        }
        //
        if ( projPts.size() > 2 )
          for ( size_t k = 1; k < projPts.size() - 1; ++k ) // Skip ends as they are added individually.
            m_contour->AddPoint(projPts[k], -1);
      }

      // Set contour closed.
      m_contour->SetClosed(true);

      m_notifier.SendLogMessage(LogInfo(Normal) << "Contour closed.");
    }
    m_model->CommitCommand();

    // Update viewer.
    mgr->Actualize(m_contour);
  }

  // Custom point on the mesh was clicked.
  else if ( eventId == EVENT_SELECT_WORLD_POINT )
  {
    // Get picking ray.
    gp_Lin pickRay = *( (gp_Lin*) pCallData );

    // Prepare a tool to find the intersected facet.
    asiAlgo_HitFacet        HitFacet        (m_bvh, m_notifier, m_plotter);
    asiAlgo_MeshProjectLine ProjectLineMesh (m_bvh, m_notifier, m_plotter);

    // Find intersection.
    gp_XYZ hit;
    int facet_idx;
    if ( !HitFacet(pickRay, facet_idx, hit) )
    {
      m_notifier.SendLogMessage(LogWarn(Normal) << "Cannot find the intersected facet.");
      return;
    }

    // Get active face index.
    const int fidx = m_bvh->GetFacet(facet_idx).FaceIndex;
    //
    m_notifier.SendLogMessage(LogInfo(Normal) << "Picked point (%1, %2, %3) on face %4."
                                              << hit.X()
                                              << hit.Y()
                                              << hit.Z()
                                              << fidx);

    // Business logic of reconstruction.
    asiEngine_RE reApi(m_model, m_notifier, m_plotter);

    // New vertex.
    Handle(asiData_ReVertexNode) nextVertexNode;

    // Modify data.
    m_model->OpenCommand();
    {
      // Prepare Active Contour.
      m_contour = reApi.GetOrCreate_ActiveContour();

      // Prepare active patch.
      if ( m_patch.IsNull() )
        m_patch = reApi.Create_Patch();

      // Create vertex.
      nextVertexNode = reApi.Create_Vertex(hit);

      // Connect two points by a projected polyline.
      const int numContourPts = m_contour->GetNumPoints();
      //
      if ( numContourPts > 0 )
      {
        // Get previous point.
        const gp_XYZ prevPt = m_contour->GetPoint(numContourPts - 1);

        // Add midpoints.
        std::vector<gp_XYZ> projPts;
        if ( !ProjectLineMesh.Perform(prevPt, hit, projPts, 0.001) )
        {
          m_notifier.SendLogMessage(LogErr(Normal) << "Cannot project line to mesh.");
          m_model->AbortCommand();
          return;
        }
        //
        if ( projPts.size() > 2 )
          for ( size_t k = 1; k < projPts.size() - 1; ++k ) // Skip ends as they are added individually.
            m_contour->AddPoint(projPts[k], -1);
      }

      // Add hitted point.
      const int poleIdx = m_contour->AddPoint(hit, fidx);
      m_contour->AddPoleIndex(poleIdx);

      m_notifier.SendLogMessage(LogInfo(Normal) << "Next pole index: %1." << poleIdx);
    }
    m_model->CommitCommand();

    // Update viewer.
    mgr->Actualize(m_contour);
    mgr->Actualize(nextVertexNode);

    // Update object browser.
    if ( m_pBrowser )
      m_pBrowser->Populate();
  }
}
