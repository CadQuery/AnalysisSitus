//-----------------------------------------------------------------------------
// Created on: 29 September 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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

#ifndef asiAlgo_BVHAlgo_h
#define asiAlgo_BVHAlgo_h

// asiAlgo includes
#include <asiAlgo_BVHFacets.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

//-----------------------------------------------------------------------------

const static double REAL_MIN = std::numeric_limits<double>::min();
const static double REAL_MAX = std::numeric_limits<double>::max();

//-----------------------------------------------------------------------------

//! Tools for calculation distances and projections of points to segments and triangles
class asiAlgo_BVHAlgo : public ActAPI_IAlgorithm
{
public:

  //! Tool class describing a ray.
  struct t_ray
  {
    BVH_Vec3d Origin; //!< Origin point of a ray
    BVH_Vec3d Direct; //!< Direction vector of a ray

    //! Default ctor.
    t_ray() = default;

    //! Creates a new ray with the given origin and direction.
    t_ray(const BVH_Vec3d& O,
          const BVH_Vec3d& D) : Origin(O), Direct(D) {}
  };

public:

  //! Ctor accepting progress notifier and imperative plotter.
  //! \param[in] facets   mesh to operate with.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_BVHAlgo(const Handle(asiAlgo_BVHFacets)& facets,
                  ActAPI_ProgressEntry             progress,
                  ActAPI_PlotterEntry              plotter)
  //
  : ActAPI_IAlgorithm (progress, plotter),
    m_facets          (facets)
  {}

public:

  //! \brief Projects the given point to a straight segment.
  //! \param[in] P     point to project
  //! \param[in] start leading point of the segment.
  //! \param[in] end   ending point of the segment.
  //! \return projected point.
  static gp_Pnt projectPointSegment(const gp_Pnt& P,
                                    const gp_Pnt& start,
                                    const gp_Pnt& end);

  ///

  //! \brief Calculate distance from a point to a segment
  //! \param[in] thePoint point
  //! \param[in] theStart start point of the segment
  //! \param[in] theEnd   end point of the segment
  //! \return distance from the point to the segment
  static double distancePointSegment(const gp_Pnt& thePoint, const gp_Pnt& theStart, const gp_Pnt& theEnd);

  //! \brief Calculate distance from a point to a segment and return the nearest point
  //! \param[in]  thePoint point
  //! \param[in]  theStart start point of the segment
  //! \param[in]  theEnd   end point of the segment
  //! \param[out] theProj  nearest point on the segment
  //! \return distance from the point to the segment
  static double distancePointSegment(const gp_Pnt& thePoint, const gp_Pnt& theStart, const gp_Pnt& theEnd, gp_Pnt& theProj);

  //! \brief Calculate distance from a point to a triangle
  //! \param[in]  thePoint     point
  //! \param[in]  theTriPoint1 first point of the triangle
  //! \param[in]  theTriPoint2 second point of the triangle
  //! \param[in]  theTriPoint3 third point of the triangle
  //! \param[out] theProj      nearest point on the segment
  //! \return distance from the point to the triangle
  static double distancePointTriangle(const gp_Pnt& thePoint, const gp_Pnt& theTriPoint1, const gp_Pnt& theTriPoint2, const gp_Pnt& theTriPoint3, gp_Pnt& theProj);

  //! \brief Calculate distance from a point to a triangle
  //! \param[in]  thePoint     point
  //! \param[in]  theTriPoint1 first point of the triangle
  //! \param[in]  theTriPoint2 second point of the triangle
  //! \param[in]  theTriPoint3 third point of the triangle
  //! \param[in]  theNormal    normal vector of the triangle
  //! \param[out] theProj      nearest point on the segment
  //! \return distance from the point to the triangle
  static double distancePointTriangle(const gp_Pnt& thePoint, const gp_Pnt& theTriPoint1, const gp_Pnt& theTriPoint2, const gp_Pnt& theTriPoint3, const gp_Vec& theNormal, gp_Pnt& theProj);

  //! \brief Calculate squared distance from a point to a segment
  //! \param[in] thePoint point
  //! \param[in] theStart start point of the segment
  //! \param[in] theEnd   end point of the segment
  //! \return squared distance from the point to the segment
  static double squaredDistancePointSegment(const gp_Pnt& thePoint, const gp_Pnt& theStart, const gp_Pnt& theEnd);

  //! \brief Calculate squared distance from a point to a segment and return the nearest point
  //! \param[in]  thePoint point
  //! \param[in]  theStart start point of the segment
  //! \param[in]  theEnd   end point of the segment
  //! \param[out] theProj  nearest point on the segment
  //! \return squared distance from the point to the segment
  static double squaredDistancePointSegment(const gp_Pnt& thePoint, const gp_Pnt& theStart, const gp_Pnt& theEnd, gp_Pnt& theProj);

  //! \brief Squared distance from a point to a triangle
  //! \param[in]  thePoint     point
  //! \param[in]  theTriPoint1 first point of the triangle
  //! \param[in]  theTriPoint2 second point of the triangle
  //! \param[in]  theTriPoint3 third point of the triangle
  //! \param[out] theProj      nearest point on the segment
  //! \return squared distance from the point to the triangle
  static double squaredDistancePointTriangle(const gp_Pnt& thePoint, const gp_Pnt& theTriPoint1, const gp_Pnt& theTriPoint2, const gp_Pnt& theTriPoint3, gp_Pnt& theProj);

  //! \brief Squared distance from a point to a triangle
  //! \param[in]  thePoint     point
  //! \param[in]  theTriPoint1 first point of the triangle
  //! \param[in]  theTriPoint2 second point of the triangle
  //! \param[in]  theTriPoint3 third point of the triangle
  //! \param[in]  theNormal    normal vector of the triangle
  //! \param[out] theProj      nearest point on the segment
  //! \return squared distance from the point to the triangle
  static double squaredDistancePointTriangle(const gp_Pnt& thePoint, const gp_Pnt& theTriPoint1, const gp_Pnt& theTriPoint2, const gp_Pnt& theTriPoint3, const gp_Vec& theNormal, gp_Pnt& theProj);

  //! \brief Project a point to a triangle
  //! \param[in] thePoint     point
  //! \param[in] theTriPoint1 first point of the triangle
  //! \param[in] theTriPoint2 second point of the triangle
  //! \param[in] theTriPoint3 third point of the triangle
  //! \return projected point to the triangle
  static gp_Pnt projectPointTriangle(const gp_Pnt& thePoint, const gp_Pnt& theTriPoint1, const gp_Pnt& theTriPoint2, const gp_Pnt& theTriPoint3);

  //! \brief Project a point to a triangle with precalculated normal
  //! \param[in] thePoint     point
  //! \param[in] theTriPoint1 first point of the triangle
  //! \param[in] theTriPoint2 second point of the triangle
  //! \param[in] theTriPoint3 third point of the triangle
  //! \param[in] theNormal    normal vector of the triangle
  //! \return projected point to the triangle
  static gp_Pnt projectPointTriangle(const gp_Pnt& thePoint, const gp_Pnt& theTriPoint1, const gp_Pnt& theTriPoint2, const gp_Pnt& theTriPoint3, const gp_Vec& theNormal);

  //! Conducts basic intersection test of the given point with respect to the
  //! bounding box defined by its corner points.
  //! \param[in] boxMin lower corner of the box to test.
  //! \param[in] boxMax upper corner of the box to test.
  //! \param[in] P      point to test.
  //! \param[in] prec   precision.
  //! \return true/false.
  static bool isOut(const BVH_Vec3d& boxMin,
                    const BVH_Vec3d& boxMax,
                    const gp_Pnt&    P,
                    const double     prec);

  //! Intersects triangle with a ray.
  static double intersectTriangle(const t_ray&     ray,
                                  const BVH_Vec3d& P0,
                                  const BVH_Vec3d& P1,
                                  const BVH_Vec3d& P2);

  //! Computes number of ray-mesh intersections.
  static int rayMeshHitCount(asiAlgo_BVHFacets* pMesh,
                             const t_ray&       ray);

  //! Computes squared distance from the given point to the axis-aligned
  //! bounding boxes specified with its corners.
  static double squaredDistanceToBox(const BVH_Vec3d& P,
                                     const BVH_Vec3d& boxMin,
                                     const BVH_Vec3d& boxMax);

  //! Computes squared distance to nearest triangle point.
  static double squaredDistanceToTriangle(const BVH_Vec3d& P,
                                          const BVH_Vec3d& A,
                                          const BVH_Vec3d& B,
                                          const BVH_Vec3d& C);

  //! Computes squared distance from the given point to mesh.
  static double squaredDistanceToMesh(asiAlgo_BVHFacets* pMesh,
                                      const BVH_Vec3d&   P,
                                      const double       upperDist = REAL_MAX);

protected:

  //! Facets in form of acceleration structure.
  Handle(asiAlgo_BVHFacets) m_facets;

};

#endif
