//-----------------------------------------------------------------------------
// Created on: 07 December 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2015-present, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <exe_MainWindow.h>

// exe includes
#include <exe_CommonFacilities.h>
#include <exe_Version.h>

// asiUI includes
#include <asiUI_Common.h>
#include <asiUI_ParamEditorImpl.h>
#include <asiUI_ParameterEditorListenerDefault.h>
#include <asiUI_StatusBar.h>
#include <asiUI_StatusBarImpl.h>

// asiTcl includes
#include <asiTcl_Plugin.h>

// Qt includes
#pragma warning(push, 0)
#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QDockWidget>
#include <QStringList>
#include <QTextStream>
#pragma warning(pop)

#define EXE_LOAD_MODULE(name) \
{ \
  Handle(exe_CommonFacilities) cf = exe_CommonFacilities::Instance();\
  \
  if ( !asiTcl_Plugin::Load(cf->Interp, cf, name) ) \
    cf->Progress.SendLogMessage(LogErr(Normal) << "Cannot load %1 commands." << name); \
  else \
    cf->Progress.SendLogMessage(LogInfo(Normal) << "Loaded %1 commands." << name); \
}

//-----------------------------------------------------------------------------

//! Constructor.
//! \param[in] offscreen indicates whether the application is running in
//!                      the offscreen mode.
exe_MainWindow::exe_MainWindow(const bool offscreen)
: QMainWindow(), m_bOffscreen(offscreen)
{
  this->createPartViewer();
  this->createDockWindows();
  this->setCentralWidget(Widgets.wViewerPart);

  // Prepare application name with the version number.
  TCollection_AsciiString appName(ASITUS_APP_NAME);
  appName += " ["; appName += ASITUS_VERSION_STRING; appName += "]";
  //
  this->setWindowTitle( appName.ToCString() );

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

  // Set this main window to common facilities.
  Handle(exe_CommonFacilities) cf = exe_CommonFacilities::Instance();
  //
  cf->MainWindow = this;
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
  // saying that it lacks some resources.
  Widgets.Release();
  //
  evt->accept();
}

//-----------------------------------------------------------------------------

//! Creates main (part) viewer.
void exe_MainWindow::createPartViewer()
{
  // Common facilities instance.
  Handle(exe_CommonFacilities) cf = exe_CommonFacilities::Instance();

  // Create viewer.
  Widgets.wViewerPart = new asiUI_ViewerPart(cf->Model, true);

  // Initialize desktop.
  cf->ViewerPart = Widgets.wViewerPart;
  cf->Prs.Part   = Widgets.wViewerPart->PrsMgr();
}

//-----------------------------------------------------------------------------

//! Creates main dockable widgets.
void exe_MainWindow::createDockWindows()
{
  // Common facilities instance.
  Handle(exe_CommonFacilities) cf = exe_CommonFacilities::Instance();

  // Domain viewer.
  QDockWidget* pDockDomain;
  {
    pDockDomain = new QDockWidget("Domain", this);
    pDockDomain->setAllowedAreas(Qt::AllDockWidgetAreas);
    //
    Widgets.wViewerDomain = new asiUI_ViewerDomain(cf->Model, pDockDomain);
    pDockDomain->setWidget(Widgets.wViewerDomain);
    //
    this->addDockWidget(Qt::RightDockWidgetArea, pDockDomain);

    // Initialize desktop.
    cf->ViewerDomain = Widgets.wViewerDomain;
    cf->Prs.Domain   = Widgets.wViewerDomain->PrsMgr();
  }

  // Host viewer.
  QDockWidget* pDockHost;
  {
    pDockHost = new QDockWidget("Host", this);
    pDockHost->setAllowedAreas(Qt::AllDockWidgetAreas);
    //
    Widgets.wViewerSurface = new asiUI_ViewerHost(cf->Model, pDockHost);
    pDockHost->setWidget(Widgets.wViewerSurface);
    //
    this->addDockWidget(Qt::RightDockWidgetArea, pDockHost);

    // Initialize desktop.
    cf->ViewerHost = Widgets.wViewerSurface;
    cf->Prs.Host   = Widgets.wViewerSurface->PrsMgr();
  }

  // Object browser.
  QDockWidget* pDockBrowser;
  {
    pDockBrowser = new QDockWidget("Data", this);
    pDockBrowser->setAllowedAreas(Qt::AllDockWidgetAreas);
    //
    Widgets.wBrowser = new asiUI_ObjectBrowser(cf->Model, cf->Progress, pDockBrowser);
    Widgets.wBrowser->AddAssociatedViewer(cf->ViewerPart);
    Widgets.wBrowser->AddAssociatedViewer(cf->ViewerDomain);
    Widgets.wBrowser->AddAssociatedViewer(cf->ViewerHost);
    //
    pDockBrowser->setWidget(Widgets.wBrowser);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockBrowser);

    // Initialize desktop.
    cf->ObjectBrowser = Widgets.wBrowser;
  }

  // Now we have everything to initialize an imperative plotter.
  cf->Plotter = ActAPI_PlotterEntry( new asiUI_IV(cf->Model,
                                                  cf->Prs.Part,
                                                  cf->Prs.Domain,
                                                  cf->ObjectBrowser) );

  // Set diagnostic tools once we've got plotter.
  cf->Prs.Part->SetDiagnosticTools(cf->Progress, cf->Plotter);

  // Analysis controls.
  QDockWidget* pDockAnalysis;
  {
    pDockAnalysis = new QDockWidget("Analysis", this);
    pDockAnalysis->setAllowedAreas(Qt::AllDockWidgetAreas);
    //
    Widgets.wControlsAnalysis = new asiUI_ControlsAnalysis(cf->Model,
                                                           cf->ViewerPart,
                                                           cf->ObjectBrowser,
                                                           cf->Progress,
                                                           cf->Plotter,
                                                           pDockAnalysis);
    //
    pDockAnalysis->setWidget(Widgets.wControlsAnalysis);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockAnalysis);
  }
  //
  this->tabifyDockWidget(pDockBrowser, pDockAnalysis);

  // Modeling controls.
  QDockWidget* pDockModeling;
  {
    pDockModeling = new QDockWidget("Modeling", this);
    pDockModeling->setAllowedAreas(Qt::AllDockWidgetAreas);
    //
    Widgets.wControlsModeling = new asiUI_ControlsModeling(cf->Model,
                                                           cf->ViewerPart,
                                                           cf->Progress,
                                                           cf->Plotter,
                                                           pDockModeling);
    //
    pDockModeling->setWidget(Widgets.wControlsModeling);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockModeling);
  }
  //
  this->tabifyDockWidget(pDockAnalysis, pDockModeling);

  // Mesh controls.
  QDockWidget* pDockMesh;
  {
    pDockMesh = new QDockWidget("Mesh", this);
    pDockMesh->setAllowedAreas(Qt::AllDockWidgetAreas);
    //
    Widgets.wControlsMesh = new asiUI_ControlsMesh(cf->Model,
                                                   cf->ViewerPart,
                                                   cf->Progress,
                                                   cf->Plotter,
                                                   pDockMesh);
    //
    pDockMesh->setWidget(Widgets.wControlsMesh);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockMesh);
  }
  //
  this->tabifyDockWidget(pDockModeling, pDockMesh);

  // Part controls.
  QDockWidget* pDockPart;
  {
    pDockPart = new QDockWidget("Part", this);
    pDockPart->setAllowedAreas(Qt::AllDockWidgetAreas);
    //
    Widgets.wControlsPart = new asiUI_ControlsPart(cf->Model,
                                                   cf->ViewerPart,
                                                   cf->Progress,
                                                   cf->Plotter,
                                                   pDockPart);
    //
    pDockPart->setWidget(Widgets.wControlsPart);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockPart);
  }
  //
  this->tabifyDockWidget(pDockMesh, pDockPart);

  // Parameter editor.
  QDockWidget* pDockParamEditor;
  {
    pDockParamEditor = new QDockWidget("Parameters", this);
    pDockParamEditor->setAllowedAreas(Qt::AllDockWidgetAreas);
    //
    Widgets.wParamEditor = new asiUI_ParameterEditor(cf->WidgetFactory,
                                                     pDockParamEditor);
    //
    pDockParamEditor->setWidget(Widgets.wParamEditor);
    //
    this->addDockWidget(Qt::RightDockWidgetArea, pDockParamEditor);

    // Set to common facilities.
    cf->ParamEditor = new asiUI_ParamEditorImpl(Widgets.wParamEditor);
    cf->ObjectBrowser->SetParameterEditor(cf->ParamEditor);
  }
  //
  this->tabifyDockWidget(pDockDomain, pDockParamEditor);

  // Listener for object browser.
  Listeners.pObjectBrowser = new asiUI_ObjectBrowserListener(cf);

  // Listener for analysis controls.
  Listeners.pControlsAnalysis = new asiUI_ControlsAnalysisListener(Widgets.wControlsAnalysis,
                                                                   cf);

  // Listener for modeling controls.
  Listeners.pControlsModeling = new asiUI_ControlsModelingListener(Widgets.wControlsModeling,
                                                                   cf);

  // Listener for part controls.
  Listeners.pControlsPart = new asiUI_ControlsPartListener(Widgets.wControlsPart,
                                                           cf);

  // Listener for mesh controls.
  Listeners.pControlsMesh = new asiUI_ControlsMeshListener(Widgets.wControlsMesh,
                                                           cf);

  // Listener for part viewer.
  Listeners.pViewerPart = new asiUI_ViewerPartListener(Widgets.wViewerPart,
                                                       Widgets.wViewerDomain,
                                                       Widgets.wViewerSurface,
                                                       cf->Model,
                                                       cf->Progress,
                                                       cf->Plotter);

  // Listener for domain viewer.
  Listeners.pViewerDomain = new asiUI_ViewerDomainListener(Widgets.wViewerPart,
                                                           Widgets.wViewerDomain,
                                                           Widgets.wViewerSurface,
                                                           cf->Model,
                                                           cf->Progress,
                                                           cf->Plotter);

  // Listener for host viewer.
  Listeners.pViewerHost = new asiUI_ViewerHostListener(Widgets.wViewerPart,
                                                       Widgets.wViewerDomain,
                                                       Widgets.wViewerSurface,
                                                       cf->Model,
                                                       cf->Progress,
                                                       cf->Plotter);

  // Listener for parameter editor.
  Listeners.pParamEditor = new asiUI_ParameterEditorListenerDefault(cf);

  // Signals-slots.
  Listeners.pObjectBrowser    ->Connect();
  Listeners.pControlsAnalysis ->Connect();
  Listeners.pControlsPart     ->Connect();
  Listeners.pControlsModeling ->Connect();
  Listeners.pControlsMesh     ->Connect();
  Listeners.pViewerPart       ->Connect();
  Listeners.pViewerDomain     ->Connect();
  Listeners.pViewerHost       ->Connect();
  Listeners.pParamEditor      ->Connect();

  // Log window.
  QDockWidget* pDockLogWindow;
  {
    pDockLogWindow = new QDockWidget("Logger", this);
    pDockLogWindow->setAllowedAreas(Qt::AllDockWidgetAreas);
    //
    Widgets.wLogger = new asiUI_StyledTextEdit(pDockLogWindow);
    //
    pDockLogWindow->setWidget(Widgets.wLogger);
    //
    this->addDockWidget(Qt::RightDockWidgetArea, pDockLogWindow);
  }

  // Create status bar.
  Handle(asiUI_StatusBarImpl)
    statusBar = new asiUI_StatusBarImpl(new asiUI_StatusBar);
  //
  this->setStatusBar( statusBar->GetStatusBar() );
  //
  cf->StatusBar = statusBar;
  cf->StatusBar->SetStatusText("Debug your B-Rep...");

  // Initialize and connect progress listener.
  cf->Logger           = new asiUI_Logger(Widgets.wLogger);
  cf->ProgressListener = new asiUI_ProgressListener(statusBar,
                                                    cf->Progress.Access(),
                                                    cf->Logger);
  cf->ProgressListener->Connect();

  /* ==================================
   *  Tcl console with custom commands
   * ================================== */

  // Construct the interpreter.
  cf->Interp = new asiTcl_Interp(cf->Progress, cf->Plotter);
  cf->Interp->Init(!m_bOffscreen); // Do not override channels in offscreen mode.
  cf->Interp->SetModel(cf->Model);

  // Load default commands.
  EXE_LOAD_MODULE("cmdMisc")
  EXE_LOAD_MODULE("cmdEngine")
  EXE_LOAD_MODULE("cmdRE")

  // Lookup for custom plugins and try to load them.
  QDir pluginDir( QDir::currentPath() + "/asi-plugins" );
  //
  std::cout << "Looking for plugins at "
            << QStr2AsciiStr( pluginDir.absolutePath() ).ToCString() << std::endl;
  //
  QStringList cmdLibs = pluginDir.entryList(QStringList() << "*.dll", QDir::Files);
  //
  foreach ( QString cmdLib, cmdLibs )
  {
    TCollection_AsciiString cmdLibName = QStr2AsciiStr( cmdLib.section(".", 0, 0) );
    //
    cf->Progress.SendLogMessage(LogInfo(Normal) << "Detected %1 as a custom plugin. Attempting to load it..."
                                                << cmdLibName);

    EXE_LOAD_MODULE(cmdLibName);
  }

  // Console window.
  QDockWidget* pDockConsoleWindow;
  {
    pDockConsoleWindow = new QDockWidget("Active Script", this);
    pDockConsoleWindow->setAllowedAreas(Qt::BottomDockWidgetArea);
    //
    Widgets.wConsole = new asiUI_Console(cf->Interp, pDockLogWindow);
    //
    pDockConsoleWindow->setWidget(Widgets.wConsole);
    //
    this->addDockWidget(Qt::BottomDockWidgetArea, pDockConsoleWindow);
  }
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
