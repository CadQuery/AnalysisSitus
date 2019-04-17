//-----------------------------------------------------------------------------
// Created on: 12 April (*) 2019
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
#include <asiAlgo_RecognizeVBF.h>

// asiAlgo includes
#include <asiAlgo_AttrBlendCandidate.h>
#include <asiAlgo_AttrBlendSupport.h>
#include <asiAlgo_FeatureAttrAdjacency.h>

//-----------------------------------------------------------------------------

asiAlgo_RecognizeVBF::asiAlgo_RecognizeVBF(const Handle(asiAlgo_AAG)& aag,
                                           ActAPI_ProgressEntry       progress,
                                           ActAPI_PlotterEntry        plotter)
: ActAPI_IAlgorithm (progress, plotter),
  m_aag             (aag)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_RecognizeVBF::Perform(const int fid)
{
  // Check AAG.
  if ( m_aag.IsNull() )
  {
    this->GetProgress().SendLogMessage(LogErr(Normal) << "AAG is null.");
    return false;
  }

  // Get candidate face.
  const TopoDS_Face& face = m_aag->GetFace(fid);

  // Get face attribute which may already exist here if the VBF was
  // recognized as a EBF previously.
  Handle(asiAlgo_FeatureAttr)
    blendAttrBase = m_aag->GetNodeAttribute( fid, asiAlgo_AttrBlendCandidate::GUID() );
  //
  Handle(asiAlgo_AttrBlendCandidate)
    blendAttr = Handle(asiAlgo_AttrBlendCandidate)::DownCast(blendAttrBase);

  /* ------------------------------------------------- */
  /* Heuristic 1: the face is not a blend support face */
  /* ------------------------------------------------- */

  Handle(asiAlgo_FeatureAttr)
    supportAttrBase = m_aag->GetNodeAttribute( fid, asiAlgo_AttrBlendSupport::GUID() );
  //
  if ( !supportAttrBase.IsNull() )
    return false;

  /* -------------------------------------------------------- */
  /* Heuristic 2: all VBFs have not less than 3 adjacent EBFs */
  /* -------------------------------------------------------- */

  // Get the neighbor faces.
  const TColStd_PackedMapOfInteger& nids = m_aag->GetNeighbors(fid);

  // Structure to add additionally recognized cross-edges. Those edges
  // are collected when the heuristic is being checked, but not applied
  // directly to the AAG. The edges will be added later, once all heuristics
  // are done and the status of the vertex blend is confirmed.
  struct t_crossEdge
  {
    Handle(asiAlgo_AttrBlendCandidate) EBF;    //!< EBF to adjust.
    int                                edgeId; //!< New cross edge index.

    t_crossEdge() : edgeId(0) {}
    t_crossEdge(const Handle(asiAlgo_AttrBlendCandidate)& _ebf, const int _id) : EBF(_ebf), edgeId(_id) {}
  };
  //
  std::vector<t_crossEdge> extraCrossEdges;

  // Among the neighbor faces, there should be some EBFs. At least three
  // EBFs are expected.
  int    numEBFs   = 0;
  double maxRadius = 0.; // Max radius of EBFs meeting at VBF will be assigned to VBF attribute.
  //
  for ( TColStd_MapIteratorOfPackedMapOfInteger nit(nids); nit.More(); nit.Next() )
  {
    const int nid = nit.Key();

    // Get neighbor face.
    const TopoDS_Face& neighborFace = m_aag->GetFace(nid);

    // Get blend candidate attribute.
    Handle(asiAlgo_FeatureAttr)
      neighborAttr = m_aag->GetNodeAttribute( nid, asiAlgo_AttrBlendCandidate::GUID() );
    //
    Handle(asiAlgo_AttrBlendCandidate)
      neighborBcAttr = Handle(asiAlgo_AttrBlendCandidate)::DownCast(neighborAttr);

    // EBFs are marked with blend candidate attributes.
    if ( neighborBcAttr.IsNull() )
      continue;

    if ( neighborBcAttr->Kind == BlendType_Vertex )
      return false; // A vertex blend cannot have another vertex blend as a neighbor.
                    // At least, we do not recognize such cases.

    /*
       Sometimes the adjacent EBF may have been recognized as a support face.
       The latter happens if the blend face was initially recognized as EBF
       itself. As this status of the blend face is going to be precised,
       its adjacent EBF should receive the new cross edge index which is the
       index of the edge shared with the blend face.
     */

    // Get ID of the common edge between the candidate vertex blend and the
    // neighbor EBF.
    TopoDS_Edge commonEdge = asiAlgo_Utils::GetCommonEdge(face, neighborFace);
    const int commonEdgeId = m_aag->RequestMapOfEdges().FindIndex(commonEdge);

    extraCrossEdges.push_back( t_crossEdge(neighborBcAttr, commonEdgeId) );

    // Increment the number of EBFs arriving at the candidate blend face to
    // check the heuristic.
    numEBFs++;

    // Update max radius.
    if ( neighborBcAttr->Radius > maxRadius )
      maxRadius = neighborBcAttr->Radius;
  }
  //
  if ( numEBFs < 3 )
    return false;

  /* ---------------------------------------------------------------- */
  /* Heuristic 3: VBF cannot be adjacent to EBF via terminating edges */
  /* ---------------------------------------------------------------- */

  for ( TColStd_MapIteratorOfPackedMapOfInteger nit(nids); nit.More(); nit.Next() )
  {
    const int nid = nit.Key();

    // Get neighbor face.
    const TopoDS_Face& neighborFace = m_aag->GetFace(nid);

    // Get blend candidate attribute.
    Handle(asiAlgo_FeatureAttr)
      neighborAttr = m_aag->GetNodeAttribute( nid, asiAlgo_AttrBlendCandidate::GUID() );
    //
    Handle(asiAlgo_AttrBlendCandidate)
      neighborBcAttr = Handle(asiAlgo_AttrBlendCandidate)::DownCast(neighborAttr);

    // EBFs are marked with blend candidate attributes.
    if ( neighborBcAttr.IsNull() )
      continue;

    // Get ID of the common edge between the candidate vertex blend and the
    // neighbor EBF.
    TopoDS_Edge commonEdge = asiAlgo_Utils::GetCommonEdge(face, neighborFace);
    const int commonEdgeId = m_aag->RequestMapOfEdges().FindIndex(commonEdge);

    // Check if the common edge is terminating or not.
    if ( neighborBcAttr->TerminatingEdgeIndices.Contains(commonEdgeId) )
      return false;
  }

  /* ------------------------------------------------- */
  /*  Finalize recognition as all heuristics are done  */
  /* ------------------------------------------------- */

  // Add extra cross-edges.
  for ( size_t k = 0; k < extraCrossEdges.size(); ++k )
  {
    extraCrossEdges[k].EBF->CrossEdgeIndices.Add(extraCrossEdges[k].edgeId);
    extraCrossEdges[k].EBF->SpringEdgeIndices.Remove(extraCrossEdges[k].edgeId);
  }

  // If blend attribute is not available for this face, settle down a new one.
  if ( blendAttr.IsNull() )
  {
    blendAttr = new asiAlgo_AttrBlendCandidate(0);

    if ( !m_aag->SetNodeAttribute(fid, blendAttr) )
    {
      this->GetProgress().SendLogMessage( LogErr(Normal) << "Weird iteration: blend attribute is already there." );
      return false;
    }
  }
  else
  {
    // Clean up the edges.
    blendAttr->SmoothEdgeIndices.Clear();
    blendAttr->SpringEdgeIndices.Clear();
    blendAttr->CrossEdgeIndices.Clear();
  }

  // Modify the attribute.
  blendAttr->Radius = maxRadius;
  blendAttr->Kind   = BlendType_Vertex;

  return true;
}
