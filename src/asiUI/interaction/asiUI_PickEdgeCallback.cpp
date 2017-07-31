//-----------------------------------------------------------------------------
// Created on: 01 December 2016 (*)
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
#include <asiUI_PickEdgeCallback.h>

// GUI includes
#include <asiUI_Common.h>

// Visualization includes
#include <asiVisu_PrsManager.h>
#include <asiVisu_Utils.h>

//! Instantiation routine.
//! \return instance of the callback class.
asiUI_PickEdgeCallback* asiUI_PickEdgeCallback::New()
{
  return new asiUI_PickEdgeCallback(NULL);
}

//! Constructor accepting owning viewer as a parameter.
//! \param[in] viewer owning viewer.
asiUI_PickEdgeCallback::asiUI_PickEdgeCallback(asiUI_Viewer* viewer)
: asiUI_ViewerCallback(viewer)
{}

//! Destructor.
asiUI_PickEdgeCallback::~asiUI_PickEdgeCallback()
{}

//-----------------------------------------------------------------------------

//! Listens to a dedicated event. Performs all useful operations.
//! \param pCaller   [in] caller instance.
//! \param eventId   [in] ID of the event triggered this listener.
//! \param pCallData [in] invocation context.
void asiUI_PickEdgeCallback::Execute(vtkObject*    pCaller,
                                     unsigned long eventId,
                                     void*         pCallData)
{
  asiUI_NotUsed(pCaller);
  asiUI_NotUsed(eventId);
  asiUI_NotUsed(pCallData);

  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get picking ray
  //  gp_Lin pickRay = *( (gp_Lin*) pCallData );
  //
  //  // Prepare a tool to find the intersected facet
  //  asiAlgo_HitFacet HitFacet(m_bvh, m_notifier, m_plotter);
  //
  //  // Find intersection
  //  gp_XYZ hit;
  //  int facet_idx;
  //  if ( !HitFacet(pickRay, facet_idx, hit) )
  //    std::cout << "Error: cannot find the intersected facet" << std::endl;
  //  else
  //    m_plotter.DRAW_POINT(hit, Color_Red);
}
