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

#ifndef asiUI_ViewerCallback_h
#define asiUI_ViewerCallback_h

// asiUI includes
#include <asiUI.h>

// Active Data includes
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

// VTK includes
#include <vtkCommand.h>
#include <vtkSmartPointer.h>

class asiUI_Viewer;

//! Base class for all callbacks associated with 3D viewer.
class asiUI_ViewerCallback : public vtkCommand
{
public:

  vtkTypeMacro(asiUI_ViewerCallback, vtkCommand);

public:

  asiUI_EXPORT void
    SetViewer(asiUI_Viewer* pViewer);

  asiUI_EXPORT asiUI_Viewer*
    GetViewer();

protected:

  asiUI_EXPORT
    asiUI_ViewerCallback(asiUI_Viewer* pViewer);

  asiUI_EXPORT
    ~asiUI_ViewerCallback();

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

  //! Host viewer. Notice that lifetime of a Callback instance can be longer
  //! that the lifetime of the viewer because user may close that window. In
  //! that case make sure that SetWindow(NULL) is called in order to keep the
  //! callbacks in a consistent (not initialized however) states.
  asiUI_Viewer* m_pViewer;

protected:

  ActAPI_ProgressEntry m_notifier; //!< Progress notifier.
  ActAPI_PlotterEntry  m_plotter;  //!< Imperative plotter.

};

#endif
