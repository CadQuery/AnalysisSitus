//-----------------------------------------------------------------------------
// Created on: 02 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_main_window_ubend.h>

// Analysis Situs (common) includes
#include <common_facilities.h>

// Analysis Situs (engine) includes
#include <engine_ubend.h>

// VTK includes
#include <vtkProperty.h>

// Qt includes
#pragma warning(push, 0)
#include <QDesktopWidget>
#include <QDockWidget>
#pragma warning(pop)

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
gui_main_window_ubend::gui_main_window_ubend() : QMainWindow()
{
  //---------------------------------------------------------------------------
  // Add U-bend Node
  //---------------------------------------------------------------------------

  // Add as a child for the root
  Handle(ActAPI_INode) root_n = common_facilities::Instance()->Model->GetRootNode();
  //
  common_facilities::Instance()->Model->OpenCommand();
  {
    root_n->AddChildNode( engine_ubend::Create_UBend().get() );
  }
  common_facilities::Instance()->Model->CommitCommand();

  //---------------------------------------------------------------------------
  // Create widgets
  //---------------------------------------------------------------------------

  this->createDockWindows();
  this->createMainViewer();
  //
  this->setCentralWidget(m_widgets.wViewerMain);
  this->setWindowTitle("Analysis Situs [U-bend]");
}

//! Destructor.
gui_main_window_ubend::~gui_main_window_ubend()
{}

//-----------------------------------------------------------------------------

//! Gets control on window close.
//! \param evt [in] event.
void gui_main_window_ubend::closeEvent(QCloseEvent* evt)
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
void gui_main_window_ubend::createMainViewer()
{
  m_widgets.wViewerMain = new gui_viewer_ubend();

  // Desktop used for sizing
  QDesktopWidget desktop;
  const int side   = std::min( desktop.height(), desktop.width() );
  const int width  = side*0.8;
  const int height = side*0.7;
  //
  m_widgets.wViewerMain->setMinimumSize(width, height);
}

//! Creates main dockable widgets.
void gui_main_window_ubend::createDockWindows()
{
  // Desktop used for sizing
  QDesktopWidget desktop;
  const int side   = std::min( desktop.height(), desktop.width() );
  const int width  = side*0.3;
  //const int height = width;

  //---------------------------------------------------------------------------
  // Object browser
  {
    QDockWidget* pDock = new QDockWidget("Model Browser", this);
    pDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wBrowser = new gui_object_browser(pDock);
    pDock->setWidget(m_widgets.wBrowser);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDock);
  }
  //---------------------------------------------------------------------------
  // x(t)
  {
    QDockWidget* pDock = new QDockWidget("x(t) explicit function", this);
    pDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wViewerXt = new gui_viewer_func_univariate(common_facilities::Instance()->Prs.UBend_Xt, pDock);
    pDock->setWidget(m_widgets.wViewerXt);
    pDock->setMinimumWidth(width);
    //
    vtkAxesActor* axes = common_facilities::Instance()->Prs.UBend_Xt->GetTrihedron();
    axes->AxisLabelsOn();
    axes->SetXAxisLabelText("t");
    axes->SetYAxisLabelText("x");
    axes->SetZAxisLabelText("");
    axes->GetXAxisShaftProperty()->SetColor(0.5, 0.5, 0.5);
    axes->GetYAxisShaftProperty()->SetColor(1.0, 0.0, 0.0);
    //
    this->addDockWidget(Qt::RightDockWidgetArea, pDock);
  }
  //---------------------------------------------------------------------------
  // y(t)
  {
    QDockWidget* pDock = new QDockWidget("y(t) explicit function", this);
    pDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wViewerYt = new gui_viewer_func_univariate(common_facilities::Instance()->Prs.UBend_Yt, pDock);
    pDock->setWidget(m_widgets.wViewerYt);
    pDock->setMinimumWidth(width);
    //
    vtkAxesActor* axes = common_facilities::Instance()->Prs.UBend_Yt->GetTrihedron();
    axes->AxisLabelsOn();
    axes->SetXAxisLabelText("t");
    axes->SetYAxisLabelText("y");
    axes->SetZAxisLabelText("");
    axes->GetXAxisShaftProperty()->SetColor(0.5, 0.5, 0.5);
    axes->GetYAxisShaftProperty()->SetColor(0.0, 1.0, 0.0);
    //
    this->addDockWidget(Qt::RightDockWidgetArea, pDock);
  }
  //---------------------------------------------------------------------------
  // x-y
  {
    QDockWidget* pDock = new QDockWidget("x-y parametric curve", this);
    pDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wViewerXY = new gui_viewer_curve_2d(common_facilities::Instance()->Prs.UBend_XY, pDock);
    pDock->setWidget(m_widgets.wViewerXY);
    pDock->setMinimumWidth(width);
    //
    vtkAxesActor* axes = common_facilities::Instance()->Prs.UBend_XY->GetTrihedron();
    axes->AxisLabelsOn();
    axes->SetXAxisLabelText("x");
    axes->SetYAxisLabelText("y");
    axes->SetZAxisLabelText("");
    axes->GetXAxisShaftProperty()->SetColor(1.0, 0.0, 0.0);
    axes->GetYAxisShaftProperty()->SetColor(0.0, 1.0, 0.0);
    //
    this->addDockWidget(Qt::RightDockWidgetArea, pDock);
  }
  //---------------------------------------------------------------------------
  // Section viewer
  //{
  //  QDockWidget* pDock = new QDockWidget("Single Section", this);
  //  pDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  //  //
  //  m_widgets.wViewerSection = new gui_viewer_section(false, pDock);
  //  pDock->setWidget(m_widgets.wViewerSection);
  //  pDock->setMinimumWidth(width);
  //  pDock->setMinimumHeight(height);
  //  //
  //  this->addDockWidget(Qt::LeftDockWidgetArea, pDock);
  //}
}
