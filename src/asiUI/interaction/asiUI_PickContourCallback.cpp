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

  // Prepare RE Topology Node.
  Handle(asiData_ReTopoNode) topoNode = m_model->GetReTopoNode();
  //
  if ( topoNode.IsNull() )
  {
    m_model->OpenCommand(); // Open transaction.
    {
      topoNode = asiEngine_RE(m_model, m_notifier, m_plotter).Create_Topo();
    }
    m_model->CommitCommand(); // Commit transaction.
  }

  // Get current topological situation.
  Handle(asiData_RePatchNode)  patchNode       = topoNode->GetCurrentPatch();
  Handle(asiData_ReCoEdgeNode) prevCoedgeNode  = topoNode->GetCurrentCoEdge();
  Handle(asiData_ReVertexNode) lastVertexNode  = topoNode->GetLastVertex();
  Handle(asiData_ReEdgeNode)   prevEdgeNode    = prevCoedgeNode.IsNull() ? NULL : prevCoedgeNode->GetEdge();

  // Existing vertex was selected again.
  if ( eventId == EVENT_SELECT_CELL )
  {
    Handle(asiData_ReVertexNode)
      pickedVertex = this->getPickedVertex(pCallData);
    //
    if ( pickedVertex.IsNull() )
      return;

    // Complete current edge and this way complete contour.
    if ( !prevCoedgeNode.IsNull() )
      this->completeContour(pickedVertex);
    else
      this->startNewContour(pickedVertex);
  }

  // Custom point on the mesh was clicked.
  else if ( eventId == EVENT_SELECT_WORLD_POINT )
  {
    if ( prevEdgeNode.IsNull() )
      this->startNewContour(pCallData);
    else
      this->addContourEdge(pCallData);
  }
}

//-----------------------------------------------------------------------------

bool asiUI_PickContourCallback::startNewContour(void* pCallData)
{
  m_notifier.SendLogMessage(LogInfo(Normal) << "Starting new contour from picked point...");

  // Get picked point.
  gp_XYZ hit;
  if ( !this->getPickedPoint(pCallData, hit) )
    return false;

  Handle(asiData_ReVertexNode) vertexNode;

  // Modify data.
  m_model->OpenCommand();
  {
    // Create vertex.
    vertexNode = asiEngine_RE(m_model, m_notifier, m_plotter).Create_Vertex(hit);

    // Build new edge.
    if ( this->buildNewEdge(vertexNode).IsNull() )
    {
      m_model->AbortCommand();
      return false;
    }

    // Store the reference to the first vertex.
    m_model->GetReTopoNode()->SetFirstVertex(vertexNode);
  }
  m_model->CommitCommand();

  // Update scene.
  this->GetViewer()->PrsMgr()->Actualize(vertexNode);

  // Update object browser.
  if ( m_pBrowser )
    m_pBrowser->Populate();

  return true;
}

//-----------------------------------------------------------------------------

bool asiUI_PickContourCallback::startNewContour(const Handle(asiData_ReVertexNode)& source)
{
  m_notifier.SendLogMessage(LogInfo(Normal) << "Starting new contour from vertex...");

  // Modify data.
  m_model->OpenCommand();
  {
    // Build new edge.
    if ( this->buildNewEdge(source).IsNull() )
    {
      m_model->AbortCommand();
      return false;
    }

    // Store the reference to the first vertex.
    m_model->GetReTopoNode()->SetFirstVertex(source);
  }
  m_model->CommitCommand();

  // Update object browser.
  if ( m_pBrowser )
    m_pBrowser->Populate();

  return true;
}

//-----------------------------------------------------------------------------

bool asiUI_PickContourCallback::addContourEdge(void* pCallData)
{
  m_notifier.SendLogMessage(LogInfo(Normal) << "Adding new edge...");

  // Get picked point.
  gp_XYZ hit;
  if ( !this->getPickedPoint(pCallData, hit) )
    return false;

  Handle(asiData_ReVertexNode) vertexNode;

  // Get previous edge.
  Handle(asiData_ReEdgeNode)
    prevEdgeNode = m_model->GetReTopoNode()->GetCurrentCoEdge()->GetEdge();

  // Modify data.
  m_model->OpenCommand();
  {
    // Create vertex.
    vertexNode = asiEngine_RE(m_model, m_notifier, m_plotter).Create_Vertex(hit);

    // Build new edge.
    if ( this->buildNewEdge(vertexNode, true).IsNull() )
    {
      m_model->AbortCommand();
      return false;
    }
  }
  m_model->CommitCommand();

  // Update scene.
  this->GetViewer()->PrsMgr()->Actualize(prevEdgeNode);
  this->GetViewer()->PrsMgr()->Actualize(vertexNode);

  // Update object browser.
  if ( m_pBrowser )
    m_pBrowser->Populate();

  return true;
}

//-----------------------------------------------------------------------------

bool
  asiUI_PickContourCallback::addContourEdge(const Handle(asiData_ReVertexNode)& target)
{
  m_notifier.SendLogMessage(LogInfo(Normal) << "Adding new edge between existing vertices...");

  // Get previous edge.
  Handle(asiData_ReEdgeNode)
    edgeNode = m_model->GetReTopoNode()->GetCurrentCoEdge()->GetEdge();

  // Modify data.
  m_model->OpenCommand();
  {
    // Build new edge.
    if ( this->buildNewEdge(target, true).IsNull() )
    {
      m_model->AbortCommand();
      return false;
    }
  }
  m_model->CommitCommand();

  // Update scene.
  this->GetViewer()->PrsMgr()->Actualize(edgeNode);

  // Update object browser.
  if ( m_pBrowser )
    m_pBrowser->Populate();

  return true;
}

//-----------------------------------------------------------------------------

bool
  asiUI_PickContourCallback::addContourCoEdge(const Handle(asiData_ReEdgeNode)& edge,
                                              const bool                        samesense)
{
  //m_notifier.SendLogMessage(LogInfo(Normal) << "Adding new coedge for the existing edge...");

  //// Get previous edge.
  //Handle(asiData_ReEdgeNode)
  //  edgeNode = m_model->GetReTopoNode()->GetCurrentCoEdge()->GetEdge();

  //// Modify data.
  //m_model->OpenCommand();
  //{
  //  // Build new edge.
  //  if ( this->buildNewEdge(target, true).IsNull() )
  //  {
  //    m_model->AbortCommand();
  //    return false;
  //  }
  //}
  //m_model->CommitCommand();

  //// Update scene.
  //this->GetViewer()->PrsMgr()->Actualize(edgeNode);

  //// Update object browser.
  //if ( m_pBrowser )
  //  m_pBrowser->Populate();

  return true;
}

//-----------------------------------------------------------------------------

bool
  asiUI_PickContourCallback::completeContourCoEdge(const Handle(asiData_ReEdgeNode)& edge,
                                                   const bool                        samesense)
{
  m_notifier.SendLogMessage(LogInfo(Normal) << "Complete contour by adjusting current coedge...");

  Handle(asiData_ReTopoNode)   topoNode   = m_model->GetReTopoNode();
  Handle(asiData_ReCoEdgeNode) coedgeNode = topoNode->GetCurrentCoEdge();

  m_model->OpenCommand();
  {
    coedgeNode->SetSameSense(samesense);

    // Reset current references.
    topoNode->SetCurrentPatch(NULL);
    topoNode->SetCurrentCoEdge(NULL);
    topoNode->SetLastVertex(NULL);
  }
  m_model->CommitCommand();

  // Update object browser.
  if ( m_pBrowser )
    m_pBrowser->Populate();

  return true;
}

//-----------------------------------------------------------------------------

bool
  asiUI_PickContourCallback::completeContour(const Handle(asiData_ReVertexNode)& target)
{
  m_notifier.SendLogMessage(LogInfo(Normal) << "Completing contour...");

  // Get Topology Node and the currently composed entities.
  Handle(asiData_ReTopoNode)   topoNode        = m_model->GetReTopoNode();
  Handle(asiData_RePatchNode)  patchNode       = topoNode->GetCurrentPatch();
  Handle(asiData_ReCoEdgeNode) coedgeNode      = topoNode->GetCurrentCoEdge();
  Handle(asiData_ReEdgeNode)   lastEdgeNode    = coedgeNode.IsNull() ? NULL : coedgeNode->GetEdge();
  Handle(asiData_ReVertexNode) firstVertexNode = topoNode->GetFirstVertex();
  //
  if ( lastEdgeNode.IsNull() )
    return false;

  const bool isPickedStartVertex = IsEqual( target->GetId(),
                                            firstVertexNode->GetId() );

  // Get previous vertex.
  Handle(asiData_ReVertexNode) prevVertex = lastEdgeNode->GetFirstVertex();
  //
  if ( prevVertex.IsNull() || !prevVertex->IsWellFormed() )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "Previous vertex is undefined.");
    m_model->AbortCommand();
    return false;
  }

  // Tool for line projection.
  asiAlgo_MeshProjectLine ProjectLineMesh(m_bvh, m_notifier, m_plotter);

  // Business logic of reconstruction.
  asiEngine_RE reApi(m_model, m_notifier, m_plotter);

  // Perform data model modification.
  m_model->OpenCommand();
  {
    // Set last vertex.
    topoNode->SetLastVertex(target);

    // Complete topological definition of the current edge.
    lastEdgeNode->SetLastVertex(target);

    // Get point to connect.
    const gp_XYZ prevPt  = prevVertex->GetPoint();
    const gp_XYZ finalPt = target->GetPoint();

    // Add midpoints.
    std::vector<gp_XYZ> projPts;
    if ( !this->projectLine(prevPt, finalPt, projPts) )
    {
      m_model->AbortCommand();
      return false;
    }
    //
    if ( projPts.size() > 2 )
      for ( size_t k = 0; k < projPts.size() - 1; ++k )
        lastEdgeNode->AddPolylinePole(projPts[k]);

    // Add hitted point.
    lastEdgeNode->SetFirstVertexIndex(0);
    lastEdgeNode->SetLastVertexIndex( lastEdgeNode->AddPolylinePole(finalPt) );

    // Get edges shared by the target vertex and complete contour by adding
    // the corresponding coedges while not adding new edges.
    std::vector<Handle(asiData_ReEdgeNode)> edges;
    //
    if ( !isPickedStartVertex && this->findEdgesOnVertex(target, lastEdgeNode, edges) )
    {
      // Add coedge with inverse orientation.
      const size_t edgeIdx = this->chooseMinTurnEdge(lastEdgeNode, target, edges);
      reApi.Create_CoEdge(patchNode, edges[edgeIdx], false);
    }

    // Reset current references.
    topoNode->SetCurrentPatch(NULL);
    topoNode->SetCurrentCoEdge(NULL);
    topoNode->SetLastVertex(NULL);
  }
  m_model->CommitCommand();

  // Update scene.
  this->GetViewer()->PrsMgr()->Actualize(lastEdgeNode);

  // Update object browser.
  if ( m_pBrowser )
    m_pBrowser->Populate();

  return true;
}

//-----------------------------------------------------------------------------

Handle(asiData_ReEdgeNode)
  asiUI_PickContourCallback::findEdgeOnVertices(const Handle(asiData_ReVertexNode)& v1,
                                                const Handle(asiData_ReVertexNode)& v2,
                                                bool&                               samesense)
{
  if ( v1.IsNull() || v2.IsNull() )
    return NULL;

  // Business logic of reconstruction.
  asiEngine_RE reApi(m_model, m_notifier, m_plotter);

  // Get Edges Node.
  Handle(asiData_ReEdgesNode) edgesNode = reApi.Get_Edges();
  //
  if ( edgesNode.IsNull() || !edgesNode->IsWellFormed() )
    return NULL;

  // Iterate over the existing edges trying to find the one with 
  for ( Handle(ActAPI_IChildIterator) eit = edgesNode->GetChildIterator(); eit->More(); eit->Next() )
  {
    Handle(asiData_ReEdgeNode)
      edgeNode = Handle(asiData_ReEdgeNode)::DownCast( eit->Value() );
    //
    if ( edgeNode.IsNull() || !edgeNode->IsWellFormed() )
      continue;

    Handle(asiData_ReVertexNode) currV1 = edgeNode->GetFirstVertex();
    Handle(asiData_ReVertexNode) currV2 = edgeNode->GetLastVertex();

    if ( currV1.IsNull() || currV2.IsNull() )
      continue;

    // Check equality of extremities.
    if ( IsEqual( currV1->GetId(), v1->GetId() ) &&
         IsEqual( currV2->GetId(), v2->GetId() ) )
    {
      samesense = true;
    }
    else if ( IsEqual( currV2->GetId(), v1->GetId() ) &&
              IsEqual( currV1->GetId(), v2->GetId() ) )
    {
      samesense = false;
    }
    else continue;

    return edgeNode;
  }

  return NULL;
}

//-----------------------------------------------------------------------------

size_t
  asiUI_PickContourCallback::chooseMinTurnEdge(const Handle(asiData_ReEdgeNode)&        currentEdge,
                                               const Handle(asiData_ReVertexNode)&      commonVertex,
                                               std::vector<Handle(asiData_ReEdgeNode)>& candidates)
{
  gp_Vec edgeDir;
  //
  {
    int vertexPole = -1;
    Handle(asiData_ReVertexNode) vf = currentEdge->GetFirstVertex();
    Handle(asiData_ReVertexNode) vl = currentEdge->GetLastVertex();
    //
    if ( IsEqual( vf->GetId(), commonVertex->GetId() ) )
      vertexPole = currentEdge->GetFirstVertexIndex();
    else
      vertexPole = currentEdge->GetLastVertexIndex();

    if ( vertexPole > 0 )
      edgeDir = currentEdge->GetPolylinePole(vertexPole) -
                currentEdge->GetPolylinePole(vertexPole - 1);
    else
      edgeDir = currentEdge->GetPolylinePole(vertexPole + 1) -
                currentEdge->GetPolylinePole(vertexPole);

    m_plotter.DRAW_VECTOR_AT( commonVertex->GetPoint(), edgeDir, Color_Red, "edgeDir" );
  }

  std::vector<gp_Vec> candidateVecs;
  //
  for ( size_t k = 0; k < candidates.size(); ++k )
  {
    bool toReverse = false;
    int vertexPole = -1;
    Handle(asiData_ReVertexNode) vf = candidates[k]->GetFirstVertex();
    Handle(asiData_ReVertexNode) vl = candidates[k]->GetLastVertex();
    //
    if ( IsEqual( vf->GetId(), commonVertex->GetId() ) )
      vertexPole = candidates[k]->GetFirstVertexIndex();
    else
    {
      vertexPole = candidates[k]->GetLastVertexIndex();
      toReverse = true;
    }

    gp_Vec candidateDir;
    //
    if ( vertexPole > 0 )
      candidateDir = candidates[k]->GetPolylinePole(vertexPole) -
                     candidates[k]->GetPolylinePole(vertexPole - 1);
    else
      candidateDir = candidates[k]->GetPolylinePole(vertexPole + 1) -
                     candidates[k]->GetPolylinePole(vertexPole);
    //
    if ( toReverse )
      candidateDir.Reverse();

    candidateVecs.push_back(candidateDir);

    m_plotter.DRAW_VECTOR_AT( commonVertex->GetPoint(), candidateDir, Color_Blue, "candidateDir" );
  }

  gp_Vec prevEdgeDir = this->getCoEdgeTrailingDir( this->getPrevCoEdge(currentEdge) );
  //
  m_plotter.DRAW_VECTOR_AT( commonVertex->GetPoint(), prevEdgeDir, Color_Green, "prevEdgeDir" );

  gp_Vec cross = prevEdgeDir^edgeDir;
  //
  m_plotter.DRAW_VECTOR_AT( commonVertex->GetPoint(), cross*10, Color_Yellow, "cross" );

  size_t resIdx = 0;
  double maxAng = -DBL_MAX;
  //
  for ( size_t k = 0; k < candidateVecs.size(); ++k )
  {
    const double ang = edgeDir.AngleWithRef(candidateVecs[k], cross);
    //
    m_notifier.SendLogMessage(LogInfo(Normal) << "ang = %1 deg." << ang/M_PI*180.0);
    //
    if ( ang > maxAng )
    {
      maxAng = ang;
      resIdx = k;
    }
  }

  return resIdx;
}

//-----------------------------------------------------------------------------

Handle(asiData_ReCoEdgeNode)
  asiUI_PickContourCallback::getPrevCoEdge(const Handle(asiData_ReEdgeNode)& currentEdge)
{
  Handle(asiData_ReTopoNode)   topoNode   = m_model->GetReTopoNode();
  Handle(asiData_RePatchNode)  patchNode  = topoNode->GetCurrentPatch();

  Handle(asiData_ReCoEdgeNode) prevCoedgeNode;
  int iter = 0;
  //
  for ( Handle(ActAPI_IChildIterator) cit = patchNode->GetChildIterator();
        cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) childNode = cit->Value();
    //
    const Handle(asiData_ReCoEdgeNode)&
      childCoedgeNode = Handle(asiData_ReCoEdgeNode)::DownCast(childNode);

    if ( iter && ( childCoedgeNode->GetEdge()->GetId() == currentEdge->GetId() ) )
    {
      m_notifier.SendLogMessage(LogInfo(Normal) << "Prev. edge: %1." << prevCoedgeNode->GetId());
      return prevCoedgeNode;
    }

    prevCoedgeNode = childCoedgeNode;
    iter++;
  }

  return NULL;
}

//-----------------------------------------------------------------------------

gp_Vec
  asiUI_PickContourCallback::getCoEdgeTrailingDir(const Handle(asiData_ReCoEdgeNode)& coedge)
{
  Handle(asiData_ReEdgeNode) edge = coedge->GetEdge();

  int vidx = coedge->IsSameSense() ? edge->GetLastVertexIndex()
                                   : edge->GetFirstVertexIndex();

  gp_Vec dir;
  //
  if ( vidx > 0 )
    dir = edge->GetPolylinePole(vidx) -
          edge->GetPolylinePole(vidx - 1);
  else
    dir = edge->GetPolylinePole(vidx + 1) -
          edge->GetPolylinePole(vidx);

  return dir;
}

//-----------------------------------------------------------------------------

bool
  asiUI_PickContourCallback::findEdgesOnVertex(const Handle(asiData_ReVertexNode)&      vertex,
                                               const Handle(asiData_ReEdgeNode)&        self2Skip,
                                               std::vector<Handle(asiData_ReEdgeNode)>& edges)
{
  Handle(ActAPI_HNodeMap)       nodes     = new ActAPI_HNodeMap;
  Handle(ActAPI_HParameterList) referrers = vertex->GetReferrers();
  //
  for ( ActAPI_HParameterList::Iterator rit(*referrers); rit.More(); rit.Next() )
  {
    Handle(ActAPI_INode) nextNode = rit.Value()->GetNode();
    //
    if ( self2Skip->GetId() == nextNode->GetId() )
      continue;

    nodes->Add(nextNode);
  }

  if ( nodes->IsEmpty() )
    return false;

  // Add Nodes to the vector.
  for ( ActAPI_HNodeMap::Iterator nit(*nodes); nit.More(); nit.Next() )
  {
    const Handle(asiData_ReEdgeNode)&
      reEdge = Handle(asiData_ReEdgeNode)::DownCast( nit.Value() );

    if ( !reEdge.IsNull() && reEdge->IsWellFormed() )
      edges.push_back(reEdge);
  }

  return true;
}

//-----------------------------------------------------------------------------

Handle(asiData_ReVertexNode)
  asiUI_PickContourCallback::getPickedVertex(void* pCallData)
{
  // Convert passed data to cell picker result.
  asiVisu_CellPickerResult* pPickRes = (asiVisu_CellPickerResult*) pCallData;

  // Retrieve Node information.
  asiVisu_NodeInfo* pNodeInfo = asiVisu_NodeInfo::Retrieve( pPickRes->GetPickedActor() );
  //
  if ( !pNodeInfo )
    return false;

  // Get Vertex Node.
  Handle(ActAPI_INode) node = m_model->FindNode( pNodeInfo->GetNodeId() );
  //
  if ( !node->IsInstance( STANDARD_TYPE(asiData_ReVertexNode) )
    || !node->IsWellFormed() )
    return false;

  Handle(asiData_ReVertexNode)
    vertexNode = Handle(asiData_ReVertexNode)::DownCast(node);
  //
  return vertexNode;
}

//-----------------------------------------------------------------------------

bool asiUI_PickContourCallback::getPickedPoint(void*   pCallData,
                                               gp_XYZ& result)
{
  // Get picking ray.
  gp_Lin pickRay = *( (gp_Lin*) pCallData );

  // Prepare a tool to find the intersected facet.
  asiAlgo_HitFacet HitFacet(m_bvh, m_notifier, m_plotter);

  // Find intersection.
  gp_XYZ hit;
  int facet_idx;
  //
  if ( !HitFacet(pickRay, facet_idx, hit) )
  {
    m_notifier.SendLogMessage(LogWarn(Normal) << "Cannot find the intersected facet.");
    return false;
  }

  // Get active face index.
  const int fidx = m_bvh->GetFacet(facet_idx).FaceIndex;
  //
  m_notifier.SendLogMessage(LogInfo(Normal) << "Picked point (%1, %2, %3) on face %4."
                                            << hit.X()
                                            << hit.Y()
                                            << hit.Z()
                                            << fidx);

  result = hit;
  return true;
}

//-----------------------------------------------------------------------------

bool asiUI_PickContourCallback::projectLine(const gp_XYZ&        p1,
                                            const gp_XYZ&        p2,
                                            std::vector<gp_XYZ>& result)
{
  // Tool for line projection.
  asiAlgo_MeshProjectLine ProjectLineMesh(m_bvh, m_notifier, m_plotter);

  // Add midpoints.
  if ( !ProjectLineMesh.Perform(p1, p2, result, 0.001) )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "Cannot project line to mesh.");
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------

Handle(asiData_ReCoEdgeNode)
  asiUI_PickContourCallback::buildNewEdge(const Handle(asiData_ReVertexNode)& target,
                                          const bool                          doProjectLine)
{
  // Get Topology Node and the currently composed entities.
  Handle(asiData_ReTopoNode)   topoNode       = m_model->GetReTopoNode();
  Handle(asiData_RePatchNode)  patchNode      = topoNode->GetCurrentPatch();
  Handle(asiData_ReCoEdgeNode) prevCoedgeNode = topoNode->GetCurrentCoEdge();
  Handle(asiData_ReEdgeNode)   prevEdgeNode   = prevCoedgeNode.IsNull() ? NULL : prevCoedgeNode->GetEdge();

  // Business logic of reconstruction.
  asiEngine_RE reApi(m_model, m_notifier, m_plotter);

  // Prepare active patch.
  if ( patchNode.IsNull() )
  {
    patchNode = reApi.Create_Patch();
    topoNode->SetCurrentPatch(patchNode);
  }

  // Create vertex.
  topoNode->SetLastVertex(target);

  // Create active edge.
  Handle(asiData_ReEdgeNode) nextEdgeNode = reApi.Create_Edge();
  //
  nextEdgeNode->SetFirstVertex(target);

  // Create coedge.
  Handle(asiData_ReCoEdgeNode)
    nextCoedgeNode = reApi.Create_CoEdge(patchNode, nextEdgeNode, true);
  //
  topoNode->SetCurrentCoEdge(nextCoedgeNode);

  // Complete topological and geometric definition of the previous edge.
  if ( !prevEdgeNode.IsNull() )
  {
    prevEdgeNode->SetLastVertex(target);

    // Project line if requested.
    if ( doProjectLine )
    {
      // Tool for line projection.
      asiAlgo_MeshProjectLine ProjectLineMesh(m_bvh, m_notifier, m_plotter);

      // Get previous vertex.
      Handle(asiData_ReVertexNode) prevVertex = prevEdgeNode->GetFirstVertex();
      //
      if ( prevVertex.IsNull() || !prevVertex->IsWellFormed() )
      {
        m_notifier.SendLogMessage(LogErr(Normal) << "Previous vertex is undefined.");
        m_model->AbortCommand();
        return false;
      }
      //
      const gp_XYZ prevPt = prevVertex->GetPoint();

      // Add midpoints.
      std::vector<gp_XYZ> projPts;
      if ( !this->projectLine(prevPt, target->GetPoint(), projPts) )
      {
        m_model->AbortCommand();
        return false;
      }
      //
      if ( projPts.size() > 2 )
        for ( size_t k = 0; k < projPts.size() - 1; ++k )
          prevEdgeNode->AddPolylinePole(projPts[k]);

      // Add hitted point.
      prevEdgeNode->SetFirstVertexIndex(0);
      prevEdgeNode->SetLastVertexIndex( prevEdgeNode->AddPolylinePole( target->GetPoint() ) );
    }
  }

  return nextCoedgeNode;
}

//-----------------------------------------------------------------------------

Handle(asiData_ReCoEdgeNode)
  asiUI_PickContourCallback::buildNewEdge(const Handle(asiData_ReVertexNode)& target)
{
  return this->buildNewEdge(target, false);
}

//-----------------------------------------------------------------------------

Handle(asiData_ReCoEdgeNode)
  asiUI_PickContourCallback::buildNewCoEdge(const Handle(asiData_ReEdgeNode)& edge,
                                            const bool                        samesense)
{
  //// Get Topology Node and the currently composed entities.
  //Handle(asiData_ReTopoNode)   topoNode       = m_model->GetReTopoNode();
  //Handle(asiData_ReCoEdgeNode) currCoedgeNode = topoNode->GetCurrentCoEdge();

  //Handle(asiData_ReVertexNode)
  //  nextVertex = samesense ? edge->GetLastVertex()
  //                         : edge->GetFirstVertex();

  //// Set current vertex.
  //topoNode->SetLastVertex(nextVertex);

  //// Create next coedge.
  //Handle(asiData_ReCoEdgeNode)
  //  coedgeNode = reApi.Create_CoEdge(patchNode, edgeNode, true);
  ////
  //topoNode->SetCurrentCoEdge(coedgeNode);

  //// Complete topological and geometric definition of the previous edge.
  //if ( !prevEdgeNode.IsNull() )
  //{
  //  prevEdgeNode->SetLastVertex(source);

  //  // Project line if requested.
  //  if ( doProject )
  //  {
  //    // Tool for line projection.
  //    asiAlgo_MeshProjectLine ProjectLineMesh(m_bvh, m_notifier, m_plotter);

  //    // Get previous vertex.
  //    Handle(asiData_ReVertexNode) prevVertex = prevEdgeNode->GetFirstVertex();
  //    //
  //    if ( prevVertex.IsNull() || !prevVertex->IsWellFormed() )
  //    {
  //      m_notifier.SendLogMessage(LogErr(Normal) << "Previous vertex is undefined.");
  //      m_model->AbortCommand();
  //      return false;
  //    }
  //    //
  //    const gp_XYZ prevPt = prevVertex->GetPoint();

  //    // Add midpoints.
  //    std::vector<gp_XYZ> projPts;
  //    if ( !this->projectLine(prevPt, target, projPts) )
  //    {
  //      m_model->AbortCommand();
  //      return false;
  //    }
  //    //
  //    if ( projPts.size() > 2 )
  //      for ( size_t k = 1; k < projPts.size() - 1; ++k ) // Skip ends as they are added individually.
  //        prevEdgeNode->AddPolylinePole(projPts[k]);

  //    // Add hitted point.
  //    prevEdgeNode->AddPolylinePole(target);
  //  }
  //}

  //return coedgeNode;
  return NULL;
}
