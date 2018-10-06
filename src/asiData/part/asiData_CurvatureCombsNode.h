//-----------------------------------------------------------------------------
// Created on: 29 January 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2017-2018, Sergey Slyadnev
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

#ifndef asiData_CurvatureCombsNode_h
#define asiData_CurvatureCombsNode_h

// asiData includes
#include <asiData_CurveNode.h>

// Active Data includes
#include <ActData_BaseNode.h>
#include <ActData_ParameterFactory.h>

//! Data Node to store curvature combs.
class asiData_CurvatureCombsNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_CurvatureCombsNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_CurvatureCombsNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
    PID_Name,           //!< Name of the Node.
    PID_Points,         //!< Discretization points.
    PID_PointsStatuses, //!< Statuses of the discretization points.
    PID_Parameters,     //!< Parameter values corresponding to the discr. points.
    PID_Curvatures,     //!< Curvature values at discretization points.
    PID_Combs,          //!< Comb vectors.
    PID_RefCurve,       //!< Reference to Curve Node.
    PID_ScaleFactor,    //!< Scale factor.
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

  asiData_EXPORT void
    SetPoints(const std::vector<gp_Pnt>& points);

  asiData_EXPORT void
    GetPoints(std::vector<gp_Pnt>& points) const;

  asiData_EXPORT void
    SetPointsStatuses(const std::vector<bool>& statuses);

  asiData_EXPORT void
    GetPointsStatuses(std::vector<bool>& statuses) const;

  asiData_EXPORT void
    SetParameters(const std::vector<double>& params);

  asiData_EXPORT void
    GetParameters(std::vector<double>& params) const;

  asiData_EXPORT void
    SetCurvatures(const std::vector<double>& curvatures);

  asiData_EXPORT void
    GetCurvatures(std::vector<double>& curvatures) const;

  asiData_EXPORT void
    SetCombs(const std::vector<gp_Vec>& combs);

  asiData_EXPORT void
    GetCombs(std::vector<gp_Vec>& combs) const;

  asiData_EXPORT void
    SetScaleFactor(const double scaleFactor);

  asiData_EXPORT double
    GetScaleFactor() const;

public:

  //! \return referenced Curve Node.
  Handle(asiData_CurveNode) GetRefCurve() const
  {
    return Handle(asiData_CurveNode)::DownCast( ActParamTool::AsReference( this->Parameter(PID_RefCurve) )->GetTarget() );
  }

protected:

  //! Allocation is allowed only via Instance() method.
  asiData_EXPORT
    asiData_CurvatureCombsNode();

};

#endif
