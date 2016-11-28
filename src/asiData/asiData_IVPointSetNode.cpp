//-----------------------------------------------------------------------------
// Created on: 08 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiData_IVPointSetNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_IVPointSetNode::asiData_IVPointSetNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,      PID_Name);
  REGISTER_PARAMETER(RealArray, PID_Geometry);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_IVPointSetNode::Instance()
{
  return new asiData_IVPointSetNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_IVPointSetNode::Init()
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
TCollection_ExtendedString asiData_IVPointSetNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void asiData_IVPointSetNode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy API
//-----------------------------------------------------------------------------

//! \return stored point cloud.
Handle(asiAlgo_PointCloud<double>) asiData_IVPointSetNode::GetPoints() const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Geometry) )->GetArray();
  //
  return asiAlgo_PointCloud<double>::AsPointCloud(coords);
}

//! Sets point cloud to store.
//! \param points [in] points to store.
void asiData_IVPointSetNode::SetPoints(const Handle(asiAlgo_PointCloud<double>)& points)
{
  Handle(TColStd_HArray1OfReal) arr = asiAlgo_PointCloud<double>::AsRealArray(points);
  //
  ActParamTool::AsRealArray( this->Parameter(PID_Geometry) )->SetArray( points.IsNull() ? NULL : arr );
}
