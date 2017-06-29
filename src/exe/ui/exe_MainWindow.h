//-----------------------------------------------------------------------------
// Created on: 07 December 2015
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

#ifndef exe_MainWindow_h
#define exe_MainWindow_h

// asiUI includes
#include <asiUI_ControlsFeature.h>
#include <asiUI_ControlsPartListener.h>
#include <asiUI_ObjectBrowser.h>
#include <asiUI_ViewerPartListener.h>

// Qt includes
#pragma warning(push, 0)
#include <QCloseEvent>
#include <QMainWindow>
#include <QTextEdit>
#pragma warning(pop)

//! Main window for Analysis Situs.
class exe_MainWindow : public QMainWindow
{
  Q_OBJECT

public:

  exe_MainWindow();
  virtual ~exe_MainWindow();

public:

  void closeEvent(QCloseEvent* evt);

private:

  void createPartViewer();
  void createDockWindows();
  void createMenus();
  void createToolbar();

private:

  //! Widgets.
  struct t_widgets
  {
    asiUI_ObjectBrowser*   wBrowser;         //!< Object browser.
    asiUI_ViewerDomain*    wViewerDomain;    //!< Parametric domain viewer.
    asiUI_ViewerPart*      wViewerPart;      //!< Part viewer.
    asiUI_ViewerSurface*   wViewerSurface;   //!< Surface viewer.
    asiUI_ControlsPart*    wControlsPart;    //!< Part controls.
    asiUI_ControlsFeature* wControlsFeature; //!< Feature controls.
    QTextEdit*             wLogger;          //!< Logger.

    t_widgets() : wBrowser         (NULL),
                  wViewerDomain    (NULL),
                  wViewerPart      (NULL),
                  wViewerSurface   (NULL),
                  wControlsPart    (NULL),
                  wControlsFeature (NULL),
                  wLogger          (NULL)
    {}

    void Release()
    {
      delete wBrowser;         wBrowser         = NULL;
      delete wViewerDomain;    wViewerDomain    = NULL;
      delete wViewerPart;      wViewerPart      = NULL;
      delete wViewerSurface;   wViewerSurface   = NULL;
      delete wControlsPart;    wControlsPart    = NULL;
      delete wControlsFeature; wControlsFeature = NULL;
      delete wLogger;          wLogger          = NULL;
    }
  };

  //! Listeners.
  struct t_listeners
  {
    asiUI_ControlsPartListener* pControlsPart; //!< Listener for part controls.
    asiUI_ViewerPartListener*   pViewerPart;   //!< Listener for part viewer.

    t_listeners() : pControlsPart (NULL),
                    pViewerPart   (NULL)
    {}

    void Release()
    {
      delete pControlsPart; pControlsPart = NULL;
      delete pViewerPart;   pViewerPart   = NULL;
    }
  };

  //! Menu items.
  struct t_menus
  {
  };

  //! Toolbar items.
  struct t_toolbar
  {
  };

  t_widgets   m_widgets;
  t_listeners m_listeners;
  t_menus     m_menus;
  t_toolbar   m_toolbar;

};

#endif
