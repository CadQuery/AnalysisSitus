//-----------------------------------------------------------------------------
// Created on: 10 July 2017
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

#ifndef asiData_TriangulationNode_h
#define asiData_TriangulationNode_h

// asiData includes
#include <asiData_OctreeNode.h>

// asiAlgo includes
#include <asiAlgo_BVHFacets.h>

// Active Data includes
#include <ActData_BaseNode.h>
#include <ActData_TriangulationParameter.h>
#include <ActData_Utils.h>

//-----------------------------------------------------------------------------
// Mesh Node
//-----------------------------------------------------------------------------

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
    PID_BVH,           //!< BVH structure.
  //-------------------//
  // Options           //
  //-------------------//
    PID_Options,       //!< Array of options (application-specific).
  //-------------------//
  // Presentation      //
  //-------------------//
    PID_GroupPrs,      //!< Presentation group.
    PID_DisplayMode,   //!< Display mode.
    PID_UseScalars,    //!< Indicates whether to use scalars for coloring.
    PID_Color,         //!< Color.
    PID_HasVertices,   //!< Indicates whether vertices are active.
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

  asiData_EXPORT Handle(ActData_TriangulationParameter)
    GetTriangulationParam() const;

  asiData_EXPORT Handle(Poly_Triangulation)
    GetTriangulation() const;

  asiData_EXPORT void
    SetTriangulation(const Handle(Poly_Triangulation)& triangulation);

  asiData_EXPORT Handle(asiAlgo_BVHFacets)
    GetBVH() const;

  asiData_EXPORT void
    SetBVH(const Handle(asiAlgo_BVHFacets)&);

  asiData_EXPORT Handle(TColStd_HArray1OfReal)
    GetOptions() const;

  asiData_EXPORT void
    SetOptions(const Handle(TColStd_HArray1OfReal)& options);

  asiData_EXPORT void
    SetUseScalars(const bool);

  asiData_EXPORT bool
    GetUseScalars() const;

  asiData_EXPORT void
    SetColor(const int color) const;

  asiData_EXPORT int
    GetColor() const;

  asiData_EXPORT void
    SetDisplayMode(const int mode) const;

  asiData_EXPORT int
    GetDisplayMode() const;

  asiData_EXPORT void
    SetHasVertices(const bool);

  asiData_EXPORT bool
    HasVertices() const;

public:

  asiData_EXPORT Handle(asiData_OctreeNode)
    GetOctree() const;

// Initialization:
public:

  asiData_EXPORT void
    Init();

protected:

  //! Allocation is allowed only via Instance() method.
  asiData_EXPORT
    asiData_TriangulationNode();

};

#endif
