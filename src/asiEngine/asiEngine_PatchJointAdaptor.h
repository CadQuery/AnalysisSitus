//-----------------------------------------------------------------------------
// Created on: 28 June 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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

#ifndef asiEngine_PatchJointAdaptor_h
#define asiEngine_PatchJointAdaptor_h

// asiEngine includes
#include <asiEngine_Model.h>

// asiData includes
#include <asiData_ReEdgeNode.h>
#include <asiData_RePatchNode.h>

// asiAlgo includes
#include <asiAlgo_PatchJointAdaptor.h>

//-----------------------------------------------------------------------------

//! Adaptor for working with a joint edge between two reverse engineering
//! patches.
class asiEngine_PatchJointAdaptor : public asiAlgo_PatchJointAdaptor
{
public:

  //! Ctor.
  //! \param[in] model    Data Model instance.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiEngine_EXPORT
    asiEngine_PatchJointAdaptor(const Handle(asiEngine_Model)& model,
                                ActAPI_ProgressEntry           progress = NULL,
                                ActAPI_PlotterEntry            plotter  = NULL);

public:

  //! Initializes adaptor with joint edge.
  //! \param[in] edgeNode edge representing the joint.
  asiEngine_EXPORT bool
    Init(const Handle(asiData_ReEdgeNode)& edgeNode);

  asiEngine_EXPORT bool
    AlignEdges();

  //! Dumps the initialized topology near the joint curve to the plotter.
  asiEngine_EXPORT void
    DumpGraphically();

public:

  //! \return left patch.
  const Handle(asiData_RePatchNode)& GetPatchLeft() const
  {
    return m_patchLeft;
  }

  //! \return right patch.
  const Handle(asiData_RePatchNode)& GetPatchRight() const
  {
    return m_patchRight;
  }

  //! \return left coedge.
  const Handle(asiData_ReCoedgeNode)& GetCoedgeLeft() const
  {
    return m_coedgeLeft;
  }

  //! \return left-top coedge.
  const Handle(asiData_ReCoedgeNode)& GetCoedgeLeftTop() const
  {
    return m_coedgeLeftTop;
  }

  //! \return left-bottom coedge.
  const Handle(asiData_ReCoedgeNode)& GetCoedgeLeftBot() const
  {
    return m_coedgeLeftBot;
  }

  //! \return right coedge.
  const Handle(asiData_ReCoedgeNode)& GetCoedgeRight() const
  {
    return m_coedgeRight;
  }

  //! \return right-top coedge.
  const Handle(asiData_ReCoedgeNode)& GetCoedgeRightTop() const
  {
    return m_coedgeRightTop;
  }

  //! \return right-bottom coedge.
  const Handle(asiData_ReCoedgeNode)& GetCoedgeRightBot() const
  {
    return m_coedgeRightBot;
  }

protected:

  asiEngine_EXPORT bool
    alignControlPoles(Handle(Geom_BSplineCurve)& curveLeft,
                      const bool                 sameSenseLeft,
                      Handle(Geom_BSplineCurve)& curveRight,
                      const bool                 sameSenseRight);

  //! Prepares curve representation for the passed coedge.
  //! \param[in] coedgeNode coedge to prepare a curve representation for.
  //! \return curve representation for coedge.
  asiEngine_EXPORT Handle(Geom_Curve)
    getCoedgeCurveRepr(const Handle(asiData_ReCoedgeNode)& coedgeNode) const;

protected:

  Handle(asiEngine_Model)      m_model;          //!< Data Model instance.
  Handle(asiData_ReEdgeNode)   m_edge;           //!< Joint edge.
  //
  Handle(asiData_RePatchNode)  m_patchLeft;      //!< Patch on the left.
  Handle(asiData_ReCoedgeNode) m_coedgeLeft;     //!< Left coedge.
  Handle(asiData_ReCoedgeNode) m_coedgeLeftTop;  //!< Left-top coedge.
  Handle(asiData_ReCoedgeNode) m_coedgeLeftBot;  //!< Left-bottom coedge.
  //
  Handle(asiData_RePatchNode)  m_patchRight;     //!< Patch on the right.
  Handle(asiData_ReCoedgeNode) m_coedgeRight;    //!< Right coedge.
  Handle(asiData_ReCoedgeNode) m_coedgeRightTop; //!< Right-top coedge.
  Handle(asiData_ReCoedgeNode) m_coedgeRightBot; //!< Right-bottom coedge.

};

#endif
