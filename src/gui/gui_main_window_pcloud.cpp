//-----------------------------------------------------------------------------
// Created on: 21 June 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_main_window_pcloud.h>

// Qt includes
#pragma warning(push, 0)
#include <QDesktopWidget>
#include <QDockWidget>
#pragma warning(pop)

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
gui_main_window_pcloud::gui_main_window_pcloud() : QMainWindow()
{
  this->createPartViewer();
  this->createDockWindows();

  this->setCentralWidget(m_widgets.wViewer);
  this->setWindowTitle("Point Clouds");
}

//! Destructor.
gui_main_window_pcloud::~gui_main_window_pcloud()
{}

//-----------------------------------------------------------------------------

//! Gets control on window close.
//! \param evt [in] event.
void gui_main_window_pcloud::closeEvent(QCloseEvent* evt)
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

//! Creates main (part) viewer.
void gui_main_window_pcloud::createPartViewer()
{
  m_widgets.wViewer = new gui_viewer_part();

  // Desktop used for sizing
  QDesktopWidget desktop;
  const int side   = std::min( desktop.height(), desktop.width() );
  const int width  = side*0.25;
  const int height = side*0.25;
  //
  m_widgets.wViewer->setMinimumSize(width, height);
}

//! Creates main dockable widgets.
void gui_main_window_pcloud::createDockWindows()
{
  // Desktop used for sizing
  /*QDesktopWidget desktop;
  const int side  = std::min( desktop.height(), desktop.width() );
  const int width = side*0.25;*/

  //---------------------------------------------------------------------------
  // Part controls
  QDockWidget* pDockCommon;
  {
    pDockCommon = new QDockWidget("Common", this);
    pDockCommon->setAllowedAreas(Qt::LeftDockWidgetArea);
    //
    m_widgets.wControls = new gui_controls_pcloud(pDockCommon);
    pDockCommon->setWidget(m_widgets.wControls);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockCommon);
  }
  //---------------------------------------------------------------------------
  // Object browser
  QDockWidget* pDockBrowser;
  {
    pDockBrowser = new QDockWidget("Model", this);
    pDockBrowser->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wBrowser = new gui_object_browser(pDockBrowser);
    pDockBrowser->setWidget(m_widgets.wBrowser);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockBrowser);
  }
  //---------------------------------------------------------------------------
  this->tabifyDockWidget(pDockBrowser, pDockCommon);
}
