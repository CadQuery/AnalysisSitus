//-----------------------------------------------------------------------------
// Created on: 10 July 2017
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

#ifndef asiData_TriangulationNode_h
#define asiData_TriangulationNode_h

// asiData includes
#include <asiData.h>

// Active Data includes
#include <ActData_BaseNode.h>
#include <ActData_TriangulationParameter.h>
#include <ActData_Utils.h>

//-----------------------------------------------------------------------------
// Mesh Node
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiData_TriangulationNode, ActData_BaseNode)

//! Node representing surface triangulation.
class asiData_TriangulationNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_TriangulationNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_TriangulationNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //-------------------//
  // Common            //
  //-------------------//
    PID_Name,          //!< Name of the Node.
  //-------------------//
  // Geometry          //
  //-------------------//
    PID_Triangulation, //!< Tessellation.
  //-------------------//
  // Options           //
  //-------------------//
    PID_Options,       //!< Array of options (application-specific).
  //-------------------//
  // Presentation      //
  //-------------------//
    PID_DisplayMode,   //!< Display mode.
    PID_HasColor,      //!< Indicates whether the Color Parameter is active.
    PID_Color,         //!< Color.
  //-------------------//
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

  asiData_EXPORT Handle(Poly_Triangulation)
    GetTriangulation() const;

  asiData_EXPORT void
    SetTriangulation(const Handle(Poly_Triangulation)& triangulation);

  asiData_EXPORT Handle(TColStd_HArray1OfReal)
    GetOptions() const;

  asiData_EXPORT void
    SetOptions(const Handle(TColStd_HArray1OfReal)& options);

  asiData_EXPORT void
    SetHasColor(const bool hasColor) const;

  asiData_EXPORT bool
    HasColor() const;

  asiData_EXPORT void
    SetColor(const int color) const;

  asiData_EXPORT int
    GetColor() const;

  asiData_EXPORT void
    SetDisplayMode(const int mode) const;

  asiData_EXPORT int
    GetDisplayMode() const;

// Initialization:
public:

  asiData_EXPORT void
    Init();

protected:

  //! Allocation is allowed only via Instance method.
  asiData_EXPORT
    asiData_TriangulationNode();

};

#endif
