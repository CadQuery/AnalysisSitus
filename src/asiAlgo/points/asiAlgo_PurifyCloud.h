//-----------------------------------------------------------------------------
// Created on: 21 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Sergey Slyadnev
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

#ifndef asiAlgo_PurifyCloud_h
#define asiAlgo_PurifyCloud_h

// asiAlgo includes
#include <asiAlgo_BaseCloud.h>
#include <asiAlgo_PointWithAttr.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <gp_XYZ.hxx>
#include <NCollection_CellFilter.hxx>

//-----------------------------------------------------------------------------
// Two-dimensional inspector for cloud purification
//-----------------------------------------------------------------------------

//! Inspector tool (see OCCT Cell Filter) for purification of
//! points which are coincident with a certain tolerance.
template <typename TPoint>
class asiAlgo_PointWithAttrInspector2d
{
public:

  //! Cloud dimension.
  enum { Dimension = 2 };

  //! Point index is used as a cell ID.
  typedef int Target;

  //! Point type.
  typedef TPoint Point;

  //! Constructor accepting the tolerance.
  //! \param[in] tol    tolerance to set.
  //! \param[in] points working collection of points.
  asiAlgo_PointWithAttrInspector2d(const double                                      tol,
                                   const Handle(asiAlgo_PointWithAttrCloud<TPoint>)& points)
  : m_fTol       (tol),
    m_pointCloud (points)
  {}

  //! Sets current point to search for coincidence.
  //! \param[in] target current target.
  //! \param[in] P      point coordinates to set as current.
  void SetCurrent(const int target, const TPoint& P)
  {
    m_iCurrentTarget = target;
    m_current        = P;
  }

  //! Cleans up the list of resulting indices.
  void ClearIndices2Purge()
  {
    m_indices2Purge.Clear();
  }

  //! Returns the list of indices falling to the current cell.
  //! \return list of indices.
  const TColStd_PackedMapOfInteger& GetIndices2Purge() const
  {
    return m_indices2Purge;
  }

  //! Implementation of inspection method.
  //! \param[in] target target.
  //! \return verdict (remove or keep).
  NCollection_CellFilter_Action Inspect(const int target)
  {
    if ( m_iCurrentTarget != target )
    {
      TPoint       P     = m_pointCloud->GetElement(target);
      const double delta = (P - m_current).Modulus();

      if ( delta < m_fTol )
      {
        m_indices2Purge.Add(target);
        return CellFilter_Purge;
      }
    }

    return CellFilter_Keep;
  }

  //! Auxiliary method to shift point coordinates by the given value.
  //! Useful for preparing a points range for Inspect() with tolerance.
  TPoint Shift(const TPoint& P, double tol) const
  {
    TPoint pt(P);
    pt.SetX(P.X() + tol);
    pt.SetY(P.Y() + tol);
    //
    return pt;
  }

  //! Returns coordinates.
  //! \param[in] i 0-based index of coordinate.
  //! \param[in] P point to access coordinates for.
  //! \return requested coordinate.
  static double Coord(int i, const TPoint& P)
  {
    return P.Coord(i + 1);
  }

private:

  double                                     m_fTol;           //!< Linear tolerance to use.
  TColStd_PackedMapOfInteger                 m_indices2Purge;  //!< Indices to exclude.
  Handle(asiAlgo_PointWithAttrCloud<TPoint>) m_pointCloud;     //!< Source point cloud.
  TPoint                                     m_current;        //!< Current point.
  int                                        m_iCurrentTarget; //!< Current target.

};

//-----------------------------------------------------------------------------
// Three-dimensional inspector
//-----------------------------------------------------------------------------

//! Inspector tool (see OCCT Cell Filter) for purification of
//! points which are coincident with a certain tolerance.
class asiAlgo_Inspector3d
{
public:

  //! Cloud dimension.
  enum { Dimension = 3 };

  //! Point index is used as a cell ID.
  typedef int Target;

  //! Point type.
  typedef gp_XYZ Point;

  //! Constructor accepting the tolerance.
  //! \param[in] tol    tolerance to set.
  //! \param[in] points working collection of points.
  asiAlgo_Inspector3d(const double                             tol,
                      const Handle(asiAlgo_BaseCloud<double>)& points)
  : m_fTol       (tol),
    m_pointCloud (points)
  {}

  //! Sets current point to search for coincidence.
  //! \param[in] target current target.
  //! \param[in] P      point coordinates to set as current.
  void SetCurrent(const int target, const Point& P)
  {
    m_iCurrentTarget = target;
    m_current        = P;
  }

  //! Cleans up the list of resulting indices.
  void ClearIndices2Purge()
  {
    m_indices2Purge.Clear();
  }

  //! Returns the list of indices falling to the current cell.
  //! \return list of indices.
  const TColStd_PackedMapOfInteger& GetIndices2Purge() const
  {
    return m_indices2Purge;
  }

  //! Implementation of inspection method.
  //! \param[in] target target.
  //! \return verdict (remove or keep).
  NCollection_CellFilter_Action Inspect(const int target)
  {
    if ( m_iCurrentTarget != target )
    {
      Point        P     = m_pointCloud->GetElement(target);
      const double delta = (P - m_current).Modulus();

      if ( delta < m_fTol )
      {
        m_indices2Purge.Add(target);
        return CellFilter_Purge;
      }
    }

    return CellFilter_Keep;
  }

  //! Auxiliary method to shift point coordinates by the given value.
  //! Useful for preparing a points range for Inspect() with tolerance.
  Point Shift(const Point& P, double tol) const
  {
    Point pt(P);
    pt.SetX(P.X() + tol);
    pt.SetY(P.Y() + tol);
    pt.SetZ(P.Z() + tol);
    //
    return pt;
  }

  //! Returns coordinates.
  //! \param[in] i 0-based index of coordinate.
  //! \param[in] P point to access coordinates for.
  //! \return requested coordinate.
  static double Coord(int i, const Point& P)
  {
    return P.Coord(i + 1);
  }

private:

  double                            m_fTol;           //!< Linear tolerance to use.
  TColStd_PackedMapOfInteger        m_indices2Purge;  //!< Indices to exclude.
  Handle(asiAlgo_BaseCloud<double>) m_pointCloud;     //!< Source point cloud.
  Point                             m_current;        //!< Current point.
  int                               m_iCurrentTarget; //!< Current target.

};

//-----------------------------------------------------------------------------
// Purification algorithm
//-----------------------------------------------------------------------------

//! Auxiliary functions for purification of point clouds. These tools
//! allow reducing the number of points by getting rid of those which
//! fall into the given tolerant sphere.
class asiAlgo_PurifyCloud : public ActAPI_IAlgorithm
{
public:

  //! Ctor accepting progress notifier and imperative plotter.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_PurifyCloud(ActAPI_ProgressEntry progress = NULL,
                      ActAPI_PlotterEntry  plotter  = NULL)
  : ActAPI_IAlgorithm(progress, plotter) {}

public:

  //! Performs purification on the passed 3D point cloud.
  //! \param[in]  tol    tolerance to use.
  //! \param[in]  source source point cloud.
  //! \param[out] result resulting point cloud.
  void Perform3d(const double                             tol,
                 const Handle(asiAlgo_BaseCloud<double>)& source,
                 Handle(asiAlgo_BaseCloud<double>)&       result)
  {
    this->PerformCommon<asiAlgo_BaseCloud<double>,
                        asiAlgo_Inspector3d>(tol, source, result);
  }

public:

  template<typename HCloudType,
           typename InspectorType>
  void PerformCommon(const double              tol,
                     const Handle(HCloudType)& source,
                     Handle(HCloudType)&       result)
  {
    // Check if there are any points to purify.
    const int source_size = source->GetNumberOfElements();
    if ( !source_size )
      return;

    // Populate cell filter with the source points.
    NCollection_CellFilter<InspectorType> CellFilter(tol);
    InspectorType Inspector(tol, source);
    TColStd_PackedMapOfInteger allIndices;
    //
    for ( int p_idx = 0; p_idx < source_size; ++p_idx )
    {
      InspectorType::Point P = source->GetElement(p_idx);
      CellFilter.Add(p_idx, P);
      //
      allIndices.Add(p_idx);
    }

    // Now classify each point to know which cell it belongs to.
    TColStd_PackedMapOfInteger traversed;
    int                        next_idx     = 0;
    bool                       allTraversed = false;
    //
    do
    {
      if ( traversed.Contains(next_idx) )
        continue;
      else
        traversed.Add(next_idx);

      // Classify next point of the source cloud. Notice that Shift() is always
      // done at Precision::Confusion() value in order for the cell filter
      // to work properly.
      InspectorType::Point P = source->GetElement(next_idx);
      //
      Inspector.SetCurrent(next_idx, P);
      InspectorType::Point P_min = Inspector.Shift( P, -Precision::Confusion() );
      InspectorType::Point P_max = Inspector.Shift( P,  Precision::Confusion() );
      //
      CellFilter.Inspect(P_min, P_max, Inspector);

      // Get indices of unvisited points.
      TColStd_PackedMapOfInteger nonTraversedIndices;
      //
      nonTraversedIndices.Subtraction( allIndices, traversed );
      nonTraversedIndices.Subtract( Inspector.GetIndices2Purge() );
      //
      if ( nonTraversedIndices.IsEmpty() )
        allTraversed = 1;
      else
        next_idx = nonTraversedIndices.GetMinimalMapped();
    }
    while ( !allTraversed );

    //----------------------------------------------------
    // Copy remaining points to another (resulting) cloud
    //----------------------------------------------------

    const TColStd_PackedMapOfInteger& indices2Purge = Inspector.GetIndices2Purge();
    const int indices_size = indices2Purge.Extent();
    //
    if ( !indices_size )
      result = source;
    else
    {
      // Prepare result.
      if ( result.IsNull() )
        result = new HCloudType;

      // Populate.
      for ( int i = 0; i < source_size; ++i )
      {
        if ( indices2Purge.Contains(i) )
          continue;

        result->AddElement( source->GetElement(i) );
      }
    }
  }

};

#endif
