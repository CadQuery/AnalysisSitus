//-----------------------------------------------------------------------------
// Created on: 15 April 2016
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

// Own include
#include <asiAlgo_QuickHull2d.h>

// OCCT includes
#include <gce_MakeLin2d.hxx>

//-----------------------------------------------------------------------------
// Function: constructor
//-----------------------------------------------------------------------------

template <typename TPoint>
asiAlgo_QuickHull2d<TPoint>::asiAlgo_QuickHull2d(const Handle(asiAlgo_PointWithAttrCloud<TPoint>)& cloud,
                                                 ActAPI_ProgressEntry                              progress,
                                                 ActAPI_PlotterEntry                               plotter)
: ActAPI_IAlgorithm(progress, plotter)
{
  m_cloud = cloud;
  m_P_up = m_P_down = m_P_left = m_P_right = NULL;
}

//-----------------------------------------------------------------------------
// Function: destructor
//-----------------------------------------------------------------------------

template <typename TPoint>
asiAlgo_QuickHull2d<TPoint>::~asiAlgo_QuickHull2d()
{
  this->releaseNode(m_P_up);
  this->releaseNode(m_P_down);
  this->releaseNode(m_P_left);
  this->releaseNode(m_P_right);
}

//-----------------------------------------------------------------------------
// Function: Perform
//-----------------------------------------------------------------------------

template <typename TPoint>
bool asiAlgo_QuickHull2d<TPoint>::Perform()
{
  if ( m_cloud.IsNull() || !m_cloud->Length() )
    return false;

  if ( m_cloud->Length() == 1 )
  {
    m_hull = new TColStd_HSequenceOfInteger;
    m_hull->Append(1);

    return true; // One point is a convex hull for itself
  }

  // Allocate new hull
  m_hull = new TColStd_HSequenceOfInteger;

  //----------------------------------------------------
  // Find points with minimum and maximum x coordinates
  //----------------------------------------------------

  double xMin = DBL_MAX, xMax = -DBL_MAX;
  int p_left_idx = -1, p_right_idx = -1;
  //
  for ( int p_idx = 1; p_idx <= m_cloud->Length(); ++p_idx )
  {
    const gp_XY& P = m_cloud->Value(p_idx);
    const double x = P.X();

    if ( x < xMin )
    {
      xMin       = x;
      p_left_idx = p_idx;
    }

    if ( x > xMax )
    {
      xMax        = x;
      p_right_idx = p_idx;
    }
  }

  if ( p_left_idx == -1 || p_right_idx == -1 || p_left_idx == p_right_idx )
    return false;

  //-----------------------
  // Find point to proceed
  //-----------------------

  int p_up_idx   = this->findDistantPoint(p_left_idx, p_right_idx, true);
  int p_down_idx = this->findDistantPoint(p_left_idx, p_right_idx, false);

#if defined DRAW_DEBUG
  DRAW_INITGROUP(sentinel)
  gp_XY P;

  P = m_cloud->Value(p_left_idx);
  DRAW_POINT( gp_Pnt( P.X(), P.Y(), 0.0 ), sentinel, Draw_vert, Draw_Circle )
  DrawTrSurf::Set( TCollection_AsciiString("p").Cat(p_left_idx).ToCString(), gp_Pnt( P.X(), P.Y(), 0.0 ) );

  P = m_cloud->Value(p_right_idx);
  DRAW_POINT( gp_Pnt( P.X(), P.Y(), 0.0 ), sentinel, Draw_vert, Draw_Circle )
  DrawTrSurf::Set( TCollection_AsciiString("p").Cat(p_right_idx).ToCString(), gp_Pnt( P.X(), P.Y(), 0.0 ) );

  P = m_cloud->Value(p_up_idx);
  DRAW_POINT( gp_Pnt( P.X(), P.Y(), 0.0 ), sentinel, Draw_vert, Draw_Circle )
  DrawTrSurf::Set( TCollection_AsciiString("p").Cat(p_up_idx).ToCString(), gp_Pnt( P.X(), P.Y(), 0.0 ) );

  P = m_cloud->Value(p_down_idx);
  DRAW_POINT( gp_Pnt( P.X(), P.Y(), 0.0 ), sentinel, Draw_vert, Draw_Circle )
  DrawTrSurf::Set( TCollection_AsciiString("p").Cat(p_down_idx).ToCString(), gp_Pnt( P.X(), P.Y(), 0.0 ) );
#endif

  m_P_left  = new PNode(p_left_idx);
  m_P_right = new PNode(p_right_idx);
  m_P_up    = new PNode(p_up_idx);
  m_P_down  = new PNode(p_down_idx);

  // Up
  this->process(m_P_left, m_P_up,    true,  true);
  this->process(m_P_up,   m_P_right, false, true);

  // Down
  this->process(m_P_left, m_P_down,  true,  false);
  this->process(m_P_down, m_P_right, false, false);

  //-------------------------------------------------------------
  // Traverse the prepared binary tree in order to extract nodes
  //-------------------------------------------------------------

  this->addNode(m_P_left);
  this->traverseTriplet(m_P_down, false);
  this->addNode(m_P_right);
  this->traverseTriplet(m_P_up, true);

  return true;
}

//-----------------------------------------------------------------------------
// Function: findDistantPoint
//-----------------------------------------------------------------------------

template <typename TPoint>
int asiAlgo_QuickHull2d<TPoint>::findDistantPoint(const int  p1_idx,
                                                  const int  p2_idx,
                                                  const bool up) const
{
  if ( p1_idx == -1 || p2_idx == -1 )
    return -1; // Cannot construct line

  //------------------------------------------------------
  // Build a line to divide the entire cloud on two parts
  //------------------------------------------------------

  gp_Pnt2d P1 = m_cloud->Value(p1_idx);
  gp_Pnt2d P2 = m_cloud->Value(p2_idx);

  gp_Lin2d Lin = gce_MakeLin2d(P1, P2);
  gp_Vec2d vecLin( Lin.Direction() );

  //----------------------------------------------------
  // Find the point with maximal distance from the line
  //----------------------------------------------------

  double dMax = -DBL_MAX;
  int res_idx = -1;
  //
  for ( int p_idx = 1; p_idx <= m_cloud->Length(); ++p_idx )
  {
    const gp_XY& Pi = m_cloud->Value(p_idx);

    gp_Vec2d vecP1Pi( Pi - P1.XY() );
    if ( vecP1Pi.Magnitude() < gp::Resolution() )
      continue; // Skip coincident points

    // Check if the point should be processed
    const double ang = vecLin.Angle(vecP1Pi);
    if ( (up && ang < 0) || (!up && ang > 0) )
      continue;

    const double d = Lin.Distance(Pi);
    if ( d > dMax )
    {
      dMax    = d;
      res_idx = p_idx;
    }
  }

  // Cannot proceed with the sentinel points
  if ( res_idx == p1_idx || res_idx == p2_idx )
    return -1;

  return res_idx;
}

//-----------------------------------------------------------------------------
// Function: process
//-----------------------------------------------------------------------------

template <typename TPoint>
void asiAlgo_QuickHull2d<TPoint>::process(PNode*     prev,
                                          PNode*     curr,
                                          const bool left,
                                          const bool up)
{
  const int next_idx = this->findDistantPoint(prev->idx, curr->idx, up);

  if ( next_idx == -1 )
    return; // No more points to proceed

#if defined DRAW_DEBUG
  DRAW_INITGROUP(distant)
  gp_XY P;
  P = m_cloud->Value(next_idx);
  DRAW_POINT( gp_Pnt( P.X(), P.Y(), 0.0 ), distant, Draw_rouge, Draw_Circle )
  DrawTrSurf::Set( TCollection_AsciiString("p").Cat(next_idx).ToCString(), gp_Pnt( P.X(), P.Y(), 0.0 ) );
#endif

  // Allocate next traversal node
  PNode* next = new PNode(next_idx, left ? curr : prev);
  if ( left )
    curr->left = next;
  else
    prev->right = next;

  // Traverse recursively
  this->process(prev, next, true, up);
  this->process(next, curr, false, up);
}

//-----------------------------------------------------------------------------
// Function: traverseTriplet
//-----------------------------------------------------------------------------

template <typename TPoint>
void asiAlgo_QuickHull2d<TPoint>::traverseTriplet(PNode*     parent,
                                                  const bool ccw)
{
  PNode* first  = ccw ? parent->right : parent->left;
  PNode* second = parent;
  PNode* third  = ccw ? parent->left : parent->right;

  if ( first )
    this->traverseTriplet(first, ccw);

  this->addNode(second);

  if ( third )
    this->traverseTriplet(third, ccw);
}

//-----------------------------------------------------------------------------
// Function: releaseNode
//-----------------------------------------------------------------------------

template <typename TPoint>
void asiAlgo_QuickHull2d<TPoint>::releaseNode(PNode* node)
{
  if ( node && node->left )
    this->releaseNode(node->left);
  if ( node && node->right )
    this->releaseNode(node->right);

  delete node;
  node = NULL;
}

//-----------------------------------------------------------------------------
// Function: addNode
//-----------------------------------------------------------------------------

template <typename TPoint>
void asiAlgo_QuickHull2d<TPoint>::addNode(PNode* node)
{
  if ( node->idx == -1 )
    return;

  m_hull->Append(node->idx);

#if defined DRAW_DEBUG
  DRAW_INITGROUP(pole)
  const gp_XY& P = m_cloud->Value(node->idx);
  DRAW_POINT( gp_Pnt( P.X(), P.Y(), 0.0 ), pole, Draw_bleu, Draw_Square )
#endif
}
