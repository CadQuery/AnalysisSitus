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

#ifndef asiData_IVNode_h
#define asiData_IVNode_h

// Visualization includes
#include <asiData_IVCurvesNode.h>
#include <asiData_IVPoints2dNode.h>
#include <asiData_IVPointsNode.h>
#include <asiData_IVSurfacesNode.h>
#include <asiData_IVTessNode.h>
#include <asiData_IVTextNode.h>
#include <asiData_IVTopoNode.h>

//-----------------------------------------------------------------------------
// Root Node for all items visualized in the imperative viewer
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiData_IVNode, ActData_BaseNode)

//! Root Node for all items visualized in the imperative viewer.
class asiData_IVNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_IVNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_IVNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
  //------------------//
    PID_Last = PID_Name + ActData_BaseNode::RESERVED_PARAM_RANGE
  };

public:

  asiData_EXPORT static Handle(ActAPI_INode)
    Instance();

// Generic naming support:
public:

  asiData_EXPORT virtual TCollection_ExtendedString
    GetName();

  asiData_EXPORT virtual void
    SetName(const TCollection_ExtendedString& name);

// Handy accessors to the stored data:
public:

  asiData_EXPORT Handle(asiData_IVPoints2dNode)
    Points2d();

  asiData_EXPORT Handle(asiData_IVPointsNode)
    Points();

  asiData_EXPORT Handle(asiData_IVCurvesNode)
    Curves();

  asiData_EXPORT Handle(asiData_IVSurfacesNode)
    Surfaces();

  asiData_EXPORT Handle(asiData_IVTopoNode)
    Topology();

  asiData_EXPORT Handle(asiData_IVTessNode)
    Tessellation();

  asiData_EXPORT Handle(asiData_IVTextNode)
    Text();

// Initialization:
public:

  asiData_EXPORT void
    Init();

protected:

  //! Allocation is allowed only via Instance method.
  asiData_EXPORT
    asiData_IVNode();

private:

  enum Child
  {
    Child_Points2d = 1,
    Child_Points,
    Child_Curves,
    Child_Surfaces,
    Child_Topology,
    Child_Tessellation,
    Child_Text
  };

};

#endif
