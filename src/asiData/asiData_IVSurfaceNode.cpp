//-----------------------------------------------------------------------------
// Created on: 08 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiData_IVSurfaceNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_IVSurfaceNode::asiData_IVSurfaceNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,  PID_Name);
  REGISTER_PARAMETER(Shape, PID_Geometry);
  REGISTER_PARAMETER(Real,  PID_ULimit);
  REGISTER_PARAMETER(Real,  PID_VLimit);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_IVSurfaceNode::Instance()
{
  return new asiData_IVSurfaceNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_IVSurfaceNode::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
  //
  this->SetSurface(NULL);
  this->SetLimits(0.0, 0.0);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_IVSurfaceNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void asiData_IVSurfaceNode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! \return stored geometry.
Handle(Geom_Surface) asiData_IVSurfaceNode::GetSurface() const
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
//! \param surface [in] geometry to store.
void asiData_IVSurfaceNode::SetSurface(const Handle(Geom_Surface)& surface)
{
  // Create a fictive face to take advantage of topology Parameter of Active Data
  TopoDS_Face F;
  if ( !surface.IsNull() )
    F = BRepBuilderAPI_MakeFace( surface, Precision::Confusion() );

  // Store
  ActParamTool::AsShape( this->Parameter(PID_Geometry) )->SetShape(F);
}

//! Sets parametric bounds for infinite surfaces.
//! \param uLimit [in] limit by U curvilinear axis.
//! \param vLimit [in] limit by V curvilinear axis.
void asiData_IVSurfaceNode::SetLimits(const double uLimit, const double vLimit)
{
  ActParamTool::AsReal( this->Parameter(PID_ULimit) )->SetValue(uLimit);
  ActParamTool::AsReal( this->Parameter(PID_VLimit) )->SetValue(vLimit);
}

//! Gets parametric bounds used for infinite surfaces.
//! \param uLimit [out] limit by U curvilinear axis.
//! \param vLimit [out] limit by V curvilinear axis.
void asiData_IVSurfaceNode::GetLimits(double& uLimit, double& vLimit) const
{
  uLimit = ActParamTool::AsReal( this->Parameter(PID_ULimit) )->GetValue();
  vLimit = ActParamTool::AsReal( this->Parameter(PID_VLimit) )->GetValue();
}
