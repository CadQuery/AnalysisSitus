//-----------------------------------------------------------------------------
// Created on: 05 October 2018
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
#include <asiVisu_TessNormalsDataProvider.h>

// asiData includes
#include <asiData_TessNode.h>

// asiAlgo includes
#include <asiAlgo_PointCloudUtils.h>
#include <asiAlgo_Utils.h>

// Active Data includes
#include <ActData_Mesh_ElementsIterator.h>

// OCCT includes
#include <Bnd_Box.hxx>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param[in] N source Data Node.
asiVisu_TessNormalsDataProvider::asiVisu_TessNormalsDataProvider(const Handle(asiData_TessNormsNode)& N)
: asiVisu_VectorsDataProvider(N)
{}

//-----------------------------------------------------------------------------

//! \return points where normals are located.
Handle(asiAlgo_BaseCloud<float>) asiVisu_TessNormalsDataProvider::GetPointsf()
{
  // Get vectors.
  Handle(asiData_TessNormsNode)
    normsNode = Handle(asiData_TessNormsNode)::DownCast(m_source);
  //
  m_vectors = asiAlgo_PointCloudUtils::AsCloudf( normsNode->GetVectors() );

  // Get mask of node IDs.
  Handle(HIntArray) nodeIds = normsNode->GetIDs();
  //
  TColStd_PackedMapOfInteger nodeIdsMap;
  //
  for ( int i = 0; i < nodeIds->Length(); ++i )
    nodeIdsMap.Add( nodeIds->Value(i) );

  // Access parent Tessellation Node to get coordinates of mesh vertices.
  Handle(asiData_TessNode)
    tessNode = Handle(asiData_TessNode)::DownCast( normsNode->GetParentNode() );
  //
  Handle(ActData_Mesh) mesh = tessNode->GetMesh();

  // Get array of node indices and extract nodes of interest.
  m_points = new asiAlgo_BaseCloud<float>;
  //
  for ( ActData_Mesh_ElementsIterator nit(mesh, ActData_Mesh_ET_Node); nit.More(); nit.Next() )
  {
    Handle(ActData_Mesh_Node)
      node = Handle(ActData_Mesh_Node)::DownCast( nit.GetValue() );
    //
    if ( nodeIdsMap.Contains( node->GetID() ) )
      m_points->AddElement( node->Pnt().X(), node->Pnt().Y(), node->Pnt().Z() );
  }

  return m_points;
}

//-----------------------------------------------------------------------------

//! \return normals to visualize.
Handle(asiAlgo_BaseCloud<float>) asiVisu_TessNormalsDataProvider::GetVectorsf()
{
  // Return cached vectors available after GetPointsf() invocation.
  return m_vectors;
}

//-----------------------------------------------------------------------------

//! \return max modulus for a normal.
double asiVisu_TessNormalsDataProvider::GetMaxVectorModulus() const
{
  Bnd_Box aabb;

  for ( int i = 0; i < m_points->GetNumberOfElements(); ++i )
  {
    float x, y, z;
    m_points->GetElement(i, x, y, z);
    //
    aabb.Add( gp_Pnt(x, y, z)  );
  }

  const double size = ( aabb.CornerMax().XYZ() - aabb.CornerMin().XYZ() ).Modulus()*0.025;

  return size;
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList)
  asiVisu_TessNormalsDataProvider::translationSources() const
{
  // Resulting Parameters.
  ActParamStream out;

  // Register Parameter as sensitive.
  out << m_source->Parameter(asiData_TessNormsNode::PID_IDs)
      << m_source->Parameter(asiData_TessNormsNode::PID_Vectors);

  return out;
}
