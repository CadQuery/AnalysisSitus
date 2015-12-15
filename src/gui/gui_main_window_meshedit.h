//-----------------------------------------------------------------------------
// Created on: 14 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef gui_main_window_meshedit_h
#define gui_main_window_meshedit_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <gui_object_browser.h>
#include <gui_viewer_mesh.h>

// Qt includes
#pragma warning(push, 0)
#include <QCloseEvent>
#include <QMainWindow>
#pragma warning(pop)

//! Main window for Mesh Editor composition.
class gui_main_window_meshedit : public QMainWindow
{
  Q_OBJECT

public:

  gui_main_window_meshedit();
  virtual ~gui_main_window_meshedit();

public:

  void closeEvent(QCloseEvent* evt);

private:

  void createMeshViewer();
  void createDockWindows();

private:

  //! Widgets.
  struct t_widgets
  {
    gui_object_browser* wBrowser;    //!< Object browser.
    gui_viewer_mesh*    wViewerMesh; //!< Mesh viewer.

    t_widgets() : wBrowser    (NULL),
                  wViewerMesh (NULL)
    {}

    void Release()
    {
      delete wBrowser;    wBrowser    = NULL;
      delete wViewerMesh; wViewerMesh = NULL;
    }
  };

  t_widgets m_widgets; //!< Involved widgets.

};

#endif
