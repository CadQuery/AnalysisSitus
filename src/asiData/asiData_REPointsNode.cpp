//-----------------------------------------------------------------------------
// Created on: 06 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
Handle(asiAlgo_BaseCloud<double>) asiData_REPointsNode::GetPoints() const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Points) )->GetArray();

  if ( coords.IsNull() )
    return NULL;

  // TODO: get rid of copying here
  return asiAlgo_PointCloudUtils::AsPointCloud(coords);
}

//! \return stored point cloud with float coordinates.
Handle(asiAlgo_BaseCloud<float>) asiData_REPointsNode::GetPointsf() const
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
void asiData_REPointsNode::SetPoints(const Handle(asiAlgo_BaseCloud<double>)& points)
{
  // TODO: get rid of copying here
  Handle(TColStd_HArray1OfReal)
    coords = points.IsNull() ? NULL : asiAlgo_PointCloudUtils::AsRealArray(points);

  // Store
  ActParamTool::AsRealArray( this->Parameter(PID_Points) )->SetArray(coords);
}
