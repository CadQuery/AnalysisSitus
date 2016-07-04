//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_re_surface_node.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
geom_re_surface_node::geom_re_surface_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,  PID_Name);
  REGISTER_PARAMETER(Shape, PID_Surface);
  REGISTER_PARAMETER(Real,  PID_ULimit);
  REGISTER_PARAMETER(Real,  PID_VLimit);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) geom_re_surface_node::Instance()
{
  return new geom_re_surface_node();
}

//! Performs initial actions required to make Node WELL-FORMED.
//! \param surface [in] surface to store.
//! \param uLimit  [in] limit by U curvilinear axis.
//! \param vLimit  [in] limit by V curvilinear axis.
void geom_re_surface_node::Init(const Handle(Geom_Surface)& surface,
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
TCollection_ExtendedString geom_re_surface_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void geom_re_surface_node::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Convenience methods
//-----------------------------------------------------------------------------

//! Sets surface to store.
//! \param surface [in] surface to store.
void geom_re_surface_node::SetSurface(const Handle(Geom_Surface)& surface)
{
  TopoDS_Face F;
  BRep_Builder BB;
  BB.MakeFace(F);
  BB.UpdateFace( F, surface, TopLoc_Location(), Precision::Confusion() );
  //
  ActParamTool::AsShape( this->Parameter(PID_Surface) )->SetShape(F);
}

//! \return stored surface.
Handle(Geom_Surface) geom_re_surface_node::GetSurface() const
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
void geom_re_surface_node::SetLimits(const double uLimit, const double vLimit)
{
  ActParamTool::AsReal( this->Parameter(PID_ULimit) )->SetValue(uLimit);
  ActParamTool::AsReal( this->Parameter(PID_VLimit) )->SetValue(vLimit);
}

//! Gets parametric bounds used for infinite surfaces.
//! \param uLimit [out] limit by U curvilinear axis.
//! \param vLimit [out] limit by V curvilinear axis.
void geom_re_surface_node::GetLimits(double& uLimit, double& vLimit) const
{
  uLimit = ActParamTool::AsReal( this->Parameter(PID_ULimit) )->GetValue();
  vLimit = ActParamTool::AsReal( this->Parameter(PID_VLimit) )->GetValue();
}
