//-----------------------------------------------------------------------------
// Created on: 19 September 2016
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
#include <asiVisu_ContourPointsDataProvider.h>

// asiAlgo includes
#include <asiAlgo_PointCloudUtils.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>

//-----------------------------------------------------------------------------

//! Constructor.
asiVisu_ContourPointsDataProvider::asiVisu_ContourPointsDataProvider(const Handle(asiData_ContourNode)& contour)
: asiVisu_PointsDataProvider(contour)
{}

//-----------------------------------------------------------------------------

//! \return point cloud to visualize.
Handle(asiAlgo_BaseCloud<double>) asiVisu_ContourPointsDataProvider::GetPoints() const
{
  Handle(asiData_ContourNode)
    contour_n = Handle(asiData_ContourNode)::DownCast(m_node);
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
    return NULL;

  // Get B-Rep representation of the contour
  TopoDS_Wire contourWire = contour_n->AsShape();

  // Number of vertices
  TopTools_IndexedMapOfShape vertices;
  TopExp::MapShapes(contourWire, TopAbs_VERTEX, vertices);
  const int nVertices = vertices.Extent();
  //
  if ( !nVertices )
    return NULL;

  // Build point cloud from vertices
  Handle(HRealArray) coords = new HRealArray(0, nVertices*3 - 1);
  //
  for ( int v = 0; v < nVertices; ++v )
  {
    const TopoDS_Vertex& V = TopoDS::Vertex( vertices(v + 1) );
    gp_Pnt               P = BRep_Tool::Pnt(V);

    coords->SetValue( v*3,     P.X() );
    coords->SetValue( v*3 + 1, P.Y() );
    coords->SetValue( v*3 + 2, P.Z() );
  }
  //
  return asiAlgo_PointCloudUtils::AsPointCloud(coords);
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_ContourPointsDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  Handle(asiData_ContourNode)
    points_n = Handle(asiData_ContourNode)::DownCast(m_node);
  //
  if ( points_n.IsNull() || !points_n->IsWellFormed() )
    return out;

  // Register Parameter as sensitive
  out << points_n->Parameter(asiData_ContourNode::PID_Coords)
      << points_n->Parameter(asiData_ContourNode::PID_Geometry);

  return out;
}
