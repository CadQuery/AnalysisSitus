//-----------------------------------------------------------------------------
// Created on: 30 January 2019
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
#include <asiVisu_SurfaceDeviationDataProvider.h>

// asiData includes
#include <asiData_SurfDeviationNode.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param[in] N source Node.
asiVisu_SurfaceDeviationDataProvider::asiVisu_SurfaceDeviationDataProvider(const Handle(ActAPI_INode)& N)
: asiVisu_SurfaceDataProvider(),
  m_node(N)
{}

//-----------------------------------------------------------------------------

//! \return ID of the associated Data Node.
ActAPI_DataObjectId asiVisu_SurfaceDeviationDataProvider::GetNodeID() const
{
  return m_node->GetId();
}

//-----------------------------------------------------------------------------

//! \return surface type.
Handle(Standard_Type) asiVisu_SurfaceDeviationDataProvider::GetSurfaceType() const
{
  // Cast to Deviation Node.
  Handle(asiData_SurfDeviationNode)
    sdNode = Handle(asiData_SurfDeviationNode)::DownCast(m_node);

  // Get surface.
  Handle(asiData_IVSurfaceNode) surface_n = sdNode->GetSurfaceNode();
  //
  if ( surface_n.IsNull() || !surface_n->IsWellFormed() )
    return NULL;

  return surface_n->GetSurface()->DynamicType();
}

//-----------------------------------------------------------------------------

// Accessor for the parametric surface.
//! \param[out] uMin min bound for U curvilinear axis.
//! \param[out] uMax max bound for U curvilinear axis.
//! \param[out] vMin min bound for V curvilinear axis.
//! \param[out] vMax max bound for V curvilinear axis.
//! \return surface.
Handle(Geom_Surface)
  asiVisu_SurfaceDeviationDataProvider::GetSurface(double& uMin,
                                                   double& uMax,
                                                   double& vMin,
                                                   double& vMax) const
{
  // Cast to Deviation Node.
  Handle(asiData_SurfDeviationNode)
    sdNode = Handle(asiData_SurfDeviationNode)::DownCast(m_node);

  // Get surface.
  Handle(asiData_IVSurfaceNode) surface_n = sdNode->GetSurfaceNode();
  //
  if ( surface_n.IsNull() || !surface_n->IsWellFormed() )
    return NULL;

  // Access data from the Node.
  double uLimit, vLimit;
  surface_n->GetLimits(uLimit, vLimit);
  //
  uMin = -uLimit;
  uMax =  uLimit;
  vMin = -vLimit;
  vMax =  vLimit;

  return surface_n->GetSurface();
}

//-----------------------------------------------------------------------------

//! \return reference mesh.
Handle(asiAlgo_BVHFacets)
  asiVisu_SurfaceDeviationDataProvider::GetReferenceMesh()
{
  // Cast to Deviation Node.
  Handle(asiData_SurfDeviationNode)
    sdNode = Handle(asiData_SurfDeviationNode)::DownCast(m_node);

  // Get triangulation.
  Handle(asiData_TriangulationNode) tris_n = sdNode->GetTriangulationNode();
  //
  if ( tris_n.IsNull() || !tris_n->IsWellFormed() )
    return NULL;

  return tris_n->GetBVH(); // Return BVH structure.
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList)
  asiVisu_SurfaceDeviationDataProvider::translationSources() const
{
  // Resulting Parameters.
  ActParamStream out;

  // Add sensitive Parameters.
  out << m_node->Parameter(asiData_SurfDeviationNode::PID_SurfaceRef)
      << m_node->Parameter(asiData_SurfDeviationNode::PID_MeshRef);

  return out;
}
