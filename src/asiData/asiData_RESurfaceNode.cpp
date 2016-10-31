//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiData_RESurfaceNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_RESurfaceNode::asiData_RESurfaceNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,  PID_Name);
  REGISTER_PARAMETER(Shape, PID_Surface);
  REGISTER_PARAMETER(Real,  PID_ULimit);
  REGISTER_PARAMETER(Real,  PID_VLimit);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_RESurfaceNode::Instance()
{
  return new asiData_RESurfaceNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
//! \param surface [in] surface to store.
//! \param uLimit  [in] limit by U curvilinear axis.
//! \param vLimit  [in] limit by V curvilinear axis.
void asiData_RESurfaceNode::Init(const Handle(Geom_Surface)& surface,
                                const double                uLimit,
                                const double                vLimit)
{
  this->InitParameter(PID_Name, "Name");
  //
  this->SetSurface(surface);
  this->SetLimits(uLimit, vLimit);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_RESurfaceNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void asiData_RESurfaceNode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Convenience methods
//-----------------------------------------------------------------------------

//! Sets surface to store.
//! \param surface [in] surface to store.
void asiData_RESurfaceNode::SetSurface(const Handle(Geom_Surface)& surface)
{
  TopoDS_Face F;
  BRep_Builder BB;
  BB.MakeFace(F);
  BB.UpdateFace( F, surface, TopLoc_Location(), Precision::Confusion() );
  //
  ActParamTool::AsShape( this->Parameter(PID_Surface) )->SetShape(F);
}

//! \return stored surface.
Handle(Geom_Surface) asiData_RESurfaceNode::GetSurface() const
{
  TopoDS_Shape S = ActParamTool::AsShape( this->Parameter(PID_Surface) )->GetShape();
  //
  if ( S.IsNull() || S.ShapeType() != TopAbs_FACE )
    return NULL;
  //
  return BRep_Tool::Surface( TopoDS::Face(S) );
}

//! Sets parametric bounds for infinite surfaces.
//! \param uLimit [in] limit by U curvilinear axis.
//! \param vLimit [in] limit by V curvilinear axis.
void asiData_RESurfaceNode::SetLimits(const double uLimit, const double vLimit)
{
  ActParamTool::AsReal( this->Parameter(PID_ULimit) )->SetValue(uLimit);
  ActParamTool::AsReal( this->Parameter(PID_VLimit) )->SetValue(vLimit);
}

//! Gets parametric bounds used for infinite surfaces.
//! \param uLimit [out] limit by U curvilinear axis.
//! \param vLimit [out] limit by V curvilinear axis.
void asiData_RESurfaceNode::GetLimits(double& uLimit, double& vLimit) const
{
  uLimit = ActParamTool::AsReal( this->Parameter(PID_ULimit) )->GetValue();
  vLimit = ActParamTool::AsReal( this->Parameter(PID_VLimit) )->GetValue();
}
