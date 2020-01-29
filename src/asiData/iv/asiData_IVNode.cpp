//-----------------------------------------------------------------------------
// Created on: 08 April 2016
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
#include <asiData_IVNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_IVNode::asiData_IVNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name, PID_Name);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_IVNode::Instance()
{
  return new asiData_IVNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_IVNode::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_IVNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void asiData_IVNode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! \return underlying 2D points.
Handle(asiData_IVPoints2dNode) asiData_IVNode::Points2d()
{
  Handle(asiData_IVPoints2dNode)
    points_n = Handle(asiData_IVPoints2dNode)::DownCast( this->GetChildNode(Child_Points2d) );
  //
  if ( !points_n.IsNull() && points_n->IsWellFormed() )
    return points_n;
  //
  return nullptr;
}

//! \return underlying points.
Handle(asiData_IVPointsNode) asiData_IVNode::Points()
{
  Handle(asiData_IVPointsNode)
    points_n = Handle(asiData_IVPointsNode)::DownCast( this->GetChildNode(Child_Points) );
  //
  if ( !points_n.IsNull() && points_n->IsWellFormed() )
    return points_n;
  //
  return nullptr;
}

//! \return underlying curves 2D Node.
Handle(asiData_IVCurves2dNode) asiData_IVNode::Curves2d()
{
  Handle(asiData_IVCurves2dNode)
    curves2d_n = Handle(asiData_IVCurves2dNode)::DownCast( this->GetChildNode(Child_Curves2d) );
  //
  if ( !curves2d_n.IsNull() && curves2d_n->IsWellFormed() )
    return curves2d_n;
  //
  return nullptr;
}

//! \return underlying curves.
Handle(asiData_IVCurvesNode) asiData_IVNode::Curves()
{
  Handle(asiData_IVCurvesNode)
    curves_n = Handle(asiData_IVCurvesNode)::DownCast( this->GetChildNode(Child_Curves) );
  //
  if ( !curves_n.IsNull() && curves_n->IsWellFormed() )
    return curves_n;
  //
  return nullptr;
}

//! \return underlying surfaces.
Handle(asiData_IVSurfacesNode) asiData_IVNode::Surfaces()
{
  Handle(asiData_IVSurfacesNode)
    surf_n = Handle(asiData_IVSurfacesNode)::DownCast( this->GetChildNode(Child_Surfaces) );
  //
  if ( !surf_n.IsNull() && surf_n->IsWellFormed() )
    return surf_n;
  //
  return nullptr;
}

//! \return underlying topologies.
Handle(asiData_IVTopoNode) asiData_IVNode::Topology()
{
  Handle(asiData_IVTopoNode)
    topo_n = Handle(asiData_IVTopoNode)::DownCast( this->GetChildNode(Child_Topology) );
  //
  if ( !topo_n.IsNull() && topo_n->IsWellFormed() )
    return topo_n;
  //
  return nullptr;
}

//! \return underlying tessellations.
Handle(asiData_IVTessNode) asiData_IVNode::Tessellation()
{
  Handle(asiData_IVTessNode)
    tess_n = Handle(asiData_IVTessNode)::DownCast( this->GetChildNode(Child_Tessellation) );
  //
  if ( !tess_n.IsNull() && tess_n->IsWellFormed() )
    return tess_n;
  //
  return nullptr;
}

//! \return underlying text container.
Handle(asiData_IVTextNode) asiData_IVNode::Text()
{
  Handle(asiData_IVTextNode)
    text_n = Handle(asiData_IVTextNode)::DownCast( this->GetChildNode(Child_Text) );
  //
  if ( !text_n.IsNull() && text_n->IsWellFormed() )
    return text_n;
  //
  return nullptr;
}
