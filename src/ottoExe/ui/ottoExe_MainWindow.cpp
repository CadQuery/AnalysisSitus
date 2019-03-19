//-----------------------------------------------------------------------------
// Created on: 16 February 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <ottoExe_MainWindow.h>

// ottoExe includes
#include <ottoExe_CommonFacilities.h>

// asiTcl includes
#include <asiTcl_Plugin.h>

// asiUI includes
#include <asiUI_Common.h>
#include <asiUI_ParamEditorImpl.h>
#include <asiUI_StatusBar.h>
#include <asiUI_StatusBarImpl.h>
#include <asiUI_StyledTextEdit.h>

// Qt includes
#pragma warning(push, 0)
#include <QApplication>
#include <QTextStream>
#include <QDesktopWidget>
#include <QDockWidget>
#pragma warning(pop)

#define ottoExe_LOAD_MODULE(name) \
{ \
  Handle(ottoExe_CommonFacilities) cf = ottoExe_CommonFacilities::Instance();\
  \
  if ( !asiTcl_Plugin::Load(cf->Interp, cf, name) ) \
    cf->Progress.SendLogMessage(LogErr(Normal) << "Cannot load %1 commands." << name); \
  else \
    cf->Progress.SendLogMessage(LogInfo(Normal) << "Loaded %1 commands." << name); \
}

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
ottoExe_MainWindow::ottoExe_MainWindow() : QMainWindow()
{
  this->createPartViewer();
  this->createDockWindows();

  this->setCentralWidget(m_widgets.wViewerPart);
  this->setWindowTitle("Ottobock Workbench");

  //---------------------------------------------------------------------------
  // Apply fantastic dark theme
  //---------------------------------------------------------------------------

  QFile f(":qdarkstyle/style.qss");
  if ( !f.exists() )
  {
    printf("Unable to set stylesheet, file not found\n");
  }
  else
  {
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&f);
    qApp->setStyleSheet( ts.readAll() );
  }

  //---------------------------------------------------------------------------
  // Set focus on Tcl console
  //---------------------------------------------------------------------------

  m_widgets.wConsole->setFocus();
}

//! Destructor.
ottoExe_MainWindow::~ottoExe_MainWindow()
{}

//-----------------------------------------------------------------------------

//! Gets control on window close.
//! \param evt [in] event.
void ottoExe_MainWindow::closeEvent(QCloseEvent* evt)
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
void ottoExe_MainWindow::createPartViewer()
{
  // Common facilities instance
  Handle(ottoExe_CommonFacilities) cf = ottoExe_CommonFacilities::Instance();

  // Create viewer
  m_widgets.wViewerPart = new asiUI_ViewerPart(cf->Model, true);

  // Desktop used for sizing
  QDesktopWidget desktop;
  const int side   = std::min( desktop.height(), desktop.width() );
  const int width  = side*0.05;
  const int height = side*0.05;
  //
  m_widgets.wViewerPart->setMinimumSize(width, height);

  // Initialize desktop
  cf->ViewerPart = m_widgets.wViewerPart;
  cf->Prs.Part   = m_widgets.wViewerPart->PrsMgr();

  // Bind presentation manager to a limb node
  cf->Prs.Part->SetPresentation( Handle(ottoEngine_Model)::DownCast(cf->Model)->GetLimbNode() );
}

//! Creates main dockable widgets.
void ottoExe_MainWindow::createDockWindows()
{
  // Common facilities instance.
  Handle(ottoExe_CommonFacilities) cf = ottoExe_CommonFacilities::Instance();

  // Desktop used for sizing.
  QDesktopWidget desktop;
  const int side  = std::min( desktop.height(), desktop.width() );
  const int width = side*0.02;

  //---------------------------------------------------------------------------
  // Object browser.
  QDockWidget* pDockBrowser;
  {
    pDockBrowser = new QDockWidget("Data", this);
    pDockBrowser->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wBrowser = new asiUI_ObjectBrowser(cf->Model, cf->Progress, pDockBrowser);
    m_widgets.wBrowser->AddAssociatedViewer(cf->ViewerPart);
    m_widgets.wBrowser->AddAssociatedViewer(cf->ViewerHost);
    m_widgets.wBrowser->AddAssociatedViewer(cf->ViewerDomain);
    //
    pDockBrowser->setWidget(m_widgets.wBrowser);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockBrowser);

    // Initialize desktop.
    cf->ObjectBrowser = m_widgets.wBrowser;
  }
  //---------------------------------------------------------------------------
  // Part controls.
  QDockWidget* pDockCommon;
  {
    pDockCommon = new QDockWidget("Common", this);
    pDockCommon->setAllowedAreas(Qt::LeftDockWidgetArea);
    //
    m_widgets.wControlsPart = new asiUI_ControlsPart(cf->Model,
                                                     cf->ViewerPart,
                                                     cf->Progress,
                                                     cf->Plotter,
                                                     pDockCommon);
    pDockCommon->setWidget(m_widgets.wControlsPart);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockCommon);
  }

  // Mesh controls.
  QDockWidget* pDockMesh;
  {
    pDockMesh = new QDockWidget("Mesh", this);
    pDockMesh->setAllowedAreas(Qt::AllDockWidgetAreas);
    //
    m_widgets.wControlsMesh = new asiUI_ControlsMesh(cf->Model,
                                                     cf->ViewerPart,
                                                     cf->Progress,
                                                     cf->Plotter,
                                                     pDockMesh);
    //
    pDockMesh->setWidget(m_widgets.wControlsMesh);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockMesh);
  }

  //---------------------------------------------------------------------------
  // Ottobock custom controls.
  QDockWidget* pDockOtto;
  {
    pDockOtto = new QDockWidget("Ottobock", this);
    pDockOtto->setAllowedAreas(Qt::LeftDockWidgetArea);
    //
    m_widgets.wControls = new ottoExe_Controls(pDockOtto);
    pDockOtto->setWidget(m_widgets.wControls);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockOtto);
  }
  //---------------------------------------------------------------------------
  this->tabifyDockWidget(pDockBrowser, pDockCommon);
  this->tabifyDockWidget(pDockCommon,  pDockMesh);
  this->tabifyDockWidget(pDockMesh,    pDockOtto);
  //
  pDockOtto->raise();

  // Now we have everything to initialize an imperative plotter.
  cf->Plotter = new asiUI_IV(cf->Model, cf->Prs.Part, cf->Prs.Domain, cf->ObjectBrowser);

  // Parameter editor.
  QDockWidget* pDockParamEditor;
  {
    pDockParamEditor = new QDockWidget("Parameters", this);
    pDockParamEditor->setAllowedAreas(Qt::AllDockWidgetAreas);
    //
    m_widgets.wParamEditor = new asiUI_ParameterEditor(cf->WidgetFactory,
                                                     pDockParamEditor);
    //
    pDockParamEditor->setWidget(m_widgets.wParamEditor);
    //
    this->addDockWidget(Qt::RightDockWidgetArea, pDockParamEditor);

    // Set to common facilities.
    cf->ParamEditor = new asiUI_ParamEditorImpl(m_widgets.wParamEditor);
    cf->ObjectBrowser->SetParameterEditor(cf->ParamEditor);
  }

  // Listener for part controls.
  m_listeners.pControlsPart = new asiUI_ControlsPartListener(m_widgets.wControlsPart,
                                                             cf);

  // Listener for mesh controls.
  m_listeners.pControlsMesh = new asiUI_ControlsMeshListener(m_widgets.wControlsMesh,
                                                             cf);

  // Listener for part viewer.
  m_listeners.pViewerPart = new asiUI_ViewerPartListener(m_widgets.wViewerPart,
                                                         NULL,
                                                         NULL,
                                                         cf->Model,
                                                         cf->Progress,
                                                         cf->Plotter);

  // Listener for parameter editor.
  m_listeners.pParamEditor = new asiUI_ParameterEditorListener(cf);

  // Signals-slots.
  m_listeners.pControlsPart ->Connect();
  m_listeners.pControlsMesh ->Connect();
  m_listeners.pViewerPart   ->Connect();
  m_listeners.pParamEditor  ->Connect();

  // Log window.
  QDockWidget* pDockLogWindow;
  {
    pDockLogWindow = new QDockWidget("Logger", this);
    pDockLogWindow->setAllowedAreas(Qt::AllDockWidgetAreas);
    //
    m_widgets.wLogger = new asiUI_StyledTextEdit(pDockLogWindow);
    //
    pDockLogWindow->setWidget(m_widgets.wLogger);
    //
    this->addDockWidget(Qt::BottomDockWidgetArea, pDockLogWindow);
  }

  // Create status bar.
  Handle(asiUI_StatusBarImpl)
    statusBar = new asiUI_StatusBarImpl(new asiUI_StatusBar);
  //
  this->setStatusBar( statusBar->GetStatusBar() );
  //
  cf->StatusBar = statusBar;
  cf->StatusBar->SetStatusText("Copyright (c) OPEN CASCADE 2019-present");

  // Initialize and connect progress listener
  cf->Logger           = new asiUI_Logger(m_widgets.wLogger);
  cf->ProgressListener = new asiUI_ProgressListener(statusBar, cf->Progress.Access(), cf->Logger);
  cf->ProgressListener->Connect();

  /* ==================================
   *  Tcl console with custom commands
   * ================================== */

  // Construct the interpreter.
  cf->Interp = new asiTcl_Interp;
  cf->Interp->Init();
  cf->Interp->SetModel(cf->Model);
  cf->Interp->SetPlotter(cf->Plotter);
  cf->Interp->SetProgress(cf->Progress);

  // Load commands.
  ottoExe_LOAD_MODULE("cmdMisc")
  ottoExe_LOAD_MODULE("cmdEngine")
  ottoExe_LOAD_MODULE("cmdRE")
  ottoExe_LOAD_MODULE("ottoCmd")

  // Console window.
  QDockWidget* pDockConsoleWindow;
  {
    pDockConsoleWindow = new QDockWidget("Console", this);
    pDockConsoleWindow->setAllowedAreas(Qt::BottomDockWidgetArea);
    //
    m_widgets.wConsole = new asiUI_Console(cf->Interp, pDockLogWindow);
    //
    pDockConsoleWindow->setWidget(m_widgets.wConsole);
    //
    this->addDockWidget(Qt::BottomDockWidgetArea, pDockConsoleWindow);
  }
}
