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
#include <asiAlgo_MeshComputeNorms.h>

// Active Data includes
#include <ActData_Mesh_ElementsIterator.h>
#include <ActData_Mesh_Quadrangle.h>
#include <ActData_Mesh_Triangle.h>

//-----------------------------------------------------------------------------

asiAlgo_MeshComputeNorms::asiAlgo_MeshComputeNorms(const Handle(ActData_Mesh)& mesh,
                                                   ActAPI_ProgressEntry        progress,
                                                   ActAPI_PlotterEntry         plotter)
: m_mesh(mesh), ActAPI_IAlgorithm(progress, plotter)
{}

//-----------------------------------------------------------------------------

void asiAlgo_MeshComputeNorms::Perform(const bool doElemNorms)
{
  if ( doElemNorms )
  {
    // Loop over the mesh elements.
    for ( ActData_Mesh_ElementsIterator eit(m_mesh, ActData_Mesh_ET_Face); eit.More(); eit.Next() )
    {
      const Handle(ActData_Mesh_Element)& elem = eit.GetValue();
      //
      gp_Vec elemNorm = this->computeElemNorm(elem);

      // Add vector to the resulting normal field.
      m_norms.Bind(elem->GetID(), elemNorm);
    }
  }
  else
  {
    // Loop over the nodes.
    for ( ActData_Mesh_ElementsIterator nit(m_mesh, ActData_Mesh_ET_Node); nit.More(); nit.Next() )
    {
      // Access next node with its owner mesh elements.
      const Handle(ActData_Mesh_Node)&
        node = Handle(ActData_Mesh_Node)::DownCast( nit.GetValue() );
      //
      const ActData_Mesh_ListOfElements& lstInvElements = node->InverseElements();

      // Skip free nodes.
      if ( !lstInvElements.Extent() )
        continue;

      // Calculate normal by averaging norm vectors among all owner elements.
      gp_Vec avrNorm;
      int numElems = 0;
      //
      for ( ActData_Mesh_ListOfElements::Iterator eit(lstInvElements); eit.More(); eit.Next(), ++numElems )
      {
        gp_Vec elemNorm = this->computeElemNorm( eit.Value() );
        avrNorm += elemNorm;
      }
      //
      avrNorm /= numElems;
      //
      avrNorm.Normalize();

      // Add vector to the resulting normal field.
      m_norms.Bind(node->GetID(), avrNorm);
    }
  }
}

//-----------------------------------------------------------------------------

gp_Vec asiAlgo_MeshComputeNorms::computeElemNorm(const Handle(ActData_Mesh_Element)& elem) const
{
  gp_Vec res;

  // Get nodes of the element.
  std::vector<Handle(ActData_Mesh_Node)> nodes;
  //
  this->elementNodes(elem, nodes);

  // Compute norm vector.
  if ( elem->IsKind( STANDARD_TYPE(ActData_Mesh_Triangle) ) )
  {
    const gp_Pnt& P0 = nodes[0]->Pnt();
    const gp_Pnt& P1 = nodes[1]->Pnt();
    const gp_Pnt& P2 = nodes[2]->Pnt();

    gp_Vec P0P1 = P1.XYZ() - P0.XYZ();
    gp_Vec P0P2 = P2.XYZ() - P0.XYZ();
    gp_Vec P1P2 = P2.XYZ() - P1.XYZ();
    //
    res = P0P1.Crossed(P0P2);
    res.Normalize();
  }
  else if ( elem->IsKind( STANDARD_TYPE(ActData_Mesh_Quadrangle) ) )
  {
    const gp_Pnt& P0 = nodes[0]->Pnt();
    const gp_Pnt& P1 = nodes[1]->Pnt();
    const gp_Pnt& P2 = nodes[2]->Pnt();
    const gp_Pnt& P3 = nodes[3]->Pnt();

    gp_Vec P0P1 = P1.XYZ() - P0.XYZ();
    gp_Vec P0P2 = P2.XYZ() - P0.XYZ();
    gp_Vec P0P3 = P3.XYZ() - P0.XYZ();
    gp_Vec P1P2 = P2.XYZ() - P1.XYZ();
    gp_Vec P2P3 = P3.XYZ() - P2.XYZ();
    gp_Vec P1P3 = P3.XYZ() - P1.XYZ();

    gp_Vec QuadNorm1 = P0P1.Crossed(P0P2);
    gp_Vec QuadNorm2 = P0P2.Crossed(P0P3);
    //
    res = QuadNorm1 + QuadNorm2;
    res = ( ( res.Magnitude() < gp::Resolution() ) ? gp_Vec() : res.Normalized() );
  }

  return res;
}

//-----------------------------------------------------------------------------

void asiAlgo_MeshComputeNorms::elementNodes(const Handle(ActData_Mesh_Element)&     elem,
                                            std::vector<Handle(ActData_Mesh_Node)>& nodes) const
{
  nodes.clear();
  int  numNodes = elem->NbNodes();
  int* nodeIDs  = NULL;

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
    nodes.push_back( m_mesh->FindNode(nodeIDs[n]) );
}
