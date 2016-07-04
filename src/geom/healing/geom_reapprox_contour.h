//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_reapprox_contour_h
#define geom_reapprox_contour_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <Geom_Curve.hxx>
#include <gp_Lin.hxx>
#include <NCollection_Sequence.hxx>
#include <TColgp_HSequenceOfPnt.hxx>
#include <TopoDS_Wire.hxx>

#define BarrierAngleDeg 15

//! Contour re-approximation (healing) is a common tool which is designed to
//! fix the following problems in the input wires:
//!
//! - Too many edges (e.g. a very fine chain of straight line segments);
//! - Self-interactions (overlapping and mutual intersections).
//!
//! In all our modeling routines where some wire is used as an input, contour
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
class geom_reapprox_contour
{
public:

  struct Segment
  {
    Segment() : HasPrincipal(false) {}

    bool                          HasPrincipal;
    gp_Lin                        Principal;
    Handle(TColgp_HSequenceOfPnt) Pts;
  };

public:

  ASitus_EXPORT
    geom_reapprox_contour(const TopoDS_Shape& Contour,
                           const double       precision,
                           const double       barrierAngleDeg = BarrierAngleDeg);

public:

  //! \return center point of the contour.
  inline const gp_Pnt& Center() const
  {
    return m_center;
  }

  //! \return contour orientation vector.
  inline const gp_Vec& Orientation() const
  {
    return m_ori;
  }

public:

  ASitus_EXPORT bool
    operator()(TopoDS_Wire&         Wire,
               const bool           useSegments,
               const bool           useAccumulatedAngle,
               ActAPI_ProgressEntry Journal);

protected:

  ASitus_EXPORT bool
    doSegmentation(NCollection_Sequence<Segment>& Segments,
                   const bool                     useSegments,
                   const bool                     useAccumulatedAngle,
                   ActAPI_ProgressEntry           Journal);

  ASitus_EXPORT bool
    doApprox(const NCollection_Sequence<Segment>&      Segments,
             NCollection_Sequence<Handle(Geom_Curve)>& Curves,
             ActAPI_ProgressEntry                      Journal);

  ASitus_EXPORT bool
    doWire(const NCollection_Sequence<Handle(Geom_Curve)>& Curves,
           TopoDS_Wire&                                    W,
           ActAPI_ProgressEntry                            Journal) const;

private:

  ASitus_EXPORT void
    makeCoarser(const TColgp_SequenceOfPnt& source,
                const double                resolution,
                TColgp_SequenceOfPnt&       result) const;

private:

  TopoDS_Shape m_contour;       //!< Original contour.
  double       m_fPrec;         //!< Precision;
  double       m_fBarrierAngle; //!< Barrier angle for segmentation.
  gp_Pnt       m_center;        //!< Center point of the contour.
  gp_Vec       m_ori;           //!< Orientation vector.
  bool         m_bClosed;       //!< Indicates whether the contour is closed.

};

#endif
