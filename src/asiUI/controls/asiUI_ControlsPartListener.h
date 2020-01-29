//-----------------------------------------------------------------------------
// Created on: 06 November 2016 (that's nice refactoring)
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

#ifndef asiUI_ControlsPartListener_h
#define asiUI_ControlsPartListener_h

// asiUI includes
#include <asiUI_ControlsPart.h>
#include <asiUI_ViewerDomain.h>
#include <asiUI_ViewerHost.h>

// Qt includes
#pragma warning(push, 0)
#include <QObject>
#pragma warning(pop)

#pragma warning(disable : 4251)

//-----------------------------------------------------------------------------

//! Default slots for controls operating with parts.
class asiUI_EXPORT asiUI_ControlsPartListener : public QObject
{
  Q_OBJECT

public:

  asiUI_ControlsPartListener(asiUI_ControlsPart*            wControls,
                             const Handle(asiEngine_Model)& model,
                             asiUI_ViewerPart*              pPartViewer,
                             asiUI_ViewerHost*              pHostViewer,
                             asiUI_ViewerDomain*            pViewerDomain,
                             asiUI_ObjectBrowser*           pBrowser,
                             QMainWindow*                   pMainWindow,
                             ActAPI_ProgressEntry           notifier);

  virtual
    ~asiUI_ControlsPartListener();

public:

  virtual void
    Connect();

protected slots:

  void onPartLoaded          (const QString&);
  void onPartAdded           (const QString&);
  void onPartModified        ();
  void onSelectionFacesOn    ();
  void onSelectionEdgesOn    ();
  void onSelectionVerticesOn ();

protected:

  virtual void
    reinitializeEverything(const bool fitAll          = true,
                           const bool populateBrowser = true);

  void
    reinitializePickers();

protected:

  asiUI_ControlsPart*     m_wControls;    //!< Controls.
  Handle(asiEngine_Model) m_model;        //!< Data Model instance.
  asiUI_ViewerPart*       m_partViewer;   //!< Part viewer.
  asiUI_ViewerHost*       m_hostViewer;   //!< Host viewer.
  asiUI_ViewerDomain*     m_domainViewer; //!< Domain viewer.
  asiUI_ObjectBrowser*    m_browser;      //!< Object browser.
  QMainWindow*            m_mainWindow;   //!< Main Qt window.
  ActAPI_ProgressEntry    m_progress;     //!< Progress notifier.

};

#pragma warning(default : 4251)

#endif
