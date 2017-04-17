//-----------------------------------------------------------------------------
// Created on: 07 December 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <exe_MainWindow.h>

// exe includes
#include <exe_CommonFacilities.h>
#include <exe_Version.h>

// asiUI includes
#include <asiUI_StatusBar.h>
#include <asiUI_StatusBarImpl.h>

// Qt includes
#pragma warning(push, 0)
#include <QDesktopWidget>
#include <QDockWidget>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Constructor.
exe_MainWindow::exe_MainWindow() : QMainWindow()
{
  this->createPartViewer();
  this->createDockWindows();
  this->setCentralWidget(m_widgets.wViewerPart);

  // Prepare application name with the version number
  TCollection_AsciiString appName(ASitus_APP_NAME);
  appName += " ["; appName += ASitus_Version_STRING; appName += "]";
  //
  this->setWindowTitle( appName.ToCString() );
}

//-----------------------------------------------------------------------------

//! Destructor.
exe_MainWindow::~exe_MainWindow()
{}

//-----------------------------------------------------------------------------

//! Gets control on window close.
//! \param evt [in] event.
void exe_MainWindow::closeEvent(QCloseEvent* evt)
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
void exe_MainWindow::createPartViewer()
{
  // Common facilities instance
  Handle(exe_CommonFacilities) cf = exe_CommonFacilities::Instance();

  // Create viewer
  m_widgets.wViewerPart = new asiUI_ViewerPart(cf->Model);

  // Desktop used for sizing
  QDesktopWidget desktop;
  const int side   = std::min( desktop.height(), desktop.width() );
  const int width  = side*0.5;
  const int height = side*0.5;
  //
  m_widgets.wViewerPart->setMinimumSize(width, height);

  // Initialize desktop
  cf->ViewerPart = m_widgets.wViewerPart;
  cf->Prs.Part   = m_widgets.wViewerPart->PrsMgr();
}

//-----------------------------------------------------------------------------

//! Creates main dockable widgets.
void exe_MainWindow::createDockWindows()
{
  // Desktop used for sizing
  QDesktopWidget desktop;
  const int side  = std::min( desktop.height(), desktop.width() );
  const int width = side*0.4;

  // Common facilities instance
  Handle(exe_CommonFacilities) cf = exe_CommonFacilities::Instance();

  // Object browser
  QDockWidget* pDockBrowser;
  {
    pDockBrowser = new QDockWidget("Stored Objects", this);
    pDockBrowser->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wBrowser = new asiUI_ObjectBrowser(cf->Model, cf->ViewerPart, pDockBrowser);
    //
    pDockBrowser->setWidget(m_widgets.wBrowser);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockBrowser);

    // Initialize desktop
    cf->ObjectBrowser = m_widgets.wBrowser;
  }

  // Face Domain viewer
  {
    QDockWidget* pDock = new QDockWidget("Face Domain", this);
    pDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wViewerDomain = new asiUI_ViewerDomain(cf->Model, pDock);
    pDock->setWidget(m_widgets.wViewerDomain);
    pDock->setMinimumWidth(width);
    //
    this->addDockWidget(Qt::RightDockWidgetArea, pDock);

    // Initialize desktop
    cf->ViewerDomain = m_widgets.wViewerDomain;
    cf->Prs.Domain   = m_widgets.wViewerDomain->PrsMgr();
  }

  // Surface viewer
  {
    QDockWidget* pDock = new QDockWidget("Host Surface", this);
    pDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wViewerSurface = new asiUI_ViewerSurface(pDock);
    pDock->setWidget(m_widgets.wViewerSurface);
    pDock->setMinimumWidth(width);
    //
    this->addDockWidget(Qt::RightDockWidgetArea, pDock);

    // Initialize desktop
    cf->ViewerHost = m_widgets.wViewerSurface;
    cf->Prs.Host   = m_widgets.wViewerSurface->PrsMgr();
  }

  // Now we have everything to initialize an imperative plotter
  cf->Plotter = new asiUI_JournalIV(cf->Model, cf->Prs.Part, cf->Prs.Domain, cf->ObjectBrowser);

  // Feature controls
  QDockWidget* pDockFeature;
  {
    pDockFeature = new QDockWidget("Features", this);
    pDockFeature->setAllowedAreas(Qt::LeftDockWidgetArea);
    //
    m_widgets.wControlsFeature = new asiUI_ControlsFeature(cf->Model,
                                                           cf->ViewerPart,
                                                           cf->ProgressNotifier,
                                                           cf->Plotter,
                                                           pDockFeature);
    //
    pDockFeature->setWidget(m_widgets.wControlsFeature);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockFeature);
  }

  // Tabify widgets
  this->tabifyDockWidget(pDockBrowser, pDockFeature);

  // Part controls
  QDockWidget* pDockPart;
  {
    pDockPart = new QDockWidget("Part", this);
    pDockPart->setAllowedAreas(Qt::LeftDockWidgetArea);
    //
    m_widgets.wControlsPart = new asiUI_ControlsPart(cf->Model,
                                                     cf->ViewerPart,
                                                     cf->ProgressNotifier,
                                                     cf->Plotter,
                                                     pDockPart);
    //
    pDockPart->setWidget(m_widgets.wControlsPart);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockPart);
  }

  // Tabify widgets
  this->tabifyDockWidget(pDockFeature, pDockPart);

  // Listener for part controls
  m_listeners.pControlsPart = new asiUI_ControlsPartListener(m_widgets.wControlsPart,
                                                             m_widgets.wViewerPart,
                                                             m_widgets.wViewerDomain,
                                                             m_widgets.wViewerSurface,
                                                             cf->Model,
                                                             cf->ProgressNotifier);

  // Listener for part viewer
  m_listeners.pViewerPart = new asiUI_ViewerPartListener(m_widgets.wViewerPart,
                                                         m_widgets.wViewerDomain,
                                                         m_widgets.wViewerSurface,
                                                         cf->Model,
                                                         cf->ProgressNotifier,
                                                         cf->Plotter);

  // Signals-slots
  m_listeners.pControlsPart->Connect();
  m_listeners.pViewerPart->Connect();

  // Log window
  QDockWidget* pDockLogWindow;
  {
    pDockLogWindow = new QDockWidget("Logger", this);
    pDockLogWindow->setAllowedAreas(Qt::BottomDockWidgetArea);
    //
    m_widgets.wLogger = new QTextEdit(pDockLogWindow);
    //
    pDockLogWindow->setWidget(m_widgets.wLogger);
    //
    this->addDockWidget(Qt::BottomDockWidgetArea, pDockLogWindow);
  }

  // Create status bar
  Handle(asiUI_StatusBarImpl)
    statusBar = new asiUI_StatusBarImpl(new asiUI_StatusBar);
  //
  this->setStatusBar( statusBar->GetStatusBar() );
  //
  cf->StatusBar = statusBar;
  cf->StatusBar->SetStatusText("Load part from STEP or BREP to start analysis");

  // Initialize and connect progress listener
  cf->Logger           = new asiUI_Logger(m_widgets.wLogger);
  cf->ProgressListener = new asiUI_ProgressListener(statusBar, cf->ProgressNotifier, cf->Logger);
  cf->ProgressListener->Connect();
}

//-----------------------------------------------------------------------------

//! Creates menus.
void exe_MainWindow::createMenus()
{
}

//-----------------------------------------------------------------------------

//! Creates toolbar.
void exe_MainWindow::createToolbar()
{
}
