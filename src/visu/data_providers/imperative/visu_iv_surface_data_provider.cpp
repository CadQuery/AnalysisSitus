//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_iv_surface_data_provider.h>

// Visualization includes
#include <visu_iv_surface_node.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param N [in] source Node.
visu_iv_surface_data_provider::visu_iv_surface_data_provider(const Handle(ActAPI_INode)& N)
: visu_data_provider(),
  m_node(N)
{}

//-----------------------------------------------------------------------------

//! \return ID of the associated Data Node.
ActAPI_DataObjectId visu_iv_surface_data_provider::GetNodeID() const
{
  return m_node->GetId();
}

//-----------------------------------------------------------------------------

// Accessor for the parametric surface.
//! \param uLimit [out] absolute bound for U curvilinear axis.
//! \param vLimit [out] absolute bound for V curvilinear axis.
//! \return surface.
Handle(Geom_Surface)
  visu_iv_surface_data_provider::GetSurface(double& uLimit, double& vLimit) const
{
  Handle(visu_iv_surface_node)
    surface_n = Handle(visu_iv_surface_node)::DownCast(m_node);
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
Handle(ActAPI_HParameterList) visu_iv_surface_data_provider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  Handle(visu_iv_surface_node)
    surface_n = Handle(visu_iv_surface_node)::DownCast(m_node);
  //
  if ( surface_n.IsNull() || !surface_n->IsWellFormed() )
    return out;

  // Add sensitive Parameters
  out << surface_n->Parameter(visu_iv_surface_node::PID_Geometry);

  return out;
}
