//-----------------------------------------------------------------------------
// Created on: 06 November 2016 (that's nice refactoring)
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

#ifndef asiUI_ControlsPartListener_h
#define asiUI_ControlsPartListener_h

// asiUI includes
#include <asiUI_ControlsPart.h>
#include <asiUI_ViewerDomain.h>
#include <asiUI_ViewerHost.h>
#include <asiUI_ViewerPart.h>

// Qt includes
#include <QObject>

//! Default slots for controls operating with parts.
class asiUI_ControlsPartListener : public QObject
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_ControlsPartListener(asiUI_ControlsPart*            wControls,
                               asiUI_ViewerPart*              wViewerPart,
                               asiUI_ViewerDomain*            wViewerDomain,
                               asiUI_ViewerHost*              wViewerSurface,
                               const Handle(asiEngine_Model)& model,
                               ActAPI_ProgressEntry           notifier);

  asiUI_EXPORT virtual
    ~asiUI_ControlsPartListener();

public:

  asiUI_EXPORT virtual void
    Connect();

protected slots:

  asiUI_EXPORT void onPartLoaded       ();
  asiUI_EXPORT void onPartAdded        ();
  asiUI_EXPORT void onPartModified     ();
  asiUI_EXPORT void onVerticesOn       ();
  asiUI_EXPORT void onVerticesOff      ();
  asiUI_EXPORT void onSelectionFacesOn ();
  asiUI_EXPORT void onSelectionEdgesOn ();

protected:

  asiUI_EXPORT void
    cleanViewers();

  asiUI_EXPORT virtual void
    reinitializeEverything();

  asiUI_EXPORT void
    reinitializePickers();

protected:

  asiUI_ControlsPart*     m_wControls;      //!< Controls.
  asiUI_ViewerPart*       m_wViewerPart;    //!< Part viewer.
  asiUI_ViewerDomain*     m_wViewerDomain;  //!< Face domain viewer.
  asiUI_ViewerHost*       m_wViewerSurface; //!< Surface viewer.
  Handle(asiEngine_Model) m_model;          //!< Data Model instance.
  ActAPI_ProgressEntry    m_notifier;       //!< Progress notifier.

};

#endif
