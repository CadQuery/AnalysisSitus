//-----------------------------------------------------------------------------
// Created on: 15 December 2017
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
#include <asiAlgo_EulerKEV.h>

// OCCT includes
#include <ShapeAnalysis_Edge.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

asiAlgo_EulerKEV::asiAlgo_EulerKEV(const TopoDS_Shape&  masterCAD,
                                   const TopoDS_Edge&   edge,
                                   ActAPI_ProgressEntry progress,
                                   ActAPI_PlotterEntry  plotter)
//
: asiAlgo_Euler (masterCAD, progress, plotter),
  m_edge        (edge),
  m_bSurvive    (false)
{
  m_vertex = ShapeAnalysis_Edge().FirstVertex(m_edge);
}

//-----------------------------------------------------------------------------

asiAlgo_EulerKEV::asiAlgo_EulerKEV(const TopoDS_Shape&  masterCAD,
                                   const TopoDS_Edge&   edge,
                                   const TopoDS_Vertex& vertex,
                                   const bool           toSurvive,
                                   ActAPI_ProgressEntry progress,
                                   ActAPI_PlotterEntry  plotter)
//
: asiAlgo_Euler (masterCAD, progress, plotter),
  m_edge        (edge),
  m_vertex      (vertex),
  m_bSurvive    (toSurvive)
{}

//-----------------------------------------------------------------------------

asiAlgo_EulerKEV::asiAlgo_EulerKEV(const TopoDS_Shape&              masterCAD,
                                   const TopoDS_Edge&               edge,
                                   const TopoDS_Vertex&             vertex,
                                   const bool                       toSurvive,
                                   const Handle(BRepTools_ReShape)& ctx,
                                   ActAPI_ProgressEntry             progress,
                                   ActAPI_PlotterEntry              plotter)
//
: asiAlgo_Euler (masterCAD, ctx, progress, plotter),
  m_edge        (edge),
  m_vertex      (vertex),
  m_bSurvive    (toSurvive)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_EulerKEV::perform(const bool doApply)
{
  // Get vertices owned by the edge to kill
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(m_edge, V1, V2, 0);

  // The vertex participating in the operation should belong to the edge
  if ( !V1.IsSame(m_vertex) && !V2.IsSame(m_vertex) )
  {
    // Vertex to delete does not belong to the edge
    return false; // Failure
  }

  TopoDS_Vertex vertex2Kill, vertex2Survive;
  if ( m_bSurvive && m_vertex.IsSame(V1) ) // V1 is asked to survive
  {
    vertex2Kill    = V2;
    vertex2Survive = V1;
  }
  else if ( m_bSurvive && m_vertex.IsSame(V2) ) // V2 is asked to survive
  {
    vertex2Kill    = V1;
    vertex2Survive = V2;
  }
  else
  {
    vertex2Kill    = m_vertex;
    vertex2Survive = m_vertex.IsSame(V1) ? V2 : V1;
  }

  // Prepare modification: remove vertex
  m_ctx->Replace(vertex2Kill, vertex2Survive);

  // Prepare modification: remove edge
  m_ctx->Remove(m_edge);

  // Apply modification
  if ( doApply )
    m_result = m_ctx->Apply(m_master);

  return true; // Success
}
