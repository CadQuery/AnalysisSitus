//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_k_neighbors_hull_h
#define geom_k_neighbors_hull_h

// Analysis Situs includes
#include <asitusGeom.h>

// Common includes
#include <geom_common_point_cloud_2d.h>

// OCCT includes
#include <TColStd_HSequenceOfInteger.hxx>

//! Algorithm computing a non-convex hull of two-dimensional point cloud
//! using a k-nearest neighbors approach described in a paper of A. Moreira
//! and M.Y. Santos "Concave Hull: A K-nearest Neighbours Approach
//! for the Computation of the Region Occupied by a Set of Points".
//!
//! This algorithm generally works as a "blind man" which follows
//! incognita path and knows nothing about his perspectives to reach
//! the end. Of course, such an approach cannot be truly effective as
//! the algorithm can stuck into a no-way situation (and will have to
//! re-select "better" path from the very beginning). Such attempts are
//! hardly limited and, moreover, each new attempt makes the resulting
//! envelope coarser. However, we know for sure that this algorithm
//! demonstrates quite good results on many cases, especially, if the
//! cloud density is not very high.
//!
//! NOTE: there are infinite solutions for the problem of reconstruction of
//!       con-convex hulls. This one should not be suggested as the best one.
//!       It implements just one (quite simple) solution from a big variety
//!       of hypothetical choices.
class geom_k_neighbors_hull
{
public:

  //! Initializes tool with the passed two-dimensional point cloud.
  //! \param cloud      [in] initial cloud.
  //! \param k          [in] number of neighbors to start calculation with.
  //! \param limitIters [in] limit for number of iterations (0 is unlimited).
  //! \param Journal    [in] Journal instance.
  ASitus_EXPORT
    geom_k_neighbors_hull(const geom_common_point_2d_vec& cloud,
                          const int                       k,
                          const int                       limitIters,
                          ActAPI_ProgressEntry            Journal);

public:

  //! Constructs non-convex hull from the initial cloud.
  //! \return true in case of success, false -- otherwise.
  ASitus_EXPORT bool
    Perform();

public:

  //! Returns initial point cloud.
  //! \return initial point cloud.
  inline const geom_common_point_2d_vec& Cloud() const
  {
    return m_cloud;
  }

  //! Returns resulting non-convex hull.
  //! \return resulting non-convex hull.
  inline const geom_common_point_2d_vec& Hull() const
  {
    return m_hull;
  }

  //! Returns the current value of k.
  //! \return current value of k.
  inline int K() const
  {
    return m_iK;
  }

protected:

  //! Attempts to remove duplicated points from the cloud. Duplications
  //! are recognized with some user-specified precision.
  //!
  //! \param cloud [in] cloud to sparse (will be kept unchanged).
  //! \param prec  [in] precision to use for resolving coincident points.
  //! \return cloud after processing.
  ASitus_EXPORT geom_common_point_2d_vec
    sparseCloud(const geom_common_point_2d_vec& cloud,
                const double                    prec) const;

  //! Creates deep copy of the passed point cloud.
  //! \param cloud [in] point cloud to copy.
  //! \return deep copy of point cloud.
  ASitus_EXPORT geom_common_point_2d_vec
    copyCloud(const geom_common_point_2d_vec& cloud) const;

  //! Performs actual reconstruction of non-convex hull.
  //! \param dataset [in/out] working cloud.
  //! \return true in case of success, false -- otherwise.
  ASitus_EXPORT bool
    perform(geom_common_point_2d_vec& dataset);

  //! Selects a point with minimal Y coordinate.
  //! \param point     [out] found point.
  //! \param point_idx [out] zero-based index of the found point.
  //! \param cloud     [in]  working point cloud.
  ASitus_EXPORT void
    findMinYPoint(geom_common_point<gp_XY>&       point,
                  size_t&                         point_idx,
                  const geom_common_point_2d_vec& cloud) const;

  //! Adds the passed point to the target cloud.
  //! \param point [in]     point to add.
  //! \param cloud [in/out] target point cloud.
  //! \return index of the just added point in the target cloud.
  ASitus_EXPORT size_t
    addPoint(const geom_common_point<gp_XY>& point,
             geom_common_point_2d_vec&       cloud);

  //! Removes point with the passed index from the target cloud.
  //! \param point_idx [in]     zero-based index of point to remove.
  //! \param cloud     [in/out] target point cloud.
  ASitus_EXPORT void
    removePoint(const size_t              point_idx,
                geom_common_point_2d_vec& cloud);

  //! Returns all points which are nearest to the given one. Only k nearest
  //! points are consulted.
  //! \param point_idx [in]     0-based index of the point to find neighbors for.
  //! \param k         [in/out] number of neighbors to consult.
  //! \param cloud     [in/out] working point cloud.
  //! \return indices of the nearest points.
  ASitus_EXPORT std::vector<int>
    nearestPoints(const size_t              point_idx,
                  int&                      k,
                  geom_common_point_2d_vec& cloud);

  //! Checks whether the passed point belongs to the interior of the given
  //! polygon. This check is performed using simplest ray casting method.
  //! \param point [in] point to check.
  //! \param poly  [in] target polygon to check the point against.
  //! \return true if the point is inside, false -- otherwise.
  ASitus_EXPORT bool
    isPointInPolygon(const geom_common_point<gp_XY>& point,
                     const geom_common_point_2d_vec& poly) const;

  //! Checks whether the passed link intersects the given polygon.
  //! \param P1   [in] first point of the link.
  //! \param P2   [in] second point of the link.
  //! \param poly [in] polygon to check for intersections with.
  //! \return true/false.
  ASitus_EXPORT bool
    checkIntersections(const geom_common_point<gp_XY>& P1,
                       const geom_common_point<gp_XY>& P2,
                       const geom_common_point_2d_vec& poly) const;

  //! Starting from the last link of the resulting polygon, checks the angles
  //! between this link and each point from the given list. The points in the
  //! list are then reordered, so those having greater angles come first.
  //!
  //! \param point_indices [in]     zero-based point indices to check.
  //! \param dataset       [in/out] initial dataset.
  //! \param poly          [in]     resulting polygon.
  //! \return indices of the nearest points in angle-descending order.
  ASitus_EXPORT std::vector<int>
    sortByAngle(const std::vector<int>&         point_indices,
                geom_common_point_2d_vec&       dataset,
                const geom_common_point_2d_vec& poly) const;

private:

  int                      m_iK;          //!< Number of neighbors to consult.
  int                      m_iK_init;     //!< Initial value of k.
  int                      m_iK_limit;    //!< Max k number.
  int                      m_iLimitIters; //!< Limit for number of iterations.
  int                      m_iIterNum;    //!< Current iteration number.
  geom_common_point_2d_vec m_cloud;       //!< Initial cloud.
  geom_common_point_2d_vec m_hull;        //!< Resulting hull.
  ActAPI_ProgressEntry     m_JEntry;      //!< Journal Sentry.

};

#endif
