//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_contour_capture_h
#define geom_contour_capture_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <BOPAlgo_Builder.hxx>
#include <TopoDS_Shell.hxx>

//! Contour Capture algorithm is based on Requicha's "generate-and-classify"
//! approach in combination with OCCT Boolean Operations (BOPs). The algorithm
//! lies in the pool of "boundary evaluation and merging" algorithms. It
//! consists of the following phases:
//!
//! - Imprint: make the input contour a part of the working shell;
//! - Selection: pick up the sub-shell bounded by the imprinted contour.
//!
//! The following requirements to the contour should be met in order for
//! imprint to end up with a good result:
//!
//! - Contour should be geometrically correct: no out-of-tolerance gaps and
//!   self-interactions should exist.
//! - Contour should be placed in a reasonable vicinity of the shell.
//!
//! To fit these requirements, the input contour can be pre-processed. If the
//! input contour contains unreasonably huge amount of edges (e.g. if it was
//! constructed as a chain of straight line segments), this becomes an
//! additional source of errors in the involved geometric computations.
//! Moreover, the accompanying overhead in elapsed CPU time becomes
//! unreasonably significant. Therefore, the contour should be not only healed,
//! but also simplified so as to reduce the number of the involved edges. It
//! is also important to keep the contour smooth as C0 geometry is generally
//! not recommended for in modeling context. On the other hand, if the input
//! contour is of good quality, the healing stage can be skipped.
//!
//! The input contour may lie at a certain distance from the shell surface.
//! This distance should be a priori estimated by the user (in average) in
//! order to give the algorithm a hint for its internal classification
//! routines. Normally, the reasonable tolerance value is around 0.1 [mm].
//! If imprint stage fails for the values greater than 1 [mm], it is highly
//! recommended to project a contour to the hull shape manually (using
//! specific projection tool). Contour projection is a relatively "heavy:
//! operation. Moreover, it is not always easy to select a projection
//! direction automatically, especially for the highly curved zones on a hull
//! (e.g. near bulb). Therefore, projection stage is not a part of the imprint
//! algorithm. However, if the input contour is not too far, it can still be
//! classified against the host faces and projected to them gradually (this
//! is done by the algorithm at the end).
//!
//! The second phase is a selection routine. It automatically picks up the
//! demanded part of the shell (the part bounded by the input contour) from the
//! immersed geometry. The idea is to pick up a point somewhere in the middle
//! of the target sub-shell and accumulate all those faces which are neighbor
//! to each other starting from the initial one. The process is finished once
//! the immersed contour is reached.
//!
//! Imprint stage performs injection of the contour topology into the topology
//! of a body. The implemented algorithm may use two alternative mechanisms for
//! imprint:
//!
//! - Generate-and-classify algorithm as a main working branch;
//! - General Fuse (basis of BOPs) for singularity cases.
//!
//! Generate-and-classify imprint is based on set membership classification
//! methods presented by Requicha and Tilove in their "classic" papers. These
//! approaches work well in cases when the edges of the input contour are not
//! overlapping (or almost overlapping) with the boundary edges of the body.
//! Classification algorithms are based on precise curve-curve intersections
//! rather than on computing interferences between tolerant volumes (like in
//! General Fuse). This peculiarity complicates processing of overlapping
//! situations, so if any singularity is detected (and cannot be escaped),
//! General Fuse algorithm is launched as an alternative solution. General
//! Fuse is not used alone (or in a main branch) because it offers less
//! control over precision and tolerance values. While Requicha's algorithm
//! depends mostly on curve-curve intersection in 3D (extrema), General Fuse
//! performs much more intensive job and works with precision values which are
//! not easy to calibrate. On the other hand, these sorts of algorithms
//! (tolerant BOPs) are known to be good for treatment of coincidence
//! situations.
class geom_contour_capture
{
public:

  ASitus_EXPORT
    geom_contour_capture();

public:

  ASitus_EXPORT void
    Init(const TopoDS_Shape& Body);

public:

  ASitus_EXPORT bool
    operator()(const TopoDS_Shape&  Contour,
               const double         tolerance,
               const bool           doSimplifyContour,
               TopoDS_Shell&        Result,
               TopoDS_Shape&        ResultContour,
               gp_Pnt&              ContourCenter,
               gp_Vec&              ContourOrientation,
               ActAPI_ProgressEntry Journal);

protected:

  ASitus_EXPORT bool
    generalFuse(const TopoDS_Shape& Body,
                const TopoDS_Shape& Contour,
                const double        fuzz,
                TopoDS_Shape&       ResultBody,
                TopoDS_Shape&       ResultContour) const;

  ASitus_EXPORT TopoDS_Shape
    updateContour(BOPAlgo_Builder&    BOP,
                  const TopoDS_Shape& Contour) const;

protected:

  TopoDS_Shape m_body; //!< Working shape.

};

#endif
