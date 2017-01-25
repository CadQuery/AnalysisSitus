//-----------------------------------------------------------------------------
// Created on: 06 December 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiData_RENormalsNode.h>

// asiAlgo includes
#include <asiAlgo_PointCloudUtils.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_RENormalsNode::asiData_RENormalsNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,      PID_Name);
  REGISTER_PARAMETER(RealArray, PID_Normals);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_RENormalsNode::Instance()
{
  return new asiData_RENormalsNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_RENormalsNode::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
  //
  ActParamTool::AsRealArray( this->Parameter(PID_Normals) )->SetArray(NULL);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_RENormalsNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void asiData_RENormalsNode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy API
//-----------------------------------------------------------------------------

//! \return stored normals in form of point cloud.
Handle(asiAlgo_PointCloud<double>) asiData_RENormalsNode::GetNormals() const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Normals) )->GetArray();

  // TODO: get rid of copying here
  return asiAlgo_PointCloudUtils::AsPointCloud(coords);
}

//! \return stored normals with float coordinates.
Handle(asiAlgo_PointCloud<float>) asiData_RENormalsNode::GetNormalsf() const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Normals) )->GetArray();

  // TODO: get rid of copying here
  return asiAlgo_PointCloudUtils::AsPointCloudf(coords);
}

//! Stores normals.
//! \param points [in] normals to store.
void asiData_RENormalsNode::SetNormals(const Handle(asiAlgo_PointCloud<double>)& points)
{
  // TODO: get rid of copying here
  Handle(TColStd_HArray1OfReal)
    coords = points.IsNull() ? NULL : asiAlgo_PointCloudUtils::AsRealArray(points);

  // Store
  ActParamTool::AsRealArray( this->Parameter(PID_Normals) )->SetArray(coords);
}

//! Stores normals with float coordinates.
//! \param points [in] normals to store.
void asiData_RENormalsNode::SetNormalsf(const Handle(asiAlgo_PointCloud<float>)& points)
{
  // TODO: get rid of copying here
  Handle(TColStd_HArray1OfReal)
    coords = points.IsNull() ? NULL : asiAlgo_PointCloudUtils::AsRealArray(points);

  // Store
  ActParamTool::AsRealArray( this->Parameter(PID_Normals) )->SetArray(coords);
}

//! \return true if the stored normal field is empty, false -- otherwise.
bool asiData_RENormalsNode::IsEmpty() const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Normals) )->GetArray();

  return coords.IsNull();
}
