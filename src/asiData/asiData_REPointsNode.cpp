//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiData_REPointsNode.h>

// asiAlgo includes
#include <asiAlgo_PointCloudUtils.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_REPointsNode::asiData_REPointsNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,      PID_Name);
  REGISTER_PARAMETER(RealArray, PID_Points);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_REPointsNode::Instance()
{
  return new asiData_REPointsNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_REPointsNode::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
  //
  ActParamTool::AsRealArray( this->Parameter(PID_Points) )->SetArray(NULL);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_REPointsNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void asiData_REPointsNode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy API
//-----------------------------------------------------------------------------

//! Accessor for a point by its zero-based index.
//! \param zeroBasedIndex [in] 0-based index of a point to access.
//! \return point.
gp_Pnt asiData_REPointsNode::GetPoint(const int zeroBasedIndex) const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Points) )->GetArray();

  const int coordIdx = zeroBasedIndex*3;

  return gp_Pnt( coords->Value(coordIdx + 0),
                 coords->Value(coordIdx + 1),
                 coords->Value(coordIdx + 2) );
}

//! \return stored point cloud.
Handle(asiAlgo_PointCloud<double>) asiData_REPointsNode::GetPoints() const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Points) )->GetArray();

  if ( coords.IsNull() )
    return NULL;

  // TODO: get rid of copying here
  return asiAlgo_PointCloudUtils::AsPointCloud(coords);
}

//! \return stored point cloud with float coordinates.
Handle(asiAlgo_PointCloud<float>) asiData_REPointsNode::GetPointsf() const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Points) )->GetArray();

  if ( coords.IsNull() )
    return NULL;

  // TODO: get rid of copying here
  return asiAlgo_PointCloudUtils::AsPointCloudf(coords);
}

//! Stores point cloud.
//! \param points [in] point cloud to store.
void asiData_REPointsNode::SetPoints(const Handle(asiAlgo_PointCloud<double>)& points)
{
  // TODO: get rid of copying here
  Handle(TColStd_HArray1OfReal)
    coords = points.IsNull() ? NULL : asiAlgo_PointCloudUtils::AsRealArray(points);

  // Store
  ActParamTool::AsRealArray( this->Parameter(PID_Points) )->SetArray(coords);
}

//! \return underlying normals Node.
Handle(asiData_RENormalsNode) asiData_REPointsNode::GetNormals()
{
  Handle(asiData_RENormalsNode)
    normal_n = Handle(asiData_RENormalsNode)::DownCast( this->GetChildNode(Child_Normals) );
  //
  if ( !normal_n.IsNull() && normal_n->IsWellFormed() )
    return normal_n;
  //
  return NULL;
}

//! \return underlying segments Node.
Handle(asiData_RESegmentsNode) asiData_REPointsNode::GetSegments()
{
  Handle(asiData_RESegmentsNode)
    segments_n = Handle(asiData_RESegmentsNode)::DownCast( this->GetChildNode(Child_Segments) );
  //
  if ( !segments_n.IsNull() && segments_n->IsWellFormed() )
    return segments_n;
  //
  return NULL;
}
