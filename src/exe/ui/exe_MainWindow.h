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
#include <asiUI_ControlsPart.h>
#include <asiUI_ObjectBrowser.h>
#include <asiUI_Viewer_domain.h>
#include <asiUI_ViewerPart.h>
#include <asiUI_Viewer_surface.h>

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
    asiUI_ObjectBrowser* wBrowser;       //!< Object browser.
    asiUI_Viewer_domain*  wViewerDomain;  //!< Parametric domain viewer.
    asiUI_ViewerPart*    wViewerPart;    //!< Part viewer.
    asiUI_Viewer_surface* wViewerSurface; //!< Surface viewer.
    asiUI_ControlsPart*  wControlsPart;  //!< Part controls.

    t_widgets() : wBrowser       (NULL),
                  wViewerDomain  (NULL),
                  wViewerPart    (NULL),
                  wViewerSurface (NULL),
                  wControlsPart  (NULL)
    {}

    void Release()
    {
      delete wBrowser;       wBrowser       = NULL;
      delete wViewerDomain;  wViewerDomain  = NULL;
      delete wViewerPart;    wViewerPart    = NULL;
      delete wViewerSurface; wViewerSurface = NULL;
      delete wControlsPart;  wControlsPart  = NULL;
    }
  };

  t_widgets m_widgets;

};

#endif
