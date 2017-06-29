//-----------------------------------------------------------------------------
// Created on: 09 February 2016
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

#ifndef asiData_DesignLawNode_h
#define asiData_DesignLawNode_h

// A-Situs includes
#include <asiData.h>

// A-Situs (calculus) includes
#include <asiAlgo_DesignLaw.h>

// Active Data includes
#include <ActData_BaseNode.h>

//-----------------------------------------------------------------------------
// Part Node
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiData_DesignLawNode, ActData_BaseNode)

//! Node representing design law in persistent fashion.
class asiData_DesignLawNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_DesignLawNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_DesignLawNode, Instance)

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
    PID_Poles,        //!< One-dimensional poles.
    PID_Knots,        //!< Knots.
    PID_Mults,        //!< Multiplicities.
    PID_Degree,       //!< Degree.
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

  asiData_EXPORT void
    SetPoles(const Handle(HRealArray)& poles);

  asiData_EXPORT Handle(HRealArray)
    GetPoles() const;

  asiData_EXPORT void
    SetKnots(const Handle(HRealArray)& knots);

  asiData_EXPORT Handle(HRealArray)
    GetKnots() const;

  asiData_EXPORT void
    SetMults(const Handle(HIntArray)& mults);

  asiData_EXPORT Handle(HIntArray)
    GetMults() const;

  asiData_EXPORT void
    SetDegree(const int& degree);

  asiData_EXPORT int
    GetDegree() const;

  asiData_EXPORT Handle(asiAlgo_DesignLaw)
    AsLaw() const;

// Initialization:
public:

  asiData_EXPORT void
    Init(const Handle(asiAlgo_DesignLaw)& law);

protected:

  //! Allocation is allowed only via Instance method.
  asiData_EXPORT
    asiData_DesignLawNode();

};

#endif
