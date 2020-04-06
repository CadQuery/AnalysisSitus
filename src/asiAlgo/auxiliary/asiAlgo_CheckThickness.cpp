//-----------------------------------------------------------------------------
// Created on: 02 April 2020
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
#include <asiAlgo_CheckThickness.h>

// asiAlgo includes
#include <asiAlgo_HitFacet.h>
#include <asiAlgo_MeshField.h>
#include <asiAlgo_MeshMerge.h>

// OpenCascade includes
#include <gp_Lin.hxx>

//-----------------------------------------------------------------------------

asiAlgo_CheckThickness::asiAlgo_CheckThickness(const TopoDS_Shape&  shape,
                                               ActAPI_ProgressEntry progress,
                                               ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm ( progress, plotter ),
  m_bIsCustomDir    ( false ),
  m_customDir       ( gp::DZ() ),
  m_fMinThick       ( 0. ),
  m_fMaxThick       ( 0. )
{
  // Merge facets.
  asiAlgo_MeshMerge meshMerge(shape);
  //
  m_resField.triangulation = meshMerge.GetResultPoly()->GetTriangulation();

  // Build BVH.
  m_bvh = new asiAlgo_BVHFacets(m_resField.triangulation);
}

//-----------------------------------------------------------------------------

asiAlgo_CheckThickness::asiAlgo_CheckThickness(const Handle(Poly_Triangulation)& tris,
                                               ActAPI_ProgressEntry              progress,
                                               ActAPI_PlotterEntry               plotter)
: ActAPI_IAlgorithm ( progress, plotter ),
  m_fMinThick       ( 0. ),
  m_fMaxThick       ( 0. )
{
  m_resField.triangulation = tris;

  // Build BVH.
  m_bvh = new asiAlgo_BVHFacets(m_resField.triangulation);
}

//-----------------------------------------------------------------------------

bool asiAlgo_CheckThickness::Perform_RayMethod()
{
  if ( m_resField.triangulation.IsNull() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Null triangulation.");
    return false;
  }

  asiAlgo_HitFacet HitFacet(m_bvh/*, m_progress, m_plotter*/);

  // Prepare scalar field.
  Handle(asiAlgo_MeshScalarField) field = new asiAlgo_MeshScalarField;
  m_resField.fields.push_back(field);

  // Cast a ray from each facet.
  double minScalar = DBL_MAX, maxScalar = -DBL_MAX;
  //
  for ( int tidx = 1; tidx <= m_resField.triangulation->NbTriangles(); ++tidx )
  {
    const Poly_Triangle& tri = m_resField.triangulation->Triangle(tidx);

    // Get nodes.
    int n1, n2, n3;
    tri.Get(n1, n2, n3);
    //
    gp_Pnt P0 = m_resField.triangulation->Node(n1);
    gp_Pnt P1 = m_resField.triangulation->Node(n2);
    gp_Pnt P2 = m_resField.triangulation->Node(n3);

    // Center point.
    gp_Pnt C = ( P0.XYZ() + P1.XYZ() + P2.XYZ() ) / 3.;

    /* Initialize norm. */

    gp_Vec V1(P0, P1);
    //
    if ( V1.SquareMagnitude() < 1e-8 )
      continue; // Skip invalid facet.
    //
    V1.Normalize();

    gp_Vec V2(P0, P2);
    //
    if ( V2.SquareMagnitude() < 1e-8 )
      continue; // Skip invalid facet.
    //
    V2.Normalize();

    // Compute norm.
    gp_Vec N = V1.Crossed(V2);
    //
    if ( N.SquareMagnitude() < 1e-8 )
      continue; // Skip invalid facet
    //
    N.Normalize();

    // Direction to analyze thickness.
    bool   isDirDefined = true;
    gp_Dir dir;
    gp_Dir localDir = N.Reversed();
    //
    if ( !m_bIsCustomDir )
    {
      dir = localDir;
    }
    else
    {
      if ( Abs( m_customDir.Dot(localDir) ) > 0.001 ) // Check for general position.
        dir = m_customDir;
      else
        isDirDefined = false;
    }

    if ( !isDirDefined )
      continue;

    /* Shoot a ray to find intersection. */

    // Exclude the originating facet from the intersection test.
    int facet2Skip = -1;
    if ( m_bvh->FindFacet(tidx, facet2Skip) )
      HitFacet.SetFacetToSkip(facet2Skip);

    // Thickness scalar.
    double thickness = 0.;

    // Do the intersection test.
    gp_XYZ hit;
    int facet_idx;
    //
    if ( !HitFacet(gp_Lin(C, dir), facet_idx, hit) )
      if ( m_bIsCustomDir && !HitFacet(gp_Lin( C, dir.Reversed() ), facet_idx, hit) )
        m_progress.SendLogMessage(LogWarn(Normal) << "Cannot find the intersected facet.");

    // Now thickness is simply a distance.
    if ( facet_idx != -1 )
     thickness = C.Distance(hit);

    /*if ( tidx == 1 )
    {
      m_plotter.REDRAW_POINT("C", C, Color_Blue);
      m_plotter.REDRAW_POINT("hit", hit, Color_Red);
      m_plotter.REDRAW_VECTOR_AT("N", C, N.Reversed(), Color_Blue);

      return false;
    }*/

    // Store scalars in the field.
    if ( facet_idx != -1 )
    {
      double *pThick = field->data.ChangeSeek(tidx);
      if ( pThick == nullptr )
        field->data.Bind(tidx, thickness);
      else if ( thickness > *pThick)
        *pThick = thickness;

      // Update the extreme values.
      if ( thickness < minScalar )
      {
        minScalar = thickness;
      }
      if ( thickness > maxScalar )
      {
        maxScalar = thickness;
      }
    }
  }

  // Set extreme thickness values.
  m_fMinThick = minScalar;
  m_fMaxThick = maxScalar;

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_CheckThickness::Perform_SphereMethod()
{
  m_progress.SendLogMessage(LogErr(Normal) << "Sphere-based method is not yet implemented.");
  return false;
}
