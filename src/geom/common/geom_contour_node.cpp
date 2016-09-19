//-----------------------------------------------------------------------------
// Created on: 19 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_contour_node.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
geom_contour_node::geom_contour_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name, PID_Name);
  REGISTER_PARAMETER(Int,  PID_Coords);
  REGISTER_PARAMETER(Bool, PID_IsClosed);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) geom_contour_node::Instance()
{
  return new geom_contour_node();
}

//! Performs initial actions required to make the Node WELL-FORMED.
void geom_contour_node::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");

  // Set default values to primitive Parameters
  this->SetCoords(NULL);
  this->SetClosed(false);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString geom_contour_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void geom_contour_node::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

void geom_contour_node::SetCoords(const Handle(HRealArray)& coords)
{
}

Handle(HRealArray) geom_contour_node::GetCoords() const
{
}

//! Adds another point to the contour.
//! \param[in] point point to add.
void geom_contour_node::AddPoint(const gp_XYZ& point)
{
}

//! Sets closeness property for the contour.
//! \param[in] isClosed indicates whether the contour is
//!            closed or not.
void geom_contour_node::SetClosed(const bool isClosed)
{
  ActParamTool::AsBool( this->Parameter(PID_IsClosed) )->SetValue(isClosed);
}

//! \return true if the contour is defined as closed,
//!         false -- otherwise.
bool geom_contour_node::IsClosed() const
{
  return ActParamTool::AsBool( this->Parameter(PID_IsClosed) )->GetValue();
}
