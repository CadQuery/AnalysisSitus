//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <tess_node.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// Mesh (Active Data) includes
#include <Mesh_Node.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
tess_node::tess_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name, PID_Name);
  REGISTER_PARAMETER(Mesh, PID_Mesh);
  REGISTER_PARAMETER(Int,  PID_DisplayMode);
  REGISTER_PARAMETER(Bool, PID_HasColor);
  REGISTER_PARAMETER(Int,  PID_Color);
}

//! Returns new DETACHED instance of Mesh Node ensuring its correct
//! allocation in a heap.
//! \return new instance of Mesh Node.
Handle(ActAPI_INode) tess_node::Instance()
{
  return new tess_node();
}

//! Performs initial actions required to make Mesh Node WELL-FORMED.
void tess_node::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");

  // Set empty initial mesh
  ActParamTool::AsMesh( this->Parameter(PID_Mesh) )->SetMesh(new Mesh, MT_Silent);

  // Set Presentation values
  this->SetHasColor(false);
  this->SetColor(-1); // No color
  this->SetDisplayMode(1); // No display mode
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString tess_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void tess_node::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! \return stored tessellation.
Handle(Mesh) tess_node::GetMesh() const
{
  return ActParamTool::AsMesh( this->Parameter(PID_Mesh) )->GetMesh();
}

//! Sets tessellation to store.
//! \param mesh [in] tessellation to store.
void tess_node::SetMesh(const Handle(Mesh)& mesh)
{
  ActParamTool::AsMesh( this->Parameter(PID_Mesh) )->SetMesh(mesh);
}

//! Sets the Boolean value indicating whether the color Parameter of this
//! Data Node is in force.
//! \param hasColor [in] value to set.
void tess_node::SetHasColor(const bool hasColor) const
{
  ActParamTool::AsBool( this->Parameter(PID_HasColor) )->SetValue(hasColor);
}

//! Accessor for the value of the Boolean Parameter indicating whether the
//! Color Parameter of this Data Node is in force.
//! \return true/false.
bool tess_node::HasColor() const
{
  return ActParamTool::AsBool( this->Parameter(PID_HasColor) )->GetValue();
}

//! Sets color.
//! \param theColor [in] color to set.
void tess_node::SetColor(const int theColor) const
{
  ActParamTool::AsInt( this->Parameter(PID_Color) )->SetValue(theColor);
}

//! Accessor for the stored color value.
//! \return color value.
int tess_node::GetColor() const
{
  return ActParamTool::AsInt( this->Parameter(PID_Color) )->GetValue();
}

//! Sets display mode.
//! \param theMode [in] display mode value to set.
void tess_node::SetDisplayMode(const int theMode) const
{
  ActParamTool::AsInt( this->Parameter(PID_DisplayMode) )->SetValue(theMode);
}

//! Accessor for the stored display mode value.
//! \return display mode value.
int tess_node::GetDisplayMode() const
{
  return ActParamTool::AsInt( this->Parameter(PID_DisplayMode) )->GetValue();
}
