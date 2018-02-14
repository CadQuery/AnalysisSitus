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

#ifndef asiEngine_Curve_h
#define asiEngine_Curve_h

// asiEngine includes
#include <asiEngine_Model.h>

// asiData includes
#include <asiData_CurvatureCombsNode.h>

//! Data Model API for operating with curves.
class asiEngine_Curve
{
public:

  //! ctor.
  //! \param[in] model    Data Model instance.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiEngine_Curve(const Handle(asiEngine_Model)& model,
                  ActAPI_ProgressEntry           progress = NULL,
                  ActAPI_PlotterEntry            plotter  = NULL)
  //
  : m_model(model), m_progress(progress), m_plotter(plotter) {}

public:

  //! Creates Data Node represening a single parametric curve.
  //! \param[in] name   name to set.
  //! \param[in] parent parent Data Node (e.g., this can be a Part Node).
  //! \return newly constructed Curve Node.
  asiEngine_EXPORT Handle(asiData_CurveNode)
    Create(const TCollection_ExtendedString& name,
           const Handle(ActAPI_INode)&       parent);

  //! Creates Data Node represening curvature combs.
  //! \param[in] parent      parent Data Node representing a curve.
  //! \param[in] scaleFactor scale factor.
  //! \param[in] points      discretization points.
  //! \param[in] params      discretization parameters.
  //! \param[in] curvatures  curvatures.
  //! \param[in] combs       curvature combs.
  //! \return newly constructed Curvature Combs Node.
  asiEngine_EXPORT Handle(asiData_CurvatureCombsNode)
    CreateCurvatureCombs(const Handle(asiData_CurveNode)& parent,
                         const double                     scaleFactor,
                         const std::vector<gp_Pnt>&       points,
                         const std::vector<double>&       params,
                         const std::vector<double>&       curvatures,
                         const std::vector<gp_Vec>&       combs);

  //! Creates Data Node represening curvature combs if it does not exist.
  //! If such Node already exists, it is updated.
  //! \param[in] parent      parent Data Node representing a curve.
  //! \param[in] scaleFactor scale factor.
  //! \param[in] points      discretization points.
  //! \param[in] params      discretization parameters.
  //! \param[in] curvatures  curvatures.
  //! \param[in] combs       curvature combs.
  //! \return newly constructed Curvature Combs Node.
  asiEngine_EXPORT Handle(asiData_CurvatureCombsNode)
    CreateOrUpdateCurvatureCombs(const Handle(asiData_CurveNode)& parent,
                                 const double                     scaleFactor,
                                 const std::vector<gp_Pnt>&       points,
                                 const std::vector<double>&       params,
                                 const std::vector<double>&       curvatures,
                                 const std::vector<gp_Vec>&       combs);

  //! Deletes all sub-Nodes for the given Curve Node.
  //! \param[in] parent parent Curve Node to clean the underlying objects for.
  asiEngine_EXPORT void
    Clean_All(const Handle(asiData_CurveNode)& parent);

protected:

  void _cleanChildren(const Handle(ActAPI_INode)& parent);

protected:

  Handle(asiEngine_Model) m_model;    //!< Data Model instance.
  ActAPI_ProgressEntry    m_progress; //!< Progress notifier.
  ActAPI_PlotterEntry     m_plotter;  //!< Imperative plotter.

};

#endif
