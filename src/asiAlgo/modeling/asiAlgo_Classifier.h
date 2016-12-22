//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_Classifier_h
#define asiAlgo_Classifier_h

// asiAlgo includes
#include <asiAlgo.h>
#include <asiAlgo_Membership.h>

// OCCT includes
#include <gp_Ax1.hxx>
#include <gp_Pnt.hxx>
#include <math_BullardGenerator.hxx>

//! Common classification functions.
namespace asiAlgo_Classifier
{
  asiAlgo_EXPORT gp_Ax1
    CastRay(const gp_Pnt&          pos,
            math_BullardGenerator& rng);

};

#endif
