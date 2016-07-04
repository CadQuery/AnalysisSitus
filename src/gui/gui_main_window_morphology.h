//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_main_window_morphology_h
#define gui_main_window_morphology_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <gui_controls_features.h>
#include <gui_controls_morphology.h>
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

//! Main window for Morphology composition.
class gui_main_window_morphology : public QMainWindow
{
  Q_OBJECT

public:

  gui_main_window_morphology();
  virtual ~gui_main_window_morphology();

public:

  void closeEvent(QCloseEvent* evt);

private:

  void createPartViewer();
  void createDockWindows();

private:

  //! Widgets.
  struct t_widgets
  {
    gui_object_browser*        wBrowser;            //!< Object browser.
    gui_viewer_domain*         wViewerDomain;       //!< Parametric domain viewer.
    gui_viewer_part*           wViewerPart;         //!< Part viewer.
    gui_viewer_surface*        wViewerSurface;      //!< Surface viewer.
    gui_controls_part*         wControlsPart;       //!< Part controls.
    gui_controls_features*     wControlsFeatures;   //!< Controls for features.
    gui_controls_morphology*   wControlsMorphology; //!< Controls for morphology.

    t_widgets() : wBrowser            (NULL),
                  wViewerDomain       (NULL),
                  wViewerPart         (NULL),
                  wViewerSurface      (NULL),
                  wControlsPart       (NULL),
                  wControlsFeatures   (NULL),
                  wControlsMorphology (NULL)
    {}

    void Release()
    {
      delete wBrowser;            wBrowser            = NULL;
      delete wViewerDomain;       wViewerDomain       = NULL;
      delete wViewerPart;         wViewerPart         = NULL;
      delete wViewerSurface;      wViewerSurface      = NULL;
      delete wControlsPart;       wControlsPart       = NULL;
      delete wControlsFeatures;   wControlsFeatures   = NULL;
      delete wControlsMorphology; wControlsMorphology = NULL;
    }
  };

  t_widgets m_widgets;

};

#endif
