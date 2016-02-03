//-----------------------------------------------------------------------------
// Created on: 01 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <gui_main_window_dmu.h>

// Qt includes
#pragma warning(push, 0)
#include <QDesktopWidget>
#include <QDockWidget>
#pragma warning(pop)

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
gui_main_window_dmu::gui_main_window_dmu() : QMainWindow()
{
  this->createViewer();
  this->createDockWindows();

  this->setCentralWidget(m_widgets.wViewer);
  this->setWindowTitle("Analysis Situs [dMU Prototype]");
}

//! Destructor.
gui_main_window_dmu::~gui_main_window_dmu()
{}

//-----------------------------------------------------------------------------

//! Gets control on window close.
//! \param evt [in] event.
void gui_main_window_dmu::closeEvent(QCloseEvent* evt)
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
void gui_main_window_dmu::createViewer()
{
  m_widgets.wViewer = new gui_viewer_dmu();

  // Desktop used for sizing
  QDesktopWidget desktop;
  const int side   = std::min( desktop.height(), desktop.width() );
  const int width  = side*0.8;
  const int height = side*0.7;
  //
  m_widgets.wViewer->setMinimumSize(width, height);
}

//! Creates main dockable widgets.
void gui_main_window_dmu::createDockWindows()
{
  //---------------------------------------------------------------------------
  // Object browser
  {
    QDockWidget* pDock = new QDockWidget("XDE model browser", this);
    pDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wBrowser = new gui_object_browser_xde(pDock);
    pDock->setWidget(m_widgets.wBrowser);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDock);
  }
}
