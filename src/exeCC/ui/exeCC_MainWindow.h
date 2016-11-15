//-----------------------------------------------------------------------------
// Created on: 16 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeCC_MainWindow_h
#define exeCC_MainWindow_h

// exeCC includes
#include <exeCC_Controls.h>

// asiUI includes
#include <asiUI_ControlsPart.h>
#include <asiUI_ControlsPartListener.h>
#include <asiUI_ObjectBrowser.h>
#include <asiUI_ViewerDomain.h>
#include <asiUI_ViewerPart.h>
#include <asiUI_ViewerPartListener.h>
#include <asiUI_ViewerSurface.h>

// Qt includes
#pragma warning(push, 0)
#include <QCloseEvent>
#include <QMainWindow>
#pragma warning(pop)

//! Main window for Contour Capture composition.
class exeCC_MainWindow : public QMainWindow
{
  Q_OBJECT

public:

  exeCC_MainWindow();
  virtual ~exeCC_MainWindow();

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
    asiUI_ViewerDomain*  wViewerDomain;  //!< Parametric domain viewer.
    asiUI_ViewerPart*    wViewerPart;    //!< Part viewer.
    asiUI_ViewerSurface* wViewerSurface; //!< Surface viewer.
    asiUI_ControlsPart*  wControlsPart;  //!< Part controls.
    exeCC_Controls*      wControlsCC;    //!< Controls for contour capture.

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

  //! Listeners.
  struct t_listeners
  {
    asiUI_ControlsPartListener* pControlsPart; //!< Listener for part controls.
    asiUI_ViewerPartListener*   pViewerPart;   //!< Listener for part viewer.

    t_listeners() : pControlsPart (NULL),
                    pViewerPart   (NULL)
    {}

    void Release()
    {
      delete pControlsPart; pControlsPart = NULL;
      delete pViewerPart;   pViewerPart   = NULL;
    }
  };

  t_widgets   m_widgets;
  t_listeners m_listeners;

};

#endif
