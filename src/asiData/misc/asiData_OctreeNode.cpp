//-----------------------------------------------------------------------------
// Created on: 23 January 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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
#include <asiData_OctreeNode.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

asiData_OctreeNode::asiData_OctreeNode() : ActData_BaseNode()
{
  // Register standard Active Data Parameters.
  REGISTER_PARAMETER(Name,         PID_Name);
  REGISTER_PARAMETER(TreeFunction, PID_BuildFunc);
  //
  REGISTER_PARAMETER(Group,        PID_ResolutionGroup);
  REGISTER_PARAMETER(Real,         PID_MinCellSize);
  REGISTER_PARAMETER(Real,         PID_MaxCellSize);
  REGISTER_PARAMETER(Real,         PID_Precision);
  //
  REGISTER_PARAMETER(Group,        PID_DomainGroup);
  REGISTER_PARAMETER(Real,         PID_DomainMinX);
  REGISTER_PARAMETER(Real,         PID_DomainMinY);
  REGISTER_PARAMETER(Real,         PID_DomainMinZ);
  REGISTER_PARAMETER(Real,         PID_DomainMaxX);
  REGISTER_PARAMETER(Real,         PID_DomainMaxY);
  REGISTER_PARAMETER(Real,         PID_DomainMaxZ);
  REGISTER_PARAMETER(Bool,         PID_DomainIsCube);
  //
  REGISTER_PARAMETER(Group,        PID_OutputsGroup);
  REGISTER_PARAMETER(Int,          PID_SamplingStrategy);
  REGISTER_PARAMETER(Bool,         PID_ExtractPoints);
  REGISTER_PARAMETER(Real,         PID_MaxVectorSize);
  REGISTER_PARAMETER(Int,          PID_NumElements);

  // Register custom Parameters specific to Analysis Situs.
  this->registerParameter(PID_Octree, asiData_OctreeParameter::Instance(), false);
}

//-----------------------------------------------------------------------------

Handle(ActAPI_INode) asiData_OctreeNode::Instance()
{
  return new asiData_OctreeNode();
}

//-----------------------------------------------------------------------------

void asiData_OctreeNode::Init()
{
  // Set default values.
  this->SetOctree           (nullptr);
  this->SetMinCellSize      (1.);
  this->SetMaxCellSize      (100.);
  this->SetPrecision        (1.);
  this->SetDomainMinCorner  (0., 0., 0.);
  this->SetDomainMaxCorner  (0., 0., 0.);
  this->SetDomainCube       (false);
  this->SetSamplingStrategy (0x01 | 0x02 | 0x04);
  this->SetExtractPoints    (false);
  this->SetMaxVectorSize    (1.);
  this->SetNumElements      (0);

  // Initialize Parameter flags.
  this->InitParameter(PID_Name,             "Name",              "",                ParameterFlag_IsVisible, true);
  //
  this->InitParameter(PID_ResolutionGroup,  "Resolution",        "",                ParameterFlag_IsVisible, true);
  this->InitParameter(PID_MinCellSize,      "Min cell size",     "",                ParameterFlag_IsVisible, true);
  this->InitParameter(PID_MaxCellSize,      "Max cell size",     "",                ParameterFlag_IsVisible, true);
  this->InitParameter(PID_Precision,        "Precision",         "",                ParameterFlag_IsVisible, true);
  //
  this->InitParameter(PID_DomainGroup,      "Domain",            "",                ParameterFlag_IsVisible, true);
  this->InitParameter(PID_DomainMinX,       "Min X",             "",                ParameterFlag_IsVisible, true);
  this->InitParameter(PID_DomainMinY,       "Min Y",             "",                ParameterFlag_IsVisible, true);
  this->InitParameter(PID_DomainMinZ,       "Min Z",             "",                ParameterFlag_IsVisible, true);
  this->InitParameter(PID_DomainMaxX,       "Max X",             "",                ParameterFlag_IsVisible, true);
  this->InitParameter(PID_DomainMaxY,       "Max Y",             "",                ParameterFlag_IsVisible, true);
  this->InitParameter(PID_DomainMaxZ,       "Max Z",             "",                ParameterFlag_IsVisible, true);
  this->InitParameter(PID_DomainIsCube,     "Is cube",           "",                ParameterFlag_IsVisible, true);
  //
  this->InitParameter(PID_OutputsGroup,     "Outputs",           "",                ParameterFlag_IsVisible, true);
  this->InitParameter(PID_SamplingStrategy, "Sampling strategy", "Octree_SS",       ParameterFlag_IsVisible, true);
  this->InitParameter(PID_ExtractPoints,    "Extract points",    "",                ParameterFlag_IsVisible, true);
  this->InitParameter(PID_MaxVectorSize,    "Max norm modulus",  "",                ParameterFlag_IsVisible, true);
  this->InitParameter(PID_NumElements,      "Num. of elements",  "Octree_NumElems", ParameterFlag_IsVisible, true);
  
}

//-----------------------------------------------------------------------------

TCollection_ExtendedString asiData_OctreeNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//-----------------------------------------------------------------------------

void asiData_OctreeNode::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------

void* asiData_OctreeNode::GetOctree() const
{
  return Handle(asiData_OctreeParameter)::DownCast( this->Parameter(PID_Octree) )->GetOctree();
}

//-----------------------------------------------------------------------------

void asiData_OctreeNode::SetOctree(void* pOctree)
{
  Handle(asiData_OctreeParameter)::DownCast( this->Parameter(PID_Octree) )->SetOctree(pOctree);
}

//-----------------------------------------------------------------------------

int asiData_OctreeNode::GetSamplingStrategy() const
{
  return ActParamTool::AsInt( this->Parameter(PID_SamplingStrategy) )->GetValue();
}

//-----------------------------------------------------------------------------

void asiData_OctreeNode::SetSamplingStrategy(const int mode)
{
  ActParamTool::AsInt( this->Parameter(PID_SamplingStrategy) )->SetValue(mode);
}

//-----------------------------------------------------------------------------

bool asiData_OctreeNode::GetExtractPoints() const
{
  return ActParamTool::AsBool( this->Parameter(PID_ExtractPoints) )->GetValue();
}

//-----------------------------------------------------------------------------

void asiData_OctreeNode::SetExtractPoints(const bool toExtract)
{
  ActParamTool::AsBool( this->Parameter(PID_ExtractPoints) )->SetValue(toExtract);
}

//-----------------------------------------------------------------------------

double asiData_OctreeNode::GetMaxVectorSize() const
{
  return ActParamTool::AsReal( this->Parameter(PID_MaxVectorSize) )->GetValue();
}

//-----------------------------------------------------------------------------

void asiData_OctreeNode::SetMaxVectorSize(const double maxSize)
{
  ActParamTool::AsReal( this->Parameter(PID_MaxVectorSize) )->SetValue(maxSize);
}

//-----------------------------------------------------------------------------

int asiData_OctreeNode::GetNumElements() const
{
  return ActParamTool::AsInt( this->Parameter(PID_NumElements) )->GetValue();
}

//-----------------------------------------------------------------------------

void asiData_OctreeNode::SetNumElements(const int numElems)
{
  ActParamTool::AsInt( this->Parameter(PID_NumElements) )->SetValue(numElems);
}

//-----------------------------------------------------------------------------

void asiData_OctreeNode::GetDomainMinCorner(double& x,
                                            double& y,
                                            double& z) const
{
  x = ActParamTool::AsReal( this->Parameter(PID_DomainMinX) )->GetValue();
  y = ActParamTool::AsReal( this->Parameter(PID_DomainMinY) )->GetValue();
  z = ActParamTool::AsReal( this->Parameter(PID_DomainMinZ) )->GetValue();
}

//-----------------------------------------------------------------------------

void asiData_OctreeNode::SetDomainMinCorner(const double x,
                                            const double y,
                                            const double z)
{
  ActParamTool::AsReal( this->Parameter(PID_DomainMinX) )->SetValue(x);
  ActParamTool::AsReal( this->Parameter(PID_DomainMinY) )->SetValue(y);
  ActParamTool::AsReal( this->Parameter(PID_DomainMinZ) )->SetValue(z);
}

//-----------------------------------------------------------------------------

void asiData_OctreeNode::GetDomainMaxCorner(double& x,
                                            double& y,
                                            double& z) const
{
  x = ActParamTool::AsReal( this->Parameter(PID_DomainMaxX) )->GetValue();
  y = ActParamTool::AsReal( this->Parameter(PID_DomainMaxY) )->GetValue();
  z = ActParamTool::AsReal( this->Parameter(PID_DomainMaxZ) )->GetValue();
}

//-----------------------------------------------------------------------------

void asiData_OctreeNode::SetDomainMaxCorner(const double x,
                                            const double y,
                                            const double z)
{
  ActParamTool::AsReal( this->Parameter(PID_DomainMaxX) )->SetValue(x);
  ActParamTool::AsReal( this->Parameter(PID_DomainMaxY) )->SetValue(y);
  ActParamTool::AsReal( this->Parameter(PID_DomainMaxZ) )->SetValue(z);
}

//-----------------------------------------------------------------------------

bool asiData_OctreeNode::IsDomainCube() const
{
  return ActParamTool::AsBool( this->Parameter(PID_DomainIsCube) )->GetValue();
}

//-----------------------------------------------------------------------------

void asiData_OctreeNode::SetDomainCube(const bool isCube)
{
  ActParamTool::AsBool( this->Parameter(PID_DomainIsCube) )->SetValue(isCube);
}

//-----------------------------------------------------------------------------

double asiData_OctreeNode::GetMinCellSize() const
{
  return ActParamTool::AsReal( this->Parameter(PID_MinCellSize) )->GetValue();
}

//-----------------------------------------------------------------------------

void asiData_OctreeNode::SetMinCellSize(const double cellSize)
{
  return ActParamTool::AsReal( this->Parameter(PID_MinCellSize) )->SetValue(cellSize);
}

//-----------------------------------------------------------------------------

double asiData_OctreeNode::GetMaxCellSize() const
{
  return ActParamTool::AsReal( this->Parameter(PID_MaxCellSize) )->GetValue();
}

//-----------------------------------------------------------------------------

void asiData_OctreeNode::SetMaxCellSize(const double cellSize)
{
  return ActParamTool::AsReal( this->Parameter(PID_MaxCellSize) )->SetValue(cellSize);
}

//-----------------------------------------------------------------------------

double asiData_OctreeNode::GetPrecision() const
{
  return ActParamTool::AsReal( this->Parameter(PID_Precision) )->GetValue();
}

//-----------------------------------------------------------------------------

void asiData_OctreeNode::SetPrecision(const double value)
{
  return ActParamTool::AsReal( this->Parameter(PID_Precision) )->SetValue(value);
}
