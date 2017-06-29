//-----------------------------------------------------------------------------
// Created on: 11 April 2016
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
#include <asiVisu_IVSurfaceDataProvider.h>

// Visualization includes
#include <asiData_IVSurfaceNode.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param N [in] source Node.
asiVisu_IVSurfaceDataProvider::asiVisu_IVSurfaceDataProvider(const Handle(ActAPI_INode)& N)
: asiVisu_DataProvider(),
  m_node(N)
{}

//-----------------------------------------------------------------------------

//! \return ID of the associated Data Node.
ActAPI_DataObjectId asiVisu_IVSurfaceDataProvider::GetNodeID() const
{
  return m_node->GetId();
}

//-----------------------------------------------------------------------------

// Accessor for the parametric surface.
//! \param uLimit [out] absolute bound for U curvilinear axis.
//! \param vLimit [out] absolute bound for V curvilinear axis.
//! \return surface.
Handle(Geom_Surface)
  asiVisu_IVSurfaceDataProvider::GetSurface(double& uLimit, double& vLimit) const
{
  Handle(asiData_IVSurfaceNode)
    surface_n = Handle(asiData_IVSurfaceNode)::DownCast(m_node);
  //
  if ( surface_n.IsNull() || !surface_n->IsWellFormed() )
    return NULL;

  // Access data from the Node
  surface_n->GetLimits(uLimit, vLimit);
  return surface_n->GetSurface();
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_IVSurfaceDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  Handle(asiData_IVSurfaceNode)
    surface_n = Handle(asiData_IVSurfaceNode)::DownCast(m_node);
  //
  if ( surface_n.IsNull() || !surface_n->IsWellFormed() )
    return out;

  // Add sensitive Parameters
  out << surface_n->Parameter(asiData_IVSurfaceNode::PID_Geometry);

  return out;
}
