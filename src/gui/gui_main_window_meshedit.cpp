//-----------------------------------------------------------------------------
// Created on: 14 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <gui_main_window_meshedit.h>

// Qt includes
#pragma warning(push, 0)
#include <QDesktopWidget>
#include <QDockWidget>
#pragma warning(pop)

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
gui_main_window_meshedit::gui_main_window_meshedit() : QMainWindow()
{
  this->createMeshViewer();
  this->createDockWindows();

  this->setCentralWidget(m_widgets.wViewerMesh);
  this->setWindowTitle("Analysis Situs [Mesh Editor]");
}

//! Destructor.
gui_main_window_meshedit::~gui_main_window_meshedit()
{}

//-----------------------------------------------------------------------------

//! Gets control on window close.
//! \param evt [in] event.
void gui_main_window_meshedit::closeEvent(QCloseEvent* evt)
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

//! Creates main (mesh) viewer.
void gui_main_window_meshedit::createMeshViewer()
{
  m_widgets.wViewerMesh = new gui_viewer_mesh();

  // Desktop used for sizing
  QDesktopWidget desktop;
  const int side   = std::min( desktop.height(), desktop.width() );
  const int width  = side*0.8;
  const int height = side*0.7;
  //
  m_widgets.wViewerMesh->setMinimumSize(width, height);
}

//! Creates main dockable widgets.
void gui_main_window_meshedit::createDockWindows()
{
  //---------------------------------------------------------------------------
  // Object browser
  {
    QDockWidget* pDock = new QDockWidget("Stored Objects", this);
    pDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wBrowser = new gui_object_browser(pDock);
    pDock->setWidget(m_widgets.wBrowser);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDock);
  }
}
