//-----------------------------------------------------------------------------
// Created on: 02 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <geom_ubend_node.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
geom_ubend_node::geom_ubend_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,  PID_Name);
  REGISTER_PARAMETER(Shape, PID_Path);
  REGISTER_PARAMETER(Shape, PID_Sections);
  REGISTER_PARAMETER(Shape, PID_Skin);
  REGISTER_PARAMETER(Int,   PID_DisplayMode);
  REGISTER_PARAMETER(Bool,  PID_HasColor);
  REGISTER_PARAMETER(Int,   PID_Color);
}

//! Returns new DETACHED instance of U-bend Node ensuring its correct
//! allocation in a heap.
//! \return new instance of Node.
Handle(ActAPI_INode) geom_ubend_node::Instance()
{
  return new geom_ubend_node();
}

//! Performs initial actions required to make Node WELL-FORMED.
void geom_ubend_node::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");

  // Set empty initial shapes
  ActParamTool::AsShape( this->Parameter(PID_Path) )     ->SetShape(TopoDS_Shape(), MT_Silent);
  ActParamTool::AsShape( this->Parameter(PID_Sections) ) ->SetShape(TopoDS_Shape(), MT_Silent);
  ActParamTool::AsShape( this->Parameter(PID_Skin) )     ->SetShape(TopoDS_Shape(), MT_Silent);

  // Set default values to primitive Parameters
  this->SetHasColor    (false);
  this->SetColor       (-1);
  this->SetDisplayMode (1);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString geom_ubend_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void geom_ubend_node::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! Sets path shape to store.
//! \param shape [in] shape to store.
void geom_ubend_node::SetPath(const TopoDS_Shape& path)
{
  ActParamTool::AsShape( this->Parameter(PID_Path) )->SetShape(path);
}

//! \return stored path shape.
TopoDS_Shape geom_ubend_node::GetPath() const
{
  return ActParamTool::AsShape( this->Parameter(PID_Path) )->GetShape();
}

//! Sets sections to store.
//! \param shape [in] shape to store.
void geom_ubend_node::SetSections(const TopoDS_Shape& path)
{
  ActParamTool::AsShape( this->Parameter(PID_Sections) )->SetShape(path);
}

//! \return stored sections shape.
TopoDS_Shape geom_ubend_node::GetSections() const
{
  return ActParamTool::AsShape( this->Parameter(PID_Sections) )->GetShape();
}

//! Sets skin shape to store.
//! \param shape [in] shape to store.
void geom_ubend_node::SetSkin(const TopoDS_Shape& shape)
{
  ActParamTool::AsShape( this->Parameter(PID_Skin) )->SetShape(shape);
}

//! \return stored skin shape.
TopoDS_Shape geom_ubend_node::GetSkin() const
{
  return ActParamTool::AsShape( this->Parameter(PID_Skin) )->GetShape();
}

//! Sets the Boolean value indicating whether the color Parameter of this
//! Data Node is in force.
//! \param hasColor [in] value to set.
void geom_ubend_node::SetHasColor(const bool hasColor)
{
  ActParamTool::AsBool( this->Parameter(PID_HasColor) )->SetValue(hasColor);
}

//! Accessor for the value of the Boolean Parameter indicating whether the
//! Color Parameter of this Data Node is in force.
//! \return true/false.
bool geom_ubend_node::HasColor() const
{
  return ActParamTool::AsBool( this->Parameter(PID_HasColor) )->GetValue() > 0;
}

//! Sets color.
//! \param color [in] color to set.
void geom_ubend_node::SetColor(const int color)
{
  ActParamTool::AsInt( this->Parameter(PID_Color) )->SetValue(color);
}

//! Accessor for the stored color value.
//! \return color value.
int geom_ubend_node::GetColor() const
{
  return ActParamTool::AsInt( this->Parameter(PID_Color) )->GetValue();
}

//! Sets display mode.
//! \param mode [in] display mode value to set.
void geom_ubend_node::SetDisplayMode(const int mode)
{
  ActParamTool::AsInt( this->Parameter(PID_DisplayMode) )->SetValue(mode);
}

//! Accessor for the stored display mode value.
//! \return display mode value.
int geom_ubend_node::GetDisplayMode() const
{
  return ActParamTool::AsInt( this->Parameter(PID_DisplayMode) )->GetValue();
}
