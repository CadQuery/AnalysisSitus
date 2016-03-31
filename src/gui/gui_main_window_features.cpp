//-----------------------------------------------------------------------------
// Created on: 31 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_main_window_features.h>

// Qt includes
#pragma warning(push, 0)
#include <QDesktopWidget>
#include <QDockWidget>
#pragma warning(pop)

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
gui_main_window_features::gui_main_window_features() : QMainWindow()
{
  this->createPartViewer();
  this->createDockWindows();

  this->setCentralWidget(m_widgets.wViewerPart);
  this->setWindowTitle("Features");
}

//! Destructor.
gui_main_window_features::~gui_main_window_features()
{}

//-----------------------------------------------------------------------------

//! Gets control on window close.
//! \param evt [in] event.
void gui_main_window_features::closeEvent(QCloseEvent* evt)
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
void gui_main_window_features::createPartViewer()
{
  m_widgets.wViewerPart = new gui_viewer_part();

  // Desktop used for sizing
  QDesktopWidget desktop;
  const int side   = std::min( desktop.height(), desktop.width() );
  const int width  = side*0.5;
  const int height = side*0.5;
  //
  m_widgets.wViewerPart->setMinimumSize(width, height);
}

//! Creates main dockable widgets.
void gui_main_window_features::createDockWindows()
{
  // Desktop used for sizing
  QDesktopWidget desktop;
  const int side  = std::min( desktop.height(), desktop.width() );
  const int width = side*0.4;

  //---------------------------------------------------------------------------
  // Part controls
  QDockWidget* pDockCommon;
  {
    pDockCommon = new QDockWidget("Common tools", this);
    pDockCommon->setAllowedAreas(Qt::LeftDockWidgetArea);
    //
    m_widgets.wControlsPart = new gui_controls_part(pDockCommon);
    pDockCommon->setWidget(m_widgets.wControlsPart);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockCommon);
  }
  //---------------------------------------------------------------------------
  // Feature controls
  QDockWidget* pDockFeatures;
  {
    pDockFeatures = new QDockWidget("Features", this);
    pDockFeatures->setAllowedAreas(Qt::LeftDockWidgetArea);
    //
    m_widgets.wControlsFeatures = new gui_controls_features(pDockFeatures);
    pDockFeatures->setWidget(m_widgets.wControlsFeatures);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockFeatures);
  }
  //---------------------------------------------------------------------------
  // Object browser
  QDockWidget* pDockBrowser;
  {
    pDockBrowser = new QDockWidget("Stored Objects", this);
    pDockBrowser->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wBrowser = new gui_object_browser(pDockBrowser);
    pDockBrowser->setWidget(m_widgets.wBrowser);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockBrowser);
  }
  //---------------------------------------------------------------------------
  // Face Domain viewer
  {
    QDockWidget* pDock = new QDockWidget("Face Domain", this);
    pDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wViewerDomain = new gui_viewer_domain(pDock);
    pDock->setWidget(m_widgets.wViewerDomain);
    pDock->setMinimumWidth(width);
    //
    this->addDockWidget(Qt::RightDockWidgetArea, pDock);
  }
  //---------------------------------------------------------------------------
  // Surface viewer
  {
    QDockWidget* pDock = new QDockWidget("Host Surface", this);
    pDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wViewerSurface = new gui_viewer_surface(pDock);
    pDock->setWidget(m_widgets.wViewerSurface);
    pDock->setMinimumWidth(width);
    //
    this->addDockWidget(Qt::RightDockWidgetArea, pDock);
  }
  //---------------------------------------------------------------------------
  this->tabifyDockWidget(pDockBrowser, pDockCommon);
  this->tabifyDockWidget(pDockCommon,  pDockFeatures);
}
