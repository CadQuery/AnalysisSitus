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
#include <asiAlgo_MeshOffset.h>

// asiAlgo includes
#include <asiAlgo_MeshComputeNorms.h>

// Active Data includes
#include <ActData_Mesh_ElementsIterator.h>

//-----------------------------------------------------------------------------

asiAlgo_MeshOffset::asiAlgo_MeshOffset(const Handle(ActData_Mesh)& mesh,
                                       ActAPI_ProgressEntry        progress,
                                       ActAPI_PlotterEntry         plotter)
: ActAPI_IAlgorithm(progress, plotter), m_input(mesh)
{}

//-----------------------------------------------------------------------------

void asiAlgo_MeshOffset::Perform(const double dist)
{
  m_result = new ActData_Mesh;

  // Compute normal field in nodes.
  asiAlgo_MeshComputeNorms computeNorms(m_input, m_progress, m_plotter);
  computeNorms.Perform(false);
  //
  const NCollection_DataMap<int, gp_Vec>& nodeNorms = computeNorms.GetResult();

  // Loop over the nodes producing new (offset) ones for the resulting mesh.
  for ( ActData_Mesh_ElementsIterator nit(m_input, ActData_Mesh_ET_Node); nit.More(); nit.Next() )
  {
    // Access next node with its owner mesh elements.
    const Handle(ActData_Mesh_Node)&
      node = Handle(ActData_Mesh_Node)::DownCast( nit.GetValue() );
    //
    const int nodeId = node->GetID();

    // Compute offset point.
    const gp_XYZ& xyz = node->Pnt().XYZ();
    gp_XYZ        newCoords;
    //
    if ( !nodeNorms.IsBound(nodeId) )
    {
      m_progress.SendLogMessage(LogWarn(Normal) << "Normal is not defined at node %1." << nodeId);

      newCoords = xyz;
    }
    else
      newCoords = xyz + nodeNorms(nodeId).XYZ()*dist;

    // Add node to the resulting mesh.
    m_result->AddNodeWithID(newCoords.X(), newCoords.Y(), newCoords.Z(), nodeId);
  }

  // Copy elements (i.e., topology is identical).
  for ( ActData_Mesh_ElementsIterator it(m_input, ActData_Mesh_ET_Face); it.More(); it.Next() )
  {
    const Handle(ActData_Mesh_Element)& E = it.GetValue();

    m_result->AddElementWithID( E, E->GetID() );
  }
}
