//-----------------------------------------------------------------------------
// Created on: 08 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_iv_curve_node.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
visu_iv_curve_node::visu_iv_curve_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,  PID_Name);
  REGISTER_PARAMETER(Shape, PID_Geometry);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) visu_iv_curve_node::Instance()
{
  return new visu_iv_curve_node();
}

//! Performs initial actions required to make Node WELL-FORMED.
void visu_iv_curve_node::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
  //
  this->SetCurve(NULL, 0.0, 0.0);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString visu_iv_curve_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void visu_iv_curve_node::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! Returns the stored geometry.
//! \param f [out] first parameter.
//! \param l [out] last parameter.
//! \return stored geometry.
Handle(Geom_Curve) visu_iv_curve_node::GetCurve(double& f, double& l) const
{
  TopoDS_Shape sh = ActParamTool::AsShape( this->Parameter(PID_Geometry) )->GetShape();
  //
  if ( sh.IsNull() || sh.ShapeType() != TopAbs_EDGE )
    return NULL;

  // Extract edge and its host geometry
  const TopoDS_Edge& E = TopoDS::Edge(sh);
  //
  return BRep_Tool::Curve(E, f, l);
}

//! Sets curve to store.
//! \param curve [in] geometry to store.
//! \param f     [in] first parameter of the curve.
//! \param l     [in] last parameter of the curve.
void visu_iv_curve_node::SetCurve(const Handle(Geom_Curve)& curve,
                                  const double              f,
                                  const double              l)
{
  // Create a fictive edge to take advantage of topology Parameter of Active Data
  TopoDS_Edge E;
  if ( !curve.IsNull() )
    E = BRepBuilderAPI_MakeEdge(curve, f, l);

  // Store
  ActParamTool::AsShape( this->Parameter(PID_Geometry) )->SetShape(E);
}
