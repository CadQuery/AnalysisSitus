//-----------------------------------------------------------------------------
// Created on: 05 March 2018
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

#ifndef asiAlgo_FairBCurve_HeaderFile
#define asiAlgo_FairBCurve_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <Geom_BSplineCurve.hxx>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

//! Fairing algorithm for B-spline curves.
class asiAlgo_FairBCurve : public ActAPI_IAlgorithm
{
public:

  //! ctor.
  //! \param[in] curve    B-spline curve in question (the one to fair).
  //! \param[in] lambda   fairing coefficient.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_FairBCurve(const Handle(Geom_BSplineCurve)& curve,
                       const double                     lambda,
                       ActAPI_ProgressEntry             progress,
                       ActAPI_PlotterEntry              plotter);

public:

  //! Performs fairing.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Perform();

public:

  //! \return resulting curve.
  Handle(Geom_BSplineCurve) GetResult() const
  {
    return m_resultCurve;
  }

protected:

  //! Curve to fair.
  Handle(Geom_BSplineCurve) m_inputCurve;

  //! Result of fairing.
  Handle(Geom_BSplineCurve) m_resultCurve;

  //! Fairing coefficient.
  double m_fLambda;

};

#endif
