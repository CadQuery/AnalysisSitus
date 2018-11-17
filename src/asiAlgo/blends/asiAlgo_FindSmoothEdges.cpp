//-----------------------------------------------------------------------------
// Created on: 01 October 2018
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
#include <asiAlgo_FindSmoothEdges.h>

// asiAlgo includes
#include <asiAlgo_AAGIterator.h>
#include <asiAlgo_CheckDihedralAngle.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiAlgo_FindSmoothEdges::asiAlgo_FindSmoothEdges(const TopoDS_Shape&        masterCAD,
                                                 const Handle(asiAlgo_AAG)& aag,
                                                 ActAPI_ProgressEntry       progress,
                                                 ActAPI_PlotterEntry        plotter)
//
: ActAPI_IAlgorithm ( progress, plotter ),
  m_master          ( masterCAD ),
  m_aag             ( aag )
{}

//-----------------------------------------------------------------------------

asiAlgo_FindSmoothEdges::asiAlgo_FindSmoothEdges(const Handle(asiAlgo_AAG)& aag,
                                                 ActAPI_ProgressEntry       progress,
                                                 ActAPI_PlotterEntry        plotter)
//
: ActAPI_IAlgorithm ( progress, plotter ),
  m_master          ( aag->GetMasterCAD() ),
  m_aag             ( aag )
{}

//-----------------------------------------------------------------------------

bool asiAlgo_FindSmoothEdges::Perform()
{
  // Build master AAG
  if ( m_aag.IsNull() )
  {
#if defined COUT_DEBUG
    TIMER_NEW
    TIMER_GO
#endif

    m_aag = new asiAlgo_AAG(m_master);

#if defined COUT_DEBUG
    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Construct AAG")
#endif
  }

  // Loop over the faces
  Handle(asiAlgo_AAGRandomIterator) it = new asiAlgo_AAGRandomIterator(m_aag);
  //
  for ( ; it->More(); it->Next() )
  {
    const int          current_face_idx = it->GetFaceId();
    const TopoDS_Face& current_face     = m_aag->GetFace(current_face_idx);
    //
    TColStd_PackedMapOfInteger neighbor_ids;
    it->GetNeighbors(neighbor_ids);

    // Loop over the neighbors
    for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbor_ids); nit.More(); nit.Next() )
    {
      const int          neighbor_face_idx = nit.Key();
      const TopoDS_Face& neighbor_face     = m_aag->GetFace(neighbor_face_idx);

      // Check angle between the two faces
      this->detect(current_face, neighbor_face, m_result.edges);
    }
  }

  // Recover IDs
  this->getIds(m_result.edges, m_result.ids);

  // Success
  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_FindSmoothEdges::PerformForFace(const int faceIdx)
{
  // Build master AAG
  if ( m_aag.IsNull() )
  {
#if defined COUT_DEBUG
    TIMER_NEW
    TIMER_GO
#endif

   m_aag = new asiAlgo_AAG(m_master);

#if defined COUT_DEBUG
    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Construct AAG")
#endif
  }

  // Get face of interest
  const TopoDS_Face& face = m_aag->GetFace(faceIdx);

  // Get neighbors for the face of interest
  const TColStd_PackedMapOfInteger& neighbor_ids = m_aag->GetNeighbors(faceIdx);

  // Loop over the neighbors
  for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbor_ids); nit.More(); nit.Next() )
  {
    const int          neighbor_face_idx = nit.Key();
    const TopoDS_Face& neighbor_face     = m_aag->GetFace(neighbor_face_idx);

    // Check angle between the two faces
    this->detect(face, neighbor_face, m_result.edges);
  }

  // Recover IDs
  this->getIds(m_result.edges, m_result.ids);

  // Success
  return true;
}

//-----------------------------------------------------------------------------

void asiAlgo_FindSmoothEdges::detect(const TopoDS_Face&          F,
                                     const TopoDS_Face&          G,
                                     TopTools_IndexedMapOfShape& smoothEdges) const
{
  // Check angle between the two faces
  TopTools_IndexedMapOfShape commonEdges;
  asiAlgo_CheckDihedralAngle dihAngle(NULL, NULL);
  //
  asiAlgo_FeatureAngleType angle = dihAngle.AngleBetweenFaces(F, G, true, 0.0, commonEdges);

  if ( angle == FeatureAngleType_Smooth )
  {
    for ( int i = 1; i <= commonEdges.Extent(); ++i )
      smoothEdges.Add( commonEdges(i) );
  }
}

//-----------------------------------------------------------------------------

void asiAlgo_FindSmoothEdges::getIds(const TopTools_IndexedMapOfShape& edges,
                                     TColStd_PackedMapOfInteger&       indices) const
{
  const TopTools_IndexedMapOfShape& allEdges = m_aag->RequestMapOfEdges();
  //
  for ( int eidx = 1; eidx <= edges.Extent(); ++eidx )
  {
    const TopoDS_Shape& edgeOfInterest = edges(eidx);
    indices.Add( allEdges.FindIndex(edgeOfInterest) );
  }
}
