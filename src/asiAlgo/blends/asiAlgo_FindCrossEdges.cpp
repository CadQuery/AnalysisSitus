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
#include <asiAlgo_FindCrossEdges.h>

// asiAlgo includes
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiAlgo_FindCrossEdges::asiAlgo_FindCrossEdges(const TopoDS_Shape&        masterCAD,
                                               const Handle(asiAlgo_AAG)& aag,
                                               ActAPI_ProgressEntry       progress,
                                               ActAPI_PlotterEntry        plotter)
: ActAPI_IAlgorithm ( progress, plotter ),
  m_master          ( masterCAD ),
  m_aag             ( aag )
{}

//-----------------------------------------------------------------------------

asiAlgo_FindCrossEdges::asiAlgo_FindCrossEdges(const Handle(asiAlgo_AAG)& aag,
                                               ActAPI_ProgressEntry       progress,
                                               ActAPI_PlotterEntry        plotter)
: ActAPI_IAlgorithm ( progress, plotter ),
  m_master          ( aag->GetMasterCAD() ),
  m_aag             ( aag )
{}

//-----------------------------------------------------------------------------

bool asiAlgo_FindCrossEdges::PerformForFace(const int                         faceIdx,
                                            const TopTools_IndexedMapOfShape& smoothEdges,
                                            const TopTools_IndexedMapOfShape& springEdges,
                                            const double                      candidateRadius)
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

  // Get working candidate face
  const TopoDS_Face& face = m_aag->GetFace(faceIdx);

  // Get all smooth non-spring edges
  TopTools_IndexedMapOfShape remainingEdges;
  for ( TopExp_Explorer exp(face, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    if ( !smoothEdges.Contains( exp.Current() ) )
      continue;

    if ( !springEdges.Contains( exp.Current() ) )
      remainingEdges.Add( exp.Current() );
  }

  // Test remaining edges on hitting the cross-edge rule
  for ( int e = 1; e <= remainingEdges.Extent(); ++e )
  {
    const TopoDS_Edge& edge = TopoDS::Edge( remainingEdges(e) );

    double f, l;
    BRep_Tool::Range(edge, f, l);
    //
    const double midParam = (f + l)*0.5;

    // Evaluate directional curvature
    double k;
    if ( !asiAlgo_Utils::EvaluateAlongCurvature(face, edge, midParam, k) )
    {
      this->Progress().SendLogMessage( LogErr(Normal) << "Cannot evaluate directional curvature." );
      return false;
    }

#if defined COUT_DEBUG
    std::cout << "\tDirectional curvature: " << k << " ~ radius = " << (1.0 / k) <<  std::endl;
#endif

    if ( Abs(k) > 1.0e-5 )
    {
      const double actualRadius = 1 / k;
      //
      if ( Abs(candidateRadius - actualRadius) < 1.0e-5 )
        m_result.edges.Add(edge);
    }
  }

  // TODO: NYI

  // Recover IDs
  this->getIds(m_result.edges, m_result.ids);

  return true;
}

//-----------------------------------------------------------------------------

void asiAlgo_FindCrossEdges::getIds(const TopTools_IndexedMapOfShape& edges,
                                    TColStd_PackedMapOfInteger&       indices) const
{
  const TopTools_IndexedMapOfShape& allEdges = m_aag->GetMapOfEdges();
  //
  for ( int eidx = 1; eidx <= edges.Extent(); ++eidx )
  {
    const TopoDS_Shape& edgeOfInterest = edges(eidx);
    indices.Add( allEdges.FindIndex(edgeOfInterest) );
  }
}
