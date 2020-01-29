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
  REGISTER_PARAMETER(Name, PID_Name);
  REGISTER_PARAMETER(Bool, PID_IsBoundary);

  // Register custom Parameters specific to Analysis Situs.
  this->registerParameter(PID_Octree, asiData_OctreeParameter ::Instance(), false);
}

//-----------------------------------------------------------------------------

Handle(ActAPI_INode) asiData_OctreeNode::Instance()
{
  return new asiData_OctreeNode();
}

//-----------------------------------------------------------------------------

void asiData_OctreeNode::Init()
{
  this->SetOctree(nullptr);

  // Set default values to primitive Parameters.
  this->SetIsBoundary(false);

  // Initialize Parameter flags.
  this->InitParameter(PID_Name,       "Name",                 "", ParameterFlag_IsVisible, true);
  this->InitParameter(PID_IsBoundary, "Boundary voxels only", "", ParameterFlag_IsVisible, true);
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

bool asiData_OctreeNode::IsBoundary() const
{
  return ActParamTool::AsBool( this->Parameter(PID_IsBoundary) )->GetValue();
}

//-----------------------------------------------------------------------------

void asiData_OctreeNode::SetIsBoundary(const bool isBoundary)
{
  ActParamTool::AsBool( this->Parameter(PID_IsBoundary) )->SetValue(isBoundary);
}
