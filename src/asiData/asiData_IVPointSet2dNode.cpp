//-----------------------------------------------------------------------------
// Created on: 16 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiData_IVPointSet2dNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_IVPointSet2dNode::asiData_IVPointSet2dNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,      PID_Name);
  REGISTER_PARAMETER(RealArray, PID_Geometry);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_IVPointSet2dNode::Instance()
{
  return new asiData_IVPointSet2dNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_IVPointSet2dNode::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
  //
  this->SetPoints(NULL);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_IVPointSet2dNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void asiData_IVPointSet2dNode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy API
//-----------------------------------------------------------------------------

//! \return stored point cloud.
Handle(TColStd_HArray1OfReal) asiData_IVPointSet2dNode::GetPoints() const
{
  return ActParamTool::AsRealArray( this->Parameter(PID_Geometry) )->GetArray();
}

//! Sets point cloud to store.
//! \param points [in] points to store.
void asiData_IVPointSet2dNode::SetPoints(const Handle(TColStd_HArray1OfReal)& points)
{
  ActParamTool::AsRealArray( this->Parameter(PID_Geometry) )->SetArray(points);
}
