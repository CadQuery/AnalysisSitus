//-----------------------------------------------------------------------------
// Created on: 05 April 2016
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
#include <asiVisu_RESurfacesDataProvider.h>

// asiData includes
#include <asiData_RESurfaceNode.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param surfaces_n [in] Surfaces Node.
asiVisu_RESurfacesDataProvider::asiVisu_RESurfacesDataProvider(const Handle(asiData_RESurfacesNode)& surfaces_n)
: asiVisu_DataProvider(), m_surfaces(surfaces_n)
{}

//-----------------------------------------------------------------------------

//! \return ID of the associated Data Node.
ActAPI_DataObjectId asiVisu_RESurfacesDataProvider::GetNodeID() const
{
  if ( m_surfaces.IsNull() || !m_surfaces->IsWellFormed() )
    return ActAPI_DataObjectId();

  return m_surfaces->GetId();
}

//-----------------------------------------------------------------------------

//! \return number of stored surfaces.
int asiVisu_RESurfacesDataProvider::GetNumOfSurfaces() const
{
  if ( m_surfaces.IsNull() || !m_surfaces->IsWellFormed() )
    return 0;

  // Iterate over the child Nodes which are the individual surfaces
  int nbSurfaces = 0;
  for ( Handle(ActAPI_IChildIterator) cit = m_surfaces->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(asiData_RESurfaceNode) surface_n = Handle(asiData_RESurfaceNode)::DownCast( cit->Value() );
    //
    if ( surface_n.IsNull() || !surface_n->IsWellFormed() )
      continue;

    ++nbSurfaces;
  }

  return nbSurfaces;
}

//! Returns surface by its one-based index.
//! \param oneBased_idx [in] 1-based index of the surface to access.
//! \param uLimit       [out] U limit.
//! \param vLimit       [out] V limit.
//! \return surface.
Handle(Geom_Surface)
  asiVisu_RESurfacesDataProvider::GetSurface(const int oneBased_idx,
                                             double&   uLimit,
                                             double&   vLimit) const
{
  if ( m_surfaces.IsNull() || !m_surfaces->IsWellFormed() )
    return NULL;

  // Access individual surface
  Handle(asiData_RESurfaceNode)
    surface_n = Handle(asiData_RESurfaceNode)::DownCast( m_surfaces->GetChildNode(oneBased_idx) );
  //
  if ( surface_n.IsNull() || !surface_n->IsWellFormed() )
    return NULL;

  // Access limits and geometry
  surface_n->GetLimits(uLimit, vLimit);
  //
  return surface_n->GetSurface();
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_RESurfacesDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  if ( m_surfaces.IsNull() || !m_surfaces->IsWellFormed() )
    return out;

  // Iterate over the child Nodes which are the individual surfaces
  for ( Handle(ActAPI_IChildIterator) cit = m_surfaces->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(asiData_RESurfaceNode) surface_n = Handle(asiData_RESurfaceNode)::DownCast( cit->Value() );
    //
    if ( surface_n.IsNull() || !surface_n->IsWellFormed() )
      continue;

    out << surface_n->Parameter(asiData_RESurfaceNode::PID_Surface);
  }

  return out;
}
