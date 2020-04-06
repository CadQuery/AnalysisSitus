//-----------------------------------------------------------------------------
// Created on: 03 April 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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

#ifndef asiData_ThicknessNode_h
#define asiData_ThicknessNode_h

// asiData includes
#include <asiData.h>

// asiAlgo includes
#include <asiAlgo_Mesh.h>

// Active Data includes
#include <ActData_BaseNode.h>
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! This object holds the results of thickness check.
class asiData_ThicknessNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_ThicknessNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_ThicknessNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //--------------------------//
    PID_Name,                 //!< Name of the Node.
    PID_Mesh,                 //!< Mesh where thickness distribution was computed.
    PID_ThicknessFieldIds,    //!< Element ids to store thickness field.
    PID_ThicknessFieldValues, //!< Scalar values associated with the element ids.
    PID_ScalarMin,            //!< Min scalar value.
    PID_ScalarMax,            //!< Max scalar value.
    PID_IsCustomDir,          //!< Whether custom direction should be used.
    PID_Dx,                   //!< Direction in OX.
    PID_Dy,                   //!< Direction in OY.
    PID_Dz,                   //!< Direction in OZ.
    PID_CheckThicknessFunc,   //!< Tree Function to check thickness.
  //--------------------------//
    PID_Last = PID_Name + ActData_BaseNode::RESERVED_PARAM_RANGE
  };

public:

  //! Returns new DETACHED instance of the Node ensuring its correct
  //! allocation in a heap.
  //! \return new instance of the Node.
  static Handle(ActAPI_INode) Instance()
  {
    return new asiData_ThicknessNode();
  }

// Initialization:
public:

  //! Performs initial actions required to make the Node WELL-FORMED.
  asiData_EXPORT void
    Init();

// Generic naming support:
public:

  //! Accessor for the Node's name.
  //! \return name of the Node.
  asiData_EXPORT virtual TCollection_ExtendedString
    GetName();

  //! Sets name for the Node.
  //! \param[in] N name to set.
  asiData_EXPORT virtual void
    SetName(const TCollection_ExtendedString& name);

public:

  //! Sets mesh without a scalar field.
  //! \param[in] mesh mesh to store.
  asiData_EXPORT void
    SetMesh(const Handle(Poly_Triangulation)& mesh);

  //! Stores mesh with associated thickness field scalars.
  //! \param[in] mesh mesh and fields to store.
  asiData_EXPORT void
    SetMeshWithScalars(const asiAlgo_Mesh& mesh);

protected:

  //! Default constructor. Registers all involved Parameters.
  //! Allocation is allowed only via Instance() method.
  asiData_EXPORT
    asiData_ThicknessNode();

};

#endif
