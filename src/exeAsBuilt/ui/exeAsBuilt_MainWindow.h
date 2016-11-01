//-----------------------------------------------------------------------------
// Created on: 21 June 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeAsBuilt_MainWindow_h
#define exeAsBuilt_MainWindow_h

// EXE includes
#include <exeAsBuilt_ControlsPCloud.h>

// A-Situs (GUI) includes
#include <asiUI_ObjectBrowser.h>
#include <asiUI_ViewerPart.h>

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
