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
  this->setWindowTitle("Point Clouds");
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
  const int width  = side*0.25;
  const int height = side*0.25;
  //
  m_widgets.wViewer->setMinimumSize(width, height);

  // Initialize desktop
  cf->ViewerPart = m_widgets.wViewer;
  cf->Prs.Part   = m_widgets.wViewer->PrsMgr();
}

//! Creates main dockable widgets.
void exeAsBuilt_MainWindow::createDockWindows()
{
  // Desktop used for sizing
  /*QDesktopWidget desktop;
  const int side  = std::min( desktop.height(), desktop.width() );
  const int width = side*0.25;*/

  // Common facilities instance
  Handle(exeAsBuilt_CommonFacilities) cf = exeAsBuilt_CommonFacilities::Instance();

  //---------------------------------------------------------------------------
  // Object browser
  QDockWidget* pDockBrowser;
  {
    pDockBrowser = new QDockWidget("Model", this);
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
    pDockCommon = new QDockWidget("Common", this);
    pDockCommon->setAllowedAreas(Qt::LeftDockWidgetArea);
    //
    m_widgets.wControls = new exeAsBuilt_ControlsPCloud(pDockCommon);
    pDockCommon->setWidget(m_widgets.wControls);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockCommon);
  }

  // Tabify widgets
  this->tabifyDockWidget(pDockBrowser, pDockCommon);

  // Now we have everything to initialize an imperative plotter
  cf->Plotter = new asiUI_JournalIV(cf->Model, cf->Prs.Part, cf->Prs.Domain, cf->ObjectBrowser);

  // Listener for part viewer
  m_listeners.pViewerPart = new asiUI_ViewerPartListener(m_widgets.wViewer,
                                                         NULL,
                                                         NULL,
                                                         cf->Model);

  // Signals-slots
  m_listeners.pViewerPart->Connect();
}
