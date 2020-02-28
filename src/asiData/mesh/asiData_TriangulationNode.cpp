//-----------------------------------------------------------------------------
// Created on: 10 July 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017-present, Sergey Slyadnev
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
#include <asiData_TriangulationNode.h>

// asiData includes
#include <asiData_BVHParameter.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_TriangulationNode::asiData_TriangulationNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,          PID_Name);
  REGISTER_PARAMETER(Triangulation, PID_Triangulation);
  REGISTER_PARAMETER(RealArray,     PID_Options);
  REGISTER_PARAMETER(Group,         PID_GroupPrs);
  REGISTER_PARAMETER(Int,           PID_DisplayMode);
  REGISTER_PARAMETER(Bool,          PID_UseScalars);
  REGISTER_PARAMETER(Int,           PID_Color);
  REGISTER_PARAMETER(Bool,          PID_HasVertices);

  // Non-standard Parameters.
  this->registerParameter(PID_BVH, asiData_BVHParameter::Instance(), false);
}

//! Returns new DETACHED instance of Mesh Node ensuring its correct
//! allocation in a heap.
//! \return new instance of Triangulation Node.
Handle(ActAPI_INode) asiData_TriangulationNode::Instance()
{
  return new asiData_TriangulationNode();
}

//! Performs initial actions required to make the Node WELL-FORMED.
void asiData_TriangulationNode::Init()
{
  // Initialize name Parameter.
  this->InitParameter(PID_Name, "Name");

  // Set default values.
  this->SetOptions     (nullptr);
  this->SetBVH         (nullptr);
  this->SetUseScalars  (true);
  this->SetColor       (190 << 16 | 190 << 8 | 190); // Initial color.
  this->SetDisplayMode (1);
  this->SetHasVertices (false);

  // Initialize Parameter flags.
  this->InitParameter(PID_GroupPrs,    "Presentation",  "",                   ParameterFlag_IsVisible, true);
  this->InitParameter(PID_DisplayMode, "Display mode",  "PrsMeshDisplayMode", 0,                       true);
  this->InitParameter(PID_UseScalars,  "Use scalars",   "",                   ParameterFlag_IsVisible, true);
  this->InitParameter(PID_Color,       "Color",         "PrsCustomColor",     ParameterFlag_IsVisible, true);
  this->InitParameter(PID_HasVertices, "Show vertices", "",                   ParameterFlag_IsVisible, true);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_TriangulationNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param name [in] name to set.
void asiData_TriangulationNode::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! \return triangulation parameter.
Handle(ActData_TriangulationParameter)
  asiData_TriangulationNode::GetTriangulationParam() const
{
  return ActParamTool::AsTriangulation( this->Parameter(PID_Triangulation) );
}

//! \return stored tessellation.
Handle(Poly_Triangulation) asiData_TriangulationNode::GetTriangulation() const
{
  return ActParamTool::AsTriangulation( this->Parameter(PID_Triangulation) )->GetTriangulation();
}

//! Sets tessellation to store.
//! \param triangulation [in] tessellation to store.
void asiData_TriangulationNode::SetTriangulation(const Handle(Poly_Triangulation)& triangulation)
{
  ActParamTool::AsTriangulation( this->Parameter(PID_Triangulation) )->SetTriangulation(triangulation);
}

//! \return stored BVH.
Handle(asiAlgo_BVHFacets) asiData_TriangulationNode::GetBVH() const
{
  return Handle(asiData_BVHParameter)::DownCast( this->Parameter(PID_BVH) )->GetBVH();
}

//! Sets BVH to store.
//! \param bvh [in] BVH to store.
void asiData_TriangulationNode::SetBVH(const Handle(asiAlgo_BVHFacets)& bvh)
{
  Handle(asiData_BVHParameter)::DownCast( this->Parameter(PID_BVH) )->SetBVH(bvh);
}

//! Returns triangulation options.
//! \return domain-specific triangulation options.
Handle(TColStd_HArray1OfReal) asiData_TriangulationNode::GetOptions() const
{
  return ActParamTool::AsRealArray( this->Parameter(PID_Options) )->GetArray();
}

//! Sets domain-specific triangulation options.
//! \param options [in] triangulation options to set.
void asiData_TriangulationNode::SetOptions(const Handle(TColStd_HArray1OfReal)& options)
{
  ActParamTool::AsRealArray( this->Parameter(PID_Options) )->SetArray(options);
}

//! Sets the Boolean value indicating whether the scalars are to be used for
//! coloring the Part's Presentation.
//! \param on [in] value to set.
void asiData_TriangulationNode::SetUseScalars(const bool on)
{
  ActParamTool::AsBool( this->Parameter(PID_UseScalars) )->SetValue(on);
}

//! Accessor for the value of the Boolean Parameter indicating whether the
//! scalars are active.
//! \return true/false.
bool asiData_TriangulationNode::GetUseScalars() const
{
  return ActParamTool::AsBool( this->Parameter(PID_UseScalars) )->GetValue();
}

//! Sets color.
//! \param theColor [in] color to set.
void asiData_TriangulationNode::SetColor(const int theColor) const
{
  ActParamTool::AsInt( this->Parameter(PID_Color) )->SetValue(theColor);
}

//! Accessor for the stored color value.
//! \return color value.
int asiData_TriangulationNode::GetColor() const
{
  return ActParamTool::AsInt( this->Parameter(PID_Color) )->GetValue();
}

//! Sets display mode.
//! \param theMode [in] display mode value to set.
void asiData_TriangulationNode::SetDisplayMode(const int theMode) const
{
  ActParamTool::AsInt( this->Parameter(PID_DisplayMode) )->SetValue(theMode);
}

//! Accessor for the stored display mode value.
//! \return display mode value.
int asiData_TriangulationNode::GetDisplayMode() const
{
  return ActParamTool::AsInt( this->Parameter(PID_DisplayMode) )->GetValue();
}

//! Sets the Boolean value indicating whether the vertices are active
//! \param hasVertices [in] value to set.
void asiData_TriangulationNode::SetHasVertices(const bool hasVertices)
{
  ActParamTool::AsBool( this->Parameter(PID_HasVertices) )->SetValue(hasVertices);
}

//! Accessor for the value of the Boolean Parameter indicating whether the
//! vertices are active.
//! \return true/false.
bool asiData_TriangulationNode::HasVertices() const
{
  return ActParamTool::AsBool( this->Parameter(PID_HasVertices) )->GetValue();
}

//! \return underlying Node which stores octree.
Handle(asiData_OctreeNode) asiData_TriangulationNode::GetOctree() const
{
  Handle(asiData_OctreeNode) octree_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    octree_n = Handle(asiData_OctreeNode)::DownCast( cit->Value() );

    if ( !octree_n.IsNull() && octree_n->IsWellFormed() )
      return octree_n;
  }

  return nullptr;
}
