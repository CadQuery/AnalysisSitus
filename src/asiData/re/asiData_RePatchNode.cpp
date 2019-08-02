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
#include <asiData_RePatchNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

//! Default ctor. Registers all involved Parameters.
asiData_RePatchNode::asiData_RePatchNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,         PID_Name);
  REGISTER_PARAMETER(Group,        PID_GeometryGroup);
  REGISTER_PARAMETER(Shape,        PID_Geometry);
  REGISTER_PARAMETER(Int,          PID_MinNumKnots);
  REGISTER_PARAMETER(Bool,         PID_ApproxNodes);
  REGISTER_PARAMETER(Real,         PID_ApproxLambda);
  REGISTER_PARAMETER(Group,        PID_GroupPrs);
  REGISTER_PARAMETER(Bool,         PID_HasColor);
  REGISTER_PARAMETER(Int,          PID_Color);
  REGISTER_PARAMETER(Bool,         PID_EnablePlotter);
  REGISTER_PARAMETER(TreeFunction, PID_FuncBuildSurf);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_RePatchNode::Instance()
{
  return new asiData_RePatchNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_RePatchNode::Init()
{
  // Set default values.
  this->SetSurface       (NULL);
  this->SetMinNumKnots   (2);
  this->SetApproxNodes   (false);
  this->SetApproxLambda  (1e-3);
  this->SetHasColor      (false);
  this->SetColor         (2500134); // Sort of dark color.
  this->SetEnablePlotter (false); // Imperative plotter is disabled by default.

  // Initialize Parameters.
  this->InitParameter (PID_Name,          "Name");
  this->InitParameter (PID_GeometryGroup, "Geometry",            "",               ParameterFlag_IsVisible, true);
  this->InitParameter (PID_MinNumKnots,   "Min. inner knots",    "",               ParameterFlag_IsVisible, true);
  this->InitParameter (PID_ApproxNodes,   "Approx. mesh nodes",  "",               ParameterFlag_IsVisible, true);
  this->InitParameter (PID_ApproxLambda,  "Fairing coefficient", "",               ParameterFlag_IsVisible, true);
  this->InitParameter (PID_GroupPrs,      "Presentation",        "",               ParameterFlag_IsVisible, true);
  this->InitParameter (PID_HasColor,      "Colorized",           "",               ParameterFlag_IsVisible, true);
  this->InitParameter (PID_Color,         "Color",               "PrsCustomColor", ParameterFlag_IsVisible, true);
  this->InitParameter (PID_EnablePlotter, "Visual diagnostics",  "",               ParameterFlag_IsVisible, true);
}

//-----------------------------------------------------------------------------
// Getters/setters
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_RePatchNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param[in] name name to set.
void asiData_RePatchNode::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//! \return stored geometry.
Handle(Geom_Surface) asiData_RePatchNode::GetSurface() const
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
void asiData_RePatchNode::SetSurface(const Handle(Geom_Surface)& surface)
{
  // Create a fictive face to take advantage of topology Parameter of Active Data.
  TopoDS_Face F;
  if ( !surface.IsNull() )
    F = BRepBuilderAPI_MakeFace( surface, Precision::Confusion() );

  // Store surface.
  ActParamTool::AsShape( this->Parameter(PID_Geometry) )->SetShape(F);
}

//! \return stored min number of intermediate knots.
int asiData_RePatchNode::GetMinNumKnots() const
{
  return ActParamTool::AsInt( this->Parameter(PID_MinNumKnots) )->GetValue();
}

//! Sets min number of intermediate knots to use.
//! \param[in] numKnots number to store.
void asiData_RePatchNode::SetMinNumKnots(const int numKnots)
{
  ActParamTool::AsInt( this->Parameter(PID_MinNumKnots) )->SetValue(numKnots);
}

//! \return Boolean value indicating if mesh approximation mode is enabled.
bool asiData_RePatchNode::GetApproxNodes() const
{
  return ActParamTool::AsBool( this->Parameter(PID_ApproxNodes) )->GetValue();
}

//! Sets Boolean value indicating if mesh approximation mode is enabled.
//! \param[in] on true/false.
void asiData_RePatchNode::SetApproxNodes(const bool on)
{
  ActParamTool::AsBool( this->Parameter(PID_ApproxNodes) )->SetValue(on);
}

//! \return fairing coefficient for mesh approximation.
double asiData_RePatchNode::GetApproxLambda() const
{
  return ActParamTool::AsReal( this->Parameter(PID_ApproxLambda) )->GetValue();
}

//! Sets fairing coefficient for mesh approximation.
//! \param[in] lambda value to set.
void asiData_RePatchNode::SetApproxLambda(const double lambda)
{
  ActParamTool::AsReal( this->Parameter(PID_ApproxLambda) )->SetValue(lambda);
}

//! Sets the Boolean value indicating whether the color Parameter of this
//! Data Node is in force.
//! \param[in] hasColor value to set.
void asiData_RePatchNode::SetHasColor(const bool hasColor)
{
  ActParamTool::AsBool( this->Parameter(PID_HasColor) )->SetValue(hasColor);
}

//! Accessor for the value of the Boolean Parameter indicating whether the
//! Color Parameter of this Data Node is in force.
//! \return true/false.
bool asiData_RePatchNode::HasColor() const
{
  return ActParamTool::AsBool( this->Parameter(PID_HasColor) )->GetValue();
}

//! Sets color.
//! \param[in] color color to set.
void asiData_RePatchNode::SetColor(const int color)
{
  ActParamTool::AsInt( this->Parameter(PID_Color) )->SetValue(color);
}

//! Accessor for the stored color value.
//! \return color value.
int asiData_RePatchNode::GetColor() const
{
  return ActParamTool::AsInt( this->Parameter(PID_Color) )->GetValue();
}

//! Enables imperative plotter for visual diagnostics of tree function.
//! \param[in] on value to set.
void asiData_RePatchNode::SetEnablePlotter(const bool on)
{
  ActParamTool::AsBool( this->Parameter(PID_EnablePlotter) )->SetValue(on);
}

//! \return Boolean flag indicating whether the imperative plotter is
//!         activated or not.
bool asiData_RePatchNode::HasEnabledPlotter() const
{
  return ActParamTool::AsBool( this->Parameter(PID_EnablePlotter) )->GetValue();
}
