//-----------------------------------------------------------------------------
// Created on: 31 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <exeFeatures_MainWindow.h>

// exeFeatures includes
#include <exeFeatures_CommonFacilities.h>

// Qt includes
#pragma warning(push, 0)
#include <QDesktopWidget>
#include <QDockWidget>
#pragma warning(pop)

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
exeFeatures_MainWindow::exeFeatures_MainWindow() : QMainWindow()
{
  this->createPartViewer();
  this->createDockWindows();

  this->setCentralWidget(m_widgets.wViewerPart);
  this->setWindowTitle("Features");
}

//! Destructor.
exeFeatures_MainWindow::~exeFeatures_MainWindow()
{}

//-----------------------------------------------------------------------------

//! Gets control on window close.
//! \param evt [in] event.
void exeFeatures_MainWindow::closeEvent(QCloseEvent* evt)
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
void exeFeatures_MainWindow::createPartViewer()
{
  // Common facilities instance
  Handle(exeFeatures_CommonFacilities) cf = exeFeatures_CommonFacilities::Instance();

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

//! Creates main dockable widgets.
void exeFeatures_MainWindow::createDockWindows()
{
  // Common facilities instance
  Handle(exeFeatures_CommonFacilities) cf = exeFeatures_CommonFacilities::Instance();

  // Desktop used for sizing
  QDesktopWidget desktop;
  const int side  = std::min( desktop.height(), desktop.width() );
  const int width = side*0.4;

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
  //---------------------------------------------------------------------------
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
  //---------------------------------------------------------------------------
  // Part controls
  QDockWidget* pDockCommon;
  {
    pDockCommon = new QDockWidget("Common Tools", this);
    pDockCommon->setAllowedAreas(Qt::LeftDockWidgetArea);
    //
    m_widgets.wControlsPart = new asiUI_ControlsPart(cf->Model,
                                                     cf->ViewerPart,
                                                     cf->Notifier,
                                                     cf->Plotter,
                                                     pDockCommon);
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
    m_widgets.wControlsFeatures = new exeFeatures_Controls(pDockFeatures);
    pDockFeatures->setWidget(m_widgets.wControlsFeatures);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockFeatures);
  }
  //---------------------------------------------------------------------------
  // Euler controls
  QDockWidget* pDockEuler;
  {
    pDockEuler = new QDockWidget("Euler", this);
    pDockEuler->setAllowedAreas(Qt::LeftDockWidgetArea);
    //
    m_widgets.wControlsEuler = new exeFeatures_ControlsEuler(pDockEuler);
    pDockEuler->setWidget(m_widgets.wControlsEuler);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockEuler);
  }
  //---------------------------------------------------------------------------
  this->tabifyDockWidget(pDockBrowser,  pDockFeatures);
  this->tabifyDockWidget(pDockFeatures, pDockEuler);
  this->tabifyDockWidget(pDockEuler,    pDockCommon);

  // Now we have everything to initialize an imperative plotter
  cf->Plotter = new asiUI_JournalIV(cf->Model, cf->Prs.Part, cf->Prs.Domain, cf->ObjectBrowser);

  // Listener for part controls
  m_listeners.pControlsPart = new asiUI_ControlsPartListener(m_widgets.wControlsPart,
                                                             m_widgets.wViewerPart,
                                                             m_widgets.wViewerDomain,
                                                             m_widgets.wViewerSurface,
                                                             cf->Model);

  // Listener for part viewer
  m_listeners.pViewerPart = new asiUI_ViewerPartListener(m_widgets.wViewerPart,
                                                         m_widgets.wViewerDomain,
                                                         m_widgets.wViewerSurface,
                                                         cf->Model);

  // Signals-slots
  m_listeners.pControlsPart->Connect();
  m_listeners.pViewerPart->Connect();
}
