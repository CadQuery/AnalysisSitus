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
#include <asiAlgo_EulerKEF.h>

// OCCT includes
#include <TopExp.hxx>
#include <TopoDS.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiAlgo_EulerKEF::asiAlgo_EulerKEF(const TopoDS_Shape&  masterCAD,
                                   const TopoDS_Face&   face,
                                   ActAPI_ProgressEntry progress,
                                   ActAPI_PlotterEntry  plotter)
//
: asiAlgo_Euler (masterCAD, progress, plotter),
  m_face        (face)
{
  TopTools_IndexedMapOfShape edges;
  TopExp::MapShapes(m_face, TopAbs_EDGE, edges);
  //
  m_edge = TopoDS::Edge( edges.FindKey(1) );
}

//-----------------------------------------------------------------------------

asiAlgo_EulerKEF::asiAlgo_EulerKEF(const TopoDS_Shape&              masterCAD,
                                   const TopoDS_Face&               face,
                                   const Handle(BRepTools_ReShape)& ctx,
                                   ActAPI_ProgressEntry             progress,
                                   ActAPI_PlotterEntry              plotter)
//
: asiAlgo_Euler (masterCAD, ctx, progress, plotter),
  m_face        (face)
{
  TopTools_IndexedMapOfShape edges;
  TopExp::MapShapes(m_face, TopAbs_EDGE, edges);
  //
  m_edge = TopoDS::Edge( edges.FindKey(1) );
}

//-----------------------------------------------------------------------------

asiAlgo_EulerKEF::asiAlgo_EulerKEF(const TopoDS_Shape&  masterCAD,
                                   const TopoDS_Face&   face,
                                   const TopoDS_Edge&   edge,
                                   ActAPI_ProgressEntry progress,
                                   ActAPI_PlotterEntry  plotter)
//
: asiAlgo_Euler (masterCAD, progress, plotter),
  m_face        (face),
  m_edge        (edge)
{}

//-----------------------------------------------------------------------------

asiAlgo_EulerKEF::asiAlgo_EulerKEF(const TopoDS_Shape&              masterCAD,
                                   const TopoDS_Face&               face,
                                   const TopoDS_Edge&               edge,
                                   const Handle(BRepTools_ReShape)& ctx,
                                   ActAPI_ProgressEntry             progress,
                                   ActAPI_PlotterEntry              plotter)
//
: asiAlgo_Euler (masterCAD, ctx, progress, plotter),
  m_face        (face),
  m_edge        (edge)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_EulerKEF::perform(const bool doApply)
{
  TopTools_IndexedMapOfShape faces;
  TopExp::MapShapes(m_master, TopAbs_FACE, faces);

  if ( !faces.Contains(m_face) )
  {
    // Face to delete does not belong to the master.
    return false; // Failure
  }

  TopTools_IndexedMapOfShape faceEdges;
  TopExp::MapShapes(m_face, TopAbs_EDGE, faceEdges);

  if ( !faceEdges.Contains(m_edge) )
  {
    // Edge to delete does not belong to the face
    return false; // Failure
  }

  // Prepare modification: remove edge
  m_ctx->Remove(m_edge);

  // Prepare modification: remove face
  m_ctx->Remove(m_face);

  // Apply modification
  if ( doApply )
    m_result = m_ctx->Apply(m_master);

  return true; // Success
}
