//-----------------------------------------------------------------------------
// Created on: 17 February 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
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

#ifndef asiAlgo_ModBase_h
#define asiAlgo_ModBase_h

// asiAlgo includes
#include <asiAlgo_BRepNormalization.h>
#include <asiAlgo_IntersectionCurveSS.h>
#include <asiAlgo_IntersectionPointCC.h>
#include <asiAlgo_Utils.h>

// Active Data includes
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

//! Base class for geometric modifications driven by BRepTools_Modifier.
//! All such modifications are essentially homeomorphisms as they cannot
//! affect topology of a CAD model.
class asiAlgo_ModBase : public asiAlgo_BRepNormalization
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_ModBase, asiAlgo_BRepNormalization)

public:

  //! Constructor.
  //! \param[in] notifier progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_ModBase(ActAPI_ProgressEntry notifier,
                    ActAPI_PlotterEntry  plotter);

public:

  //! Initializes Modification.
  //! \param[in] target shape being modified.
  //! \return true in case of success, false -- otherwise.
  virtual bool
    Init(const TopoDS_Shape& target) = 0;

protected:

  asiAlgo_EXPORT bool
    intersectSurfaces(const TopoDS_Face&                   F1,
                      const TopoDS_Face&                   F2,
                      Handle(asiAlgo_IntersectionCurveSS)& res) const;

  asiAlgo_EXPORT bool
    intersectCurves(const Handle(Geom_Curve)&            C1,
                    const double                         f1,
                    const double                         l1,
                    const Handle(Geom_Curve)&            C2,
                    const double                         f2,
                    const double                         l2,
                    Handle(asiAlgo_IntersectionPointCC)& res) const;

  asiAlgo_EXPORT bool
    correctOriC(const Handle(Geom_Curve)& newCurve,
                const TopoDS_Edge&        oldEdge,
                Handle(Geom_Curve)&       correctedCurve) const;

protected:

  mutable ActAPI_ProgressEntry m_progress; //!< Progress notifier.
  mutable ActAPI_PlotterEntry  m_plotter;  //!< Imperative plotter.

};

#endif
