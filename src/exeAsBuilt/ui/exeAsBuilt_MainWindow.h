//-----------------------------------------------------------------------------
// Created on: 21 June 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeAsBuilt_MainWindow_h
#define exeAsBuilt_MainWindow_h

// exeAsBuilt includes
#include <exeAsBuilt_ControlsPCloud.h>

// asiUI includes
#include <asiUI_ControlsPart.h>
#include <asiUI_ControlsPartListener.h>
#include <asiUI_ObjectBrowser.h>
#include <asiUI_ViewerPart.h>
#include <asiUI_ViewerPartListener.h>

// Qt includes
#pragma warning(push, 0)
#include <QCloseEvent>
#include <QMainWindow>
#pragma warning(pop)

//! Main window for Point Cloud composition.
class exeAsBuilt_MainWindow : public QMainWindow
{
  Q_OBJECT

public:

  exeAsBuilt_MainWindow();
  virtual ~exeAsBuilt_MainWindow();

public:

  void closeEvent(QCloseEvent* evt);

private:

  void createPartViewer();
  void createDockWindows();

private:

  //! Widgets.
  struct t_widgets
  {
    asiUI_ObjectBrowser*       wBrowser;      //!< Object browser.
    asiUI_ViewerPart*          wViewer;       //!< Part viewer.
    exeAsBuilt_ControlsPCloud* wControls;     //!< Controls to operate with point clouds.
    asiUI_ControlsPart*        wControlsPart; //!< Part controls.

    t_widgets() : wBrowser      (NULL),
                  wViewer       (NULL),
                  wControls     (NULL),
                  wControlsPart (NULL)
    {}

    void Release()
    {
      delete wBrowser;      wBrowser      = NULL;
      delete wViewer;       wViewer       = NULL;
      delete wControls;     wControls     = NULL;
      delete wControlsPart; wControlsPart = NULL;
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
