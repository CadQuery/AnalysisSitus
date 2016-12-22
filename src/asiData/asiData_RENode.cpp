//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiData_RENode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_RENode::asiData_RENode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name, PID_Name);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_RENode::Instance()
{
  return new asiData_RENode();
}

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_RENode::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_RENode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void asiData_RENode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! \return underlying surfaces.
Handle(asiData_RESurfacesNode) asiData_RENode::GetSurfaces()
{
  Handle(asiData_RESurfacesNode)
    surf_n = Handle(asiData_RESurfacesNode)::DownCast( this->GetChildNode(Child_Surfaces) );
  //
  if ( !surf_n.IsNull() && surf_n->IsWellFormed() )
    return surf_n;
  //
  return NULL;
}

//! \return underlying contours.
Handle(asiData_REContoursNode) asiData_RENode::GetContours()
{
  Handle(asiData_REContoursNode)
    cont_n = Handle(asiData_REContoursNode)::DownCast( this->GetChildNode(Child_Contours) );
  //
  if ( !cont_n.IsNull() && cont_n->IsWellFormed() )
    return cont_n;
  //
  return NULL;
}

//! \return underlying points.
Handle(asiData_REPointsNode) asiData_RENode::GetPoints()
{
  Handle(asiData_REPointsNode)
    points_n = Handle(asiData_REPointsNode)::DownCast( this->GetChildNode(Child_Points) );
  //
  if ( !points_n.IsNull() && points_n->IsWellFormed() )
    return points_n;
  //
  return NULL;
}

//! \return underlying normals.
Handle(asiData_RENormalsNode) asiData_RENode::GetNormals()
{
  Handle(asiData_RENormalsNode)
    normal_n = Handle(asiData_RENormalsNode)::DownCast( this->GetChildNode(Child_Normals) );
  //
  if ( !normal_n.IsNull() && normal_n->IsWellFormed() )
    return normal_n;
  //
  return NULL;
}
