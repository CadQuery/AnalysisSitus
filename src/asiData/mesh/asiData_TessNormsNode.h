//-----------------------------------------------------------------------------
// Created on: 05 October 2018
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

#ifndef asiData_TessNormsNode_h
#define asiData_TessNormsNode_h

// asiData includes
#include <asiData.h>

// asiAlgo includes
#include <asiAlgo_BaseCloud.h>

// Active Data includes
#include <ActData_BaseNode.h>

//! Data Node representing a normal field supplementing Tessellation Node.
//! Vector results are internally stored in the following way:
//!
//! 1. Array of involved element/node IDs:
//!
//! <pre>
//!    +-----+-----+-----+-----+-----+
//!    | ID1 | ID2 | ID3 | ... | IDn |
//!    +-----+-----+-----+-----+-----+
//! </pre>
//!
//! 2. Array of the corresponding triples of vector components:
//!
//! <pre>
//!    +-------+-------+-------+-----+-------+-------+-------+
//!    | F1{1} | F1{2} | F1{3} | ... | Fn{1} | Fn{2} | Fn{3} |
//!    +-------+-------+-------+-----+-------+-------+-------+
//! </pre>
class asiData_TessNormsNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_TessNormsNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_TessNormsNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
  //------------------//
    PID_IDs,          //!< IDs of the mesh elements/nodes having associated vectors.
    PID_Vectors,      //!< Actual vector coordinates bound to the mesh elements.
    PID_IsElemental,  //!< Indicates whether the vector field corresponds to elements.
  //------------------//
    PID_Last = PID_Name + ActData_BaseNode::RESERVED_PARAM_RANGE
  };

public:

  asiData_EXPORT static Handle(ActAPI_INode)
    Instance();

// Initialization:
public:

  asiData_EXPORT void
    Init(const Handle(HIntArray)&  ids,
         const Handle(HRealArray)& vectors);

// Generic naming support:
public:

  asiData_EXPORT virtual TCollection_ExtendedString
    GetName();

  asiData_EXPORT virtual void
    SetName(const TCollection_ExtendedString& name);

// Handy accessors to the stored data:
public:

  asiData_EXPORT void
    SetIDs(const Handle(HIntArray)& ids);

  asiData_EXPORT Handle(HIntArray)
    GetIDs() const;

  asiData_EXPORT void
    SetVectors(const Handle(HRealArray)& vectors);

  asiData_EXPORT Handle(HRealArray)
    GetVectors() const;

  asiData_EXPORT void
    SetIsElemental(const bool isElemental);

  asiData_EXPORT bool
    IsElemental() const;

protected:

  //! Allocation is allowed only via Instance() method.
  asiData_EXPORT
    asiData_TessNormsNode();

};

#endif
