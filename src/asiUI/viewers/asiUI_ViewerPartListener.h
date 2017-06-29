//-----------------------------------------------------------------------------
// Created on: 07 November 2016 (99 years of October Revolution)
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

#ifndef asiUI_ViewerPartListener_h
#define asiUI_ViewerPartListener_h

// asiUI includes
#include <asiUI_ViewerDomain.h>
#include <asiUI_ViewerPart.h>
#include <asiUI_ViewerSurface.h>

// Active Data includes
#include <ActAPI_IPlotter.h>

// Qt includes
#include <QObject>

//! Default slots for part viewer.
class asiUI_EXPORT asiUI_ViewerPartListener : public QObject
{
  Q_OBJECT

public:

  asiUI_ViewerPartListener(asiUI_ViewerPart*              wViewerPart,
                           asiUI_ViewerDomain*            wViewerDomain,
                           asiUI_ViewerSurface*           wViewerSurface,
                           const Handle(asiEngine_Model)& model,
                           ActAPI_ProgressEntry           progress,
                           ActAPI_PlotterEntry            plotter);

  virtual
    ~asiUI_ViewerPartListener();

public:

  virtual void
    Connect();

protected slots:

  void
    onFacePicked(const asiVisu_PickResult& pickRes);

  void
    onEdgePicked(const asiVisu_PickResult& pickRes);

  void
    onContextMenu(const QPoint&);

protected:

  asiUI_ViewerPart*       m_wViewerPart;    //!< Part viewer.
  asiUI_ViewerDomain*     m_wViewerDomain;  //!< Face domain viewer.
  asiUI_ViewerSurface*    m_wViewerSurface; //!< Surface viewer.
  Handle(asiEngine_Model) m_model;          //!< Data Model instance.
  ActAPI_ProgressEntry    m_progress;       //!< Progress notifier.
  ActAPI_PlotterEntry     m_plotter;        //!< Imperative plotter.

};

#endif
