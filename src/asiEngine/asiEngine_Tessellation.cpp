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
#include <asiEngine_Tessellation.h>

// asiAlgo includes
#include <asiAlgo_MeshComputeNorms.h>

//-----------------------------------------------------------------------------

Handle(asiData_TessNode) asiEngine_Tessellation::CreateTessellation()
{
  // Add Tessellation Node to Partition.
  Handle(asiData_TessNode)
    tess_n = Handle(asiData_TessNode)::DownCast( asiData_TessNode::Instance() );
  //
  m_model->GetTessellationPartition()->AddNode(tess_n);

  // Initialize
  tess_n->Init();
  tess_n->SetUserFlags(NodeFlag_IsPresentedInPartView);
  tess_n->SetName("Tessellation");

  // Return the just created Node.
  return tess_n;
}

//-----------------------------------------------------------------------------

Handle(asiData_TessNormsNode)
  asiEngine_Tessellation::ComputeNorms(const Handle(asiData_TessNode)& tessNode,
                                       const bool                      doElemNorms)
{
  // Get mesh to compute normal vectors for.
  Handle(ActData_Mesh) mesh = tessNode->GetMesh();
  //
  if ( mesh.IsNull() )
    return NULL;

  // Compute normal vectors.
  asiAlgo_MeshComputeNorms algo(mesh, m_progress, m_plotter);
  //
  algo.Perform(doElemNorms);

  // Get the resulting normal field.
  Handle(HIntArray) nodeIds;
  Handle(HRealArray) vectors;
  //
  algo.GetResultArrays(nodeIds, vectors);

  // Add Tessellation Norms Node to Partition.
  Handle(asiData_TessNormsNode)
    tessNorms_n = Handle(asiData_TessNormsNode)::DownCast( asiData_TessNormsNode::Instance() );
  //
  m_model->GetTessellationNormsPartition()->AddNode(tessNorms_n);

  // Initialize.
  tessNorms_n->Init(nodeIds, vectors);
  tessNorms_n->SetIsElemental(doElemNorms);
  tessNorms_n->SetName(doElemNorms ? "Elemental norms" : "Nodal (averaged) norms");

  // Add as child.
  tessNode->AddChildNode(tessNorms_n);

  return tessNorms_n;
}
