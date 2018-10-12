//-----------------------------------------------------------------------------
// Created on: 09 February 2016
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

  //! Allocation is allowed only via Instance() method.
  asiData_EXPORT
    asiData_DesignLawNode();

};

#endif
