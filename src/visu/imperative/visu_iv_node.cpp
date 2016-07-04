//-----------------------------------------------------------------------------
// Created on: 08 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_iv_node.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
visu_iv_node::visu_iv_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name, PID_Name);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) visu_iv_node::Instance()
{
  return new visu_iv_node();
}

//! Performs initial actions required to make Node WELL-FORMED.
void visu_iv_node::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString visu_iv_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void visu_iv_node::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! \return underlying 2D points.
Handle(visu_iv_points_2d_node) visu_iv_node::Points2d()
{
  Handle(visu_iv_points_2d_node)
    points_n = Handle(visu_iv_points_2d_node)::DownCast( this->GetChildNode(Child_Points2d) );
  //
  if ( !points_n.IsNull() && points_n->IsWellFormed() )
    return points_n;
  //
  return NULL;
}

//! \return underlying points.
Handle(visu_iv_points_node) visu_iv_node::Points()
{
  Handle(visu_iv_points_node)
    points_n = Handle(visu_iv_points_node)::DownCast( this->GetChildNode(Child_Points) );
  //
  if ( !points_n.IsNull() && points_n->IsWellFormed() )
    return points_n;
  //
  return NULL;
}

//! \return underlying curves.
Handle(visu_iv_curves_node) visu_iv_node::Curves()
{
  Handle(visu_iv_curves_node)
    curves_n = Handle(visu_iv_curves_node)::DownCast( this->GetChildNode(Child_Curves) );
  //
  if ( !curves_n.IsNull() && curves_n->IsWellFormed() )
    return curves_n;
  //
  return NULL;
}

//! \return underlying surfaces.
Handle(visu_iv_surfaces_node) visu_iv_node::Surfaces()
{
  Handle(visu_iv_surfaces_node)
    surf_n = Handle(visu_iv_surfaces_node)::DownCast( this->GetChildNode(Child_Surfaces) );
  //
  if ( !surf_n.IsNull() && surf_n->IsWellFormed() )
    return surf_n;
  //
  return NULL;
}

//! \return underlying topologies.
Handle(visu_iv_topo_node) visu_iv_node::Topology()
{
  Handle(visu_iv_topo_node)
    topo_n = Handle(visu_iv_topo_node)::DownCast( this->GetChildNode(Child_Topology) );
  //
  if ( !topo_n.IsNull() && topo_n->IsWellFormed() )
    return topo_n;
  //
  return NULL;
}

//! \return underlying tessellations.
Handle(visu_iv_tess_node) visu_iv_node::Tessellation()
{
  Handle(visu_iv_tess_node)
    tess_n = Handle(visu_iv_tess_node)::DownCast( this->GetChildNode(Child_Tessellation) );
  //
  if ( !tess_n.IsNull() && tess_n->IsWellFormed() )
    return tess_n;
  //
  return NULL;
}
