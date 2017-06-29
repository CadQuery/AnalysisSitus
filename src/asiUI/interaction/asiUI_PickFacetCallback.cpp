//-----------------------------------------------------------------------------
// Created on: 19 September 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_PickFacetCallback.h>

// asiAlgo includes
#include <asiAlgo_BVHFacets.h>
#include <asiAlgo_HitFacet.h>

// asiUI includes
#include <asiUI_Common.h>

// asiVisu includes
#include <asiVisu_PrsManager.h>
#include <asiVisu_Utils.h>

// OCCT includes
#include <gp_Lin.hxx>

//! Instantiation routine.
//! \return instance of the callback class.
asiUI_PickFacetCallback* asiUI_PickFacetCallback::New()
{
  return new asiUI_PickFacetCallback(NULL, NULL);
}

//! Constructor accepting owning viewer as a parameter.
//! \param[in] bvh_facets accelerating structure for picking.
//! \param[in] viewer     owning viewer.
asiUI_PickFacetCallback::asiUI_PickFacetCallback(const Handle(asiAlgo_BVHFacets)& bvh_facets,
                                                 asiUI_Viewer*                    viewer)
: asiUI_ViewerCallback(viewer), m_bvh(bvh_facets)
{}

//! Destructor.
asiUI_PickFacetCallback::~asiUI_PickFacetCallback()
{}

//-----------------------------------------------------------------------------

//! Listens to a dedicated event. Performs all useful operations.
//! \param pCaller   [in] caller instance.
//! \param eventId   [in] ID of the event triggered this listener.
//! \param pCallData [in] invocation context.
void asiUI_PickFacetCallback::Execute(vtkObject*    vtkNotUsed(pCaller),
                                      unsigned long vtkNotUsed(eventId),
                                      void*         pCallData)
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get picking ray
  gp_Lin pickRay = *( (gp_Lin*) pCallData );

  // Prepare a tool to find the intersected facet
  asiAlgo_HitFacet HitFacet(m_bvh, m_notifier, m_plotter);

  // Find intersection
  gp_XYZ hit;
  int facet_idx;
  if ( !HitFacet(pickRay, facet_idx, hit) )
    std::cout << "Error: cannot find the intersected facet" << std::endl;
  else
    m_plotter.DRAW_POINT(hit, Color_Red);
}
