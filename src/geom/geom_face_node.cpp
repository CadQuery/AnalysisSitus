//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <geom_face_node.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
geom_face_node::geom_face_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name, PID_Name);
  REGISTER_PARAMETER(Int,  PID_SelectedFace);
}

//! Returns new DETACHED instance of Geometry Face Node ensuring its correct
//! allocation in a heap.
//! \return new instance of Geometry Face Node.
Handle(ActAPI_INode) geom_face_node::Instance()
{
  return new geom_face_node();
}

//! Performs initial actions required to make Geometry Face Node WELL-FORMED.
void geom_face_node::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");

  // Set default values to primitive Parameters
  this->SetSelectedFace(0);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString geom_face_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void geom_face_node::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! Sets index of the active face.
//! \param faceId [in] index of the active face.
void geom_face_node::SetSelectedFace(const int faceId)
{
  ActParamTool::AsInt( this->Parameter(PID_SelectedFace) )->SetValue(faceId);
}

//! \return index of the selected face.
int geom_face_node::GetSelectedFace() const
{
  return ActParamTool::AsInt( this->Parameter(PID_SelectedFace) )->GetValue();
}
