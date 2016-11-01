//-----------------------------------------------------------------------------
// Created on: 31 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeFeatures_MainWindow_h
#define exeFeatures_MainWindow_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <gui_controls_euler.h>
#include <exeFeatures_Controls.h>
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

//! Main window for Features composition.
class exeFeatures_MainWindow : public QMainWindow
{
  Q_OBJECT

public:

  exeFeatures_MainWindow();
  virtual ~exeFeatures_MainWindow();

public:

  void closeEvent(QCloseEvent* evt);

private:

  void createPartViewer();
  void createDockWindows();

private:

  //! Widgets.
  struct t_widgets
  {
    gui_object_browser*    wBrowser;          //!< Object browser.
    gui_viewer_domain*     wViewerDomain;     //!< Parametric domain viewer.
    gui_viewer_part*       wViewerPart;       //!< Part viewer.
    gui_viewer_surface*    wViewerSurface;    //!< Surface viewer.
    gui_controls_part*     wControlsPart;     //!< Part controls.
    exeFeatures_Controls* wControlsFeatures; //!< Controls for features.
    gui_controls_euler*    wControlsEuler;    //!< Controls for Euler operations.

    t_widgets() : wBrowser          (NULL),
                  wViewerDomain     (NULL),
                  wViewerPart       (NULL),
                  wViewerSurface    (NULL),
                  wControlsPart     (NULL),
                  wControlsFeatures (NULL),
                  wControlsEuler    (NULL)
    {}

    void Release()
    {
      delete wBrowser;           wBrowser           = NULL;
      delete wViewerDomain;      wViewerDomain      = NULL;
      delete wViewerPart;        wViewerPart        = NULL;
      delete wViewerSurface;     wViewerSurface     = NULL;
      delete wControlsPart;      wControlsPart      = NULL;
      delete wControlsFeatures;  wControlsFeatures  = NULL;
      delete wControlsEuler;     wControlsEuler     = NULL;
    }
  };

  t_widgets m_widgets;

};

#endif
