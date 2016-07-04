//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_re_node.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
geom_re_node::geom_re_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name, PID_Name);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) geom_re_node::Instance()
{
  return new geom_re_node();
}

//! Performs initial actions required to make Node WELL-FORMED.
void geom_re_node::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString geom_re_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void geom_re_node::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! \return underlying surfaces.
Handle(geom_re_surfaces_node) geom_re_node::Surfaces()
{
  Handle(geom_re_surfaces_node)
    surf_n = Handle(geom_re_surfaces_node)::DownCast( this->GetChildNode(Child_Surfaces) );
  //
  if ( !surf_n.IsNull() && surf_n->IsWellFormed() )
    return surf_n;
  //
  return NULL;
}

//! \return underlying contours.
Handle(geom_re_contours_node) geom_re_node::Contours()
{
  Handle(geom_re_contours_node)
    cont_n = Handle(geom_re_contours_node)::DownCast( this->GetChildNode(Child_Contours) );
  //
  if ( !cont_n.IsNull() && cont_n->IsWellFormed() )
    return cont_n;
  //
  return NULL;
}

//! \return underlying points.
Handle(geom_re_points_node) geom_re_node::Points()
{
  Handle(geom_re_points_node)
    points_n = Handle(geom_re_points_node)::DownCast( this->GetChildNode(Child_Points) );
  //
  if ( !points_n.IsNull() && points_n->IsWellFormed() )
    return points_n;
  //
  return NULL;
}
