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

#ifndef asiAlgo_ReapproxContour_HeaderFile
#define asiAlgo_ReapproxContour_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <Geom_Curve.hxx>
#include <gp_Lin.hxx>
#include <NCollection_Sequence.hxx>
#include <TColgp_HSequenceOfPnt.hxx>
#include <TopoDS_Wire.hxx>

//! Contour re-approximation (healing) is a common tool which is designed to
//! fix the following problems in the input wires (representing contours, holes
//! and feature lines):
//!
//! - Too many edges (e.g. a very fine chain of straight line segments);
//! - Self-interactions (overlapping and mutual intersections).
//!
//! In all modeling routines where some wire is used as an input, contour
//! healing can be necessary to improve performance and quality of the result.
//! Such healing is a three-stage process. It consists of the following phases:
//!
//! - Segmentation,
//! - Re-approximation,
//! - Join.
//!
//! Segmentation is used to break down the contour onto the pieces with
//! distinct principal directions. To do this, the input wire is exploded to
//! edges and discretized with a prescribed deflection. All near-coincident
//! points are removed to get rid of coarse self-interactions. The principal
//! directions are used for monotonic parameterization which allows elimination
//! of loops and small self-interactions at re-approximation stage. Join
//! operation is done in order to assemble a connected wire once all pieces
//! are re-approximated with respect to their principal directions. If the
//! principal directions cannot be detected (e.g. for G1 holes), the contour
//! healing procedure still can be used to eliminate small edges.
class asiAlgo_ReapproxContour : public ActAPI_IAlgorithm
{
public:

  //! Auxiliary structure representing a single segment.
  struct t_segment
  {
    t_segment() : HasPrincipal(false) {} //!< Default ctor.

    bool                          HasPrincipal;
    gp_Lin                        Principal;
    Handle(TColgp_HSequenceOfPnt) Pts;
  };

public:

  //! Ctor.
  //! \param[in] contour         input contour to reapproximate.
  //! \param[in] precision       precision to use.
  //! \param[in] barrierAngleDeg barrier angle for segmentation.
  //! \param[in] progress        progress notifier.
  //! \param[in] plotter         imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_ReapproxContour(const TopoDS_Shape&  contour,
                            const double         precision,
                            const double         barrierAngleDeg = 5.0,
                            ActAPI_ProgressEntry progress        = NULL,
                            ActAPI_PlotterEntry  plotter         = NULL);

public:

  //! \return center point of the contour.
  const gp_Pnt& GetCenter() const
  {
    return m_center;
  }

  //! \return contour orientation vector.
  const gp_Vec& GetOrientation() const
  {
    return m_ori;
  }

public:

  //! Performs reapproximation.
  //! \param[in]  useSegments         indicates whether to use segmentation of wire by edges.
  //! \param[in]  useAccumulatedAngle indicates whether to use accumulated angle for segmentation.
  //! \param[out] wire                resulting wire (reapproximated contour).
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    operator()(const bool   useSegments,
               const bool   useAccumulatedAngle,
               TopoDS_Wire& wire);

protected:

  //! Performs segmentation of the original contour onto point sets with
  //! associated principal directions.
  //! \param[in]  useSegments         indicates whether to use segmentation of wire by edges.
  //! \param[in]  useAccumulatedAngle indicates whether to use accumulated angle for segmentation.
  //! \param[out] segments            segmentation result.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    doSegmentation(const bool              useSegments,
                   const bool              useAccumulatedAngle,
                   std::vector<t_segment>& segments);

  //! Approximates the extracted segments using projection to principals for
  //! parameterization.
  //! \param[in]  segments point sets to approximate.
  //! \param[out] curves   approximation results (one-to-one for each segment).
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    doApprox(const std::vector<t_segment>&    segments,
             std::vector<Handle(Geom_Curve)>& curves);

  //! Prepares wire for the given collection of curves.
  //! \param[in]  curves curves to make edges and subsequently a wire from.
  //! \param[out] wire   resulting wire.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    doWire(const std::vector<Handle(Geom_Curve)>& curves,
           TopoDS_Wire&                           wire) const;

private:

  //! Makes the passed contour coarser in sense that it loses the points whose
  //! distances from each other are too small with respect to the given chord
  //! length.
  //! \param[in]  source     source points.
  //! \param[in]  resolution max allowed chord length.
  //! \param[out] result     resulting points.
  asiAlgo_EXPORT void
    makeCoarser(const TColgp_SequenceOfPnt& source,
                const double                resolution,
                TColgp_SequenceOfPnt&       result) const;

private:

  TopoDS_Shape m_contour;          //!< Original contour.
  double       m_fPrec;            //!< Precision;
  double       m_fBarrierAngleDeg; //!< Barrier angle for segmentation.
  gp_Pnt       m_center;           //!< Center point of the contour.
  gp_Vec       m_ori;              //!< Orientation vector.
  bool         m_bClosed;          //!< Indicates whether the contour is closed.

};

#endif
