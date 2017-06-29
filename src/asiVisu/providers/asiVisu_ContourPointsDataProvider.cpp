//-----------------------------------------------------------------------------
// Created on: 19 September 2016
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
