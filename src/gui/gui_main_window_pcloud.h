//-----------------------------------------------------------------------------
// Created on: 21 June 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_main_window_pcloud_h
#define gui_main_window_pcloud_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <gui_controls_pcloud.h>
#include <gui_object_browser.h>
#include <gui_viewer_part.h>

// Qt includes
#pragma warning(push, 0)
#include <QCloseEvent>
#include <QMainWindow>
#pragma warning(pop)

//! Main window for Point Cloud composition.
class gui_main_window_pcloud : public QMainWindow
{
  Q_OBJECT

public:

  gui_main_window_pcloud();
  virtual ~gui_main_window_pcloud();

public:

  void closeEvent(QCloseEvent* evt);

private:

  void createPartViewer();
  void createDockWindows();

private:

  //! Widgets.
  struct t_widgets
  {
    gui_object_browser*  wBrowser;  //!< Object browser.
    gui_viewer_part*     wViewer;   //!< Part viewer.
    gui_controls_pcloud* wControls; //!< Part controls.

    t_widgets() : wBrowser  (NULL),
                  wViewer   (NULL),
                  wControls (NULL)
    {}

    void Release()
    {
      delete wBrowser;  wBrowser  = NULL;
      delete wViewer;   wViewer   = NULL;
      delete wControls; wControls = NULL;
    }
  };

  t_widgets m_widgets;

};

#endif
