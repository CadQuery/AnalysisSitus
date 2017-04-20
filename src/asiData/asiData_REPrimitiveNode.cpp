//-----------------------------------------------------------------------------
// Created on: 20 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiData_REPrimitiveNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_REPrimitiveNode::asiData_REPrimitiveNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,  PID_Name);
  REGISTER_PARAMETER(Shape, PID_Geometry);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_REPrimitiveNode::Instance()
{
  return new asiData_REPrimitiveNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
//! \param primitive [in] B-Rep primitive to store.
void asiData_REPrimitiveNode::Init(const TopoDS_Shape& primitive)
{
  this->InitParameter(PID_Name, "Name");
  //
  this->SetShape(primitive);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_REPrimitiveNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param name [in] name to set.
void asiData_REPrimitiveNode::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------
// Convenience methods
//-----------------------------------------------------------------------------

//! Sets primitive to store.
//! \param primitive [in] primitive to store.
void asiData_REPrimitiveNode::SetShape(const TopoDS_Shape& primitive)
{
  ActParamTool::AsShape( this->Parameter(PID_Geometry) )->SetShape(primitive);
}

//! \return stored primitive.
TopoDS_Shape asiData_REPrimitiveNode::GetShape() const
{
  return ActParamTool::AsShape( this->Parameter(PID_Geometry) )->GetShape();
}
