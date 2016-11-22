//-----------------------------------------------------------------------------
// Created on: 21 June 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <exeAsBuilt_MainWindow.h>

// exeAsBuilt includes
#include <exeAsBuilt_CommonFacilities.h>

// Qt includes
#pragma warning(push, 0)
#include <QDesktopWidget>
#include <QDockWidget>
#pragma warning(pop)

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
exeAsBuilt_MainWindow::exeAsBuilt_MainWindow() : QMainWindow()
{
  this->createPartViewer();
  this->createDockWindows();

  this->setCentralWidget(m_widgets.wViewer);
  this->setWindowTitle("As-Built Reconstruction");
}

//! Destructor.
exeAsBuilt_MainWindow::~exeAsBuilt_MainWindow()
{}

//-----------------------------------------------------------------------------

//! Gets control on window close.
//! \param evt [in] event.
void exeAsBuilt_MainWindow::closeEvent(QCloseEvent* evt)
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
void exeAsBuilt_MainWindow::createPartViewer()
{
  // Common facilities instance
  Handle(exeAsBuilt_CommonFacilities) cf = exeAsBuilt_CommonFacilities::Instance();

  // Create viewer
  m_widgets.wViewer = new asiUI_ViewerPart(cf->Model);

  // Desktop used for sizing
  QDesktopWidget desktop;
  const int side   = std::min( desktop.height(), desktop.width() );
  const int width  = side*0.5;
  const int height = side*0.5;
  //
  m_widgets.wViewer->setMinimumSize(width, height);

  // Initialize desktop
  cf->ViewerPart = m_widgets.wViewer;
  cf->Prs.Part   = m_widgets.wViewer->PrsMgr();
}

//! Creates main dockable widgets.
void exeAsBuilt_MainWindow::createDockWindows()
{
  // Common facilities instance
  Handle(exeAsBuilt_CommonFacilities) cf = exeAsBuilt_CommonFacilities::Instance();

  //---------------------------------------------------------------------------
  // Object browser
  QDockWidget* pDockBrowser;
  {
    pDockBrowser = new QDockWidget("Stored Objects", this);
    pDockBrowser->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wBrowser = new asiUI_ObjectBrowser(cf->Model, pDockBrowser);
    pDockBrowser->setWidget(m_widgets.wBrowser);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockBrowser);

    // Initialize desktop
    cf->ObjectBrowser = m_widgets.wBrowser;
  }
  //---------------------------------------------------------------------------
  // Part controls
  QDockWidget* pDockCommon;
  {
    pDockCommon = new QDockWidget("Common Tools", this);
    pDockCommon->setAllowedAreas(Qt::LeftDockWidgetArea);
    //
    m_widgets.wControlsPart = new asiUI_ControlsPart(cf->Model,
                                                     cf->Prs.Part,
                                                     cf->Notifier,
                                                     cf->Plotter,
                                                     pDockCommon);
    pDockCommon->setWidget(m_widgets.wControlsPart);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockCommon);
  }
  //---------------------------------------------------------------------------
  // Point cloud controls
  QDockWidget* pDockPCloud;
  {
    pDockPCloud = new QDockWidget("Point Clouds", this);
    pDockPCloud->setAllowedAreas(Qt::LeftDockWidgetArea);
    //
    m_widgets.wControls = new exeAsBuilt_ControlsPCloud(pDockCommon);
    pDockPCloud->setWidget(m_widgets.wControls);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockCommon);
  }

  // Tabify widgets
  this->tabifyDockWidget(pDockBrowser, pDockCommon);
  this->tabifyDockWidget(pDockCommon,  pDockPCloud);

  // Now we have everything to initialize an imperative plotter
  cf->Plotter = new asiUI_JournalIV(cf->Model, cf->Prs.Part, cf->Prs.Domain, cf->ObjectBrowser);

  // Listener for part controls
  m_listeners.pControlsPart = new asiUI_ControlsPartListener(m_widgets.wControlsPart,
                                                             m_widgets.wViewer,
                                                             NULL,
                                                             NULL,
                                                             cf->Model);

  // Listener for part viewer
  m_listeners.pViewerPart = new asiUI_ViewerPartListener(m_widgets.wViewer,
                                                         NULL,
                                                         NULL,
                                                         cf->Model);

  // Signals-slots
  m_listeners.pControlsPart->Connect();
  m_listeners.pViewerPart->Connect();
}
