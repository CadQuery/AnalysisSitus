//-----------------------------------------------------------------------------
// Created on: 06 October 2018
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

#ifndef asiData_RePatchNode_h
#define asiData_RePatchNode_h

// asiData includes
#include <asiData.h>

// Active Data includes
#include <ActData_BaseNode.h>

// OCCT includes
#include <Geom_Surface.hxx>

//! Data Node representing topological patch for reverse engineering.
class asiData_RePatchNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_RePatchNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_RePatchNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //-------------------//
    PID_Name,          //!< Name of the Node.
  //-------------------//
  // Geometry          //
  //-------------------//
    PID_GroupGeometry, //!< Group of geometric props.
    PID_Surface,       //!< Stored surface.
    PID_MinNumKnots,   //!< Min number of intermediate knots.
    PID_ApproxNodes,   //!< Indicates whether to approximate inner mesh nodes.
    PID_ApproxLambda,  //!< Fairing coefficient for approximation.
  //-------------------//
  // Presentation      //
  //-------------------//
    PID_GroupPrs,      //!< Presentation group.
    PID_HasColor,      //!< Indicates whether the Color Parameter is active.
    PID_Color,         //!< Color.
    PID_EnablePlotter, //!< Indicates whether to enable imperative plotter.
  //-------------------//
  // Tree Functions    //
  //-------------------//
    PID_FuncBuildSurf, //!< Tree Function to build a surface.
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

// Initialization:
public:

  asiData_EXPORT void
    Init();

public:

  asiData_EXPORT Handle(Geom_Surface)
    GetSurface() const;

  asiData_EXPORT void
    SetSurface(const Handle(Geom_Surface)& surface);

  asiData_EXPORT int
    GetMinNumKnots() const;

  asiData_EXPORT void
    SetMinNumKnots(const int numKnots);

  asiData_EXPORT bool
    GetApproxNodes() const;

  asiData_EXPORT void
    SetApproxNodes(const bool on);

  asiData_EXPORT double
    GetApproxLambda() const;

  asiData_EXPORT void
    SetApproxLambda(const double lambda);

  asiData_EXPORT void
    SetHasColor(const bool);

  asiData_EXPORT bool
    HasColor() const;

  asiData_EXPORT void
    SetColor(const int);

  asiData_EXPORT int
    GetColor() const;

  asiData_EXPORT void
    SetEnablePlotter(const bool);

  asiData_EXPORT bool
    HasEnabledPlotter() const;

protected:

  //! Allocation is allowed only via Instance() method.
  asiData_EXPORT
    asiData_RePatchNode();

};

#endif
