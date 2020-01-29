//-----------------------------------------------------------------------------
// Created on: 26 December 2018
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

#ifndef asiAlgo_BuildCoonsSurf_h
#define asiAlgo_BuildCoonsSurf_h

// asiAlgo includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>

//-----------------------------------------------------------------------------

//! Utility to build Coons (4-sided) surfaces in form of B-surfaces.
class asiAlgo_BuildCoonsSurf : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_BuildCoonsSurf, ActAPI_IAlgorithm)

public:

  //! Ctor.
  //! \param[in] c0       curve "on the left".
  //! \param[in] c1       curve "on the right".
  //! \param[in] b0       curve "on the bottom".
  //! \param[in] b1       curve "on the top".
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_BuildCoonsSurf(const Handle(Geom_BSplineCurve)& c0,
                           const Handle(Geom_BSplineCurve)& c1,
                           const Handle(Geom_BSplineCurve)& b0,
                           const Handle(Geom_BSplineCurve)& b1,
                           ActAPI_ProgressEntry             progress = nullptr,
                           ActAPI_PlotterEntry              plotter  = nullptr);

public:

  //! Fills the contour surrounded by the passed curves.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Perform();

public:

  //! Sets curve `c0`.
  //! \param[in] c0 curve to set.
  void SetC0(const Handle(Geom_BSplineCurve)& c0)
  {
    m_c0 = c0;
  }

  //! \return curve `c0`.
  const Handle(Geom_BSplineCurve)& GetC0() const
  {
    return m_c0;
  }

  //! Sets curve `c1`.
  //! \param[in] c1 curve to set.
  void SetC1(const Handle(Geom_BSplineCurve)& c1)
  {
    m_c1 = c1;
  }

  //! \return curve `c1`.
  const Handle(Geom_BSplineCurve)& GetC1() const
  {
    return m_c1;
  }

  //! Sets curve `b0`.
  //! \param[in] b0 curve to set.
  void SetB0(const Handle(Geom_BSplineCurve)& b0)
  {
    m_b0 = b0;
  }

  //! \return curve `b0`.
  const Handle(Geom_BSplineCurve)& GetB0() const
  {
    return m_b0;
  }

  //! Sets curve `b1`.
  //! \param[in] b1 curve to set.
  void SetB1(const Handle(Geom_BSplineCurve)& b1)
  {
    m_b1 = b1;
  }

  //! \return curve `b1`.
  const Handle(Geom_BSplineCurve)& GetB1() const
  {
    return m_b1;
  }

  //! \return resulting surface.
  const Handle(Geom_BSplineSurface)& GetResult() const
  {
    return m_result;
  }

protected:

  //! Constructs a bilinear surface patch.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    performBilinear();

protected:

  Handle(Geom_BSplineCurve)   m_c0;     //!< Curve "on the left".
  Handle(Geom_BSplineCurve)   m_c1;     //!< Curve "on the right".
  Handle(Geom_BSplineCurve)   m_b0;     //!< Curve "on the bottom".
  Handle(Geom_BSplineCurve)   m_b1;     //!< Curve "on the top".
  Handle(Geom_BSplineSurface) m_result; //!< Resulting surface.

};

#endif
