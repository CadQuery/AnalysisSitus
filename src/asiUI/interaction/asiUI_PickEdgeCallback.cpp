//-----------------------------------------------------------------------------
// Created on: 01 December 2016 (*)
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
