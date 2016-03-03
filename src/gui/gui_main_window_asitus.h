//-----------------------------------------------------------------------------
// Created on: 07 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_main_window_asitus_h
#define gui_main_window_asitus_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <gui_object_browser.h>
#include <gui_viewer_domain.h>
#include <gui_viewer_part.h>
#include <gui_viewer_surface.h>

// Qt includes
#pragma warning(push, 0)
#include <QCloseEvent>
#include <QMainWindow>
#pragma warning(pop)

//! Main window for Analysis composition.
class gui_main_window_asitus : public QMainWindow
{
  Q_OBJECT

public:

  gui_main_window_asitus();
  virtual ~gui_main_window_asitus();

public:

  void closeEvent(QCloseEvent* evt);

private:

  void createPartViewer();
  void createDockWindows();

private:

  //! Widgets.
  struct t_widgets
  {
    gui_object_browser* wBrowser;       //!< Object browser.
    gui_viewer_domain*  wViewerDomain;  //!< Parametric domain viewer.
    gui_viewer_part*    wViewerPart;    //!< Part viewer.
    gui_viewer_surface* wViewerSurface; //!< Surface viewer.

    t_widgets() : wBrowser(NULL),
                  wViewerDomain(NULL),
                  wViewerPart(NULL),
                  wViewerSurface(NULL)
    {}

    void Release()
    {
      delete wBrowser;       wBrowser       = NULL;
      delete wViewerDomain;  wViewerDomain  = NULL;
      delete wViewerPart;    wViewerPart    = NULL;
      delete wViewerSurface; wViewerSurface = NULL;
    }
  };

  t_widgets m_widgets;

};

#endif
