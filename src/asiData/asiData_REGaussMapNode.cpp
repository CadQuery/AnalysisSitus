//-----------------------------------------------------------------------------
// Created on: 25 April 2017
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
#include <asiData_REGaussMapNode.h>

// asiAlgo includes
#include <asiAlgo_PointCloudUtils.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_REGaussMapNode::asiData_REGaussMapNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,      PID_Name);
  REGISTER_PARAMETER(RealArray, PID_Vectors);
  REGISTER_PARAMETER(Real,      PID_AxisX);
  REGISTER_PARAMETER(Real,      PID_AxisY);
  REGISTER_PARAMETER(Real,      PID_AxisZ);
  REGISTER_PARAMETER(Real,      PID_RefinedAxisX);
  REGISTER_PARAMETER(Real,      PID_RefinedAxisY);
  REGISTER_PARAMETER(Real,      PID_RefinedAxisZ);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_REGaussMapNode::Instance()
{
  return new asiData_REGaussMapNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_REGaussMapNode::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
  //
  ActParamTool::AsRealArray( this->Parameter(PID_Vectors) )->SetArray(NULL);

  // Set coordinates of the central axis
  this->SetCentralAxis(0.0, 0.0, 0.0);

  // Set coordinates of the refined axis
  this->SetRefinedAxis(0.0, 0.0, 0.0);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_REGaussMapNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param name [in] name to set.
void asiData_REGaussMapNode::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------
// Handy API
//-----------------------------------------------------------------------------

//! Returns vector by a given zero-based index.
//! \param zeroBasedIndex [in] zero-based index.
//! \return normal vector.
gp_Vec asiData_REGaussMapNode::GetVector(const int zeroBasedIndex) const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Vectors) )->GetArray();

  const int coordIdx = zeroBasedIndex*3;

  return gp_Vec( coords->Value(coordIdx + 0),
                 coords->Value(coordIdx + 1),
                 coords->Value(coordIdx + 2) );
}

//! \return stored vectors in form of base cloud.
Handle(asiAlgo_BaseCloud<double>) asiData_REGaussMapNode::GetVectors() const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Vectors) )->GetArray();

  if ( coords.IsNull() )
    return NULL;

  // TODO: get rid of copying here
  return asiAlgo_PointCloudUtils::AsPointCloud(coords);
}

//! \return stored vectors with float coordinates.
Handle(asiAlgo_BaseCloud<float>) asiData_REGaussMapNode::GetVectorsf() const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Vectors) )->GetArray();

  if ( coords.IsNull() )
    return NULL;

  // TODO: get rid of copying here
  return asiAlgo_PointCloudUtils::AsPointCloudf(coords);
}

//! Stores vectors.
//! \param vectors [in] vectors to store.
void asiData_REGaussMapNode::SetVectors(const Handle(asiAlgo_BaseCloud<double>)& vectors)
{
  // TODO: get rid of copying here
  Handle(TColStd_HArray1OfReal)
    coords = vectors.IsNull() ? NULL : asiAlgo_PointCloudUtils::AsRealArray(vectors);

  // Store
  ActParamTool::AsRealArray( this->Parameter(PID_Vectors) )->SetArray(coords);
}

//! Stores vectors with float coordinates.
//! \param vectors [in] normals to store.
void asiData_REGaussMapNode::SetVectorsf(const Handle(asiAlgo_BaseCloud<float>)& vectors)
{
  // TODO: get rid of copying here
  Handle(TColStd_HArray1OfReal)
    coords = vectors.IsNull() ? NULL : asiAlgo_PointCloudUtils::AsRealArray(vectors);

  // Store
  ActParamTool::AsRealArray( this->Parameter(PID_Vectors) )->SetArray(coords);
}

//! \return true if the stored vector field is empty, false -- otherwise.
bool asiData_REGaussMapNode::IsEmpty() const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Vectors) )->GetArray();

  return coords.IsNull();
}

//! Sets central axis in a Gauss map.
//! \param x [in] x coordinate.
//! \param y [in] y coordinate.
//! \param z [in] z coordinate.
void asiData_REGaussMapNode::SetCentralAxis(const double x, const double y, const double z)
{
  ActParamTool::AsReal( this->Parameter(PID_AxisX) )->SetValue(x);
  ActParamTool::AsReal( this->Parameter(PID_AxisY) )->SetValue(y);
  ActParamTool::AsReal( this->Parameter(PID_AxisZ) )->SetValue(z);
}

//! Returns coordinates of the central axis.
//! \param x [out] x coordinate.
//! \param y [out] y coordinate.
//! \param z [out] z coordinate.
void asiData_REGaussMapNode::GetCentralAxis(double& x, double& y, double& z) const
{
  x = ActParamTool::AsReal( this->Parameter(PID_AxisX) )->GetValue();
  y = ActParamTool::AsReal( this->Parameter(PID_AxisY) )->GetValue();
  z = ActParamTool::AsReal( this->Parameter(PID_AxisZ) )->GetValue();
}

//! Sets refined axis in a Gauss map.
//! \param x [in] x coordinate.
//! \param y [in] y coordinate.
//! \param z [in] z coordinate.
void asiData_REGaussMapNode::SetRefinedAxis(const double x, const double y, const double z)
{
  ActParamTool::AsReal( this->Parameter(PID_RefinedAxisX) )->SetValue(x);
  ActParamTool::AsReal( this->Parameter(PID_RefinedAxisY) )->SetValue(y);
  ActParamTool::AsReal( this->Parameter(PID_RefinedAxisZ) )->SetValue(z);
}

//! Returns coordinates of the refined axis.
//! \param x [out] x coordinate.
//! \param y [out] y coordinate.
//! \param z [out] z coordinate.
void asiData_REGaussMapNode::GetRefinedAxis(double& x, double& y, double& z) const
{
  x = ActParamTool::AsReal( this->Parameter(PID_RefinedAxisX) )->GetValue();
  y = ActParamTool::AsReal( this->Parameter(PID_RefinedAxisY) )->GetValue();
  z = ActParamTool::AsReal( this->Parameter(PID_RefinedAxisZ) )->GetValue();
}
