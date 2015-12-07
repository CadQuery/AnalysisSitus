//-----------------------------------------------------------------------------
// Created on: 07 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef gui_main_window_h
#define gui_main_window_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <gui_object_browser.h>
#include <gui_viewer_domain.h>
#include <gui_viewer_part.h>
#include <gui_viewer_surface.h>

// Qt includes
#pragma warning(push, 0)
#include <QMainWindow>
#pragma warning(pop)

//! Main window.
class gui_main_window : public QMainWindow
{
Q_OBJECT

public:

  gui_main_window();
  virtual ~gui_main_window();

private:

  void createPartViewer();
  void createStatusBar();
  void createDockWindows();

private:

  //! Widgets.
  struct t_widgets
  {
    gui_object_browser* wBrowser;       //!< Object browser..
    gui_viewer_domain*  wViewerDomain;  //!< Parametric domain viewer.
    gui_viewer_part*    wViewerPart;    //!< Part viewer.
    gui_viewer_surface* wViewerSurface; //!< Surface viewer.

    t_widgets() : wBrowser(NULL),
                  wViewerDomain(NULL),
                  wViewerPart(NULL),
                  wViewerSurface(NULL)
    {}
  };

  t_widgets m_widgets;

};

#endif
