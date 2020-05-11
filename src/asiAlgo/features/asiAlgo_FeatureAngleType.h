//-----------------------------------------------------------------------------
// Created on: 21 March 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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

#ifndef asiAlgo_FeatureAngleType_h
#define asiAlgo_FeatureAngleType_h

// asiAlgo includes
#include <asiAlgo.h>

//-----------------------------------------------------------------------------

//! Type of angle. Numerical values associated with the items are taken
//! from fundamental paper of Joshi "Graph-based heuristics for recognition
//! of machined features from a 3D solid model", 1988.
enum asiAlgo_FeatureAngleType
{
  FeatureAngleType_Undefined     = -1,
  FeatureAngleType_Concave       =  0,
  FeatureAngleType_Convex        =  1,
  FeatureAngleType_Smooth        =  2,
  FeatureAngleType_SmoothConcave =  3,
  FeatureAngleType_SmoothConvex  =  4,
  //
  FeatureAngleType_LAST
};

//! Auxiliary functions for dealing with feature angles.
namespace asiAlgo_FeatureAngle
{
  //! Returns true if the passed angle type is a smooth angle.
  //! \param[in] type type to check.
  //! \return true/false.
  inline bool IsSmooth(const asiAlgo_FeatureAngleType type)
  {
    if ( type == FeatureAngleType_Smooth ||
         type == FeatureAngleType_SmoothConcave ||
         type == FeatureAngleType_SmoothConvex )
      return true;

    return false;
  }

  //! Returns true if the passed angle type is a convex angle.
  //! \param[in] type type to check.
  //! \return true/false.
  inline bool IsConvex(const asiAlgo_FeatureAngleType type)
  {
    if ( type == FeatureAngleType_Convex ||
         type == FeatureAngleType_SmoothConvex )
      return true;

    return false;
  }

  //! Returns true if the passed angle type is a concave angle.
  //! \param[in] type type to check.
  //! \return true/false.
  inline bool IsConcave(const asiAlgo_FeatureAngleType type)
  {
    if ( type == FeatureAngleType_Concave ||
         type == FeatureAngleType_SmoothConcave )
      return true;

    return false;
  }
};

#endif
