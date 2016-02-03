//-----------------------------------------------------------------------------
// Created on: 02 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef gui_main_window_ubend_h
#define gui_main_window_ubend_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <gui_object_browser.h>
#include <gui_viewer_ubend.h>

// Qt includes
#pragma warning(push, 0)
#include <QCloseEvent>
#include <QMainWindow>
#pragma warning(pop)

//! Main window for U-bend composition.
class gui_main_window_ubend : public QMainWindow
{
  Q_OBJECT

public:

  gui_main_window_ubend();
  virtual ~gui_main_window_ubend();

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
    gui_viewer_ubend*   wViewerMain; //!< Main viewer.

    t_widgets() : wBrowser    (NULL),
                  wViewerMain (NULL)
    {}

    void Release()
    {
      delete wBrowser;    wBrowser    = NULL;
      delete wViewerMain; wViewerMain = NULL;
    }
  };

  t_widgets m_widgets; //!< Involved widgets.

};

#endif
