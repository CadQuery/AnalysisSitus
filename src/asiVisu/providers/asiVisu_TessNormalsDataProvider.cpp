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
#include <asiVisu_TessNormalsDataProvider.h>

// asiData includes
#include <asiData_TessNode.h>

// asiAlgo includes
#include <asiAlgo_PointCloudUtils.h>
#include <asiAlgo_Utils.h>

// Active Data includes
#include <ActData_Mesh_ElementsIterator.h>
#include <ActData_Mesh_Quadrangle.h>
#include <ActData_Mesh_Triangle.h>

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
Handle(asiAlgo_BaseCloud<double>) asiVisu_TessNormalsDataProvider::GetPointsd()
{
  // Get vectors.
  Handle(asiData_TessNormsNode)
    normsNode = Handle(asiData_TessNormsNode)::DownCast(m_source);
  //
  m_vectors = asiAlgo_PointCloudUtils::AsCloudd( normsNode->GetVectors() );

  // Access parent Tessellation Node to get geometry of mesh.
  Handle(asiData_TessNode)
    tessNode = Handle(asiData_TessNode)::DownCast( normsNode->GetParentNode() );
  //
  Handle(ActData_Mesh) mesh = tessNode->GetMesh();

  // Get mask of node/element IDs.
  Handle(HIntArray) ids = normsNode->GetIDs();
  //
  TColStd_PackedMapOfInteger idsMap;
  //
  for ( int i = 0; i < ids->Length(); ++i )
    idsMap.Add( ids->Value(i) );

  // Prepare the position cloud to return.
  m_points = new asiAlgo_BaseCloud<double>;

  // Get positions of the vectors.
  if ( normsNode->IsElemental() )
  {
    // Compute COGs of mesh elements.
    for ( ActData_Mesh_ElementsIterator eit(mesh, ActData_Mesh_ET_Face); eit.More(); eit.Next() )
    {
      const Handle(ActData_Mesh_Element)& elem = eit.GetValue();
      //
      if ( !idsMap.Contains( elem->GetID() ) )
        continue;

      // Get nodes of the element.
      std::vector<Handle(ActData_Mesh_Node)> nodes;
      //
      this->elementNodes(mesh, elem, nodes);

      // Compute COG.
      gp_XYZ cog;
      //
      if ( elem->IsKind( STANDARD_TYPE(ActData_Mesh_Triangle) ) )
      {
        const gp_Pnt& P0 = nodes[0]->Pnt();
        const gp_Pnt& P1 = nodes[1]->Pnt();
        const gp_Pnt& P2 = nodes[2]->Pnt();

        cog = ( P0.XYZ() + P1.XYZ() + P2.XYZ() ) / 3.0;
      }
      else if ( elem->IsKind( STANDARD_TYPE(ActData_Mesh_Quadrangle) ) )
      {
        const gp_Pnt& P0 = nodes[0]->Pnt();
        const gp_Pnt& P1 = nodes[1]->Pnt();
        const gp_Pnt& P2 = nodes[2]->Pnt();
        const gp_Pnt& P3 = nodes[3]->Pnt();

        cog = ( P0.XYZ() + P1.XYZ() + P2.XYZ() + P3.XYZ() ) / 4.0;
      }

      m_points->AddElement( cog.X(), cog.Y(), cog.Z() );
    }
  }
  else
  {
    // Get array of node indices and extract nodes of interest.
    for ( ActData_Mesh_ElementsIterator nit(mesh, ActData_Mesh_ET_Node); nit.More(); nit.Next() )
    {
      Handle(ActData_Mesh_Node)
        node = Handle(ActData_Mesh_Node)::DownCast( nit.GetValue() );
      //
      if ( idsMap.Contains( node->GetID() ) )
        m_points->AddElement( node->Pnt().X(), node->Pnt().Y(), node->Pnt().Z() );
    }
  }

  return m_points;
}

//-----------------------------------------------------------------------------

//! \return normals to visualize.
Handle(asiAlgo_BaseCloud<double>) asiVisu_TessNormalsDataProvider::GetVectorsd()
{
  // Return cached vectors available after GetPointsd() invocation.
  return m_vectors;
}

//-----------------------------------------------------------------------------

//! \return max modulus for a normal.
double asiVisu_TessNormalsDataProvider::GetMaxVectorModulus() const
{
  Bnd_Box aabb;

  for ( int i = 0; i < m_points->GetNumberOfElements(); ++i )
  {
    double x, y, z;
    m_points->GetElement(i, x, y, z);
    //
    aabb.Add( gp_Pnt(x, y, z)  );
  }

  const double size = ( aabb.CornerMax().XYZ() - aabb.CornerMin().XYZ() ).Modulus()*0.025;

  return size;
}

//-----------------------------------------------------------------------------

void asiVisu_TessNormalsDataProvider::elementNodes(const Handle(ActData_Mesh)&             mesh,
                                                   const Handle(ActData_Mesh_Element)&     elem,
                                                   std::vector<Handle(ActData_Mesh_Node)>& nodes) const
{
  nodes.clear();
  int  numNodes = elem->NbNodes();
  int* nodeIDs  = nullptr;

  if ( numNodes == 3 )
  {
    const Handle(ActData_Mesh_Triangle)&
      tri = Handle(ActData_Mesh_Triangle)::DownCast(elem);
    //
    nodeIDs = (int*) tri->GetConnections();
  }
  else if ( numNodes == 4 )
  {
    const Handle(ActData_Mesh_Quadrangle)&
      quad = Handle(ActData_Mesh_Quadrangle)::DownCast(elem);
    //
    nodeIDs = (int*) quad->GetConnections();
  }

  // Populate output collection.
  for ( int n = 0; n < numNodes; ++n )
    nodes.push_back( mesh->FindNode(nodeIDs[n]) );
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
