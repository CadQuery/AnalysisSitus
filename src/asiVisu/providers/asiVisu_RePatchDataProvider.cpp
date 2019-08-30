//-----------------------------------------------------------------------------
// Created on: 15 July 2019
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
#include <asiVisu_RePatchDataProvider.h>

// asiData includes
#include <asiData_RePatchNode.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param[in] N source Node.
asiVisu_RePatchDataProvider::asiVisu_RePatchDataProvider(const Handle(ActAPI_INode)& N)
: asiVisu_SurfaceDataProvider(),
  m_node(N)
{}

//-----------------------------------------------------------------------------

//! \return ID of the associated Data Node.
ActAPI_DataObjectId asiVisu_RePatchDataProvider::GetNodeID() const
{
  return m_node->GetId();
}

//-----------------------------------------------------------------------------

//! \return surface type.
Handle(Standard_Type) asiVisu_RePatchDataProvider::GetSurfaceType() const
{
  Handle(asiData_RePatchNode)
    patch_n = Handle(asiData_RePatchNode)::DownCast(m_node);
  //
  if ( patch_n.IsNull() || !patch_n->IsWellFormed() )
    return NULL;

  // Get surface.
  Handle(Geom_Surface) surf = patch_n->GetSurface();
  //
  if ( surf.IsNull() )
    return NULL;

  return surf->DynamicType();
}

//-----------------------------------------------------------------------------

// Accessor for the parametric surface.
//! \param[out] uMin min bound for U curvilinear axis.
//! \param[out] uMax max bound for U curvilinear axis.
//! \param[out] vMin min bound for V curvilinear axis.
//! \param[out] vMax max bound for V curvilinear axis.
//! \return surface.
Handle(Geom_Surface)
  asiVisu_RePatchDataProvider::GetSurface(double& uMin,
                                          double& uMax,
                                          double& vMin,
                                          double& vMax) const
{
  Handle(asiData_RePatchNode)
    patch_n = Handle(asiData_RePatchNode)::DownCast(m_node);
  //
  if ( patch_n.IsNull() || !patch_n->IsWellFormed() )
    return NULL;

  // Get surface.
  Handle(Geom_Surface) surf = patch_n->GetSurface();

  // Get bounds.
  if ( !surf.IsNull() )
    surf->Bounds(uMin, uMax, vMin, vMax);

  return surf;
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_RePatchDataProvider::translationSources() const
{
  // Resulting Parameters.
  ActParamStream out;

  Handle(asiData_RePatchNode)
    patch_n = Handle(asiData_RePatchNode)::DownCast(m_node);
  //
  if ( patch_n.IsNull() || !patch_n->IsWellFormed() )
    return out;

  // Add sensitive Parameters.
  out << patch_n->Parameter(asiData_RePatchNode::PID_Surface)
      << patch_n->Parameter(asiData_RePatchNode::PID_MinNumKnots);

  return out;
}
