//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_part_node.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
geom_part_node::geom_part_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,  PID_Name);
  REGISTER_PARAMETER(Shape, PID_Geometry);
  REGISTER_PARAMETER(Int,   PID_DisplayMode);
  REGISTER_PARAMETER(Bool,  PID_HasColor);
  REGISTER_PARAMETER(Int,   PID_Color);
  REGISTER_PARAMETER(Bool,  PID_HasVertices);
}

//! Returns new DETACHED instance of Geometry Node ensuring its correct
//! allocation in a heap.
//! \return new instance of Geometry Node.
Handle(ActAPI_INode) geom_part_node::Instance()
{
  return new geom_part_node();
}

//! Performs initial actions required to make Geometry Node WELL-FORMED.
void geom_part_node::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");

  // Set empty initial shape
  ActParamTool::AsShape( this->Parameter(PID_Geometry) )->SetShape( TopoDS_Shape(), MT_Silent );

  // Set default values to primitive Parameters
  this->SetHasColor    (false);
  this->SetColor       (-1);
  this->SetDisplayMode (1);
  this->SetHasVertices (false);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString geom_part_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param name [in] name to set.
void geom_part_node::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! Sets shape to store.
//! \param shape [in] shape to store.
void geom_part_node::SetShape(const TopoDS_Shape& shape)
{
  ActParamTool::AsShape( this->Parameter(PID_Geometry) )->SetShape(shape);
}

//! \return stored shape.
TopoDS_Shape geom_part_node::GetShape() const
{
  return ActParamTool::AsShape( this->Parameter(PID_Geometry) )->GetShape();
}

//! Sets the Boolean value indicating whether the color Parameter of this
//! Data Node is in force.
//! \param hasColor [in] value to set.
void geom_part_node::SetHasColor(const bool hasColor)
{
  ActParamTool::AsBool( this->Parameter(PID_HasColor) )->SetValue(hasColor);
}

//! Accessor for the value of the Boolean Parameter indicating whether the
//! Color Parameter of this Data Node is in force.
//! \return true/false.
bool geom_part_node::HasColor() const
{
  return ActParamTool::AsBool( this->Parameter(PID_HasColor) )->GetValue() > 0;
}

//! Sets color.
//! \param color [in] color to set.
void geom_part_node::SetColor(const int color)
{
  ActParamTool::AsInt( this->Parameter(PID_Color) )->SetValue(color);
}

//! Accessor for the stored color value.
//! \return color value.
int geom_part_node::GetColor() const
{
  return ActParamTool::AsInt( this->Parameter(PID_Color) )->GetValue();
}

//! Sets display mode.
//! \param mode [in] display mode value to set.
void geom_part_node::SetDisplayMode(const int mode)
{
  ActParamTool::AsInt( this->Parameter(PID_DisplayMode) )->SetValue(mode);
}

//! Accessor for the stored display mode value.
//! \return display mode value.
int geom_part_node::GetDisplayMode() const
{
  return ActParamTool::AsInt( this->Parameter(PID_DisplayMode) )->GetValue();
}

//! Sets the Boolean value indicating whether the vertices are active
//! \param hasVertices [in] value to set.
void geom_part_node::SetHasVertices(const bool hasVertices)
{
  ActParamTool::AsBool( this->Parameter(PID_HasVertices) )->SetValue(hasVertices);
}

//! Accessor for the value of the Boolean Parameter indicating whether the
//! vertices are active.
//! \return true/false.
bool geom_part_node::HasVertices() const
{
  return ActParamTool::AsBool( this->Parameter(PID_HasVertices) )->GetValue() > 0;
}

//-----------------------------------------------------------------------------

//! \return underlying face representation Node.
Handle(geom_face_node) geom_part_node::GetFaceRepresentation() const
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
Handle(geom_surf_node) geom_part_node::GetSurfaceRepresentation() const
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

//! \return underlying boundary edges representation Node.
Handle(geom_boundary_edges_node) geom_part_node::GetBoundaryEdgesRepresentation() const
{
  Handle(geom_boundary_edges_node) edges_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    edges_n = Handle(geom_boundary_edges_node)::DownCast( cit->Value() );

    if ( !edges_n.IsNull() && edges_n->IsWellFormed() )
      return edges_n;
  }

  return NULL;
}

//! \return underlying edge representation Node.
Handle(geom_edge_node) geom_part_node::GetEdgeRepresentation() const
{
  Handle(geom_edge_node) edge_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    edge_n = Handle(geom_edge_node)::DownCast( cit->Value() );

    if ( !edge_n.IsNull() && edge_n->IsWellFormed() )
      return edge_n;
  }

  return NULL;
}

//! \return underlying curve representation Node.
Handle(geom_curve_node) geom_part_node::GetCurveRepresentation() const
{
  Handle(geom_curve_node) curve_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    curve_n = Handle(geom_curve_node)::DownCast( cit->Value() );

    if ( !curve_n.IsNull() && curve_n->IsWellFormed() )
      return curve_n;
  }

  return NULL;
}
