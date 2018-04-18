//-----------------------------------------------------------------------------
// Created on: 18 April 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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

#ifndef asiData_TolerantRangeNode_h
#define asiData_TolerantRangeNode_h

// A-Situs includes
#include <asiData.h>

// Active Data includes
#include <ActData_BaseNode.h>
#include <ActData_ShapeParameter.h>

//! Node representing shapes with tolerances within a certain range.
class asiData_TolerantRangeNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_TolerantRangeNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_TolerantRangeNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
  //------------------//
  // Geometry         //
  //------------------//
    PID_Shape,        //!< Tolerant shapes (packed in a compound).
    PID_TolerMin,     //!< Min tolerance.
    PID_TolerMax,     //!< Max tolerance.
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

// Initialization:
public:

  asiData_EXPORT void
    Init();

// Convenience methods:
public:

  asiData_EXPORT TopoDS_Shape
    GetShape() const;

  asiData_EXPORT void
    SetShape(const TopoDS_Shape& shape);

  asiData_EXPORT double
    GetMinToler() const;

  asiData_EXPORT void
    SetMinToler(const double value);

  asiData_EXPORT double
    GetMaxToler() const;

  asiData_EXPORT void
    SetMaxToler(const double value);

protected:

  //! Allocation is allowed only via Instance method.
  asiData_EXPORT
    asiData_TolerantRangeNode();

};

#endif
