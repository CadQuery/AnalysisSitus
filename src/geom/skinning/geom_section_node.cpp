//-----------------------------------------------------------------------------
// Created on: 09 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <geom_section_node.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
geom_section_node::geom_section_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,  PID_Name);
  REGISTER_PARAMETER(Shape, PID_Geometry);
  REGISTER_PARAMETER(Real,  PID_Tx);
  REGISTER_PARAMETER(Real,  PID_Ty);
  REGISTER_PARAMETER(Real,  PID_Tz);
  REGISTER_PARAMETER(Real,  PID_Rx);
  REGISTER_PARAMETER(Real,  PID_Ry);
  REGISTER_PARAMETER(Real,  PID_Rz);
  REGISTER_PARAMETER(Real,  PID_CircleRadius);
}

//! Returns new DETACHED instance of Section Node ensuring its correct
//! allocation in a heap.
//! \return new instance of Section Node.
Handle(ActAPI_INode) geom_section_node::Instance()
{
  return new geom_section_node();
}

//! Performs initial actions required to make Sections Node WELL-FORMED.
void geom_section_node::Init()
{
  // Initialize Parameters
  this->InitParameter(PID_Name, "Name");
  this->SetGeometry( TopoDS_Shape() );
  this->SetRadius(1.0);

  // Transformation
  ActParamTool::AsReal( this->Parameter(PID_Tx) )->SetValue(0.0);
  ActParamTool::AsReal( this->Parameter(PID_Ty) )->SetValue(0.0);
  ActParamTool::AsReal( this->Parameter(PID_Tz) )->SetValue(0.0);
  ActParamTool::AsReal( this->Parameter(PID_Rx) )->SetValue(0.0);
  ActParamTool::AsReal( this->Parameter(PID_Ry) )->SetValue(0.0);
  ActParamTool::AsReal( this->Parameter(PID_Rz) )->SetValue(0.0);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString geom_section_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void geom_section_node::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Convenience getters and setters
//-----------------------------------------------------------------------------

//! Accessor for the stored geometry.
//! \return stored geometry.
TopoDS_Shape geom_section_node::GetGeometry() const
{
  return ActParamTool::AsShape( this->Parameter(PID_Geometry) )->GetShape();
}

//! Sets geometry to be stored.
//! \param geometry [in] geometry to store.
void geom_section_node::SetGeometry(const TopoDS_Shape& geometry)
{
  ActParamTool::AsShape( this->Parameter(PID_Geometry) )->SetShape(geometry);
}

//! Returns the stored transformation values.
//! \param Tx [out] translation along OX.
//! \param Ty [out] translation along OY.
//! \param Tz [out] translation along OZ.
//! \param Rx [out] rotation around OX.
//! \param Ry [out] rotation around OY.
//! \param Rz [out] rotation around OZ.
void geom_section_node::GetTransform(double& Tx, double& Ty, double& Tz,
                                     double& Rx, double& Ry, double& Rz) const
{
  Tx = ActParamTool::AsReal( this->Parameter(PID_Tx) )->GetValue();
  Ty = ActParamTool::AsReal( this->Parameter(PID_Ty) )->GetValue();
  Tz = ActParamTool::AsReal( this->Parameter(PID_Tz) )->GetValue();
  Rx = ActParamTool::AsReal( this->Parameter(PID_Rx) )->GetValue();
  Ry = ActParamTool::AsReal( this->Parameter(PID_Ry) )->GetValue();
  Rz = ActParamTool::AsReal( this->Parameter(PID_Rz) )->GetValue();
}

//! Sets transformation values.
//! \param Tx [in] translation along OX.
//! \param Ty [in] translation along OY.
//! \param Tz [in] translation along OZ.
//! \param Rx [in] rotation around OX.
//! \param Ry [in] rotation around OY.
//! \param Rz [in] rotation around OZ.
void geom_section_node::SetTransform(const double Tx, const double Ty, const double Tz,
                                     const double Rx, const double Ry, const double Rz)
{
  ActParamTool::AsReal( this->Parameter(PID_Tx) )->SetValue(Tx);
  ActParamTool::AsReal( this->Parameter(PID_Ty) )->SetValue(Ty);
  ActParamTool::AsReal( this->Parameter(PID_Tz) )->SetValue(Tz);
  ActParamTool::AsReal( this->Parameter(PID_Rx) )->SetValue(Rx);
  ActParamTool::AsReal( this->Parameter(PID_Ry) )->SetValue(Ry);
  ActParamTool::AsReal( this->Parameter(PID_Rz) )->SetValue(Rz);
}

//! Accessor for the stored circle radius.
//! \return circle radius.
double geom_section_node::GetRadius() const
{
  return ActParamTool::AsReal( this->Parameter(PID_CircleRadius) )->GetValue();
}

//! Sets circle radius to be stored.
//! \param radius [in] circle radius to store.
void geom_section_node::SetRadius(const double radius)
{
  ActParamTool::AsReal( this->Parameter(PID_CircleRadius) )->SetValue(radius);
}
