//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
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
