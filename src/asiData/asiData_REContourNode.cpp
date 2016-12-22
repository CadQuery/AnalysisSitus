//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiData_REContourNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_REContourNode::asiData_REContourNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,  PID_Name);
  REGISTER_PARAMETER(Shape, PID_Contour);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_REContourNode::Instance()
{
  return new asiData_REContourNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
//! \param contour [in] contour to store.
void asiData_REContourNode::Init(const TopoDS_Wire& contour)
{
  this->InitParameter(PID_Name, "Name");
  //
  this->SetContour(contour);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_REContourNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void asiData_REContourNode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Convenience methods
//-----------------------------------------------------------------------------

//! Sets contour to store.
//! \param contour [in] contour to store.
void asiData_REContourNode::SetContour(const TopoDS_Wire& contour)
{
  ActParamTool::AsShape( this->Parameter(PID_Contour) )->SetShape(contour);
}

//! \return stored contour.
TopoDS_Wire asiData_REContourNode::GetContour() const
{
  TopoDS_Shape S = ActParamTool::AsShape( this->Parameter(PID_Contour) )->GetShape();
  //
  if ( S.IsNull() || S.ShapeType() != TopAbs_WIRE )
    return TopoDS_Wire();
  //
  return TopoDS::Wire(S);
}
