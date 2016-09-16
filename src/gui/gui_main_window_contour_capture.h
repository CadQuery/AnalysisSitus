//-----------------------------------------------------------------------------
// Created on: 16 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_main_window_contour_capture_h
#define gui_main_window_contour_capture_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <gui_controls_cc.h>
#include <gui_controls_part.h>
#include <gui_object_browser.h>
#include <gui_viewer_domain.h>
#include <gui_viewer_part.h>
#include <gui_viewer_surface.h>

// Qt includes
#pragma warning(push, 0)
#include <QCloseEvent>
#include <QMainWindow>
#pragma warning(pop)

//! Main window for Contour Capture composition.
class gui_main_window_contour_capture : public QMainWindow
{
  Q_OBJECT

public:

  gui_main_window_contour_capture();
  virtual ~gui_main_window_contour_capture();

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
    gui_controls_part*  wControlsPart;  //!< Part controls.
    gui_controls_cc*    wControlsCC;    //!< Controls for contour capture.

    t_widgets() : wBrowser       (NULL),
                  wViewerDomain  (NULL),
                  wViewerPart    (NULL),
                  wViewerSurface (NULL),
                  wControlsPart  (NULL),
                  wControlsCC    (NULL)
    {}

    void Release()
    {
      delete wBrowser;       wBrowser       = NULL;
      delete wViewerDomain;  wViewerDomain  = NULL;
      delete wViewerPart;    wViewerPart    = NULL;
      delete wViewerSurface; wViewerSurface = NULL;
      delete wControlsPart;  wControlsPart  = NULL;
      delete wControlsCC;    wControlsCC    = NULL;
    }
  };

  t_widgets m_widgets;

};

#endif
