//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_RESurfacesDataProvider.h>

// Common includes
#include <common_facilities.h>

//-----------------------------------------------------------------------------

//! Constructor.
asiVisu_RESurfacesDataProvider::asiVisu_RESurfacesDataProvider()
: asiVisu_DataProvider()
{}

//-----------------------------------------------------------------------------

//! \return ID of the associated Data Node.
ActAPI_DataObjectId asiVisu_RESurfacesDataProvider::GetNodeID() const
{
  Handle(asiData_RESurfacesNode)
    surfaces_n = common_facilities::Instance()->Model->GetRENode()->Surfaces();
  //
  if ( surfaces_n.IsNull() || !surfaces_n->IsWellFormed() )
    return ActAPI_DataObjectId();

  return surfaces_n->GetId();
}

//-----------------------------------------------------------------------------

//! \return number of stored surfaces.
int asiVisu_RESurfacesDataProvider::GetNumOfSurfaces() const
{
  Handle(asiData_RESurfacesNode)
    surfaces_n = common_facilities::Instance()->Model->GetRENode()->Surfaces();
  //
  if ( surfaces_n.IsNull() || !surfaces_n->IsWellFormed() )
    return 0;

  // Iterate over the child Nodes which are the individual surfaces
  int nbSurfaces = 0;
  for ( Handle(ActAPI_IChildIterator) cit = surfaces_n->GetChildIterator(); cit->More(); cit->Next() )
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
  Handle(asiData_RESurfacesNode)
    surfaces_n = common_facilities::Instance()->Model->GetRENode()->Surfaces();
  //
  if ( surfaces_n.IsNull() || !surfaces_n->IsWellFormed() )
    return NULL;

  // Access individual surface
  Handle(asiData_RESurfaceNode) surface_n = surfaces_n->Surface(oneBased_idx);
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

  Handle(asiData_RESurfacesNode)
    surfaces_n = common_facilities::Instance()->Model->GetRENode()->Surfaces();
  //
  if ( surfaces_n.IsNull() || !surfaces_n->IsWellFormed() )
    return out;

  // Iterate over the child Nodes which are the individual surfaces
  for ( Handle(ActAPI_IChildIterator) cit = surfaces_n->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(asiData_RESurfaceNode) surface_n = Handle(asiData_RESurfaceNode)::DownCast( cit->Value() );
    //
    if ( surface_n.IsNull() || !surface_n->IsWellFormed() )
      continue;

    out << surface_n->Parameter(asiData_RESurfaceNode::PID_Surface);
  }

  return out;
}
