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

#ifndef asiData_IVTopoItemNode_h
#define asiData_IVTopoItemNode_h

// asiData includes
#include <asiData.h>

// Active Data includes
#include <ActData_BaseNode.h>

// OCCT includes
#include <TopoDS_Shape.hxx>

//-----------------------------------------------------------------------------
// Data Node representing a single topo item in IV (Imperative Viewer)
//-----------------------------------------------------------------------------

//! Data Node representing a single topo item in IV (Imperative Viewer).
class asiData_IVTopoItemNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_IVTopoItemNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_IVTopoItemNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
  //------------------//
    PID_Geometry,     //!< Stored geometry.
  //------------------//
  // Facets           //
  //------------------//
    PID_GroupTess,    //!< Tessellation group.
    PID_TessLinDefl,  //!< Linear deflection of visualization tessellation.
    PID_TessAngDefl,  //!< Angular deflection of visualization tessellation.
  //------------------//
  // Presentation     //
  //------------------//
    PID_GroupPrs,     //!< Presentation group.
    PID_HasColor,     //!< Indicates whether the Color Parameter is active.
    PID_Color,        //!< Color.
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

  asiData_EXPORT TopoDS_Shape
    GetShape() const;

  asiData_EXPORT void
    SetShape(const TopoDS_Shape& shape);

  asiData_EXPORT void
    SetLinearDeflection(const double);

  asiData_EXPORT double
    GetLinearDeflection() const;

  asiData_EXPORT void
    SetAngularDeflection(const double);

  asiData_EXPORT double
    GetAngularDeflection() const;

  asiData_EXPORT void
    SetHasColor(const bool);

  asiData_EXPORT bool
    HasColor() const;

  asiData_EXPORT void
    SetColor(const int);

  asiData_EXPORT int
    GetColor() const;

// Initialization:
public:

  asiData_EXPORT void
    Init();

protected:

  //! Allocation is allowed only via Instance() method.
  asiData_EXPORT
    asiData_IVTopoItemNode();

};

#endif
