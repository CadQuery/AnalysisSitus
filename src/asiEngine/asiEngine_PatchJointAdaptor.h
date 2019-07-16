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

#ifndef asiAlgo_PatchJointAdaptor_h
#define asiAlgo_PatchJointAdaptor_h

// asiAlgo includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>

//-----------------------------------------------------------------------------

//! Adaptor for working with a joint curve between two surface patches in
//! natural bounds.
class asiAlgo_PatchJointAdaptor : public ActAPI_IAlgorithm
{
public:

  //! Ctor.
  //! \param[in] curve     curve representing the joint.
  //! \param[in] surfLeft  surface "on the left" if looking traveling along the
  //!                      joint curve in the direction of its natural
  //!                      orientation.
  //! \param[in] surfRight surface "on the right" if looking traveling along the
  //!                      joint curve in the direction of its natural
  //!                      orientation.
  //! \param[in] progress  progress notifier.
  //! \param[in] plotter   imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_PatchJointAdaptor(const Handle(Geom_Curve)&          curve,
                              const Handle(Geom_BSplineSurface)& surfLeft,
                              const Handle(Geom_BSplineSurface)& surfRight,
                              ActAPI_ProgressEntry               progress = NULL,
                              ActAPI_PlotterEntry                plotter  = NULL);

public:

  //! Determines the isoparametric lines of both surfaces which correspond
  //! to the joint curve.
  //! \param[out] isoLeft     isoparametric curve of the left surface.
  //! \param[out] isoLeftU    indicates if the returned left isoline is U-iso.
  //! \param[out] isoLeftMin  indicates if the left isoline corresponds to
  //!                         min parametric value.
  //! \param[out] isoRight    isoparametric curve of the right surface.
  //! \param[out] isoRightU   indicates if the returned right isoline is U-iso.
  //! \param[out] isoRightMin indicates if the right isoline corresponds to
  //!                         min parametric value.
  //! \param[out] areOpposite indicates whether the extracted isos are opposite
  //!                         to each other.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    ExtractIsos(Handle(Geom_BSplineCurve)& isoLeft,
                bool&                      isoLeftU,
                bool&                      isoLeftMin,
                Handle(Geom_BSplineCurve)& isoRight,
                bool&                      isoRightU,
                bool&                      isoRightMin,
                bool&                      areOpposite) const;

  //! Makes the surfaces compatible along their joint curve by knot insertion.
  //! \param[in] isoLeft   isoparametric curve of the left surface.
  //! \param[in] isoLeftU  indicates if the left isoline is U-iso.
  //! \param[in] isoRight  isoparametric curve of the right surface.
  //! \param[in] isoRightU indicates if the right isoline is U-iso.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    UnifySurfaces(const Handle(Geom_BSplineCurve)& isoLeft,
                  const bool                       isoLeftU,
                  const Handle(Geom_BSplineCurve)& isoRight,
                  const bool                       isoRightU);

  //! Checks if the coincident border between the surface patches is
  //! well prepared for smoothing. This method checks that the corner poles
  //! are coincident.
  //! \param[in] isoLeft     isoparametric curve of the left surface.
  //! \param[in] isoRight    isoparametric curve of the right surface.
  //! \param[in] areOpposite indicates whether the passed isos have opposite
  //!                        directions.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    IsJointCompatible(const Handle(Geom_BSplineCurve)& isoLeft,
                      const Handle(Geom_BSplineCurve)& isoRight,
                      const bool                       areOpposite) const;

  //! Aligns control points of the left and right surface patches so that to
  //! ensure C1 continuity (i.e., the tangency vectors and their modules are
  //! equal).
  //! \param[out] isoLeft     isoparametric curve of the left surface.
  //! \param[out] isoLeftU    indicates if the left isoline is U-iso.
  //! \param[out] isoLeftMin  indicates if the left isoline corresponds to
  //!                         min parametric value.
  //! \param[out] isoRight    isoparametric curve of the right surface.
  //! \param[out] isoRightU   indicates if the right isoline is U-iso.
  //! \param[out] isoRightMin indicates if the right isoline corresponds to
  //!                         min parametric value.
  //! \param[out] areOpposite indicates whether the passed isos have
  //!                         opposite directions. If so, this method will
  //!                         traverse the control points of the second patch
  //!                         in the reversed order compared to the first patch.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    AlignControlPoles(const Handle(Geom_BSplineCurve)& isoLeft,
                      const bool                       isoLeftU,
                      const bool                       isoLeftMin,
                      const Handle(Geom_BSplineCurve)& isoRight,
                      const bool                       isoRightU,
                      const bool                       isoRightMin,
                      const bool                       areOpposite);

public:

  //! \return left surface.
  const Handle(Geom_BSplineSurface)& GetSurfaceLeft() const
  {
    return m_surfLeft;
  }

  //! \return right surface.
  const Handle(Geom_BSplineSurface)& GetSurfaceRight() const
  {
    return m_surfRight;
  }

protected:

  //! Determines the isoparametric line hich corresponds to the joint curve
  //! for the passed surface 
  //! \param[in]  surf   surface in question.
  //! \param[out] iso    isoparametric curve on the surface.
  //! \param[out] isoU   indicates if the returned isoline is U-iso.
  //! \param[out] isoMin indicates if the returned isoline corresponds to the
  //!                    min parameter value.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    getIso(const Handle(Geom_BSplineSurface)& surf,
           Handle(Geom_BSplineCurve)&         iso,
           bool&                              isoU,
           bool&                              isoMin) const;

protected:

  Handle(Geom_Curve)          m_curve;     //!< Joint curve.
  Handle(Geom_BSplineSurface) m_surfLeft;  //!< Surface on the left.
  Handle(Geom_BSplineSurface) m_surfRight; //!< Surface on the right.

};

#endif
