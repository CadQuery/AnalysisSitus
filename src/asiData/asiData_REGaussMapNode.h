//-----------------------------------------------------------------------------
// Created on: 25 April 2017
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

#ifndef asiData_REGaussMapNode_h
#define asiData_REGaussMapNode_h

// asiData includes
#include <asiData.h>

// asiAlgo includes
#include <asiAlgo_BaseCloud.h>

// Active Data includes
#include <ActData_BaseNode.h>

//-----------------------------------------------------------------------------
// Gauss map for Reverse Engineering
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiData_REGaussMapNode, ActData_BaseNode)

//! \todo move this Node to ABR.
//! Node representing Gauss map.
class asiData_REGaussMapNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_REGaussMapNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_REGaussMapNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
  //------------------//
    PID_Vectors,      //!< Vectors (array of coordinates).
    PID_AxisX,        //!< X coordinate of a central axis.
    PID_AxisY,        //!< Y coordinate of a central axis.
    PID_AxisZ,        //!< Z coordinate of a central axis.
    PID_RefinedAxisX, //!< X coordinate of a refined axis.
    PID_RefinedAxisY, //!< Y coordinate of a refined axis.
    PID_RefinedAxisZ, //!< Z coordinate of a refined axis.
  //------------------//
    PID_Last = PID_Name + ActData_BaseNode::RESERVED_PARAM_RANGE
  };

public:

  asiData_EXPORT static Handle(ActAPI_INode)
    Instance();

// Initialization:
public:

  asiData_EXPORT void
    Init();

// Generic naming support:
public:

  asiData_EXPORT virtual TCollection_ExtendedString
    GetName();

  asiData_EXPORT virtual void
    SetName(const TCollection_ExtendedString& name);

// Handy accessors to the stored data:
public:

  asiData_EXPORT gp_Vec
    GetVector(const int zeroBasedIndex) const;

  asiData_EXPORT Handle(asiAlgo_BaseCloud<double>)
    GetVectors() const;

  asiData_EXPORT Handle(asiAlgo_BaseCloud<float>)
    GetVectorsf() const;

  asiData_EXPORT void
    SetVectors(const Handle(asiAlgo_BaseCloud<double>)& vectors);

  asiData_EXPORT void
    SetVectorsf(const Handle(asiAlgo_BaseCloud<float>)& vectors);

  asiData_EXPORT bool
    IsEmpty() const;

  asiData_EXPORT void
    SetCentralAxis(const double x, const double y, const double z);

  asiData_EXPORT void
    GetCentralAxis(double& x, double& y, double& z) const;

  asiData_EXPORT void
    SetRefinedAxis(const double x, const double y, const double z);

  asiData_EXPORT void
    GetRefinedAxis(double& x, double& y, double& z) const;

protected:

  //! Allocation is allowed only via Instance method.
  asiData_EXPORT
    asiData_REGaussMapNode();

};

#endif
