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
  return NULL;
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
  return NULL;
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
  return NULL;
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
  return NULL;
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
  return NULL;
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
  return NULL;
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
  return NULL;
}
