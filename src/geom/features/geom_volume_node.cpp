//-----------------------------------------------------------------------------
// Created on: 13 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_volume_node.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
geom_volume_node::geom_volume_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,          PID_Name);
  REGISTER_PARAMETER(Shape,         PID_Geometry);
  REGISTER_PARAMETER(ReferenceList, PID_Features2Keep);
  REGISTER_PARAMETER(ReferenceList, PID_Features2Exclude);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) geom_volume_node::Instance()
{
  return new geom_volume_node();
}

//! Performs initial actions required to make Geometry Node WELL-FORMED.
void geom_volume_node::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");

  // Set empty initial shape
  ActParamTool::AsShape( this->Parameter(PID_Geometry) )->SetShape( TopoDS_Shape(), MT_Silent );
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString geom_volume_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void geom_volume_node::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! Sets shape to store.
//! \param shape [in] shape to store.
void geom_volume_node::SetShape(const TopoDS_Shape& shape)
{
  ActParamTool::AsShape( this->Parameter(PID_Geometry) )->SetShape(shape);
}

//! \return stored shape.
TopoDS_Shape geom_volume_node::GetShape() const
{
  return ActParamTool::AsShape( this->Parameter(PID_Geometry) )->GetShape();
}

//! Sets list of features to keep.
//! \param items [in] list of features to set.
void geom_volume_node::SetFeatures2Keep(const Handle(ActAPI_HDataCursorList)& items)
{
  Handle(ActData_ReferenceListParameter)
    ref_p = ActParamTool::AsReferenceList( this->Parameter(PID_Features2Keep) );
  //
  if ( items.IsNull() )
    ref_p->RemoveTargets();
  else
    ref_p->SetTargets(items);
}

//! \return list of features to keep.
Handle(ActAPI_HDataCursorList) geom_volume_node::GetFeatures2Keep() const
{
  Handle(ActData_ReferenceListParameter)
    ref_p = ActParamTool::AsReferenceList( this->Parameter(PID_Features2Keep) );
  //
  return ref_p->GetTargets();
}

//! Sets list of features to exclude.
//! \param items [in] list of features to set.
void geom_volume_node::SetFeatures2Exclude(const Handle(ActAPI_HDataCursorList)& items)
{
  Handle(ActData_ReferenceListParameter)
    ref_p = ActParamTool::AsReferenceList( this->Parameter(PID_Features2Exclude) );
  //
  if ( items.IsNull() )
    ref_p->RemoveTargets();
  else
    ref_p->SetTargets(items);
}

//! \return list of features to exclude.
Handle(ActAPI_HDataCursorList) geom_volume_node::GetFeatures2Exclude() const
{
  Handle(ActData_ReferenceListParameter)
    ref_p = ActParamTool::AsReferenceList( this->Parameter(PID_Features2Exclude) );
  //
  return ref_p->GetTargets();
}
