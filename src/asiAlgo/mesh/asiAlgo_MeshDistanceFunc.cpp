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

// Own include
#include <asiAlgo_MeshDistanceFunc.h>

// asiAlgo includes
#include <asiAlgo_BVHAlgo.h>

// Mobius includes
#include <mobius/cascade.h>

// OpenCascade includes
#include <Precision.hxx>

//-----------------------------------------------------------------------------

// Number of rays used to test sign.
#define NB_TEST_RAYS 3

namespace
{
  void CorrectBboxToCube(mobius::t_xyz& Pmin,
                         mobius::t_xyz& Pmax)
  {
    const double dx = fabs( Pmax.X() - Pmin.X() );
    const double dy = fabs( Pmax.Y() - Pmin.Y() );
    const double dz = fabs( Pmax.Z() - Pmin.Z() );
    double       d  = 0.;

    bool enlargeDx = false;
    bool enlargeDy = false;
    bool enlargeDz = false;
    //
    if ( dx > dy )
    {
      enlargeDy = true;

      if ( dx > dz )
      {
        d         = dx;
        enlargeDz = true;
      }
      else
      {
        d         = dz;
        enlargeDx = true;
      }
    }
    else // dy > dx
    {
      enlargeDx = true;

      if ( dy > dz )
      {
        d         = dy;
        enlargeDz = true;
      }
      else
      {
        d         = dz;
        enlargeDy = true;
      }
    }

    mobius::t_xyz aabbCenter = (Pmax + Pmin)*0.5;

    // Enlarge.
    Pmax.SetX(Pmin.X() + d);
    Pmax.SetY(Pmin.Y() + d);
    Pmax.SetZ(Pmin.Z() + d);
    //
    mobius::t_xyz cubeCenter = (Pmax + Pmin)*0.5;

    // Translate the center of the cube to the center of the AABB.
    mobius::t_xyz ccVec = aabbCenter - cubeCenter;
    //
    Pmin += ccVec;
    Pmax += ccVec;
  }
}

//-----------------------------------------------------------------------------

asiAlgo_MeshDistanceFunc::asiAlgo_MeshDistanceFunc(const Mode mode)
: mobius::poly_DistanceFunc(mode)
{}

//-----------------------------------------------------------------------------

asiAlgo_MeshDistanceFunc::asiAlgo_MeshDistanceFunc(const Handle(asiAlgo_BVHFacets)& facets,
                                                   const Mode                       mode,
                                                   const bool                       cube)
: mobius::poly_DistanceFunc(mode)
{
  this->Init(facets, cube);
}

//-----------------------------------------------------------------------------

asiAlgo_MeshDistanceFunc::asiAlgo_MeshDistanceFunc(const Handle(asiAlgo_BVHFacets)& facets,
                                                   const gp_XYZ&                    domainMin,
                                                   const gp_XYZ&                    domainMax,
                                                   const Mode                       mode,
                                                   const bool                       cube)
: mobius::poly_DistanceFunc(mode)
{
  this->Init(facets, domainMin, domainMax, cube);
}

//-----------------------------------------------------------------------------

bool asiAlgo_MeshDistanceFunc::Init(const Handle(asiAlgo_BVHFacets)& facets,
                                    const bool                       cube)
{
  if ( facets.IsNull() )
    return false;

  BVH_Box<double, 3> aabb = facets->Box();

  mobius::t_xyz Pmin = mobius::t_xyz( aabb.CornerMin().x(),
                                      aabb.CornerMin().y(),
                                      aabb.CornerMin().z() );
  mobius::t_xyz Pmax = mobius::t_xyz( aabb.CornerMax().x(),
                                      aabb.CornerMax().y(),
                                      aabb.CornerMax().z() );

  if ( cube )
  {
    CorrectBboxToCube(Pmin, Pmax);
  }

  // Coefficient to enlarge a bit the function domain so that it surely
  // encloses the initial geometry.
  const double diagCoeff = 0.025;

  m_facets    = facets;
  m_domainMin = Pmin - (Pmax - Pmin)*diagCoeff;
  m_domainMax = Pmax + (Pmax - Pmin)*diagCoeff;

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_MeshDistanceFunc::Init(const Handle(asiAlgo_BVHFacets)& facets,
                                    const gp_XYZ&                    domainMin,
                                    const gp_XYZ&                    domainMax,
                                    const bool                       cube)
{
  if ( facets.IsNull() )
    return false;

  BVH_Box<double, 3> aabb = facets->Box();

  mobius::t_xyz Pmin = mobius::cascade::GetMobiusPnt(domainMin);
  mobius::t_xyz Pmax = mobius::cascade::GetMobiusPnt(domainMax);

  if ( cube )
  {
    CorrectBboxToCube(Pmin, Pmax);
  }

  // Coefficient to enlarge a bit the function domain so that it surely
  // encloses the initial geometry.
  const double diagCoeff = 0.025;

  m_facets    = facets;
  m_domainMin = Pmin - (Pmax - Pmin)*diagCoeff;
  m_domainMax = Pmax + (Pmax - Pmin)*diagCoeff;

  return true;
}

//-----------------------------------------------------------------------------

double asiAlgo_MeshDistanceFunc::Eval(const double x,
                                      const double y,
                                      const double z) const
{
  // Project point on mesh.
  gp_Pnt P(x, y, z);

  // Get unsigned distance.
  const double
    d2 = asiAlgo_BVHAlgo::squaredDistanceToMesh( m_facets.get(), BVH_Vec3d(x, y, z) );
  //
  if ( d2 == REAL_MAX )
    return REAL_MAX;

  // Get the unsigned distance.
  const double ud = Sqrt(d2);

  // Check sign by ray casting several times with random direction.
  bool isOutside = true;
  if ( m_mode == Mode_Signed )
  {
    int vote = 0;

    for ( int rayIdx = 0; rayIdx < NB_TEST_RAYS; ++rayIdx )
    {
      if ( vote > 1 || vote < -1 )
        break;

      // Initialize random ray.
      asiAlgo_BVHAlgo::t_ray
        ray( BVH_Vec3d(x, y, z),
             BVH_Vec3d( m_RNG.RandDouble() * 2.0 - 1.0,
                        m_RNG.RandDouble() * 2.0 - 1.0,
                        m_RNG.RandDouble() * 2.0 - 1.0) );
      //
      const int numBounces = asiAlgo_BVHAlgo::rayMeshHitCount(m_facets.get(), ray);
      //
      if ( numBounces % 2 != 0 )
      {
        --vote;
      }
      else
      {
        ++vote;
      }
    }

    isOutside = vote > 0;
  }

  return (isOutside ? 1 : -1) * ud;
}
