//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_edge_edge_dist.h>

// Geometry includes
#include <geom_project_point_curve.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <GCPnts_TangentialDeflection.hxx>

// STL includes
#include <vector>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Default constructor.
geom_edge_edge_dist::geom_edge_edge_dist() : m_fPrec(0.0)
{}

//! Constructor.
//! \param baseEdge  [in] working edge.
//! \param precision [in] precision for the underlying projection method.
geom_edge_edge_dist::geom_edge_edge_dist(const TopoDS_Edge& baseEdge,
                                         const double       precision)
{
  this->Init(baseEdge, precision);
}

//! Initialization.
//! \param baseEdge  [in] working edge.
//! \param precision [in] precision for the underlying projection method.
void geom_edge_edge_dist::Init(const TopoDS_Edge& baseEdge,
                               const double       precision)
{
  m_fPrec = precision;

  double f, l;
  BRep_Tool::Range(baseEdge, f, l);
  BRepAdaptor_Curve BAC(baseEdge);
  GCPnts_TangentialDeflection Defl(BAC, f, l, precision, precision);

  for ( int pt_idx = 1; pt_idx <= Defl.NbPoints(); ++pt_idx )
  {
    gp_Pnt P = Defl.Value(pt_idx);
    m_basePts.Append(P);
  }
}

//! Computes distance between the passed probe edge using base discretization
//! prepared at construction time.
//! \param probeEdge        [in]     probe edge to check against the base one.
//! \param stopIfExceeded   [ib]     stops computation if max height is exceeded.
//! \param dist             [out]    distance between two sets.
//! \param max_height_dist  [in]     max chord height which is still recognized as
//!                                  overlapping.
//! \param max_running_dist [in]     max chord length which is used as enough
//!                                  value to treat near-coincident zone
//!                                  as overlapping.
//! \param hasOverlapping   [out]    true if overlapping detected.
//! \param Journal          [in/out] journal instance.
//! \return true in case of success, false -- otherwise.
bool geom_edge_edge_dist::operator()(const TopoDS_Edge&   probeEdge,
                                     const bool           stopIfExceeded,
                                     double&              dist,
                                     const double         max_height_dist,
                                     const double         max_running_dist,
                                     bool&                hasOverlapping,
                                     ActAPI_ProgressEntry Journal)
{
  // Query the spatial curve
  double f, l;
  Handle(Geom_Curve) C = BRep_Tool::Curve(probeEdge, f, l);

  // Check curve
  if ( C.IsNull() )
  {
    Journal.SendLogMessage( LogWarn(Normal) << "geom_edge_edge_dist.NullCurve" << probeEdge );
    return false;
  }

  // Now project each base point to the probe curve
  geom_project_point_curve ProjectPC(C, f, l, m_fPrec);

  // Now compare points
  dist = -1;
  std::vector< std::pair<int, int> > overlaps;
  std::pair<int, int> next_overlap;
  for ( int pt_idx = 1; pt_idx <= m_basePts.Length(); ++pt_idx )
  {
    const gp_Pnt& p = m_basePts(pt_idx);

#if defined DRAW_DEBUG
    DRAW_INITGROUP(geom_edge_edge_dist_P)
    DRAW_POINT(p, geom_edge_edge_dist_P, Draw_rouge, Draw_Circle)
#endif

    // Project
    gp_Pnt res_P;
    double res_param, res_gap;
    ProjectPC(p, res_P, res_param, res_gap, NULL);

    // Update max distance
    if ( res_gap > dist )
    {
      dist = res_gap;
    }

    // Check overlapping
    if ( res_gap < max_height_dist )
    {
      if ( next_overlap.first )
        next_overlap.second = pt_idx;
      else
        next_overlap.first = pt_idx;
    }
    else
    {
      if ( stopIfExceeded )
        return true;

      if ( next_overlap.first )
      {
        overlaps.push_back(next_overlap);
        next_overlap.first = next_overlap.second = 0;
      }
    }
  }
  if ( !overlaps.size() )
    overlaps.push_back(next_overlap);

  for ( int i = 0; i < (int) overlaps.size(); ++i )
  {
    if ( overlaps[i].second > overlaps[i].first )
    {
      const double laid_len = m_basePts(overlaps[i].second).Distance( m_basePts(overlaps[i].first) );

      if ( laid_len > max_running_dist )
      {
        hasOverlapping = true;
        break;
      }
    }
  }

  return true;
}
