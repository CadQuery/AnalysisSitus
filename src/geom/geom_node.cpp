//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <geom_node.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
geom_node::geom_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,  PID_Name);
  REGISTER_PARAMETER(Shape, PID_Geometry);
  REGISTER_PARAMETER(Int,   PID_DisplayMode);
  REGISTER_PARAMETER(Bool,  PID_HasColor);
  REGISTER_PARAMETER(Int,   PID_Color);
}

//! Returns new DETACHED instance of Geometry Node ensuring its correct
//! allocation in a heap.
//! \return new instance of Geometry Node.
Handle(ActAPI_INode) geom_node::Instance()
{
  return new geom_node();
}

//! Performs initial actions required to make Geometry Node WELL-FORMED.
void geom_node::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");

  // Set empty initial shape
  ActParamTool::AsShape( this->Parameter(PID_Geometry) )->SetShape(TopoDS_Shape(), MT_Silent);

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
TCollection_ExtendedString geom_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void geom_node::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! Sets shape to store.
//! \param shape [in] shape to store.
void geom_node::SetShape(const TopoDS_Shape& shape)
{
  ActParamTool::AsShape( this->Parameter(PID_Geometry) )->SetShape(shape);
}

//! \return stored shape.
TopoDS_Shape geom_node::GetShape() const
{
  return ActParamTool::AsShape( this->Parameter(PID_Geometry) )->GetShape();
}

//! Sets the Boolean value indicating whether the color Parameter of this
//! Data Node is in force.
//! \param hasColor [in] value to set.
void geom_node::SetHasColor(const bool hasColor)
{
  ActParamTool::AsBool( this->Parameter(PID_HasColor) )->SetValue(hasColor);
}

//! Accessor for the value of the Boolean Parameter indicating whether the
//! Color Parameter of this Data Node is in force.
//! \return true/false.
bool geom_node::HasColor() const
{
  return ActParamTool::AsBool( this->Parameter(PID_HasColor) )->GetValue() > 0;
}

//! Sets color.
//! \param color [in] color to set.
void geom_node::SetColor(const int color)
{
  ActParamTool::AsInt( this->Parameter(PID_Color) )->SetValue(color);
}

//! Accessor for the stored color value.
//! \return color value.
int geom_node::GetColor() const
{
  return ActParamTool::AsInt( this->Parameter(PID_Color) )->GetValue();
}

//! Sets display mode.
//! \param mode [in] display mode value to set.
void geom_node::SetDisplayMode(const int mode)
{
  ActParamTool::AsInt( this->Parameter(PID_DisplayMode) )->SetValue(mode);
}

//! Accessor for the stored display mode value.
//! \return display mode value.
int geom_node::GetDisplayMode() const
{
  return ActParamTool::AsInt( this->Parameter(PID_DisplayMode) )->GetValue();
}

//-----------------------------------------------------------------------------

//! \return underlying face representation Node.
Handle(geom_face_node) geom_node::FaceRepresentation()
{
  Handle(geom_face_node) face_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    face_n = Handle(geom_face_node)::DownCast( cit->Value() );

    if ( !face_n.IsNull() && face_n->IsWellFormed() )
      return face_n;
  }

  return NULL;
}

//! \return underlying surface representation Node.
Handle(geom_surf_node) geom_node::SurfaceRepresentation()
{
  Handle(geom_surf_node) surf_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    surf_n = Handle(geom_surf_node)::DownCast( cit->Value() );

    if ( !surf_n.IsNull() && surf_n->IsWellFormed() )
      return surf_n;
  }

  return NULL;
}
