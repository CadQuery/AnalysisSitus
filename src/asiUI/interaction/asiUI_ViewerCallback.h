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

#ifndef asiUI_ViewerCallback_h
#define asiUI_ViewerCallback_h

// asiUI includes
#include <asiUI_ObjectBrowser.h>

// asiEngine includes
#include <asiEngine_Model.h>

// asiAlgo includes
#include <asiAlgo_BVHFacets.h>

// Active Data includes
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

// VTK includes
#pragma warning(push, 0)
#include <vtkCommand.h>
#include <vtkSmartPointer.h>
#pragma warning(pop)

class asiUI_Viewer;

//! Base class for all callbacks associated with 3D viewer.
class asiUI_ViewerCallback : public vtkCommand
{
public:

  vtkTypeMacro(asiUI_ViewerCallback, vtkCommand);

public:

  //! Sets viewer.
  //! \param[in] pViewer owning viewer.
  asiUI_EXPORT void
    SetViewer(asiUI_Viewer* pViewer);

  //! Returns viewer instance.
  //! \return viewer instance.
  asiUI_EXPORT asiUI_Viewer*
    GetViewer();

protected:

  //! Constructor.
  //! \param[in] pViewer owning viewer.
  asiUI_EXPORT
    asiUI_ViewerCallback(asiUI_Viewer* pViewer);

  //! Destructor.
  asiUI_EXPORT
    ~asiUI_ViewerCallback();

public:

  //! Sets accelerating structure to use for picking.
  //! \param[in] bvh_facets accelerating structure to set.
  void SetBVH(const Handle(asiAlgo_BVHFacets)& bvh_facets)
  {
    m_bvh = bvh_facets;
  }

  //! Sets Data Model instance to access the geometry to pick.
  //! \param[in] model Data Model instance.
  void SetModel(const Handle(asiEngine_Model)& model)
  {
    m_model = model;
  }

  //! Sets Object Browser instance to update when creating new objects.
  //! \param[in] pBrowser pointer to the Object Browser to set.
  void SetObjectBrowser(asiUI_ObjectBrowser* pBrowser)
  {
    m_pBrowser = pBrowser;
  }

  //! \return pointer to the Object Browser.
  asiUI_ObjectBrowser* GetObjectBrowser() const
  {
    return m_pBrowser;
  }

public:

  //! Sets diagnotics tools for the callback.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  void SetDiagnosticTools(ActAPI_ProgressEntry progress,
                          ActAPI_PlotterEntry  plotter)
  {
    m_notifier = progress;
    m_plotter  = plotter;
  }

protected:

  //! Extracts picked point from the call data using BVH accelerating structure.
  //! \param[in]  pCallData call data.
  //! \param[out] result    extracted point.
  //! \return true in case of success, false -- otherwise.
  bool
    getPickedPoint(void*   pCallData,
                   gp_XYZ& result);

protected:

  //! Host viewer. Notice that lifetime of a Callback instance can be longer
  //! that the lifetime of the viewer because user may close that window. In
  //! that case make sure that SetWindow(NULL) is called in order to keep the
  //! callbacks in a consistent (not initialized however) states.
  asiUI_Viewer* m_pViewer;

  Handle(asiAlgo_BVHFacets) m_bvh;      //!< Accelerating structure for picking.
  Handle(asiEngine_Model)   m_model;    //!< Data Model instance.
  asiUI_ObjectBrowser*      m_pBrowser; //!< Object Browser.

protected:

  mutable ActAPI_ProgressEntry m_notifier; //!< Progress notifier.
  mutable ActAPI_PlotterEntry  m_plotter;  //!< Imperative plotter.

};

#endif
