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

// Own include
#include <asiAlgo_KHull2d.h>

// asiAlgo includes
#include <asiAlgo_PurifyCloud.h>

// OCCT includes
#include <GCE2d_MakeSegment.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Vector.hxx>

// STL includes
#include <algorithm>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #pragma message("===== warning: DRAW_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

// Instantiate for allowed types
template class asiAlgo_KHull2d<gp_XY>;

//-----------------------------------------------------------------------------

//! Comparator for points by their associated distances.
template <typename TPoint>
class DistComparator
{
public:

  DistComparator() = delete;
  DistComparator& operator=(const DistComparator&) = delete;
  DistComparator(const std::vector< asiAlgo_PointWithAttr<TPoint> >& points) : m_points(points) {}

  bool operator()(const asiAlgo_PointWithAttr<TPoint>& left, const asiAlgo_PointWithAttr<TPoint>& right)
  {
    const double l = (*((t_point_data*)(left.pData))).Dist;
    const double r = (*((t_point_data*)(right.pData))).Dist;

    return (l < r);
  }

public:

  const std::vector< asiAlgo_PointWithAttr<TPoint> >& m_points; //!< Collection of points.

};


//-----------------------------------------------------------------------------

//! Comparator for points by their associated angles.
template <typename TPoint>
class AngleComparator
{
public:

  AngleComparator() = delete;
  AngleComparator& operator=(const AngleComparator&) = delete;
  AngleComparator(const std::vector< asiAlgo_PointWithAttr<TPoint> >& points) : m_points(points) {}

  bool operator()(const asiAlgo_PointWithAttr<TPoint>& left, const asiAlgo_PointWithAttr<TPoint>& right)
  {
    const double l = (*((t_point_data*)(left.pData))).Ang;
    const double r = (*((t_point_data*)(right.pData))).Ang;

    return (l < r);
  }

public:

  const std::vector< asiAlgo_PointWithAttr<TPoint> >& m_points; //!< Collection of points.

};

//-----------------------------------------------------------------------------
// Custom data associated with each point in a cloud
//-----------------------------------------------------------------------------

//! Custom data associated with each point.
struct t_point_data
{
  t_point_data() : Dist(0.0), Idx(0) {}
  t_point_data(const double _d, const int _i) : Dist(_d), Idx(_i) {}

  double Dist; //!< Distance.
  double Ang;  //!< Angle.
  int    Idx;  //!< Zero-based index of point in the initial cloud.
};

//-----------------------------------------------------------------------------

template <typename TPoint>
asiAlgo_KHull2d<TPoint>::asiAlgo_KHull2d(const Handle(asiAlgo_PointWithAttrCloud<TPoint>)& cloud,
                                         const int                                         k,
                                         const int                                         limitIters,
                                         ActAPI_ProgressEntry                              progress,
                                         ActAPI_PlotterEntry                               plotter)
//
: ActAPI_IAlgorithm(progress, plotter)
{
  m_cloud       = cloud;
  m_iK          = k;
  m_iK_init     = k;
  m_iK_limit    = 25; // We do not want to fall into stack overflow.
  m_iLimitIters = limitIters;
  m_iIterNum    = 0;
}

//-----------------------------------------------------------------------------
// Interface methods
//-----------------------------------------------------------------------------

template <typename TPoint>
bool asiAlgo_KHull2d<TPoint>::Perform()
{
  // ...
  // Input: list of points to process (m_cloud);
  //        number of neighbors (m_iK).
  //
  // Output: an ordered list of points representing the computed
  //         polygon (m_hull).
  // ...

  // Check initial cloud.
  if ( !m_cloud->GetNumberOfElements() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Empty point cloud.");
    return false;
  }

  // Check if k (number of consulted neighbors) is sufficient.
  if ( m_iK < 2 )
  {
    m_progress.SendLogMessage(LogWarn(Normal) << "Too small value of K.");
    m_iK = 2;
  }

  // Check if k does not exceed the limit.
  if ( m_iK > m_iK_limit )
  {
    m_progress.SendLogMessage(LogWarn(Normal) << "Value of K exceeds the limit.");
    m_iK = m_iK_limit;
  }

  // Remove coincident points from the cloud.
  m_cloud = this->sparseCloud( m_cloud, Precision::Confusion() );
  //
  const int nPoints = m_cloud->GetNumberOfElements();

  // Nullify number of iterations.
  m_iIterNum = 0;

  // Check if cloud is degenerated.
  if ( nPoints <= 3 )
  {
    m_hull = m_cloud;
    return true;
  }

  // Build hull.
  Handle(asiAlgo_PointWithAttrCloud<TPoint>) dataset = this->copyCloud(m_cloud);
  //
  return this->perform(dataset);
}

//-----------------------------------------------------------------------------

template <typename TPoint>
Handle(asiAlgo_PointWithAttrCloud<TPoint>)
  asiAlgo_KHull2d<TPoint>::sparseCloud(const Handle(asiAlgo_PointWithAttrCloud<TPoint>)& cloud,
                                       const double                                      prec) const
{
  Handle(asiAlgo_PointWithAttrCloud<TPoint>) res;

  // Sparse cloud using the appropriate type of inspector.
  asiAlgo_PurifyCloud sparser(m_progress, m_plotter);
  //
  sparser.PerformCommon< asiAlgo_PointWithAttrCloud<TPoint>,
                         asiAlgo_PointWithAttrInspector2d<TPoint> >(prec, cloud, res);

  return res;
}

//-----------------------------------------------------------------------------

template <typename TPoint>
Handle(asiAlgo_PointWithAttrCloud<TPoint>)
  asiAlgo_KHull2d<TPoint>::copyCloud(const Handle(asiAlgo_PointWithAttrCloud<TPoint>)& cloud) const
{
  Handle(asiAlgo_PointWithAttrCloud<TPoint>) res = new asiAlgo_PointWithAttrCloud<TPoint>;

  for ( int idx = 0; idx < cloud->GetNumberOfElements(); ++idx )
  {
    asiAlgo_PointWithAttr<TPoint> P( cloud->GetElement(idx) );
    P.Status = 0;
    P.pData  = NULL;

    // Add to result.
    res->AddElement(P);
  }

  return res;
}

//-----------------------------------------------------------------------------

template <typename TPoint>
bool asiAlgo_KHull2d<TPoint>::perform(Handle(asiAlgo_PointWithAttrCloud<TPoint>)& dataset)
{
  // Make sure that k neighbors are available.
  m_iK = Min(m_iK, dataset->GetNumberOfElements() - 1);

  // Check if we reached the limit in the number of consulted neighbors.
  if ( m_iK > m_iK_limit )
  {
    m_progress.SendLogMessage(LogWarn(Normal) << "Value of K reached the limit.");
    return true; // Let's return at least anything (result is normally not empty by this stage).
  }

  // Start from beginning (with the new or initial K).
  if ( m_hull.IsNull() )
    m_hull = new asiAlgo_PointWithAttrCloud<TPoint>;
  else
    m_hull->Clear();

  /* ==================================
   *  Perform reconstruction of a hull
   * ================================== */

  // Point with minimal Y coordinate.
  asiAlgo_PointWithAttr<TPoint> firstPoint;
  int firstPointIdx;
  //
  this->findMinYPoint(firstPoint, firstPointIdx, dataset);

#if defined DRAW_DEBUG
  m_plotter.DRAW_POINT(firstPoint.Coord, Color_Green, "seed_pt");
#endif

  // Add the first point to the resulting cloud.
  this->addPoint(firstPoint, m_hull);

  // Remove the traversed point from the working cloud.
  this->removePoint(firstPointIdx, dataset);

  // Working variables.
  asiAlgo_PointWithAttr<TPoint> currentPoint(firstPoint);
  int currentPointIdx = firstPointIdx;
  int step            = 2;

  // Main loop.
  while ( ( step == 2 || (currentPoint.Coord - firstPoint.Coord).SquareModulus() > gp::Resolution() ) && dataset->GetNumberOfElements() )
  {
    m_iIterNum++;
    if ( m_iIterNum == m_iLimitIters )
    {
      m_progress.SendLogMessage(LogWarn(Normal) << "Algorithm reached the limit of iterations (max %1 iterations allowed)."
                                                << m_iLimitIters);
      return true; // Just halt with a positive result.
    }

    // Check if all points are inside.
    bool allInside = true;
    int  j         = m_cloud->GetNumberOfElements();
    while ( allInside && j )
    {
      --j;
      allInside = this->isPointInPolygon(m_cloud->GetElement(j), m_hull);
    }
    if ( allInside )
      return true;

    if ( step == 5 )
    {
      // Now we can restore the first point as we got far enough from it.
      dataset->ChangeElement(firstPointIdx).Status = 0;
    }

    // Find k neighbors.
    std::vector<int>
      kNearestPoints_indices = this->nearestPoints(currentPointIdx, m_iK, dataset);

    // Sort the candidates in descending order by right-hand rule.
    std::vector<int>
      cPoints_indices = this->sortByAngle(kNearestPoints_indices, dataset, m_hull);

    if ( !cPoints_indices.size() )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Cannot find nearest points.");
      return false;
    }

    /* ==============================
     *  Check for self-intersections
     * ============================== */

    bool its          = true;
    int  i            = 0;
    int  cPoint_index = 0;

    while ( its && ( i < cPoints_indices.size() ) )
    {
      cPoint_index = cPoints_indices.at(i++);
      const asiAlgo_PointWithAttr<TPoint>& cPoint = dataset->GetElement(cPoint_index);

      // Check against the currently traversed path.
      its = this->checkIntersections(currentPoint, cPoint, m_hull);
    }

    if ( its ) // If there are some intersections, then we are in a trouble...
    {
      // Increase k and try again.
      const int prevK = m_iK;
      //
      m_iK = m_iK_init++ + 1;

      // Info.
      m_progress.SendLogMessage(LogNotice(Normal) << "Incrementing K because of intersections (%1 increased to %2)."
                                                  << prevK << m_iK);

      Handle(asiAlgo_PointWithAttrCloud<TPoint>) copyDS = this->copyCloud(m_cloud);
      return this->perform(copyDS);
    }

    if ( cPoint_index == firstPointIdx )
      break;

    /* ==================================
     *  Choose the next point to proceed
     * ================================== */

    // Set working variables.
    currentPoint    = dataset->GetElement(cPoint_index);
    currentPointIdx = cPoint_index;
    ++step;

    // Adjust working clouds.
    this->addPoint(currentPoint, m_hull);
    this->removePoint(cPoint_index, dataset);
  }

  /* =========================================
   *  Check that all points have been covered
   * ========================================= */

  bool allInside = true;
  int  i         = m_cloud->GetNumberOfElements();

  while ( allInside && i )
  {
    --i;
    allInside = this->isPointInPolygon(m_cloud->GetElement(i), m_hull);
  }

  if ( !allInside ) // If there are some points still, then we are in a trouble...
  {
    // Increase k and try again.
    const int prevK = m_iK;
    //
    m_iK = m_iK_init++ + 1;

    // Info.
    m_progress.SendLogMessage(LogNotice(Normal) << "Incrementing K because of incomplete coverage (%1 increased to %2)."
                                                << prevK << m_iK);

    Handle(asiAlgo_PointWithAttrCloud<TPoint>) copyDS = this->copyCloud(m_cloud);
    return this->perform(copyDS);
  }

  return true; // Success.
}

//-----------------------------------------------------------------------------

template <typename TPoint>
void asiAlgo_KHull2d<TPoint>::findMinYPoint(asiAlgo_PointWithAttr<TPoint>&                    point,
                                            int&                                              point_idx,
                                            const Handle(asiAlgo_PointWithAttrCloud<TPoint>)& cloud) const
{
  // Working variables.
  double minY = RealLast();

  // Let's do simple loop.
  for ( int idx = 0; idx < cloud->GetNumberOfElements(); ++idx )
  {
    const asiAlgo_PointWithAttr<TPoint>& nextPoint = cloud->GetElement(idx);
    //
    if ( nextPoint.Coord.Y() < minY )
    {
      minY      = nextPoint.Coord.Y();
      point     = nextPoint;
      point_idx = idx;
    }
  }
}

//-----------------------------------------------------------------------------

template <typename TPoint>
int asiAlgo_KHull2d<TPoint>::addPoint(const asiAlgo_PointWithAttr<TPoint>&        point,
                                      Handle(asiAlgo_PointWithAttrCloud<TPoint>)& cloud)
{
  cloud->AddElement(point);

#if defined DRAW_DEBUG
  m_plotter.DRAW_POINT(point.Coord, Color_Blue, "next_pt");
#endif

  return cloud->GetNumberOfElements();
}

//-----------------------------------------------------------------------------

template <typename TPoint>
void asiAlgo_KHull2d<TPoint>::removePoint(const int                                   point_idx,
                                          Handle(asiAlgo_PointWithAttrCloud<TPoint>)& cloud)
{
  cloud->ChangeElement(point_idx).Status = 1; // Signaled.
}

//-----------------------------------------------------------------------------

template <typename TPoint>
std::vector<int>
  asiAlgo_KHull2d<TPoint>::nearestPoints(const int                                   point_idx,
                                         int&                                        k,
                                         Handle(asiAlgo_PointWithAttrCloud<TPoint>)& cloud)
{
  // WARNING: currently implemented approach is suboptimal

  // Access seed point.
  const asiAlgo_PointWithAttr<TPoint>& S = cloud->GetElement(point_idx);

  /* =====================================
   *  Build distance field for each point
   * ===================================== */

  // Fill collections to use sorting.
  std::vector< asiAlgo_PointWithAttr<TPoint> > PointRefs;

  // Calculate Euclidian distances for all points.
  const int nPoints = cloud->GetNumberOfElements();
  for ( int idx = 0; idx < nPoints; ++idx )
  {
    if ( idx == point_idx )
      continue;

    asiAlgo_PointWithAttr<TPoint>& P = cloud->ChangeElement(idx);

    if ( P.Status > 0 ) // Signaled state (i.e., removed point) -> skip.
      continue;

    // Calculate Euclidian distance.
    const double dist2 = (P.Coord - S.Coord).SquareModulus();

    if ( !P.pData )
      P.pData = new t_point_data;

    (*( (t_point_data*) (P.pData) )).Dist = dist2;
    (*( (t_point_data*) (P.pData) )).Idx  = idx;

    PointRefs.push_back(P);
  }

  /* ===========================================
   *  Sort points by their associated distances
   * =========================================== */

  // Sort points by distance.
  std::sort( PointRefs.begin(), PointRefs.end(), DistComparator<TPoint>(PointRefs) );

  k = Min( k, int( PointRefs.size() ) );

  std::vector<int> res;
  for ( int i = 0; i < k; ++i )
  {
    const int idx = (*( (t_point_data*) (PointRefs[i].pData) )).Idx;
    res.push_back(idx);
  }

  /* ======================
   *  Finalize calculation
   * ====================== */

  // Release allocated heap memory.
  for ( int idx = 0; idx < cloud->GetNumberOfElements(); ++idx )
  {
    asiAlgo_PointWithAttr<TPoint>& P = cloud->ChangeElement(idx);
    delete P.pData; P.pData = NULL;
  }

  return res;
}

//-----------------------------------------------------------------------------

template <typename TPoint>
bool asiAlgo_KHull2d<TPoint>::isPointInPolygon(const asiAlgo_PointWithAttr<TPoint>&              point,
                                               const Handle(asiAlgo_PointWithAttrCloud<TPoint>)& poly) const
{
  // Working variables.
  const double x      = point.Coord.X();
  const double y      = point.Coord.Y();
  const int    nVerts = poly->GetNumberOfElements();
  bool         sign   = false;
  const double eps    = gp::Resolution();

  // Loop over the polygon vertices.
  for ( int i = 0; i < nVerts; ++i )
  {
    // Access next segment of the polygon.
    const int     j  = ((i == nVerts - 1) ? 0 : i + 1);
    const TPoint& Pi = poly->GetElement(i).Coord;
    const TPoint& Pj = poly->GetElement(j).Coord;

    // Check if sample point is coincident with pole.
    if ( (point.Coord - Pi).SquareModulus() < eps )
      return true;

    // Coordinates of poles.
    const double xi = Pi.X();
    const double xj = Pj.X();
    const double yi = Pi.Y();
    const double yj = Pj.Y();

    // Check if sample point belongs to a horizontal segment. If so,
    // we are Ok to proceed.
    if ( Abs(y - yi) > eps || Abs(y - yj) > eps )
    {
      // Check if current link has a chance to be intersected by a horizontal
      // ray passing from the sample point.
      if ( (y < yi) == (y < yj) )
        continue;

      // If link is a candidate, count only those which are located on the right.
      const double f = (xj - xi)*(y - yi)/(yj - yi) + xi;
      if ( x < f )
        sign = !sign;
    }
    else // Point is on horizontal segment.
    {
      // Let's count links which the sample point does not belong to.
      if ( (x < xi) && (x < xj) )
        sign = !sign;
    }
  }

  return sign;
}

//-----------------------------------------------------------------------------

template <typename TPoint>
bool asiAlgo_KHull2d<TPoint>::checkIntersections(const asiAlgo_PointWithAttr<TPoint>&              P1,
                                                 const asiAlgo_PointWithAttr<TPoint>&              P2,
                                                 const Handle(asiAlgo_PointWithAttrCloud<TPoint>)& poly) const
{
  const double eps = gp::Resolution();

  // Number of vertices.
  const int nVerts = poly->GetNumberOfElements();
  //
  if ( nVerts <= 1 )
    return false; // There cannot be intersection with just one point.

  // Create OCCT segment for (P1,P2).
  Handle(Geom2d_TrimmedCurve) L = GCE2d_MakeSegment(P1.Coord, P2.Coord);

  // Loop over the polygon vertices.
  for ( int i = 0; i < nVerts; ++i )
  {
    // Access next segment of the polygon.
    const int     j  = ((i == nVerts - 1) ? 0 : i + 1);
    const TPoint& Pi = poly->GetElement(i).Coord;
    const TPoint& Pj = poly->GetElement(j).Coord;

    if ( (P1.Coord - Pi).SquareModulus() < eps ||
         (P1.Coord - Pj).SquareModulus() < eps ||
         (P2.Coord - Pi).SquareModulus() < eps ||
         (P2.Coord - Pj).SquareModulus() < eps )
      continue; // No intersection can happen with adjacent segments.

    // Create OCCT segment for link.
    Handle(Geom2d_TrimmedCurve) LL = GCE2d_MakeSegment(Pi, Pj);

    // Check for intersections.
    Geom2dAPI_InterCurveCurve IntCC(L, LL);
    if ( IntCC.NbPoints() )
    {
      m_plotter.DRAW_LINK(P1.Coord, P2.Coord, Color_Red, "ilink_a");
      m_plotter.DRAW_LINK(Pi,       Pj,       Color_Red, "ilink_b");
      return true;
    }
  }

  return false; // No intersections found.
}

//-----------------------------------------------------------------------------

template <typename TPoint>
std::vector<int>
  asiAlgo_KHull2d<TPoint>::sortByAngle(const std::vector<int>&                           point_indices,
                                       Handle(asiAlgo_PointWithAttrCloud<TPoint>)&       dataset,
                                       const Handle(asiAlgo_PointWithAttrCloud<TPoint>)& poly) const
{
  const int     nPoints = poly->GetNumberOfElements();
  const TPoint& PLast   = poly->GetElement(nPoints - 1).Coord;

  // Last link of the polygon.
  gp_Dir2d seedDir;
  if ( nPoints == 1 )
    seedDir = -gp::DX2d();
  else
    seedDir = poly->GetElement(nPoints - 2).Coord - PLast;

  /* ================================
   *  Calculate angles between links
   * ================================ */

  // Prepare collections for sorting.
  std::vector< asiAlgo_PointWithAttr<TPoint> > PointRefs;

  // Loop over the candidate links.
  for ( int i = 0; i < point_indices.size(); ++i )
  {
    // Next candidate link.
    const int                      next_idx = point_indices.at(i);
    asiAlgo_PointWithAttr<TPoint>& P        = dataset->ChangeElement(next_idx);
    gp_Dir2d                       PNext    = P.Coord - PLast;

//#if defined DRAW_DEBUG
//    if ( m_iIterNum == 171 )
//    {
//      DRAW_INITGROUP(sample)
//      DRAW_POINT(gp_Pnt2d(P.Coord), sample, Draw_vert, Draw_Losange)
//    }
//#endif

    // Check angle.
    double ang = gp_Vec2d(PNext).Angle(seedDir);
    //
    if ( Abs(ang) < 1.e-6 )
      ang = 0;
    else if ( ang < 0 )
      ang += 2*M_PI;

    if ( ang > 0 )
    {
      if ( !P.pData )
        P.pData = new t_point_data;

      (*( (t_point_data*) (P.pData) )).Ang = ang;
      (*( (t_point_data*) (P.pData) )).Idx = next_idx;

      PointRefs.push_back(P);
    }
  }

  /* ========================================
   *  Sort points by their associated angles
   * ======================================== */

  // Sort points by angles.
  std::sort( PointRefs.begin(), PointRefs.end(), AngleComparator<TPoint>(PointRefs) );

  std::vector<int> res;
  for ( int i = 0; i < PointRefs.size(); ++i ) // TODO: the order depends on orientation (!)
  {
    const int idx = (*( (t_point_data*) (PointRefs[i].pData) )).Idx;
    res.push_back( (int) idx );
  }

  /* ======================
   *  Finalize calculation
   * ====================== */

  // Release allocated heap memory.
  for ( int idx = 0; idx < point_indices.size(); ++idx )
  {
    asiAlgo_PointWithAttr<TPoint>& P = dataset->ChangeElement(idx);
    delete P.pData; P.pData = NULL;
  }

  return res;
}
