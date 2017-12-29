//-----------------------------------------------------------------------------
// Created on: 29 December 2017
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

#ifndef asiUI_Viewer3dListener_h
#define asiUI_Viewer3dListener_h

// asiUI includes
#include <asiUI_Viewer.h>

// asiEngine includes
#include <asiEngine_Model.h>

// Active Data includes
#include <ActAPI_IPlotter.h>

// Qt includes
#pragma warning(push, 0)
#include <QMenu>
#pragma warning(pop)

//! Default slots for 3D viewers.
class asiUI_EXPORT asiUI_Viewer3dListener : public QObject
{
  Q_OBJECT

public:

  asiUI_Viewer3dListener(asiUI_Viewer*                  pViewer,
                         const Handle(asiEngine_Model)& model,
                         ActAPI_ProgressEntry           progress,
                         ActAPI_PlotterEntry            plotter);

  virtual
    ~asiUI_Viewer3dListener();

public:

  virtual void
    Connect();

protected slots:

  virtual void
    onContextMenu(const QPoint&);

protected:

  virtual void
    populateMenu(QMenu&) {}

  virtual void
    executeAction(QAction*) {}

protected:

  asiUI_Viewer*           m_pViewer;  //!< Manager 3D viewer.
  Handle(asiEngine_Model) m_model;    //!< Data Model instance.
  ActAPI_ProgressEntry    m_progress; //!< Progress notifier.
  ActAPI_PlotterEntry     m_plotter;  //!< Imperative plotter.

};

#endif
