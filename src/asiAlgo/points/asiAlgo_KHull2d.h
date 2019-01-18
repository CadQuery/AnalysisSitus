//-----------------------------------------------------------------------------
// Created on: 16 January 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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

#ifndef asiAlgo_KHull2d_HeaderFile
#define asiAlgo_KHull2d_HeaderFile

// asiAlgo includes
#include <asiAlgo_PointWithAttr.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <TColStd_HSequenceOfInteger.hxx>

//-----------------------------------------------------------------------------

//! Algorithm computing a non-convex hull of two-dimensional point cloud
//! using a k-nearest neighbors approach described in a paper of A. Moreira
//! and M.Y. Santos "Concave Hull: A K-nearest Neighbours Approach
//! for the Computation of the Region Occupied by a Set of Points".
//!
//! This algorithm generally works as a "blind rat" which follows
//! incognita path and knows nothing about its perspectives to reach
//! the end. Of course, such an approach cannot be truly effective as
//! the algorithm can stuck in a no-way situation (and will have to
//! re-select a "better" path from the very beginning). Such attempts are
//! hardly limited and, moreover, each new attempt makes the resulting
//! envelope coarser. However, we know for sure that this algorithm
//! demonstrates quite good results on many cases, especially, if the
//! cloud density is not very high.
//!
//! NOTE: there are infinite solutions for the problem of reconstruction of
//!       con-convex hulls. This one should not be suggested as the best one.
//!       It implements just one (quite simple) solution from a big variety
//!       of hypothetical choices.
template <typename TPoint>
class asiAlgo_KHull2d : public ActAPI_IAlgorithm
{
public:

  //! Initializes tool with the passed two-dimensional point cloud.
  //! \param[in] cloud      initial cloud.
  //! \param[in] k          number of neighbors to start calculation with.
  //! \param[in] limitIters limit for the number of iterations (0 is unlimited).
  //! \param[in] progress   progress notifier.
  //! \param[in] plotter    imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_KHull2d(const Handle(asiAlgo_PointWithAttrCloud<TPoint>)& cloud,
                    const int                                         k,
                    const int                                         limitIters,
                    ActAPI_ProgressEntry                              progress,
                    ActAPI_PlotterEntry                               plotter);

public:

  //! Constructs non-convex hull from the initial cloud.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Perform();

public:

  //! Returns initial point cloud.
  //! \return initial point cloud.
  const Handle(asiAlgo_PointWithAttrCloud<TPoint>)& GetInputPoints() const
  {
    return m_cloud;
  }

  //! Returns resulting non-convex hull.
  //! \return resulting non-convex hull.
  const Handle(asiAlgo_PointWithAttrCloud<TPoint>)& GetHull() const
  {
    return m_hull;
  }

  //! Returns the current value of k.
  //! \return current value of k.
  int GetK() const
  {
    return m_iK;
  }

protected:

  //! Attempts to remove duplicated points from the cloud. Duplications
  //! are recognized with some user-specified precision.
  //!
  //! \param[in] cloud cloud to sparse (will be kept unchanged).
  //! \param[in] prec  precision to use for resolving coincident points.
  //! \return cloud after processing.
  asiAlgo_EXPORT Handle(asiAlgo_PointWithAttrCloud<TPoint>)
    sparseCloud(const Handle(asiAlgo_PointWithAttrCloud<TPoint>)& cloud,
                const double                                      prec) const;

  //! Creates deep copy of the passed point cloud.
  //! \param[in] cloud point cloud to copy.
  //! \return deep copy of point cloud.
  asiAlgo_EXPORT Handle(asiAlgo_PointWithAttrCloud<TPoint>)
    copyCloud(const Handle(asiAlgo_PointWithAttrCloud<TPoint>)& cloud) const;

  //! Performs actual reconstruction of non-convex hull.
  //! \param[in,out] dataset working cloud.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    perform(Handle(asiAlgo_PointWithAttrCloud<TPoint>)& dataset);

  //! Selects a point with minimal Y coordinate.
  //! \param[out] point     found point.
  //! \param[out] point_idx zero-based index of the found point.
  //! \param[in]  cloud     working point cloud.
  asiAlgo_EXPORT void
    findMinYPoint(asiAlgo_PointWithAttr<TPoint>&                    point,
                  int&                                           point_idx,
                  const Handle(asiAlgo_PointWithAttrCloud<TPoint>)& cloud) const;

  //! Adds the passed point to the target cloud.
  //! \param point [in]     point to add.
  //! \param cloud [in/out] target point cloud.
  //! \return index of the just added point in the target cloud.
  asiAlgo_EXPORT int
    addPoint(const asiAlgo_PointWithAttr<TPoint>&        point,
             Handle(asiAlgo_PointWithAttrCloud<TPoint>)& cloud);

  //! Removes point with the passed index from the target cloud.
  //! \param[in]     point_idx zero-based index of point to remove.
  //! \param[in,out] cloud     target point cloud.
  asiAlgo_EXPORT void
    removePoint(const int                                   point_idx,
                Handle(asiAlgo_PointWithAttrCloud<TPoint>)& cloud);

  //! Returns all points which are nearest to the given one. Only k nearest
  //! points are consulted.
  //! \param[in]     point_idx 0-based index of the point to find neighbors for.
  //! \param[in,out] k         number of neighbors to consult.
  //! \param[in,out] cloud     working point cloud.
  //! \return indices of the nearest points.
  asiAlgo_EXPORT std::vector<int>
    nearestPoints(const int                                   point_idx,
                  int&                                        k,
                  Handle(asiAlgo_PointWithAttrCloud<TPoint>)& cloud);

  //! Checks whether the passed point belongs to the interior of the given
  //! polygon. This check is performed using simplest ray casting method.
  //! \param[in] point point to check.
  //! \param[in] poly  target polygon to check the point against.
  //! \return true if the point is inside, false -- otherwise.
  asiAlgo_EXPORT bool
    isPointInPolygon(const asiAlgo_PointWithAttr<TPoint>&              point,
                     const Handle(asiAlgo_PointWithAttrCloud<TPoint>)& poly) const;

  //! Checks whether the passed link intersects the given polygon.
  //! \param[in] P1   first point of the link.
  //! \param[in] P2   second point of the link.
  //! \param[in] poly polygon to check for intersections with.
  //! \return true/false.
  asiAlgo_EXPORT bool
    checkIntersections(const asiAlgo_PointWithAttr<TPoint>&              P1,
                       const asiAlgo_PointWithAttr<TPoint>&              P2,
                       const Handle(asiAlgo_PointWithAttrCloud<TPoint>)& poly) const;

  //! Starting from the last link of the resulting polygon, checks the angles
  //! between this link and each point from the given list. The points in the
  //! list are then reordered, so those having greater angles come first.
  //!
  //! \param[in]     point_indices zero-based point indices to check.
  //! \param[in,out] dataset       initial dataset.
  //! \param[in]     poly          resulting polygon.
  //! \return indices of the nearest points in angle-descending order.
  asiAlgo_EXPORT std::vector<int>
    sortByAngle(const std::vector<int>&                           point_indices,
                Handle(asiAlgo_PointWithAttrCloud<TPoint>)&       dataset,
                const Handle(asiAlgo_PointWithAttrCloud<TPoint>)& poly) const;

private:

  int                                        m_iK;          //!< Number of neighbors to consult.
  int                                        m_iK_init;     //!< Initial value of k.
  int                                        m_iK_limit;    //!< Max k number.
  int                                        m_iLimitIters; //!< Limit for number of iterations.
  int                                        m_iIterNum;    //!< Current iteration number.
  Handle(asiAlgo_PointWithAttrCloud<TPoint>) m_cloud;       //!< Initial cloud.
  Handle(asiAlgo_PointWithAttrCloud<TPoint>) m_hull;        //!< Resulting hull.

};

#endif
