//-----------------------------------------------------------------------------
// Created on: 11 December(*) 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiData_IVCurve2dNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_IVCurve2dNode::asiData_IVCurve2dNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,  PID_Name);
  REGISTER_PARAMETER(Shape, PID_Curve);
  REGISTER_PARAMETER(Shape, PID_Surface);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_IVCurve2dNode::Instance()
{
  return new asiData_IVCurve2dNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_IVCurve2dNode::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
  //
  this->SetCONS(nullptr, nullptr, 0.0, 0.0);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_IVCurve2dNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void asiData_IVCurve2dNode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! Returns the stored geometry.
//! \param surface [out] host surface.
//! \param f       [out] first parameter.
//! \param l       [out] last parameter.
//! \return stored geometry.
Handle(Geom2d_Curve)
  asiData_IVCurve2dNode::GetCONS(Handle(Geom_Surface)& surface,
                                 double&               f,
                                 double&               l) const
{
  // Get stored curve
  TopoDS_Shape curveSh = ActParamTool::AsShape( this->Parameter(PID_Curve) )->GetShape();
  //
  if ( curveSh.IsNull() || curveSh.ShapeType() != TopAbs_EDGE )
    return nullptr;

  // Get stored host surface
  TopoDS_Shape surfSh = ActParamTool::AsShape( this->Parameter(PID_Surface) )->GetShape();
  //
  if ( surfSh.IsNull() || surfSh.ShapeType() != TopAbs_FACE )
    return nullptr;

  // Extract host surface
  const TopoDS_Face& F = TopoDS::Face(surfSh);

  // Extract edge and its host geometry
  const TopoDS_Edge& E = TopoDS::Edge(curveSh);

  // Get host surface
  surface = BRep_Tool::Surface(F);

  return BRep_Tool::CurveOnSurface(E, F, f, l);
}

//! Sets curve to store.
//! \param curve   [in] geometry to store.
//! \param surface [in] host surface.
//! \param f       [in] first parameter of the curve.
//! \param l       [in] last parameter of the curve.
void asiData_IVCurve2dNode::SetCONS(const Handle(Geom2d_Curve)& curve,
                                    const Handle(Geom_Surface)& surface,
                                    const double                f,
                                    const double                l)
{
  if ( !curve.IsNull() && !surface.IsNull() )
  {
    // Create a fictive face
    TopoDS_Face F = BRepBuilderAPI_MakeFace( surface, Precision::Confusion() );

    // Create a fictive edge to take advantage of topology Parameter of Active Data
    TopoDS_Edge E = BRepBuilderAPI_MakeEdge(curve, surface, f, l);

    // Store
    ActParamTool::AsShape( this->Parameter(PID_Curve) )->SetShape(E);
    ActParamTool::AsShape( this->Parameter(PID_Surface) )->SetShape(F);
  }
}
