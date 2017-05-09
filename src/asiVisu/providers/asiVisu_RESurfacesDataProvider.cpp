//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
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
