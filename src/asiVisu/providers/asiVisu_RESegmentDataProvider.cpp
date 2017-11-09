//-----------------------------------------------------------------------------
// Created on: 01 March 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_RESegmentDataProvider.h>

// asiData includes
#include <asiData_REPointsNode.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param segment_n [in] Segment Node.
asiVisu_RESegmentDataProvider::asiVisu_RESegmentDataProvider(const Handle(asiData_RESegmentNode)& segment_n)
: asiVisu_PointsDataProvider(segment_n)
{}

//-----------------------------------------------------------------------------

//! \return point cloud to visualize.
Handle(asiAlgo_BaseCloud<double>) asiVisu_RESegmentDataProvider::GetPoints() const
{
  Handle(asiData_RESegmentNode)
    segment_n = Handle(asiData_RESegmentNode)::DownCast(m_node);
  //
  if ( segment_n.IsNull() || !segment_n->IsWellFormed() )
    return NULL;

  // Get the entire point cloud
  Handle(ActAPI_INode)              points_base = segment_n->GetParentNode()  // Segments
                                                           ->GetParentNode(); // Points
  Handle(asiData_REPointsNode)      points_n    = Handle(asiData_REPointsNode)::DownCast(points_base);
  Handle(asiAlgo_BaseCloud<double>) universum   = points_n->GetPoints();
  //
  return universum;
}

//-----------------------------------------------------------------------------

//! \return indices of points to extract from the input point cloud. This
//!         opens the door to working on point cloud regions while keeping
//!         a link to the original data.
Handle(TColStd_HPackedMapOfInteger)
  asiVisu_RESegmentDataProvider::GetIndices() const
{
  Handle(asiData_RESegmentNode)
    segment_n = Handle(asiData_RESegmentNode)::DownCast(m_node);
  //
  if ( segment_n.IsNull() || !segment_n->IsWellFormed() )
    return NULL;

  // Get indices of the points in a segment
  return segment_n->GetIndices();
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_RESegmentDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  // Get Segment Node
  Handle(asiData_RESegmentNode)
    segment_n = Handle(asiData_RESegmentNode)::DownCast(m_node);
  //
  if ( segment_n.IsNull() || !segment_n->IsWellFormed() )
    return out;

  // Get the entire point cloud
  Handle(ActAPI_INode) points_n = segment_n->GetParentNode()  // Segments
                                           ->GetParentNode(); // Points

  // Register Parameters as sensitive: both input point cloud and its filter
  out << points_n->Parameter(asiData_REPointsNode::PID_Points)
      << segment_n->Parameter(asiData_RESegmentNode::PID_Indices);

  return out;
}
