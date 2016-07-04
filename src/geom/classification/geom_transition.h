//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_transition_h
#define geom_transition_h

// Analysis Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_intersection_point_probe_curve.h>
#include <geom_intersection_point_probe_surface.h>
#include <geom_intersection_point_ray_surface.h>

//! Resolves transition type at a point using its neighborhood information.
class geom_transition
{
public:

  //! Transition type.
  enum Type
  {
    Unknown = 0,
    InIn,
    InOut,
    OutIn,
    OutOut
  };

public:

  ASitus_EXPORT static Type
    Resolve(const geom_intersection_point_ray_surface& PtOnRaySurface);

  ASitus_EXPORT static Type
    Resolve(const geom_intersection_point_probe_surface& PtOnCurveSurface);

  ASitus_EXPORT static Type
    Resolve(const geom_intersection_point_probe_curve& PtOnCurveCurve);

protected:

  ASitus_EXPORT static Type
    resolveByDirections(const gp_Dir& pathDir,
                        const gp_Dir& bndNormal);

protected:

  geom_transition() {}
  geom_transition(const geom_transition&) {}
  void operator=(const geom_transition&) {}

};

#endif
