//-----------------------------------------------------------------------------
// Created on: 14 February 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
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

#ifndef asiUI_Plot2d_h
#define asiUI_Plot2d_h

// asiUI includes
#include <asiUI_VtkWindow.h>

// asiEngine includes
#include <asiEngine_Model.h>

// VTK includes
#include <vtkContextView.h>

//-----------------------------------------------------------------------------

//! Visualizes two-dimensional plot with basic interaction and exporting
//! services. This plotter is nice to prepare scientific publications ;)
class asiUI_Plot2d : public QWidget
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_Plot2d(ActAPI_ProgressEntry progress,
                 ActAPI_PlotterEntry  plotter,
                 QWidget*             parent = NULL);

  asiUI_EXPORT
    ~asiUI_Plot2d();

public:

  asiUI_EXPORT void
    SetLogScale(const bool logScale);

  asiUI_EXPORT void
    Render(const std::vector<double>& x,
           const std::vector<double>& fx,
           const std::string&         xLabel,
           const std::string&         fxLabel,
           const std::string&         plotTitle);

public slots:

  asiUI_EXPORT void
    onMouseEvent();

  asiUI_EXPORT void
    onDumpGraphics();

protected:

  bool                            m_bLogScale;   //!< Whether log scale is used.
  vtkSmartPointer<vtkContextView> m_contextView; //!< VTK context view.
  asiUI_VtkWindow*                m_pViewer;     //!< Viewer widget.
  ActAPI_ProgressEntry            m_progress;    //!< Progress notifier.
  ActAPI_PlotterEntry             m_plotter;     //!< Imperative plotter.

};

#endif
