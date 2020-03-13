//-----------------------------------------------------------------------------
// Created on: 12 March 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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
#include <asiVisu_DomainPointsDataProvider.h>

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
asiVisu_DomainPointsDataProvider::asiVisu_DomainPointsDataProvider(const Handle(asiData_FaceNode)& face)
: asiVisu_PointsDataProvider(face)
{
  // Access owning geometry
  m_partNode = Handle(asiData_PartNode)::DownCast( m_node->GetParentNode() );
}

//-----------------------------------------------------------------------------

//! \return global index of the OCCT face to be visualized.
int asiVisu_DomainPointsDataProvider::GetFaceIndexAmongSubshapes() const
{
  const int globalId = Handle(asiData_FaceNode)::DownCast(m_node)->GetSelectedFace();

  return globalId;
}

//-----------------------------------------------------------------------------

//! \return local index of the OCCT face to be visualized.
int asiVisu_DomainPointsDataProvider::GetFaceIndexAmongFaces() const
{
  const int globalId = this->GetFaceIndexAmongSubshapes();

  if ( globalId )
  {
    if ( m_partNode->GetAAG().IsNull() )
      return 0;

    const TopTools_IndexedMapOfShape&
      faces = m_partNode->GetAAG()->GetMapOfFaces();

    const TopTools_IndexedMapOfShape&
      subShapes = m_partNode->GetAAG()->RequestMapOfSubShapes();

    return faces.FindIndex( subShapes.FindKey(globalId) );
  }

  return 0;
}

//-----------------------------------------------------------------------------

//! \return topological face extracted from the part by its stored ID.
TopoDS_Face asiVisu_DomainPointsDataProvider::ExtractFace() const
{
  const int fIdx = this->GetFaceIndexAmongSubshapes();
  if ( !fIdx )
    return TopoDS_Face();

  if ( m_partNode->GetAAG().IsNull() )
    return TopoDS_Face();

  const TopTools_IndexedMapOfShape&
    subShapes = m_partNode->GetAAG()->RequestMapOfSubShapes();

  const TopoDS_Shape& shape = subShapes.FindKey(fIdx);
  //
  if ( shape.ShapeType() != TopAbs_FACE )
    return TopoDS_Face();

  // Access face by the stored index
  const TopoDS_Face& F = TopoDS::Face(shape);
  return F;
}

//-----------------------------------------------------------------------------

//! \return point cloud to visualize.
Handle(asiAlgo_BaseCloud<double>) asiVisu_DomainPointsDataProvider::GetPoints() const
{
  TopoDS_Face face = this->ExtractFace();

  // Number of vertices
  TopTools_IndexedMapOfShape vertices;
  TopExp::MapShapes(face, TopAbs_VERTEX, vertices);
  const int nVertices = vertices.Extent();
  //
  if ( !nVertices )
    return nullptr;

  // Build point cloud from vertices
  Handle(HRealArray) coords = new HRealArray(0, nVertices*3 - 1);
  //
  for ( int v = 0; v < nVertices; ++v )
  {
    const TopoDS_Vertex& V = TopoDS::Vertex( vertices(v + 1) );
    gp_Pnt2d             P = BRep_Tool::Parameters(V, face);

    coords->SetValue( v*3,     P.X() );
    coords->SetValue( v*3 + 1, P.Y() );
    coords->SetValue( v*3 + 2, 0. );
  }
  //
  return asiAlgo_PointCloudUtils::AsCloudd(coords);
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_DomainPointsDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  // Register Parameter as sensitive.
  out << m_node->Parameter(asiData_FaceNode::PID_SelectedFace);

  return out;
}
