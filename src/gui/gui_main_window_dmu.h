//-----------------------------------------------------------------------------
// Created on: 01 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef gui_main_window_dmu_h
#define gui_main_window_dmu_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <gui_object_browser_xde.h>
#include <gui_viewer_dmu.h>

// Qt includes
#pragma warning(push, 0)
#include <QCloseEvent>
#include <QMainWindow>
#pragma warning(pop)

//! Main window for dMU composition.
class gui_main_window_dmu : public QMainWindow
{
  Q_OBJECT

public:

  gui_main_window_dmu();
  virtual ~gui_main_window_dmu();

public:

  void closeEvent(QCloseEvent* evt);

private:

  void createViewer();
  void createDockWindows();

private:

  //! Widgets.
  struct t_widgets
  {
    gui_object_browser_xde* wBrowser; //!< Object browser.
    gui_viewer_dmu*         wViewer;  //!< Viewer.

    t_widgets() : wBrowser (NULL),
                  wViewer  (NULL)
    {}

    void Release()
    {
      delete wBrowser; wBrowser = NULL;
      delete wViewer;  wViewer  = NULL;
    }
  };

  t_widgets m_widgets; //!< Involved widgets.

};

#endif
