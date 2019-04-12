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

  // All neighbor faces should be EBFs.
  for ( TColStd_MapIteratorOfPackedMapOfInteger nit(nids); nit.More(); nit.Next() )
  {
    const int nid = nit.Key();

    // Get blend candidate attribute.
    Handle(asiAlgo_FeatureAttr)
      attr = m_aag->GetNodeAttribute( nid, asiAlgo_AttrBlendCandidate::GUID() );
    //
    if ( attr.IsNull() )
      return false;
  }

  // Prepare face attribute.
  Handle(asiAlgo_AttrBlendCandidate)
    BlendAttr = new asiAlgo_AttrBlendCandidate(0);
  //
  BlendAttr->Kind = BlendType_Vertex;
  //
  if ( !m_aag->SetNodeAttribute(fid, BlendAttr) )
  {
    this->GetProgress().SendLogMessage( LogErr(Normal) << "Weird iteration: blend attribute is already there." );
    return false;
  }

  return true;
}
