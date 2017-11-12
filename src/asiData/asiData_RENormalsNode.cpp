//-----------------------------------------------------------------------------
// Created on: 06 December 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sergey Slyadnev nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

//! Returns normal vector by a given zero-based index.
//! \param zeroBasedIndex [in] zero-based index.
//! \return normal vector.
gp_Vec asiData_RENormalsNode::GetNormal(const int zeroBasedIndex) const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Normals) )->GetArray();

  const int coordIdx = zeroBasedIndex*3;

  return gp_Vec( coords->Value(coordIdx + 0),
                 coords->Value(coordIdx + 1),
                 coords->Value(coordIdx + 2) );
}

//! \return stored normals in form of base cloud.
Handle(asiAlgo_BaseCloud<double>) asiData_RENormalsNode::GetNormals() const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Normals) )->GetArray();

  if ( coords.IsNull() )
    return NULL;

  // TODO: get rid of copying here
  return asiAlgo_PointCloudUtils::AsPointCloud(coords);
}

//! \return stored normals with float coordinates.
Handle(asiAlgo_BaseCloud<float>) asiData_RENormalsNode::GetNormalsf() const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Normals) )->GetArray();

  if ( coords.IsNull() )
    return NULL;

  // TODO: get rid of copying here
  return asiAlgo_PointCloudUtils::AsPointCloudf(coords);
}

//! Stores normals.
//! \param normals [in] normals to store.
void asiData_RENormalsNode::SetNormals(const Handle(asiAlgo_BaseCloud<double>)& normals)
{
  // TODO: get rid of copying here
  Handle(TColStd_HArray1OfReal)
    coords = normals.IsNull() ? NULL : asiAlgo_PointCloudUtils::AsRealArray(normals);

  // Store
  ActParamTool::AsRealArray( this->Parameter(PID_Normals) )->SetArray(coords);
}

//! Stores normals with float coordinates.
//! \param normals [in] normals to store.
void asiData_RENormalsNode::SetNormalsf(const Handle(asiAlgo_BaseCloud<float>)& normals)
{
  // TODO: get rid of copying here
  Handle(TColStd_HArray1OfReal)
    coords = normals.IsNull() ? NULL : asiAlgo_PointCloudUtils::AsRealArray(normals);

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
