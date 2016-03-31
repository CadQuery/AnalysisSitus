//-----------------------------------------------------------------------------
// Created on: 21 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_angle_h
#define geom_angle_h

//-----------------------------------------------------------------------------

//! Type of angle. Numerical values associated with the items are taken
//! from fundamental paper of Joshi "Graph-based heuristics for recognition
//! of machined features from a 3D solid model", 1988.
enum geom_angle
{
  Angle_Undefined = -1,
  Angle_Concave   =  0,
  Angle_Convex    =  1
};

#endif
