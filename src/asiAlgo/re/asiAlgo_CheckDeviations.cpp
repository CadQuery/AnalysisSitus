//-----------------------------------------------------------------------------
// Created on: 21 August 2019
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
#include <asiAlgo_CheckDeviations.h>

// asiAlgo includes
#include <asiAlgo_MeshField.h>
#include <asiAlgo_MeshMerge.h>
#include <asiAlgo_ProjectPointOnMesh.h>

//-----------------------------------------------------------------------------

asiAlgo_CheckDeviations::asiAlgo_CheckDeviations(const Handle(asiAlgo_BaseCloud<double>)& points,
                                                 ActAPI_ProgressEntry                     progress,
                                                 ActAPI_PlotterEntry                      plotter)
: ActAPI_IAlgorithm (progress, plotter),
  m_points          (points)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_CheckDeviations::Perform(const TopoDS_Shape& part)
{
  m_progress.SetMessageKey("Merge facets");

  // Merge facets.
  asiAlgo_MeshMerge meshMerge(part);
  //
  m_result.triangulation = meshMerge.GetResultPoly()->GetTriangulation();

  return this->internalPerform();
}

//-----------------------------------------------------------------------------

bool asiAlgo_CheckDeviations::Perform(const Handle(Poly_Triangulation)& tris)
{
  m_result.triangulation = tris;

  return this->internalPerform();
}

//-----------------------------------------------------------------------------

bool asiAlgo_CheckDeviations::internalPerform()
{
  m_progress.SetMessageKey("Compute BVH");

  // Build BVH.
  m_bvh = new asiAlgo_BVHFacets(m_result.triangulation);

  m_progress.SetMessageKey("Project points to facets");
  m_progress.Init( m_points->GetNumberOfElements() );

  // Prepare scalar field.
  Handle(asiAlgo_MeshScalarField) field = new asiAlgo_MeshScalarField;
  //
  m_result.fields.push_back(field);

  // Project each point individually.
  double minScalar    = DBL_MAX, maxScalar    = -DBL_MAX;
  int    minScalarIdx = -1,      maxScalarIdx = -1;
  //
  asiAlgo_ProjectPointOnMesh pointToMesh(m_bvh);
  //
  for ( int k = 0; k < m_points->GetNumberOfElements(); ++k )
  {
    gp_Pnt P      = m_points->GetElement(k);
    gp_Pnt P_proj = pointToMesh.Perform(P);
    gp_Vec V      = P.XYZ() - P_proj.XYZ();

    // Get distance.
    const double ud = V.Magnitude();
    //
    if ( ud < minScalar )
    {
      minScalar    = ud;
      minScalarIdx = k;
    }
    //
    if ( ud > maxScalar )
    {
      maxScalar    = ud;
      maxScalarIdx = k;
    }

    // Get effective facet. The returned facet index is the 1-based index of
    // a triangle in the merged triangulation from part.
    const int facetInd = pointToMesh.GetFacetIds().size() ? pointToMesh.GetFacetIds()[0] : -1;
    //
    if ( facetInd == -1 )
      continue;
    //
    const asiAlgo_BVHFacets::t_facet& facet = m_bvh->GetFacet(facetInd);

    // Check normal to derive signed distance.
    double sd;
    //
    if ( facet.N.Dot(V) < 0 )
      sd = -ud;
    else
      sd = ud;

    // Convert facet index to triangle index.
    const int triangleId = facet.FaceIndex;

    // Get indices of nodes.
    const Poly_Triangle& triangle = m_result.triangulation->Triangle(triangleId);
    //
    int n1, n2, n3;
    triangle.Get(n1, n2, n3);

    // Store scalars in the field.
    field->data.Bind(n1, sd);
    field->data.Bind(n2, sd);
    field->data.Bind(n3, sd);

    // Progress notifier.
    m_progress.StepProgress(1);
    //
    if ( m_progress.IsCancelling() )
    {
      m_progress.SetProgressStatus(ActAPI_ProgressStatus::Progress_Canceled);
      return false;
    }
  }

  if ( minScalarIdx == -1 || maxScalarIdx == -1 )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot find min/max-distance points.");
    m_progress.SetProgressStatus(ActAPI_ProgressStatus::Progress_Failed);
    return false;
  }

  // Render the results.
  m_progress.SendLogMessage(LogInfo(Normal) << "Min distance: %1." << minScalar);
  m_progress.SendLogMessage(LogInfo(Normal) << "Max distance: %1." << maxScalar);
  //
  m_plotter.REDRAW_POINT("minScalarPt", m_points->GetElement(minScalarIdx), Color_Green);
  m_plotter.REDRAW_POINT("maxScalarPt", m_points->GetElement(maxScalarIdx), Color_Red);

  m_progress.SetProgressStatus(ActAPI_ProgressStatus::Progress_Succeeded);
  return true;
}
