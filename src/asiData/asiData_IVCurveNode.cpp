//-----------------------------------------------------------------------------
// Created on: 08 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Own include
#include <asiData_IVCurveNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_IVCurveNode::asiData_IVCurveNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,  PID_Name);
  REGISTER_PARAMETER(Shape, PID_Geometry);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_IVCurveNode::Instance()
{
  return new asiData_IVCurveNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_IVCurveNode::Init()
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
TCollection_ExtendedString asiData_IVCurveNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void asiData_IVCurveNode::SetName(const TCollection_ExtendedString& theName)
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
Handle(Geom_Curve) asiData_IVCurveNode::GetCurve(double& f, double& l) const
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
void asiData_IVCurveNode::SetCurve(const Handle(Geom_Curve)& curve,
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
