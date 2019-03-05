//-----------------------------------------------------------------------------
// Created on: 28 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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
#include <asiUI_ViewerCallback.h>

// asiAlgo includes
#include <asiAlgo_HitFacet.h>

// OCCT includes
#include <gp_Lin.hxx>

//-----------------------------------------------------------------------------

asiUI_ViewerCallback::asiUI_ViewerCallback(asiUI_Viewer* pViewer)
: vtkCommand(), m_pViewer(pViewer), m_pBrowser(NULL)
{}

//-----------------------------------------------------------------------------

asiUI_ViewerCallback::~asiUI_ViewerCallback()
{}

//-----------------------------------------------------------------------------

void asiUI_ViewerCallback::SetViewer(asiUI_Viewer* pViewer)
{
  m_pViewer = pViewer;
}

//-----------------------------------------------------------------------------

asiUI_Viewer* asiUI_ViewerCallback::GetViewer()
{
  return m_pViewer;
}

//-----------------------------------------------------------------------------

bool asiUI_ViewerCallback::getPickedPoint(void*   pCallData,
                                          gp_XYZ& result,
                                          gp_XYZ& norm)
{
  // Get picking ray.
  gp_Lin pickRay = *( (gp_Lin*) pCallData );

  // Prepare a tool to find the intersected facet.
  asiAlgo_HitFacet HitFacet(m_bvh, m_notifier, m_plotter);

  // Find intersection.
  gp_XYZ hit;
  int facet_idx;
  //
  if ( !HitFacet(pickRay, facet_idx, hit) )
  {
    m_notifier.SendLogMessage(LogWarn(Normal) << "Cannot find the intersected facet.");
    return false;
  }

  // Get facet properties, such as its normal and the corresponding face index.
  const asiAlgo_BVHFacets::t_facet& facet = m_bvh->GetFacet(facet_idx);
  const int                         fidx  = facet.FaceIndex;
  //
  norm = facet.N.XYZ();
  //
  m_plotter.REDRAW_VECTOR_AT("norm", hit, gp_Vec(norm), Color_Red);
  m_notifier.SendLogMessage(LogInfo(Normal) << "Picked point (%1, %2, %3) on face %4."
                                            << hit.X()
                                            << hit.Y()
                                            << hit.Z()
                                            << fidx);

  result = hit;
  return true;
}
