//-----------------------------------------------------------------------------
// Created on: 08 April 2016
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
#include <asiData_IVSurfaceNode.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_IVSurfaceNode::asiData_IVSurfaceNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,  PID_Name);
  REGISTER_PARAMETER(Int,   PID_SurfaceType);
  REGISTER_PARAMETER(Shape, PID_Geometry);
  REGISTER_PARAMETER(Group, PID_GroupBounds);
  REGISTER_PARAMETER(Real,  PID_UMin);
  REGISTER_PARAMETER(Real,  PID_UMax);
  REGISTER_PARAMETER(Real,  PID_VMin);
  REGISTER_PARAMETER(Real,  PID_VMax);
  REGISTER_PARAMETER(Group, PID_GroupPrs);
  REGISTER_PARAMETER(Bool,  PID_HasColor);
  REGISTER_PARAMETER(Int,   PID_Color);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_IVSurfaceNode::Instance()
{
  return new asiData_IVSurfaceNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_IVSurfaceNode::Init()
{
  // Set default values.
  this->SetSurfaceType (SurfaceType_General);
  this->SetSurface     (NULL);
  this->SetLimits      (0.0, 0.0, 0.0, 0.0);
  this->SetHasColor    (false);
  this->SetColor       (2500134); // Sort of dark color.

  // Initialize properties.
  this->InitParameter (PID_Name, "Name", "", ParameterFlag_IsVisible, true);
  //
  this->InitParameter (PID_GroupBounds, "Bounds", "", ParameterFlag_IsVisible, true);
  this->InitParameter (PID_UMin,        "U min",  "", ParameterFlag_IsVisible, true);
  this->InitParameter (PID_UMax,        "U max",  "", ParameterFlag_IsVisible, true);
  this->InitParameter (PID_VMin,        "V min",  "", ParameterFlag_IsVisible, true);
  this->InitParameter (PID_VMax,        "V max",  "", ParameterFlag_IsVisible, true);
  //
  this->InitParameter (PID_GroupPrs, "Presentation", "",               ParameterFlag_IsVisible, true);
  this->InitParameter (PID_HasColor, "Colorized",    "",               ParameterFlag_IsVisible, true);
  this->InitParameter (PID_Color,    "Color",        "PrsCustomColor", ParameterFlag_IsVisible, true);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_IVSurfaceNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void asiData_IVSurfaceNode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! \return stored surface type.
asiData_IVSurfaceNode::SurfaceType
  asiData_IVSurfaceNode::GetSurfaceType() const
{
  return (SurfaceType) ActParamTool::AsInt( this->Parameter(PID_SurfaceType) )->GetValue();
}

//! Sets surface type to store.
//! \param[in] type surface type to store.
void asiData_IVSurfaceNode::SetSurfaceType(const SurfaceType type) const
{
  ActParamTool::AsInt( this->Parameter(PID_SurfaceType) )->SetValue(type);
}

//! \return stored geometry.
Handle(Geom_Surface) asiData_IVSurfaceNode::GetSurface() const
{
  TopoDS_Shape sh = ActParamTool::AsShape( this->Parameter(PID_Geometry) )->GetShape();
  //
  if ( sh.IsNull() || sh.ShapeType() != TopAbs_FACE )
    return NULL;

  // Extract face and its host geometry
  const TopoDS_Face& F = TopoDS::Face(sh);
  //
  return BRep_Tool::Surface(F);
}

//! Sets surface to store.
//! \param[in] surface geometry to store.
void asiData_IVSurfaceNode::SetSurface(const Handle(Geom_Surface)& surface)
{
  // Create a fictive face to take advantage of topology Parameter of Active Data.
  TopoDS_Face F;
  if ( !surface.IsNull() )
    F = BRepBuilderAPI_MakeFace( surface, Precision::Confusion() );

  // Store surface.
  ActParamTool::AsShape( this->Parameter(PID_Geometry) )->SetShape(F);

  // Store surface type.
  if ( !surface.IsNull() && surface->IsInstance( STANDARD_TYPE(Geom_Plane) ) )
    this->SetSurfaceType(SurfaceType_Plane);
}

//! Sets parametric bounds for infinite surfaces.
//! \param[in] uMin min limit by U curvilinear axis.
//! \param[in] uMax max limit by U curvilinear axis.
//! \param[in] vMin min limit by V curvilinear axis.
//! \param[in] vMax max limit by V curvilinear axis.
void asiData_IVSurfaceNode::SetLimits(const double uMin, const double uMax,
                                      const double vMin, const double vMax)
{
  ActParamTool::AsReal( this->Parameter(PID_UMin) )->SetValue(uMin);
  ActParamTool::AsReal( this->Parameter(PID_UMax) )->SetValue(uMax);
  ActParamTool::AsReal( this->Parameter(PID_VMin) )->SetValue(vMin);
  ActParamTool::AsReal( this->Parameter(PID_VMax) )->SetValue(vMax);
}

//! Gets parametric bounds used for infinite surfaces.
//! \param[out] uMin min limit by U curvilinear axis.
//! \param[out] uMax max limit by U curvilinear axis.
//! \param[out] vMin min limit by V curvilinear axis.
//! \param[out] vMax max limit by V curvilinear axis.
void asiData_IVSurfaceNode::GetLimits(double& uMin, double& uMax,
                                      double& vMin, double& vMax) const
{
  uMin = ActParamTool::AsReal( this->Parameter(PID_UMin) )->GetValue();
  uMax = ActParamTool::AsReal( this->Parameter(PID_UMax) )->GetValue();
  vMin = ActParamTool::AsReal( this->Parameter(PID_VMin) )->GetValue();
  vMax = ActParamTool::AsReal( this->Parameter(PID_VMax) )->GetValue();
}


//! Sets the Boolean value indicating whether the color Parameter of this
//! Data Node is in force.
//! \param[in] hasColor value to set.
void asiData_IVSurfaceNode::SetHasColor(const bool hasColor)
{
  ActParamTool::AsBool( this->Parameter(PID_HasColor) )->SetValue(hasColor);
}

//! Accessor for the value of the Boolean Parameter indicating whether the
//! Color Parameter of this Data Node is in force.
//! \return true/false.
bool asiData_IVSurfaceNode::HasColor() const
{
  return ActParamTool::AsBool( this->Parameter(PID_HasColor) )->GetValue();
}

//! Sets color.
//! \param[in] color color to set.
void asiData_IVSurfaceNode::SetColor(const int color)
{
  ActParamTool::AsInt( this->Parameter(PID_Color) )->SetValue(color);
}

//! Accessor for the stored color value.
//! \return color value.
int asiData_IVSurfaceNode::GetColor() const
{
  return ActParamTool::AsInt( this->Parameter(PID_Color) )->GetValue();
}
