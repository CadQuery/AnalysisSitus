//-----------------------------------------------------------------------------
// Created on: 01 March 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
