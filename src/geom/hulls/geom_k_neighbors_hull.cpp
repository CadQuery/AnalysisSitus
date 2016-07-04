//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_k_neighbors_hull.h>

// Common includes
#include <geom_purify_cloud.h>

// OCCT includes
#include <GCE2d_MakeSegment.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_QuickSort.hxx>
#include <NCollection_Vector.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//-----------------------------------------------------------------------------
// Custom data associated with each point in a cloud
//-----------------------------------------------------------------------------

//! Custom data associated with each point.
struct t_point_data
{
  t_point_data() : Dist(0.0), Idx(0) {}
  t_point_data(const double _d, const size_t _i) : Dist(_d), Idx(_i) {}

  double Dist; //!< Distance.
  double Ang;  //!< Angle.
  size_t Idx;  //!< Zero-based index of point in the initial cloud.
};

//-----------------------------------------------------------------------------
// Comparator for points by their distances
//-----------------------------------------------------------------------------

//! Comparator for sorting points by their associated distance values.
class t_dist_comparator : public NCollection_Comparator< geom_common_point<gp_XY> >
{
public:

  //! Checks if theLeft > theRight.
  //! \param theLeft  [in] first operand.
  //! \param theRight [in] second operand.
  //! \return true/false.
  virtual unsigned int IsGreater(const geom_common_point<gp_XY>& theLeft,
                                 const geom_common_point<gp_XY>& theRight) const
  {
    const double l = (*( (t_point_data*) (theLeft.pData) )).Dist;
    const double r = (*( (t_point_data*) (theRight.pData) )).Dist;
    return (l > r) && Abs(l - r) > RealEpsilon();
  }

  //! Checks if theLeft == theRight.
  //! \param theLeft  [in] first operand.
  //! \param theRight [in] second operand.
  //! \return true/false.
  virtual unsigned int IsEqual(const geom_common_point<gp_XY>& theLeft,
                               const geom_common_point<gp_XY>& theRight) const
  {
    const double l = (*( (t_point_data*) (theLeft.pData) )).Dist;
    const double r = (*( (t_point_data*) (theRight.pData) )).Dist;
    return Abs(l - r) < RealEpsilon();
  }

};

//-----------------------------------------------------------------------------
// Comparator for points by their angles
//-----------------------------------------------------------------------------

//! Comparator for sorting points by their associated angle values.
class t_ang_comparator : public NCollection_Comparator< geom_common_point<gp_XY> >
{
public:

  //! Checks if theLeft > theRight.
  //! \param theLeft  [in] first operand.
  //! \param theRight [in] second operand.
  //! \return true/false.
  virtual unsigned int IsGreater(const geom_common_point<gp_XY>& theLeft,
                                 const geom_common_point<gp_XY>& theRight) const
  {
    const double l = (*( (t_point_data*) (theLeft.pData) )).Ang;
    const double r = (*( (t_point_data*) (theRight.pData) )).Ang;
    return (l > r) && Abs(l - r) > RealEpsilon();
  }

  //! Checks if theLeft == theRight.
  //! \param theLeft  [in] first operand.
  //! \param theRight [in] second operand.
  //! \return true/false.
  virtual unsigned int IsEqual(const geom_common_point<gp_XY>& theLeft,
                               const geom_common_point<gp_XY>& theRight) const
  {
    const double l = (*( (t_point_data*) (theLeft.pData) )).Ang;
    const double r = (*( (t_point_data*) (theRight.pData) )).Ang;
    return Abs(l - r) < RealEpsilon();
  }

};

//-----------------------------------------------------------------------------
// Construction and destruction
//-----------------------------------------------------------------------------

geom_k_neighbors_hull::geom_k_neighbors_hull(const geom_common_point_2d_vec& cloud,
                                             const int                       k,
                                             const int                       limitIters,
                                             ActAPI_ProgressEntry            Journal)
: m_JEntry(Journal)
{
  m_cloud       = cloud;
  m_iK          = k;
  m_iK_init     = k;
  m_iK_limit    = 25; // We do not want to fall into stack overflow
  m_iLimitIters = limitIters;
  m_iIterNum    = 0;
}

//-----------------------------------------------------------------------------
// Interface methods
//-----------------------------------------------------------------------------

bool geom_k_neighbors_hull::Perform()
{
  // ...
  // Input: list of points to process (m_cloud);
  //        number of neighbors (m_iK).
  //
  // Output: an ordered list of points representing the computed
  //         polygon (m_hull)
  // ...

  // Check initial cloud
  if ( !m_cloud.size() )
  {
    m_JEntry.SendLogMessage( LogErr(Normal) << "geom_k_neighbors_hull.EmptyCloud" );
    return false;
  }

  // Check if k (number of consulted neighbors) is sufficient
  if ( m_iK < 2 )
  {
    m_JEntry.SendLogMessage( LogWarn(Normal) << "geom_k_neighbors_hull.TooSmallValueOfK" );
    m_iK = 2;
  }

  // Check if k does not exceed the limit
  if ( m_iK > m_iK_limit )
  {
    m_JEntry.SendLogMessage( LogWarn(Normal) << "geom_k_neighbors_hull.ValueOfKExceedsLimit" );
    m_iK = m_iK_limit;
  }

    // Remove equal points from the cloud
  m_cloud = this->sparseCloud( m_cloud, Precision::Confusion() );
  const int nPoints = (int) m_cloud.size();

  // Nullify number of iterations
  m_iIterNum = 0;

  // Check if cloud is degenerated
  if ( nPoints <= 3 )
  {
    m_hull = m_cloud;
    return true;
  }

  // Build hull
  geom_common_point_2d_vec dataset = this->copyCloud(m_cloud);
  return this->perform(dataset);
}

//-----------------------------------------------------------------------------
// Internal methods
//-----------------------------------------------------------------------------

geom_common_point_2d_vec
  geom_k_neighbors_hull::sparseCloud(const geom_common_point_2d_vec& cloud,
                                     const double                    prec) const
{
  geom_common_point_2d_vec res;
  geom_purify_cloud::Purify<geom_common_point_2d_vec,
                            geom_inspector_2D>(prec, cloud, res);
  return res;
}

geom_common_point_2d_vec
  geom_k_neighbors_hull::copyCloud(const geom_common_point_2d_vec& cloud) const
{
  geom_common_point_2d_vec res( cloud.size() );
  for ( size_t idx = 0; idx < cloud.size(); ++idx )
  {
    geom_common_point<gp_XY> P( cloud.at(idx) );
    P.Status = 0;
    P.pData  = NULL;

    // Add to result
    res.at(idx) = P;
  }

  return res;
}

bool geom_k_neighbors_hull::perform(geom_common_point_2d_vec& dataset)
{
  // Make sure that k neighbors are available
  m_iK = Min(m_iK, (int) dataset.size() - 1);

  // Check if we reached the limit in the number of consulted neighbors
  if ( m_iK > m_iK_limit )
  {
    m_JEntry.SendLogMessage( LogWarn(Normal) << "geom_k_neighbors_hull.ValueOfKReachedLimit" );
    return true; // Let's return at least anything (result is normally not empty by this stage)
  }

  // Start from beginning (with the new or initial K)
  m_hull.clear();

  /* ==================================
   *  Perform reconstruction of a hull
   * ================================== */

  // Point with minimal Y coordinate
  geom_common_point<gp_XY> firstPoint;
  size_t firstPointIdx;
  this->findMinYPoint(firstPoint, firstPointIdx, dataset);

#if defined DRAW_DEBUG
  DRAW_INITGROUP(seed_pt)
  DRAW_POINT(gp_Pnt2d(firstPoint.Coord), seed_pt, Draw_vert, Draw_Square)
#endif

  // Add the first point to the resulting cloud
  this->addPoint(firstPoint, m_hull);

  // Remove the traversed point from the working cloud
  this->removePoint(firstPointIdx, dataset);

  // Working variables
  geom_common_point<gp_XY> currentPoint(firstPoint);
  size_t currentPointIdx = firstPointIdx;
  int step = 2;

  // Main loop
  while ( ( step == 2 || (currentPoint.Coord - firstPoint.Coord).SquareModulus() > gp::Resolution() ) && dataset.size() )
  {
    m_iIterNum++;
    if ( m_iIterNum == m_iLimitIters )
    {
      m_JEntry.SendLogMessage( LogWarn(Normal) << "geom_k_neighbors_hull.ReachedMaxNumOfIterations" );
      return true; // Just halt with a positive result
    }

    // Check if all points are inside
    bool   allInside = true;
    size_t j         = m_cloud.size();
    while ( allInside && j )
    {
      --j;
      allInside = this->isPointInPolygon(m_cloud.at(j), m_hull);
    }
    if ( allInside )
      return true;

    if ( step == 5 )
    {
      // Now we can restore the first point as we got far enough from it
      dataset.at(firstPointIdx).Status = 0;
    }

    // Find k neighbors
    std::vector<int>
      kNearestPoints_indices = this->nearestPoints(currentPointIdx, m_iK, dataset);

    // Sort the candidates in descending order by right-hand rule
    std::vector<int>
      cPoints_indices = this->sortByAngle(kNearestPoints_indices, dataset, m_hull);

    if ( !cPoints_indices.size() )
    {
     m_JEntry.SendLogMessage( LogErr(Normal) << "geom_k_neighbors_hull.CannotFindNearestPoints" );
      return false;
    }

    /* ==============================
     *  Check for self-intersections
     * ============================== */

    bool   its          = true;
    size_t i            = 0;
    size_t cPoint_index = 0;

    while ( its && ( i < cPoints_indices.size() ) )
    {
      cPoint_index = cPoints_indices.at(i++);
      const geom_common_point<gp_XY>& cPoint = dataset.at(cPoint_index);

      // Check against the currently traversed path
      its = this->checkIntersections(currentPoint, cPoint, m_hull);
    }

    if ( its ) // If there are some intersections, then we are in a trouble...
    {
      m_JEntry.SendLogMessage( LogWarn(Normal) << "geom_k_neighbors_hull.IncrementedKDueIntersections" );

      // Increase k and try again
      m_iK = m_iK_init++ + 1;
      geom_common_point_2d_vec copyDS = this->copyCloud(m_cloud);
      return this->perform(copyDS);
    }

    if ( cPoint_index == firstPointIdx )
      break;

    /* ==================================
     *  Choose the next point to proceed
     * ================================== */

    // Set working variables
    currentPoint    = dataset.at(cPoint_index);
    currentPointIdx = cPoint_index;
    ++step;

    // Adjust working clouds
    this->addPoint(currentPoint, m_hull);
    this->removePoint(cPoint_index, dataset);
  }

  /* =========================================
   *  Check that all points have been covered
   * ========================================= */

  bool   allInside = true;
  size_t i         = m_cloud.size();

  while ( allInside && i )
  {
    --i;
    allInside = this->isPointInPolygon(m_cloud.at(i), m_hull);
  }

  if ( !allInside ) // If there are some points still, then we are in a trouble...
  {
    m_JEntry.SendLogMessage( LogWarn(Normal) << "geom_k_neighbors_hull.IncrementedKDueIncompleteCoverage" );

    m_iK = m_iK_init++ + 1;
    geom_common_point_2d_vec copyDS = this->copyCloud(m_cloud);
    return this->perform(copyDS);
  }

  return true; // Success
}

void geom_k_neighbors_hull::findMinYPoint(geom_common_point<gp_XY>&       point,
                                          size_t&                         point_idx,
                                          const geom_common_point_2d_vec& cloud) const
{
  // Working variables
  double minY = RealLast();

  // Let's do simple loop
  for ( size_t idx = 0; idx < cloud.size(); ++idx )
  {
    const geom_common_point<gp_XY>& nextPoint = cloud.at(idx);
    if ( nextPoint.Coord.Y() < minY )
    {
      minY      = nextPoint.Coord.Y();
      point     = nextPoint;
      point_idx = idx;
    }
  }
}

size_t geom_k_neighbors_hull::addPoint(const geom_common_point<gp_XY>& point,
                                       geom_common_point_2d_vec&       cloud)
{
  cloud.push_back(point);

  return cloud.size();
}

void geom_k_neighbors_hull::removePoint(const size_t              point_idx,
                                        geom_common_point_2d_vec& cloud)
{
  cloud.at(point_idx).Status = 1; // Signaled
}

std::vector<int> geom_k_neighbors_hull::nearestPoints(const size_t              point_idx,
                                                      int&                      k,
                                                      geom_common_point_2d_vec& cloud)
{
  // WARNING: currently implemented approach is suboptimal

  // Access seed point
  geom_common_point<gp_XY> S = cloud.at(point_idx);

  /* =====================================
   *  Build distance field for each point
   * ===================================== */

  // NCollection is used because of QuickSort which follows
  NCollection_Vector< geom_common_point<gp_XY> > PointRefs;

  // Calculate Euclidean distances for all points
  const size_t nPoints = cloud.size();
  for ( size_t idx = 0; idx < nPoints; ++idx )
  {
    if ( idx == point_idx )
      continue;

    geom_common_point<gp_XY>& P = cloud.at(idx);

    if ( P.Status > 0 ) // Signaled state -> skip
      continue;

    // Calculate Euclidean distance
    const double dist2 = (P.Coord - S.Coord).SquareModulus();

    if ( !P.pData )
      P.pData = new t_point_data;

    (*( (t_point_data*) (P.pData) )).Dist = dist2;
    (*( (t_point_data*) (P.pData) )).Idx = idx;

    PointRefs.Append(P);
  }

  /* ===========================================
   *  Sort points by their associated distances
   * =========================================== */

  // Sort using OCCT quick sort implementation
  t_dist_comparator Comp;
  NCollection_QuickSort< NCollection_Vector< geom_common_point<gp_XY> >, geom_common_point<gp_XY> >
    ::Perform( PointRefs, Comp, 0, PointRefs.Length() - 1 );

  k = Min( k, PointRefs.Length() );

  std::vector<int> res;
  for ( int i = 0; i < k; ++i )
  {
    const size_t idx = (*( (t_point_data*) (PointRefs(i).pData) )).Idx;
    res.push_back( (int) idx );
  }

  /* ======================
   *  Finalize calculation
   * ====================== */

  // Release allocated heap memory
  for ( size_t idx = 0; idx < cloud.size(); ++idx )
  {
    geom_common_point<gp_XY>& P = cloud.at(idx);
    delete P.pData; P.pData = NULL;
  }

  return res;
}

bool geom_k_neighbors_hull::isPointInPolygon(const geom_common_point<gp_XY>& point,
                                             const geom_common_point_2d_vec& poly) const
{
  // Working variables
  const double x      = point.Coord.X();
  const double y      = point.Coord.Y();
  const size_t nVerts = poly.size();
  bool         sign   = false;
  const double eps    = gp::Resolution();

  // Loop over the polygon vertices
  for ( size_t i = 0; i < nVerts; ++i )
  {
    // Access next segment of the polygon
    const size_t j  = ( (i == nVerts - 1) ? 0 : i + 1 );
    const gp_XY& Pi = poly.at(i).Coord;
    const gp_XY& Pj = poly.at(j).Coord;

    // Check if sample point is coincident with pole
    if ( (point.Coord - Pi).SquareModulus() < eps )
      return true;

    // Coordinates of poles
    const double xi = Pi.X();
    const double xj = Pj.X();
    const double yi = Pi.Y();
    const double yj = Pj.Y();

    // Check if sample point belongs to a horizontal segment. If so,
    // we are Ok to proceed
    if ( Abs(y - yi) > eps || Abs(y - yj) > eps )
    {
      // Check if current link has a chance to be intersected by a horizontal
      // ray passing from the sample point
      if ( (y < yi) == (y < yj) )
        continue;

      // If link is a candidate, count only those which are located on the right
      const double f = (xj - xi)*(y - yi)/(yj - yi) + xi;
      if ( x < f )
        sign = !sign;
    }
    else // Point is on horizontal segment
    {
      // Let's count links which the sample point does not belong to
      if ( (x < xi) && (x < xj) )
        sign = !sign;
    }
  }

  return sign;
}

bool geom_k_neighbors_hull::checkIntersections(const geom_common_point<gp_XY>& P1,
                                               const geom_common_point<gp_XY>& P2,
                                               const geom_common_point_2d_vec& poly) const
{
  const double eps = gp::Resolution();

  // Number of vertices
  const size_t nVerts = poly.size();
  if ( nVerts <= 1 )
    return false; // There cannot be intersection with just one point

  // Create OCCT segment for (P1,P2)
  Handle(Geom2d_TrimmedCurve) L = GCE2d_MakeSegment(P1.Coord, P2.Coord);

  // Loop over the polygon vertices
  for ( size_t i = 0; i < nVerts; ++i )
  {
    // Access next segment of the polygon
    const size_t j  = ((i == nVerts - 1) ? 0 : i + 1);
    const gp_XY& Pi = poly.at(i).Coord;
    const gp_XY& Pj = poly.at(j).Coord;

    if ( (P1.Coord - Pi).SquareModulus() < eps ||
         (P1.Coord - Pj).SquareModulus() < eps ||
         (P2.Coord - Pi).SquareModulus() < eps ||
         (P2.Coord - Pj).SquareModulus() < eps )
      continue; // No intersection can happen with adjacent segments

    // Create OCCT segment for link
    Handle(Geom2d_TrimmedCurve) LL = GCE2d_MakeSegment(Pi, Pj);

    // Check for intersections
    Geom2dAPI_InterCurveCurve IntCC(L, LL);
    if ( IntCC.NbPoints() )
      return true;
  }

  return false; // No intersections found
}

std::vector<int>
  geom_k_neighbors_hull::sortByAngle(const std::vector<int>&         point_indices,
                                     geom_common_point_2d_vec&       dataset,
                                     const geom_common_point_2d_vec& poly) const
{
  const size_t nPoints = poly.size();
  const gp_XY  PLast   = poly.at(nPoints - 1).Coord;

  // Last link of the polygon
  gp_Dir2d seedDir;
  if ( poly.size() == 1 )
    seedDir = -gp::DX2d();
  else
    seedDir = poly.at(nPoints - 2).Coord - PLast;

  /* ================================
   *  Calculate angles between links
   * ================================ */

  // NCollection is used because of QuickSort which follows
  NCollection_Vector< geom_common_point<gp_XY> > PointRefs;

  // Loop over the candidate links
  for ( size_t i = 0; i < point_indices.size(); ++i )
  {
    // Next candidate link
    const size_t              next_idx = point_indices.at(i);
    geom_common_point<gp_XY>& P        = dataset.at(next_idx);
    gp_Dir2d                  PNext    = P.Coord - PLast;

    // Check angle
    double ang = gp_Vec2d(PNext).Angle(seedDir);
    if ( ang < 0 )
      ang += 2*M_PI;

    if ( !P.pData )
      P.pData = new t_point_data;

    (*( (t_point_data*) (P.pData) )).Ang = ang;
    (*( (t_point_data*) (P.pData) )).Idx = next_idx;
    PointRefs.Append(P);
  }

  /* ========================================
   *  Sort points by their associated angles
   * ======================================== */

  // Sort using OCCT quick sort implementation
  t_ang_comparator Comp;
  NCollection_QuickSort< NCollection_Vector< geom_common_point<gp_XY> >, geom_common_point<gp_XY> >
    ::Perform( PointRefs, Comp, 0, PointRefs.Length() - 1 );

  std::vector<int> res;
  for ( int i = 0; i < PointRefs.Length(); ++i ) // TODO: the order depends on orientation (!)
  {
    const size_t idx = (*( (t_point_data*) (PointRefs(i).pData) )).Idx;
    res.push_back( (int) idx );
  }

  /* ======================
   *  Finalize calculation
   * ====================== */

  // Release allocated heap memory
  for ( size_t idx = 0; idx < point_indices.size(); ++idx )
  {
    geom_common_point<gp_XY>& P = dataset.at(idx);
    delete P.pData; P.pData = NULL;
  }

  return res;
}
