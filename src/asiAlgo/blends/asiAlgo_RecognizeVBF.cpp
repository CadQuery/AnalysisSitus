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

  // Get face in question.
  const TopoDS_Face& face = m_aag->GetFace(fid);

  // Get the neighbor faces.
  const TColStd_PackedMapOfInteger& nids = m_aag->GetNeighbors(fid);

  // Among the neighbor faces, there should be some EBFs. At least three
  // EBFs are expected.
  int numEBFs = 0;
  //
  TColStd_PackedMapOfInteger specialEdges, allEdges;
  for ( TColStd_MapIteratorOfPackedMapOfInteger nit(nids); nit.More(); nit.Next() )
  {
    const int nid = nit.Key();

    Handle(asiAlgo_FeatureAttrAdjacency)
      adjAttr = Handle(asiAlgo_FeatureAttrAdjacency)::DownCast( m_aag->GetArcAttribute( asiAlgo_AAG::t_arc(fid, nid) ) );
    //
    allEdges.Unite( adjAttr->GetEdgeIndices() );

    // Get blend candidate attribute.
    Handle(asiAlgo_FeatureAttr)
      attr = m_aag->GetNodeAttribute( nid, asiAlgo_AttrBlendCandidate::GUID() );
    //
    if ( attr.IsNull() )
      continue;

    // Downcast.
    Handle(asiAlgo_AttrBlendCandidate)
      bcAttr = Handle(asiAlgo_AttrBlendCandidate)::DownCast(attr);

    // Get common edge. This should be either a cross or a smooth edge.
    TopoDS_Edge commonEdge   = asiAlgo_Utils::GetCommonEdge( face, m_aag->GetFace(nid) );
    const int   commonEdgeId = m_aag->RequestMapOfEdges().FindIndex(commonEdge);

    if ( !bcAttr->CrossEdgeIndices.Contains(commonEdgeId) &&
         !bcAttr->SpringEdgeIndices.Contains(commonEdgeId) ) // Spring edges may be detected on VBFs.
      return false; // All edges of a vertex blend are special.

    specialEdges.Add(commonEdgeId);
    numEBFs++;
  }

  if ( numEBFs < 3 )
    return false;

  // If any edges remain unrecognized, the candidate face cannot be a VBF.
  TColStd_PackedMapOfInteger unrecEdges = allEdges;
  unrecEdges.Subtract(specialEdges);
  //
  if ( !unrecEdges.IsEmpty() )
    return false;

  // Get face attribute which may already exist here if the VBF was
  // recognized as a EBF previously.
  Handle(asiAlgo_AttrBlendCandidate) blendAttr;
  //
  Handle(asiAlgo_FeatureAttr)
    attr = m_aag->GetNodeAttribute( fid, asiAlgo_AttrBlendCandidate::GUID() );
  //
  if ( attr.IsNull() )
  {
    blendAttr = new asiAlgo_AttrBlendCandidate(0);

    if ( !m_aag->SetNodeAttribute(fid, blendAttr) )
    {
      this->GetProgress().SendLogMessage( LogErr(Normal) << "Weird iteration: blend attribute is already there." );
      return false;
    }
  }
  else
    blendAttr = Handle(asiAlgo_AttrBlendCandidate)::DownCast(attr);

  // Modify the attribute.
  blendAttr->Kind = BlendType_Vertex;

  return true;
}
