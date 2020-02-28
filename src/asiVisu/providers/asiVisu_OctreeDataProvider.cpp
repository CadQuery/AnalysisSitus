//-----------------------------------------------------------------------------
// Created on: 25 November 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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
#include <asiVisu_OctreeDataProvider.h>

// asiData includes
#include <asiData_IVTopoItemNode.h>
#include <asiData_PartNode.h>
#include <asiData_TriangulationNode.h>

//-----------------------------------------------------------------------------

asiVisu_OctreeDataProvider::asiVisu_OctreeDataProvider(const Handle(asiData_OctreeNode)& N)
: asiVisu_DataProvider (),
  m_node               (N)
{
  // Get parent Node which is the owner of an octree by design.
  m_ownerNode = m_node->GetParentNode();
  //
  if ( m_ownerNode.IsNull() || !m_ownerNode->IsWellFormed() )
    Standard_ProgramError::Raise("Inconsistent data model.");
}

//-----------------------------------------------------------------------------

asiAlgo_BVHFacets* asiVisu_OctreeDataProvider::GetFacets() const
{
  /* Part owner */
  if ( m_ownerNode->IsKind( STANDARD_TYPE(asiData_PartNode) ) )
    return Handle(asiData_PartNode)::DownCast(m_ownerNode)->GetBVH().get();

  /* Triangulation owner */
  if ( m_ownerNode->IsKind( STANDARD_TYPE(asiData_TriangulationNode) ) )
    return Handle(asiData_TriangulationNode)::DownCast(m_ownerNode)->GetBVH().get();

  /* IV topo item owner */
  if ( m_ownerNode->IsKind( STANDARD_TYPE(asiData_IVTopoItemNode) ) )
    return Handle(asiData_IVTopoItemNode)::DownCast(m_ownerNode)->GetBVH().get();

  /* Unexpected type of owner */
  return nullptr;
}

//-----------------------------------------------------------------------------

void* asiVisu_OctreeDataProvider::GetOctree() const
{
  return m_node->GetOctree();
}

//-----------------------------------------------------------------------------

bool asiVisu_OctreeDataProvider::IsPointExtraction() const
{
  return m_node->GetExtractPoints();
}

//-----------------------------------------------------------------------------

int asiVisu_OctreeDataProvider::GetSamplingStrategy() const
{
  return m_node->GetSamplingStrategy();
}

//-----------------------------------------------------------------------------

double asiVisu_OctreeDataProvider::GetMaxVectorModulus() const
{
  return m_node->GetMaxVectorSize();
}

//-----------------------------------------------------------------------------

Handle(ActAPI_HParameterList)
  asiVisu_OctreeDataProvider::translationSources() const
{
  ActAPI_ParameterStream out;
  out << m_node->Parameter(asiData_OctreeNode::PID_Octree)
      << m_node->Parameter(asiData_OctreeNode::PID_SamplingStrategy)
      << m_node->Parameter(asiData_OctreeNode::PID_ExtractPoints)
      << m_node->Parameter(asiData_OctreeNode::PID_MaxVectorSize);

  /* Part owner */
  if ( m_ownerNode->IsKind( STANDARD_TYPE(asiData_PartNode) ) )
    out << m_ownerNode->Parameter(asiData_PartNode::PID_BVH);

  /* Triangulation owner */
  if ( m_ownerNode->IsKind( STANDARD_TYPE(asiData_TriangulationNode) ) )
    out << m_ownerNode->Parameter(asiData_TriangulationNode::PID_BVH);

  /* IV topo item owner */
  if ( m_ownerNode->IsKind( STANDARD_TYPE(asiData_IVTopoItemNode) ) )
    out << m_ownerNode->Parameter(asiData_IVTopoItemNode::PID_BVH);

  return out.List;
}
