//-----------------------------------------------------------------------------
// Created on: 06 November 2018
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
#include <asiVisu_ReEdgePolylineDataProvider.h>

// asiAlgo includes
#include <asiAlgo_PointCloudUtils.h>

//-----------------------------------------------------------------------------

asiVisu_ReEdgePolylineDataProvider::asiVisu_ReEdgePolylineDataProvider(const Handle(asiData_ReEdgeNode)& N)
: asiVisu_PointsDataProvider(N)
{}

//-----------------------------------------------------------------------------

Handle(asiAlgo_BaseCloud<double>)
  asiVisu_ReEdgePolylineDataProvider::GetPoints() const
{
  Handle(asiData_ReEdgeNode)
    edge_n = Handle(asiData_ReEdgeNode)::DownCast(m_node);
  //
  if ( edge_n.IsNull() || !edge_n->IsWellFormed() )
    return NULL;

  // Prepare result.
  Handle(asiAlgo_BaseCloud<double>) res = new asiAlgo_BaseCloud<double>;

  // Get coordinates.
  Handle(HRealArray) hCoords = edge_n->GetPolyline();
  const int          numPts  = edge_n->GetNumPolylinePoles();
  //
  for ( int idx = 0; idx < numPts; ++idx )
  {
    const double x = hCoords->Value(3*idx + 0);
    const double y = hCoords->Value(3*idx + 1);
    const double z = hCoords->Value(3*idx + 2);
    //
    res->AddElement(x, y, z);
  }

  return res;
}

//-----------------------------------------------------------------------------

Handle(ActAPI_HParameterList)
  asiVisu_ReEdgePolylineDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  Handle(asiData_ReEdgeNode)
    edge_n = Handle(asiData_ReEdgeNode)::DownCast(m_node);
  //
  if ( edge_n.IsNull() || !edge_n->IsWellFormed() )
    return out;

  // Register Parameter as sensitive.
  out << edge_n->Parameter(asiData_ReEdgeNode::PID_Polyline);

  return out;
}
