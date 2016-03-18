//-----------------------------------------------------------------------------
// Created on: 03 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_main_window_hull_h
#define gui_main_window_hull_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <gui_controls_hull.h>
#include <gui_object_browser.h>
#include <gui_viewer_part.h>

// Qt includes
#pragma warning(push, 0)
#include <QCloseEvent>
#include <QMainWindow>
#pragma warning(pop)

//! Main window for hull reconstruction composition.
class gui_main_window_hull : public QMainWindow
{
  Q_OBJECT

public:

  gui_main_window_hull();
  virtual ~gui_main_window_hull();

public:

  void closeEvent(QCloseEvent* evt);

private:

  void createMainViewer();
  void createDockWindows();

private:

  //! Widgets.
  struct t_widgets
  {
    gui_object_browser* wBrowser;    //!< Object browser.
    gui_viewer_part*    wViewerMain; //!< Main viewer.
    gui_controls_hull*  wControls;   //!< Controls.

    t_widgets() : wBrowser    (NULL),
                  wViewerMain (NULL),
                  wControls   (NULL)
    {}

    void Release()
    {
      delete wBrowser;    wBrowser    = NULL;
      delete wViewerMain; wViewerMain = NULL;
      delete wControls;   wControls   = NULL;
    }
  };

  t_widgets m_widgets; //!< Involved widgets.

};

#endif
