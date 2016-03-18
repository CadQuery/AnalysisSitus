//-----------------------------------------------------------------------------
// Created on: 03 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_main_window_hull.h>

// Analysis Situs (common) includes
#include <common_facilities.h>

// VTK includes
#include <vtkProperty.h>

// Qt includes
#pragma warning(push, 0)
#include <QDesktopWidget>
#include <QDockWidget>
#pragma warning(pop)

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
gui_main_window_hull::gui_main_window_hull() : QMainWindow()
{
  this->createDockWindows();
  this->createMainViewer();
  //
  this->setCentralWidget(m_widgets.wViewerMain);
  this->setWindowTitle("Analysis Situs [Hull Reconstruction]");
}

//! Destructor.
gui_main_window_hull::~gui_main_window_hull()
{}

//-----------------------------------------------------------------------------

//! Gets control on window close.
//! \param evt [in] event.
void gui_main_window_hull::closeEvent(QCloseEvent* evt)
{
  // It seems that we have to destruct objects properly and manually in
  // order to avoid some side effects from VTK. E.g. if we don't kill the
  // widgets explicitly here, we may sometimes get a warning window of VTK
  // saying that it lacks some resources
  m_widgets.Release();
  //
  evt->accept();
}

//-----------------------------------------------------------------------------

//! Creates main viewer.
void gui_main_window_hull::createMainViewer()
{
  m_widgets.wViewerMain = new gui_viewer_part();

  // Desktop used for sizing
  QDesktopWidget desktop;
  const int side   = std::min( desktop.height(), desktop.width() );
  const int width  = side*0.8;
  const int height = side*0.7;
  //
  m_widgets.wViewerMain->setMinimumSize(width, height);
}

//! Creates main dockable widgets.
void gui_main_window_hull::createDockWindows()
{
  // Desktop used for sizing
  //QDesktopWidget desktop;
  //const int side   = std::min( desktop.height(), desktop.width() );
  //const int width  = side*0.3;
  //const int height = width;

  //---------------------------------------------------------------------------
  // Object browser
  {
    QDockWidget* pDock = new QDockWidget("Model Browser", this);
    pDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wBrowser = new gui_object_browser(pDock);
    pDock->setWidget(m_widgets.wBrowser);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDock);
  }
  //---------------------------------------------------------------------------
  // Controls
  {
    QDockWidget* pDock = new QDockWidget("Tools", this);
    pDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wControls = new gui_controls_hull(pDock);
    pDock->setWidget(m_widgets.wControls);
    //
    this->addDockWidget(Qt::RightDockWidgetArea, pDock);
  }
}
