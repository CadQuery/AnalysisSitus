//-----------------------------------------------------------------------------
// Created on: 06 October 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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
//    * Neither the name of the copyright holder(s) nor the
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
#include <asiData_ReVertexNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default ctor. Registers all involved Parameters.
asiData_ReVertexNode::asiData_ReVertexNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,      PID_Name);
  REGISTER_PARAMETER(RealArray, PID_Geometry); // Array of position coordinates.
  REGISTER_PARAMETER(RealArray, PID_Norm);     // Array of normal coordinates.
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_ReVertexNode::Instance()
{
  return new asiData_ReVertexNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
//! \param[in] x  X coordinate.
//! \param[in] y  Y coordinate.
//! \param[in] z  Z coordinate.
//! \param[in] nx X coordinate of a normal vector.
//! \param[in] ny Y coordinate of a normal vector.
//! \param[in] nz Z coordinate of a normal vector.
void asiData_ReVertexNode::Init(const double x,
                                const double y,
                                const double z,
                                const double nx,
                                const double ny,
                                const double nz)
{
  // Initialize name Parameter.
  this->InitParameter(PID_Name, "Name");

  // Initialize arrays.
  ActParamTool::AsRealArray( this->Parameter(PID_Geometry) )->SetArray(nullptr);
  ActParamTool::AsRealArray( this->Parameter(PID_Norm) )->SetArray(nullptr);

  // Initialize coordinates.
  this->SetPoint(x, y, z);
  this->SetNormal(nx, ny, nz);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_ReVertexNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param[in] name name to set.
void asiData_ReVertexNode::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! Sets point coordinates.
//! \param[in] x X coordinate.
//! \param[in] y Y coordinate.
//! \param[in] z Z coordinate.
void asiData_ReVertexNode::SetPoint(const double x, const double y, const double z)
{
  Handle(HRealArray)
    arr = ActParamTool::AsRealArray( this->Parameter(PID_Geometry) )->GetArray();
  //
  if ( arr.IsNull() )
  {
    arr = new HRealArray(0, 2);
  }

  arr->ChangeValue(0) = x;
  arr->ChangeValue(1) = y;
  arr->ChangeValue(2) = z;

  ActParamTool::AsRealArray( this->Parameter(PID_Geometry) )->SetArray(arr);
}

//! Returns point coordinates.
//! \param[out] x X coordinate.
//! \param[out] y Y coordinate.
//! \param[out] z Z coordinate.
//! \return false if coordinates cannot be accessed.
bool asiData_ReVertexNode::GetPoint(double& x, double& y, double& z) const
{
  Handle(HRealArray)
    arr = ActParamTool::AsRealArray( this->Parameter(PID_Geometry) )->GetArray();
  //
  if ( arr.IsNull() || arr->Lower() != 0 || arr->Upper() != 2 )
    return false;

  x = arr->Value(0);
  y = arr->Value(1);
  z = arr->Value(2);

  return true;
}

//! \return point without any checks on the status of data container.
gp_XYZ asiData_ReVertexNode::GetPoint() const
{
  double x, y, z;
  this->GetPoint(x, y, z);

  return gp_XYZ(x, y, z);
}

//! Sets normal coordinates.
//! \param[in] x X coordinate.
//! \param[in] y Y coordinate.
//! \param[in] z Z coordinate.
void asiData_ReVertexNode::SetNormal(const double x, const double y, const double z)
{
  Handle(HRealArray)
    arr = ActParamTool::AsRealArray( this->Parameter(PID_Norm) )->GetArray();
  //
  if ( arr.IsNull() )
  {
    arr = new HRealArray(0, 2);
  }

  arr->ChangeValue(0) = x;
  arr->ChangeValue(1) = y;
  arr->ChangeValue(2) = z;

  ActParamTool::AsRealArray( this->Parameter(PID_Norm) )->SetArray(arr);
}

//! Returns normal coordinates.
//! \param[out] x X coordinate.
//! \param[out] y Y coordinate.
//! \param[out] z Z coordinate.
//! \return false if coordinates cannot be accessed.
bool asiData_ReVertexNode::GetNormal(double& x, double& y, double& z) const
{
  Handle(HRealArray)
    arr = ActParamTool::AsRealArray( this->Parameter(PID_Norm) )->GetArray();
  //
  if ( arr.IsNull() || arr->Lower() != 0 || arr->Upper() != 2 )
    return false;

  x = arr->Value(0);
  y = arr->Value(1);
  z = arr->Value(2);

  return true;
}

//! \return normal vector without any checks on the status of data container.
gp_Vec asiData_ReVertexNode::GetNormal() const
{
  double x, y, z;
  this->GetNormal(x, y, z);

  return gp_Vec(x, y, z);
}
