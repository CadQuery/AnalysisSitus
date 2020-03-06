//-----------------------------------------------------------------------------
// Created on: 20 January 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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

#ifndef asiAlgo_MeshDistanceFunc_h
#define asiAlgo_MeshDistanceFunc_h

// asiAlgo includes
#include <asiAlgo_BullardRNG.h>
#include <asiAlgo_BVHFacets.h>

// Mobius includes
#include <mobius/poly_DistanceFunc.h>

//-----------------------------------------------------------------------------

//! Distance function to be used for spatial shape representations, such as
//! Discrete Distance Fields (DDF).
class asiAlgo_MeshDistanceFunc : public mobius::poly_DistanceFunc
{
public:

  //! Ctor.
  //! \param[in] mode distance calculation mode.
  asiAlgo_EXPORT
    asiAlgo_MeshDistanceFunc(const Mode mode = Mode_Unsigned);

  //! Ctor with initialization.
  //! \param[in] facets faceted representation of a shape to measure distance to..
  //! \param[in] mode   distance calculation mode.
  //! \param[in] cube   indicates whether the distance function should be
  //!                   defined in a cube rather than in the bounding
  //!                   parallelepiped.
  asiAlgo_EXPORT
    asiAlgo_MeshDistanceFunc(const Handle(asiAlgo_BVHFacets)& facets,
                             const Mode                       mode = Mode_Unsigned,
                             const bool                       cube = false);

  //! Ctor with initialization.
  //! \param[in] facets    faceted representation of a shape to measure distance to..
  //! \param[in] domainMin min corner point of the function domain.
  //! \param[in] domainMax max corner point of the function domain.
  //! \param[in] mode      distance calculation mode.
  //! \param[in] cube      indicates whether the distance function should be
  //!                      defined in a cube rather than in the bounding
  //!                      parallelepiped.
  asiAlgo_EXPORT
    asiAlgo_MeshDistanceFunc(const Handle(asiAlgo_BVHFacets)& facets,
                             const gp_XYZ&                    domainMin,
                             const gp_XYZ&                    domainMax,
                             const Mode                       mode = Mode_Unsigned,
                             const bool                       cube = false);

public:

  //! Initializes the distance function with the existing mesh.
  //! \param[in] facets faceted representation of a shape to measure distance to.
  //! \param[in] cube   indicates whether the distance function should be
  //!                   defined in a cube rather than in the bounding
  //!                   parallelepiped.
  //! \return true/false.
  asiAlgo_EXPORT bool
    Init(const Handle(asiAlgo_BVHFacets)& facets,
         const bool                       cube = false);

  //! Initializes the distance function with the existing mesh.
  //! \param[in] facets    faceted representation of a shape to measure distance to.
  //! \param[in] domainMin min corner point of the function domain.
  //! \param[in] domainMax max corner point of the function domain.
  //! \param[in] cube      indicates whether the distance function should be
  //!                      defined in a cube rather than in the bounding
  //!                      parallelepiped.
  //! \return true/false.
  asiAlgo_EXPORT bool
    Init(const Handle(asiAlgo_BVHFacets)& facets,
         const gp_XYZ&                    domainMin,
         const gp_XYZ&                    domainMax,
         const bool                       cube = false);

public:

  //! Evaluates function for the given coordinates.
  //! \param[in] x first argument.
  //! \param[in] y second argument.
  //! \param[in] z third argument.
  //! \return evaluated distance.
  asiAlgo_EXPORT virtual double
    Eval(const double x, const double y, const double z) const;

protected:

  Handle(asiAlgo_BVHFacets)  m_facets; //!< BVH for shape represented with facets.
  mutable asiAlgo_BullardRNG m_RNG;    //!< Random number generator.

public:

  mutable ActAPI_ProgressEntry asiProgress;
  mutable ActAPI_PlotterEntry  asiPlotter;

};

#endif
