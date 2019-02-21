//-----------------------------------------------------------------------------
// Created on: 02 November 2018
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
#include <asiVisu_ReVertexDataProvider.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param[in] N Vertex Node to source data from.
asiVisu_ReVertexDataProvider::asiVisu_ReVertexDataProvider(const Handle(asiData_ReVertexNode)& N)
: asiVisu_PointsDataProvider(N)
{
  m_cloud = new asiAlgo_BaseCloud<double>;
  m_cloud->AddElement(0., 0., 0.); // Always one point.
}

//-----------------------------------------------------------------------------

//! \return point cloud to visualize.
Handle(asiAlgo_BaseCloud<double>) asiVisu_ReVertexDataProvider::GetPoints() const
{
  Handle(asiData_ReVertexNode)
    vertex_n = Handle(asiData_ReVertexNode)::DownCast(m_node);
  //
  if ( vertex_n.IsNull() || !vertex_n->IsWellFormed() )
    return NULL;

  double x, y, z;
  if ( !vertex_n->GetPoint(x, y, z) )
    return NULL;

  m_cloud->SetElement(0, x, y, z);
  return m_cloud;
}

//-----------------------------------------------------------------------------

//! \return NULL filter.
Handle(TColStd_HPackedMapOfInteger) asiVisu_ReVertexDataProvider::GetIndices() const
{
  return NULL;
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_ReVertexDataProvider::translationSources() const
{
  // Resulting Parameters.
  ActParamStream out;

  Handle(asiData_ReVertexNode)
    points_n = Handle(asiData_ReVertexNode)::DownCast(m_node);
  //
  if ( points_n.IsNull() || !points_n->IsWellFormed() )
    return out;

  // Register Parameter as sensitive.
  out << points_n->Parameter(asiData_ReVertexNode::PID_Geometry);

  return out;
}
