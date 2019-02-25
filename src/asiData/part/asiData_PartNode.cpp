//-----------------------------------------------------------------------------
// Created on: 28 November 2015
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
#include <asiData_PartNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_PartNode::asiData_PartNode() : ActData_BaseNode()
{
  // Register standard Active Data Parameters.
  REGISTER_PARAMETER(Name,  PID_Name);
  REGISTER_PARAMETER(Shape, PID_Geometry);
  REGISTER_PARAMETER(Group, PID_GroupTess);
  REGISTER_PARAMETER(Real,  PID_TessLinDefl);
  REGISTER_PARAMETER(Real,  PID_TessAngDefl);
  REGISTER_PARAMETER(Int,   PID_DisplayMode);
  REGISTER_PARAMETER(Bool,  PID_HasColor);
  REGISTER_PARAMETER(Int,   PID_Color);
  REGISTER_PARAMETER(Bool,  PID_HasVertices);

  // Register custom Parameters specific to Analysis Situs application.
  this->registerParameter(PID_AAG,    asiData_AAGParameter::Instance(),    false);
  this->registerParameter(PID_BVH,    asiData_BVHParameter::Instance(),    false);
  this->registerParameter(PID_Naming, asiData_NamingParameter::Instance(), false);
}

//! Returns new DETACHED instance of Geometry Node ensuring its correct
//! allocation in a heap.
//! \return new instance of Geometry Node.
Handle(ActAPI_INode) asiData_PartNode::Instance()
{
  return new asiData_PartNode();
}

//! Performs initial actions required to make Geometry Node WELL-FORMED.
void asiData_PartNode::Init(const bool resetNaming)
{
  // Initialize name Parameter.
  this->InitParameter(PID_Name, "Name");

  // Set empty initial B-Rep and AAG.
  this->setShape ( TopoDS_Shape() );
  this->setAAG   ( NULL );
  this->setBVH   ( NULL );
  //
  if ( resetNaming )
    this->setNaming ( NULL );

  // Set default values to primitive Parameters.
  this->SetLinearDeflection  (0.0);
  this->SetAngularDeflection (0.0);
  this->SetHasColor          (false);
  this->SetColor             (-1);
  this->SetDisplayMode       (1);
  this->SetHasVertices       (false);

  // Initialize Parameter flags.
  this->InitParameter(PID_Name,        "Name",               "", ParameterFlag_IsVisible, true);
  this->InitParameter(PID_GroupTess,   "Tessellation",       "", ParameterFlag_IsVisible, true);
  this->InitParameter(PID_TessAngDefl, "Angular deflection", "", ParameterFlag_IsVisible, true);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_PartNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param name [in] name to set.
void asiData_PartNode::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! \return stored shape.
TopoDS_Shape asiData_PartNode::GetShape() const
{
  return ActParamTool::AsShape( this->Parameter(PID_Geometry) )->GetShape();
}

//! \return stored AAG.
Handle(asiAlgo_AAG) asiData_PartNode::GetAAG() const
{
  return Handle(asiData_AAGParameter)::DownCast( this->Parameter(PID_AAG) )->GetAAG();
}

//! \return stored BVH.
Handle(asiAlgo_BVHFacets) asiData_PartNode::GetBVH() const
{
  return Handle(asiData_BVHParameter)::DownCast( this->Parameter(PID_BVH) )->GetBVH();
}

//! \return stored naming service.
Handle(asiAlgo_Naming) asiData_PartNode::GetNaming() const
{
  return Handle(asiData_NamingParameter)::DownCast( this->Parameter(PID_Naming) )->GetNaming();
}

//! \return true if naming service is initialized.
bool asiData_PartNode::HasNaming() const
{
  return !this->GetNaming().IsNull();
}

//! Sets linear deflection.
//! \param defl [in] value to set.
void asiData_PartNode::SetLinearDeflection(const double defl)
{
  ActParamTool::AsReal( this->Parameter(PID_TessLinDefl) )->SetValue(defl);
}

//! \return linear deflection.
double asiData_PartNode::GetLinearDeflection() const
{
  return ActParamTool::AsReal( this->Parameter(PID_TessLinDefl) )->GetValue();
}

//! Sets angular deflection.
//! \param defl [in] value to set.
void asiData_PartNode::SetAngularDeflection(const double defl)
{
  ActParamTool::AsReal( this->Parameter(PID_TessAngDefl) )->SetValue(defl);
}

//! \return angular deflection.
double asiData_PartNode::GetAngularDeflection() const
{
  return ActParamTool::AsReal( this->Parameter(PID_TessAngDefl) )->GetValue();
}

//! Sets the Boolean value indicating whether the color Parameter of this
//! Data Node is in force.
//! \param hasColor [in] value to set.
void asiData_PartNode::SetHasColor(const bool hasColor)
{
  ActParamTool::AsBool( this->Parameter(PID_HasColor) )->SetValue(hasColor);
}

//! Accessor for the value of the Boolean Parameter indicating whether the
//! Color Parameter of this Data Node is in force.
//! \return true/false.
bool asiData_PartNode::HasColor() const
{
  return ActParamTool::AsBool( this->Parameter(PID_HasColor) )->GetValue();
}

//! Sets color.
//! \param color [in] color to set.
void asiData_PartNode::SetColor(const int color)
{
  ActParamTool::AsInt( this->Parameter(PID_Color) )->SetValue(color);
}

//! Accessor for the stored color value.
//! \return color value.
int asiData_PartNode::GetColor() const
{
  return ActParamTool::AsInt( this->Parameter(PID_Color) )->GetValue();
}

//! Sets display mode.
//! \param mode [in] display mode value to set.
void asiData_PartNode::SetDisplayMode(const int mode)
{
  ActParamTool::AsInt( this->Parameter(PID_DisplayMode) )->SetValue(mode);
}

//! Accessor for the stored display mode value.
//! \return display mode value.
int asiData_PartNode::GetDisplayMode() const
{
  return ActParamTool::AsInt( this->Parameter(PID_DisplayMode) )->GetValue();
}

//! Sets the Boolean value indicating whether the vertices are active
//! \param hasVertices [in] value to set.
void asiData_PartNode::SetHasVertices(const bool hasVertices)
{
  ActParamTool::AsBool( this->Parameter(PID_HasVertices) )->SetValue(hasVertices);
}

//! Accessor for the value of the Boolean Parameter indicating whether the
//! vertices are active.
//! \return true/false.
bool asiData_PartNode::HasVertices() const
{
  return ActParamTool::AsBool( this->Parameter(PID_HasVertices) )->GetValue();
}

//-----------------------------------------------------------------------------

//! \return underlying face representation Node.
Handle(asiData_FaceNode) asiData_PartNode::GetFaceRepresentation() const
{
  Handle(asiData_FaceNode) face_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    face_n = Handle(asiData_FaceNode)::DownCast( cit->Value() );

    if ( !face_n.IsNull() && face_n->IsWellFormed() )
      return face_n;
  }

  return NULL;
}

//! \return underlying normal vectors representation Node.
Handle(asiData_FaceNormsNode) asiData_PartNode::GetNormsRepresentation() const
{
  Handle(asiData_FaceNormsNode) norms_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    norms_n = Handle(asiData_FaceNormsNode)::DownCast( cit->Value() );

    if ( !norms_n.IsNull() && norms_n->IsWellFormed() )
      return norms_n;
  }

  return NULL;
}

//! \return underlying surface representation Node.
Handle(asiData_SurfNode) asiData_PartNode::GetSurfaceRepresentation() const
{
  Handle(asiData_SurfNode) surf_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    surf_n = Handle(asiData_SurfNode)::DownCast( cit->Value() );

    if ( !surf_n.IsNull() && surf_n->IsWellFormed() )
      return surf_n;
  }

  return NULL;
}

//! \return underlying Face Contour Node.
Handle(asiData_FaceContourNode) asiData_PartNode::GetContourRepresentation() const
{
  Handle(asiData_FaceContourNode) contour_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    contour_n = Handle(asiData_FaceContourNode)::DownCast( cit->Value() );

    if ( !contour_n.IsNull() && contour_n->IsWellFormed() )
      return contour_n;
  }

  return NULL;
}

//! \return underlying boundary edges representation Node.
Handle(asiData_BoundaryEdgesNode) asiData_PartNode::GetBoundaryEdgesRepresentation() const
{
  Handle(asiData_BoundaryEdgesNode) edges_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    edges_n = Handle(asiData_BoundaryEdgesNode)::DownCast( cit->Value() );

    if ( !edges_n.IsNull() && edges_n->IsWellFormed() )
      return edges_n;
  }

  return NULL;
}

//! \return underlying edge representation Node.
Handle(asiData_EdgeNode) asiData_PartNode::GetEdgeRepresentation() const
{
  Handle(asiData_EdgeNode) edge_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    edge_n = Handle(asiData_EdgeNode)::DownCast( cit->Value() );

    if ( !edge_n.IsNull() && edge_n->IsWellFormed() )
      return edge_n;
  }

  return NULL;
}

//! \return underlying curve representation Node.
Handle(asiData_CurveNode) asiData_PartNode::GetCurveRepresentation() const
{
  Handle(asiData_CurveNode) curve_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    curve_n = Handle(asiData_CurveNode)::DownCast( cit->Value() );

    if ( !curve_n.IsNull() && curve_n->IsWellFormed() )
      return curve_n;
  }

  return NULL;
}

//! \return underlying contour Node.
Handle(asiData_ContourNode) asiData_PartNode::GetContour() const
{
  Handle(asiData_ContourNode) contour_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    contour_n = Handle(asiData_ContourNode)::DownCast( cit->Value() );

    if ( !contour_n.IsNull() && contour_n->IsWellFormed() )
      return contour_n;
  }

  return NULL;
}

//! \return underlying Vertex Node.
Handle(asiData_VertexNode) asiData_PartNode::GetVertexRepresentation() const
{
  Handle(asiData_VertexNode) vertex_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    vertex_n = Handle(asiData_VertexNode)::DownCast( cit->Value() );

    if ( !vertex_n.IsNull() && vertex_n->IsWellFormed() )
      return vertex_n;
  }

  return NULL;
}

//! \return underlying Node representing the group of tolerant shapes.
Handle(asiData_TolerantShapesNode) asiData_PartNode::GetTolerantShapes() const
{
  Handle(asiData_TolerantShapesNode) tolShapes_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    tolShapes_n = Handle(asiData_TolerantShapesNode)::DownCast( cit->Value() );

    if ( !tolShapes_n.IsNull() && tolShapes_n->IsWellFormed() )
      return tolShapes_n;
  }

  return NULL;
}

//-----------------------------------------------------------------------------

//! Sets shape to store.
//! \param shape [in] shape to store.
void asiData_PartNode::setShape(const TopoDS_Shape& shape)
{
  ActParamTool::AsShape( this->Parameter(PID_Geometry) )->SetShape(shape);
}

//! Sets AAG to store.
//! \param aag [in] AAG to store.
void asiData_PartNode::setAAG(const Handle(asiAlgo_AAG)& aag)
{
  Handle(asiData_AAGParameter)::DownCast( this->Parameter(PID_AAG) )->SetAAG(aag);
}

//! Sets BVH to store.
//! \param bvh [in] BVH to store.
void asiData_PartNode::setBVH(const Handle(asiAlgo_BVHFacets)& bvh)
{
  Handle(asiData_BVHParameter)::DownCast( this->Parameter(PID_BVH) )->SetBVH(bvh);
}

//! Sets naming service to store.
//! \param naming [in] naming service to store.
void asiData_PartNode::setNaming(const Handle(asiAlgo_Naming)& naming)
{
  Handle(asiData_NamingParameter)::DownCast( this->Parameter(PID_Naming) )->SetNaming(naming);
}
