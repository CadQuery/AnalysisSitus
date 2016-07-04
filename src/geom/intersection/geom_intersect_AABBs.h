//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_intersect_AABBs_h
#define geom_intersect_AABBs_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <Bnd_Box.hxx>
#include <TopoDS_Shape.hxx>

//! Checks whether two axis-aligned bounding boxes intersect.
class geom_intersect_AABBs
{
public:

  ASitus_EXPORT
    geom_intersect_AABBs(const double tolerance);

public:

  ASitus_EXPORT bool
    operator()(const TopoDS_Shape& S1,
               const TopoDS_Shape& S2);

  ASitus_EXPORT bool
    operator()(const TopoDS_Shape& S1,
               const TopoDS_Shape& S2,
               Bnd_Box&            B1,
               Bnd_Box&            B2);

protected:

  double m_fToler; //!< Tolerant thickness.

};

#endif
