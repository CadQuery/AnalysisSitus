//-----------------------------------------------------------------------------
// Created on: 20 October 2018
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
#include <asiAlgo_BlendTopoCondition.h>

// asiAlgo includes
#include <asiAlgo_EulerKEF.h>
#include <asiAlgo_EulerKEV.h>
#include <asiAlgo_EulerKFMV.h>

//-----------------------------------------------------------------------------

bool asiAlgo_BlendTopoCondition::kev(const TopoDS_Shape&      shape,
                                     const int                edgeId,
                                     TopoDS_Shape&            output,
                                     Handle(asiAlgo_History)& history)
{
  // Get edge to kill.
  TopoDS_Edge edge = this->AAG->GetNamedEdge(edgeId);

  // Run Euler operator.
  asiAlgo_EulerKEV KEV( shape,
                        TopoDS::Edge( edge.Oriented(TopAbs_EXTERNAL) ),
                        m_progress, m_plotter );
  //
  if ( !history.IsNull() )
    KEV.SetHistory(history);
  //
  if ( !KEV.Perform(true) )
    return false;

  // Set result.
  output  = KEV.GetResult();
  history = KEV.GetHistory();
  //
  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_BlendTopoCondition::kev(const TopoDS_Shape&      shape,
                                     const int                edgeId,
                                     const int                vertex2KillId,
                                     TopoDS_Shape&            output,
                                     Handle(asiAlgo_History)& history)
{
  // Get transient entities.
  TopoDS_Edge   edge        = this->AAG->GetNamedEdge(edgeId);
  TopoDS_Vertex vertex2Kill = this->AAG->GetNamedVertex(vertex2KillId);

  // Run Euler operator.
  asiAlgo_EulerKEV KEV( shape,
                        TopoDS::Edge( edge.Oriented(TopAbs_EXTERNAL) ),
                        TopoDS::Vertex( vertex2Kill.Oriented(TopAbs_EXTERNAL) ),
                        false,
                        m_progress, m_plotter );
  //
  if ( !history.IsNull() )
    KEV.SetHistory(history);
  //
  if ( !KEV.Perform(true) )
    return false;

  // Set result.
  output  = KEV.GetResult();
  history = KEV.GetHistory();
  //
  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_BlendTopoCondition::kef(const TopoDS_Shape&      shape,
                                     const int                faceId,
                                     const int                edge2KillId,
                                     const int                edge2SaveId,
                                     TopoDS_Shape&            output,
                                     Handle(asiAlgo_History)& history)
{
  // Get transient entities.
  TopoDS_Face face      = this->AAG->GetNamedFace(faceId);
  TopoDS_Edge edge2Kill = this->AAG->GetNamedEdge(edge2KillId);
  TopoDS_Edge edge2Save = this->AAG->GetNamedEdge(edge2SaveId);

  // Run Euler operator.
  asiAlgo_EulerKEF KEF( shape,
                        TopoDS::Face( face.Oriented(TopAbs_EXTERNAL) ),
                        TopoDS::Edge( edge2Kill.Oriented(TopAbs_EXTERNAL) ),
                        TopoDS::Edge( edge2Save.Oriented(TopAbs_EXTERNAL) ),
                        m_progress, m_plotter );
  //
  if ( !history.IsNull() )
    KEF.SetHistory(history);
  //
  if ( !KEF.Perform(true) )
    return false;

  // Set result.
  output  = KEF.GetResult();
  history = KEF.GetHistory();
  //
  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_BlendTopoCondition::kfmv(const TopoDS_Shape&      shape,
                                      const int                faceId,
                                      TopoDS_Shape&            output,
                                      Handle(asiAlgo_History)& history)
{
  // Get face to kill.
  TopoDS_Face face = this->AAG->GetNamedFace(faceId);

  // Run Euler operator.
  asiAlgo_EulerKFMV KFMV( shape,
                          TopoDS::Face( face.Oriented(TopAbs_EXTERNAL) ),
                          m_progress, m_plotter );
  //
  if ( !history.IsNull() )
    KFMV.SetHistory(history);
  //
  if ( !KFMV.Perform(true) )
    return false;

  // Set result.
  output  = KFMV.GetResult();
  history = KFMV.GetHistory();
  //
  return true;
}
