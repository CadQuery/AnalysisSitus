//-----------------------------------------------------------------------------
// Created on: 06 October 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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
#include <asiData_ReEdgeNode.h>

// Active Data includes
#include <ActAux_ArrayUtils.h>
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

//-----------------------------------------------------------------------------

//! Default ctor. Registers all involved Parameters.
asiData_ReEdgeNode::asiData_ReEdgeNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,      PID_Name);
  REGISTER_PARAMETER(Reference, PID_VertexFirstRef);
  REGISTER_PARAMETER(Reference, PID_VertexLastRef);
  REGISTER_PARAMETER(Int,       PID_VertexFirstIdx);
  REGISTER_PARAMETER(Int,       PID_VertexLastIdx);
  REGISTER_PARAMETER(RealArray, PID_Polyline);
  REGISTER_PARAMETER(IntArray,  PID_PolylineInds);
  REGISTER_PARAMETER(Shape,     PID_Curve);
}

//-----------------------------------------------------------------------------

Handle(ActAPI_INode) asiData_ReEdgeNode::Instance()
{
  return new asiData_ReEdgeNode();
}

//-----------------------------------------------------------------------------

void asiData_ReEdgeNode::Init(const Handle(asiData_ReVertexNode)& vfirst,
                              const Handle(asiData_ReVertexNode)& vlast)
{
  // Initialize Parameters.
  this->InitParameter(PID_Name,           "Name");
  this->InitParameter(PID_VertexFirstRef, "First vertex");
  this->InitParameter(PID_VertexLastRef,  "Last vertex");
  this->InitParameter(PID_VertexFirstIdx, "First vertex index");
  this->InitParameter(PID_VertexLastIdx,  "Last vertex index");
  this->InitParameter(PID_Polyline,       "Polyline representation");
  this->InitParameter(PID_PolylineInds,   "Polyline triangles");
  this->InitParameter(PID_Curve,          "Parametric curve representation");

  if ( !vfirst.IsNull() )
    this->ConnectReference(PID_VertexFirstRef, vfirst);
  //
  if ( !vlast.IsNull() )
    this->ConnectReference(PID_VertexLastRef, vlast);

  // Set default values.
  ActParamTool::AsRealArray ( this->Parameter(PID_Polyline) )       ->SetArray( NULL );
  ActParamTool::AsIntArray  ( this->Parameter(PID_PolylineInds) )   ->SetArray( NULL );
  ActParamTool::AsShape     ( this->Parameter(PID_Curve) )          ->SetShape( TopoDS_Shape() );
  ActParamTool::AsInt       ( this->Parameter(PID_VertexFirstIdx) ) ->SetValue(-1);
  ActParamTool::AsInt       ( this->Parameter(PID_VertexLastIdx) )  ->SetValue(-1);
}

//-----------------------------------------------------------------------------

TCollection_ExtendedString asiData_ReEdgeNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//-----------------------------------------------------------------------------

void asiData_ReEdgeNode::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------

void asiData_ReEdgeNode::SetPolyline(const Handle(HRealArray)& coords)
{
  ActParamTool::AsRealArray( this->Parameter(PID_Polyline) )->SetArray(coords);
}

//-----------------------------------------------------------------------------

void asiData_ReEdgeNode::SetPolylineTriangles(const Handle(HIntArray)& indices)
{
  ActParamTool::AsIntArray( this->Parameter(PID_PolylineInds) )->SetArray(indices);
}

//-----------------------------------------------------------------------------

int asiData_ReEdgeNode::GetNumPolylinePoles() const
{
  Handle(HRealArray) coords = this->GetPolyline();
  //
  if ( coords.IsNull() )
    return 0;

  return coords->Length() / 3;
}

//-----------------------------------------------------------------------------

Handle(HRealArray) asiData_ReEdgeNode::GetPolyline() const
{
  return ActParamTool::AsRealArray( this->Parameter(PID_Polyline) )->GetArray();
}

//-----------------------------------------------------------------------------

void asiData_ReEdgeNode::GetPolyline(std::vector<gp_XYZ>& pts) const
{
  Handle(HRealArray) coords = this->GetPolyline();
  //
  if ( coords.IsNull() )
    return;

  gp_XYZ P_center;
  for ( int i = coords->Lower(); i <= coords->Upper() - 2; i += 3 )
  {
    gp_XYZ P( coords->Value(i),
              coords->Value(i + 1),
              coords->Value(i + 2) );
    //
    P_center += P;

    pts.push_back(P);
  }
}

//-----------------------------------------------------------------------------

Handle(HIntArray) asiData_ReEdgeNode::GetPolylineTriangles() const
{
  return ActParamTool::AsIntArray( this->Parameter(PID_PolylineInds) )->GetArray();
}

//-----------------------------------------------------------------------------

gp_XYZ asiData_ReEdgeNode::GetPolylinePole(const int zeroBasedIndex) const
{
  // Get array of coordinates
  Handle(ActData_RealArrayParameter)
    coords_p = ActParamTool::AsRealArray( this->Parameter(PID_Polyline) );
  //
  Handle(HRealArray) arr = coords_p->GetArray();

  // Construct a point
  const int startIdx = zeroBasedIndex*3;
  //
  gp_XYZ res( arr->Value(startIdx),
              arr->Value(startIdx + 1),
              arr->Value(startIdx + 2) );

  return res;
}

//-----------------------------------------------------------------------------

int asiData_ReEdgeNode::AddPolylinePole(const gp_XYZ& point, const int triIndx)
{
  // Change the array of coordinates.
  Handle(HRealArray)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Polyline) )->GetArray();
  //
  coords = ActAux_ArrayUtils::Append<HRealArray, Handle(HRealArray), double>( coords, point.X() );
  coords = ActAux_ArrayUtils::Append<HRealArray, Handle(HRealArray), double>( coords, point.Y() );
  coords = ActAux_ArrayUtils::Append<HRealArray, Handle(HRealArray), double>( coords, point.Z() );
  //
  this->SetPolyline(coords);

  // Change the array of triangle indices.
  Handle(HIntArray)
    indices = ActParamTool::AsIntArray( this->Parameter(PID_PolylineInds) )->GetArray();
  //
  indices = ActAux_ArrayUtils::Append<HIntArray, Handle(HIntArray), int>( indices, triIndx );
  //
  this->SetPolylineTriangles(indices);

  // Get index of the just added point.
  const int newIndex = coords->Length() / 3 - 1;
  return newIndex;
}

//-----------------------------------------------------------------------------

Handle(Geom_Curve) asiData_ReEdgeNode::GetCurve() const
{
  double f, l;
  return this->GetCurve(f, l);
}

//-----------------------------------------------------------------------------

Handle(Geom_Curve) asiData_ReEdgeNode::GetCurve(double& f, double& l) const
{
  TopoDS_Shape sh = ActParamTool::AsShape( this->Parameter(PID_Curve) )->GetShape();
  //
  if ( sh.IsNull() || sh.ShapeType() != TopAbs_EDGE )
    return NULL;

  // Extract edge and its host geometry
  const TopoDS_Edge& E = TopoDS::Edge(sh);
  //
  return BRep_Tool::Curve(E, f, l);
}

//-----------------------------------------------------------------------------

TopoDS_Shape asiData_ReEdgeNode::GetCurveAsShape() const
{
  return ActParamTool::AsShape( this->Parameter(PID_Curve) )->GetShape();
}

//-----------------------------------------------------------------------------

void asiData_ReEdgeNode::SetCurve(const Handle(Geom_Curve)& curve)
{
  this->SetCurve( curve, curve->FirstParameter(), curve->LastParameter() );
}

//-----------------------------------------------------------------------------

void asiData_ReEdgeNode::SetCurve(const Handle(Geom_Curve)& curve,
                                  const double              f,
                                  const double              l)
{
  // Create a fictive edge to take advantage of topology Parameter of Active Data
  TopoDS_Edge E;
  if ( !curve.IsNull() )
    E = BRepBuilderAPI_MakeEdge(curve, f, l);

  // Store
  ActParamTool::AsShape( this->Parameter(PID_Curve) )->SetShape(E);
}

//-----------------------------------------------------------------------------

Handle(asiData_ReVertexNode)
  asiData_ReEdgeNode::GetFirstVertex() const
{
  Handle(ActAPI_IDataCursor)
    dc = ActParamTool::AsReference( this->Parameter(PID_VertexFirstRef) )->GetTarget();

  // Check if target is Ok.
  if (  dc.IsNull()
    || !dc->IsWellFormed()
    || !dc->IsInstance( STANDARD_TYPE(asiData_ReVertexNode) ) )
    return NULL;

  return Handle(asiData_ReVertexNode)::DownCast(dc);
}

//-----------------------------------------------------------------------------

Handle(asiData_ReVertexNode)
  asiData_ReEdgeNode::GetLastVertex() const
{
  Handle(ActAPI_IDataCursor)
    dc = ActParamTool::AsReference( this->Parameter(PID_VertexLastRef) )->GetTarget();

  // Check if target is Ok.
  if (  dc.IsNull()
    || !dc->IsWellFormed()
    || !dc->IsInstance( STANDARD_TYPE(asiData_ReVertexNode) ) )
    return NULL;

  return Handle(asiData_ReVertexNode)::DownCast(dc);
}

//-----------------------------------------------------------------------------

int asiData_ReEdgeNode::GetFirstVertexIndex() const
{
  return ActParamTool::AsInt( this->Parameter(PID_VertexFirstIdx) )->GetValue();
}

//-----------------------------------------------------------------------------

int asiData_ReEdgeNode::GetLastVertexIndex() const
{
  return ActParamTool::AsInt( this->Parameter(PID_VertexLastIdx) )->GetValue();
}

//-----------------------------------------------------------------------------

void asiData_ReEdgeNode::SetFirstVertex(const Handle(asiData_ReVertexNode)& vertex)
{
  // Set references to the vertices.
  if ( !vertex.IsNull() )
    this->ConnectReference(PID_VertexFirstRef, vertex);
  else
    this->DisconnectReference(PID_VertexFirstRef);
}

//-----------------------------------------------------------------------------

void asiData_ReEdgeNode::SetLastVertex(const Handle(asiData_ReVertexNode)& vertex)
{
  // Set references to the vertices.
  if ( !vertex.IsNull() )
    this->ConnectReference(PID_VertexLastRef, vertex);
  else
    this->DisconnectReference(PID_VertexLastRef);
}

//-----------------------------------------------------------------------------

void asiData_ReEdgeNode::SetFirstVertexIndex(const int idx)
{
  ActParamTool::AsInt( this->Parameter(PID_VertexFirstIdx) )->SetValue(idx);
}

//-----------------------------------------------------------------------------

void asiData_ReEdgeNode::SetLastVertexIndex(const int idx)
{
  ActParamTool::AsInt( this->Parameter(PID_VertexLastIdx) )->SetValue(idx);
}
