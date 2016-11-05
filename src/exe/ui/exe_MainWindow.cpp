//-----------------------------------------------------------------------------
// Created on: 07 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <exe_MainWindow.h>

// exe includes
#include <exe_CommonFacilities.h>

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
  this->setWindowTitle("Analysis Situs");
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
  m_widgets.wViewerPart = new asiUI_ViewerPart(exe_CommonFacilities::Instance()->Model);

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
    m_widgets.wBrowser = new asiUI_ObjectBrowser(cf->Model, pDockBrowser);
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

  // Part controls
  QDockWidget* pDockUtilities;
  {
    pDockUtilities = new QDockWidget("Utilities", this);
    pDockUtilities->setAllowedAreas(Qt::LeftDockWidgetArea);
    //
    m_widgets.wControlsPart = new asiUI_ControlsPart(cf->Model,
                                                     cf->Prs.Part,
                                                     cf->Notifier,
                                                     cf->Plotter,
                                                     pDockUtilities);
    //
    pDockUtilities->setWidget(m_widgets.wControlsPart);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockUtilities);
  }

  // Tabify widgets
  this->tabifyDockWidget(pDockBrowser, pDockUtilities);

  // Signals-slots
  connect( m_widgets.wControlsPart, SIGNAL ( partLoaded() ),
           this,                    SLOT   ( onPartLoaded() ) );
  //
  connect( m_widgets.wControlsPart, SIGNAL ( partModified() ),
           this,                    SLOT   ( onPartModified() ) );
  //
  connect( m_widgets.wViewerPart, SIGNAL ( facePicked() ),
           this,                  SLOT   ( onFacePicked() ) );
  //
  connect( m_widgets.wViewerPart, SIGNAL ( edgePicked() ),
           this,                  SLOT   ( onEdgePicked() ) );
  //
  connect( m_widgets.wControlsPart, SIGNAL ( verticesOn() ),
           this,                    SLOT   ( onVerticesOn() ) );
  //
  connect( m_widgets.wControlsPart, SIGNAL ( verticesOff() ),
           this,                    SLOT   ( onVerticesOff() ) );
  //
  connect( m_widgets.wControlsPart, SIGNAL ( normalsOn() ),
           this,                    SLOT   ( onNormalsOn() ) );
  //
  connect( m_widgets.wControlsPart, SIGNAL ( normalsOff() ),
           this,                    SLOT   ( onNormalsOff() ) );
  //
  connect( m_widgets.wControlsPart, SIGNAL ( selectionFacesOn() ),
           this,                    SLOT   ( onSelectionFacesOn() ) );
  //
  connect( m_widgets.wControlsPart, SIGNAL ( selectionEdgesOn() ),
           this,                    SLOT   ( onSelectionEdgesOn() ) );
}

//-----------------------------------------------------------------------------

//! Reaction on part loading.
void exe_MainWindow::onPartLoaded()
{
  Handle(exe_CommonFacilities) cf = exe_CommonFacilities::Instance();
  //
  cf->Prs.DeleteAll();
  cf->Prs.Part->Actualize( cf->Model->GetPartNode(), false, true );
  cf->Prs.Part->InitializePickers();
}

//-----------------------------------------------------------------------------

//! Reaction on part modification.
void exe_MainWindow::onPartModified()
{
  Handle(exe_CommonFacilities) cf = exe_CommonFacilities::Instance();
  //
  cf->Prs.DeleteAll();
  cf->Prs.Part->Actualize( cf->Model->GetPartNode(), false, false );
  cf->Prs.Part->InitializePickers();
}

//-----------------------------------------------------------------------------

//! Reaction on face picking.
void exe_MainWindow::onFacePicked()
{
  Handle(exe_CommonFacilities) cf = exe_CommonFacilities::Instance();
  //
  Handle(asiData_PartNode) geom_n = cf->Model->GetPartNode();
  //
  cf->Prs.Domain -> Actualize(geom_n->GetFaceRepresentation().get(), false, true);
  cf->Prs.Host   -> Actualize(geom_n->GetSurfaceRepresentation().get(), false, true);
}

//-----------------------------------------------------------------------------

//! Reaction on edge picking.
void exe_MainWindow::onEdgePicked()
{
  Handle(exe_CommonFacilities) cf = exe_CommonFacilities::Instance();
  //
  Handle(asiData_PartNode) geom_n = cf->Model->GetPartNode();
  //
  cf->Prs.Domain -> Actualize(geom_n->GetEdgeRepresentation().get(), false, true);
  cf->Prs.Host   -> Actualize(geom_n->GetCurveRepresentation().get(), false, true);
}

//-----------------------------------------------------------------------------

//! Reaction on enabling visualization of vertices.
void exe_MainWindow::onVerticesOn()
{
  Handle(exe_CommonFacilities) cf = exe_CommonFacilities::Instance();
  //
  Handle(asiData_PartNode) geom_n = cf->Model->GetPartNode();

  // WHY? ALL? THE FOLLOWING? IS? NECESSARY? WTF?!
  cf->Prs.DeleteAll();
  cf->Prs.Part->InitializePickers();
  cf->Prs.Part->Actualize( geom_n.get() );
}

//-----------------------------------------------------------------------------

//! Reaction on disabling visualization of vertices.
void exe_MainWindow::onVerticesOff()
{
  Handle(exe_CommonFacilities) cf = exe_CommonFacilities::Instance();
  //
  Handle(asiData_PartNode) geom_n = cf->Model->GetPartNode();

  // WHY? ALL? THE FOLLOWING? IS? NECESSARY? WTF?!
  cf->Prs.DeleteAll();
  cf->Prs.Part->InitializePickers();
  cf->Prs.Part->Actualize( geom_n.get() );
}

//-----------------------------------------------------------------------------

//! Reaction on enabling visualization of normals.
void exe_MainWindow::onNormalsOn()
{
  // NYI
}

//-----------------------------------------------------------------------------

//! Reaction on disabling visualization of normals.
void exe_MainWindow::onNormalsOff()
{
  // NYI
}

//-----------------------------------------------------------------------------

//! Reaction on enabling selection of faces.
void exe_MainWindow::onSelectionFacesOn()
{
  Handle(exe_CommonFacilities) cf = exe_CommonFacilities::Instance();
  //
  Handle(asiData_PartNode) geom_n = cf->Model->GetPartNode();

  // WHY? ALL? THE FOLLOWING? IS? NECESSARY? WTF?!
  cf->Prs.DeleteAll();
  cf->Prs.Part->InitializePickers();
  cf->Prs.Part->Actualize( geom_n.get() );
}

//-----------------------------------------------------------------------------

//! Reaction on enabling selection of edges.
void exe_MainWindow::onSelectionEdgesOn()
{
  Handle(exe_CommonFacilities) cf = exe_CommonFacilities::Instance();
  //
  Handle(asiData_PartNode) geom_n = cf->Model->GetPartNode();

  // WHY? ALL? THE FOLLOWING? IS? NECESSARY? WTF?!
  cf->Prs.DeleteAll();
  cf->Prs.Part->InitializePickers();
  cf->Prs.Part->Actualize( geom_n.get() );
}
