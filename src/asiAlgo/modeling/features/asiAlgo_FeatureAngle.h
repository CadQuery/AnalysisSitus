//-----------------------------------------------------------------------------
// Created on: 21 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiAlgo_FeatureAngle_h
#define asiAlgo_FeatureAngle_h

// asiAlgo includes
#include <asiAlgo.h>

//-----------------------------------------------------------------------------

//! Type of angle. Numerical values associated with the items are taken
//! from fundamental paper of Joshi "Graph-based heuristics for recognition
//! of machined features from a 3D solid model", 1988.
enum asiAlgo_FeatureAngle
{
  Angle_Undefined = -1,
  Angle_Concave   =  0,
  Angle_Convex    =  1,
  Angle_Smooth    =  2
};

#endif
