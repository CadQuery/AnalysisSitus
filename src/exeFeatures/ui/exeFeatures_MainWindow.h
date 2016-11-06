//-----------------------------------------------------------------------------
// Created on: 31 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeFeatures_MainWindow_h
#define exeFeatures_MainWindow_h

// asiUI includes
#include <asiUI_ControlsPart.h>
#include <asiUI_ObjectBrowser.h>
#include <asiUI_ViewerDomain.h>
#include <asiUI_ViewerPart.h>
#include <asiUI_ViewerSurface.h>

// exeFeatures includes
#include <exeFeatures_Controls.h>
#include <exeFeatures_ControlsEuler.h>

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
    asiUI_ObjectBrowser*       wBrowser;          //!< Object browser.
    asiUI_ViewerDomain*        wViewerDomain;     //!< Parametric domain viewer.
    asiUI_ViewerPart*          wViewerPart;       //!< Part viewer.
    asiUI_ViewerSurface*       wViewerSurface;    //!< Surface viewer.
    asiUI_ControlsPart*        wControlsPart;     //!< Part controls.
    exeFeatures_Controls*      wControlsFeatures; //!< Controls for features.
    exeFeatures_ControlsEuler* wControlsEuler;    //!< Controls for Euler operations.

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
