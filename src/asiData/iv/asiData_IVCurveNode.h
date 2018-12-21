//-----------------------------------------------------------------------------
// Created on: 08 April 2016
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

#ifndef asiData_IVCurveNode_h
#define asiData_IVCurveNode_h

// asiData includes
#include <asiData.h>

// Active Data includes
#include <ActData_BaseNode.h>

// OCCT includes
#include <Geom_Curve.hxx>

//-----------------------------------------------------------------------------
// Data Node representing a single curve in IV (Imperative Viewer)
//-----------------------------------------------------------------------------

//! Data Node representing a single curve in IV (Imperative Viewer).
class asiData_IVCurveNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_IVCurveNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_IVCurveNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
  //------------------//
    PID_Geometry,     //!< Stored geometry.
    PID_Handles,      //!< On-curve handles for interaction.
    PID_ActiveHandle, //!< Index of the currently active handle.
  //------------------//
    PID_Last = PID_Name + ActData_BaseNode::RESERVED_PARAM_RANGE
  };

public:

  //! Returns new DETACHED instance of the Node ensuring its correct
  //! allocation in a heap.
  //! \return new instance of the Node.
  asiData_EXPORT static Handle(ActAPI_INode)
    Instance();

// Generic naming support:
public:

  //! Accessor for the Node's name.
  //! \return name of the Node.
  asiData_EXPORT virtual TCollection_ExtendedString
    GetName();

  //! Sets name for the Node.
  //! \param[in] name name to set.
  asiData_EXPORT virtual void
    SetName(const TCollection_ExtendedString& name);

// Handy accessors to the stored data:
public:

  //! Returns the stored geometry.
  //! \return stored geometry.
  asiData_EXPORT Handle(Geom_Curve)
    GetCurve() const;

  //! Returns the stored geometry.
  //! \param[out] f first parameter.
  //! \param[out] l last parameter.
  //! \return stored geometry.
  asiData_EXPORT Handle(Geom_Curve)
    GetCurve(double& f, double& l) const;

  //! Sets curve to store.
  //! \param[in] curve geometry to store.
  //! \param[in] f     first parameter of the curve.
  //! \param[in] l     last parameter of the curve.
  asiData_EXPORT void
    SetCurve(const Handle(Geom_Curve)& curve,
             const double              f,
             const double              l);

  //! Sets curve to store.
  //! \param[in] curve geometry to store.
  asiData_EXPORT void
    SetCurve(const Handle(Geom_Curve)& curve);

  //! Adds on-curve handle for the given parameter `u`.
  //! \param[in] u curve parameter to add as another handle.
  asiData_EXPORT void
    AddHandle(const double u);

  //! Sets array of handle parameters.
  //! \param[in] handles array to set.
  asiData_EXPORT void
    SetHandles(const Handle(HRealArray)& handles);

  //! \return stored on-curve handles (parameter values).
  asiData_EXPORT Handle(HRealArray)
    GetHandles() const;

  //! Sets index of the currently active handle.
  //! \param[in] handleId ID of the handle to set.
  asiData_EXPORT void
    SetActiveHandle(const int handleId);

  //! \return 0-based index of the currently active handle.
  asiData_EXPORT int
    GetActiveHandle() const;

  //! \return curve parameter corresponding to the active handle.
  asiData_EXPORT double
    GetActiveHandleParam() const;

// Initialization:
public:

  //! Performs initial actions required to make Node WELL-FORMED.
  asiData_EXPORT void
    Init();

protected:

  //! Allocation is allowed only via Instance() method.
  asiData_EXPORT
    asiData_IVCurveNode();

};

#endif
