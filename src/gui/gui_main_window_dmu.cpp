//-----------------------------------------------------------------------------
// Created on: 01 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_main_window_dmu.h>

// Common includes
#include <common_facilities.h>

// Qt includes
#pragma warning(push, 0)
#include <QDesktopWidget>
#include <QDockWidget>
#pragma warning(pop)

// OCCT include
#include <AIS_Shape.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Prs3d_LineAspect.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_DirectionalLight.hxx>

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
gui_main_window_dmu::gui_main_window_dmu() : QMainWindow()
{
  this->createViewer();
  this->createDockWindows();
  //
  this->setCentralWidget(m_widgets.wViewer);
  this->setWindowTitle("Analysis Situs [CAD Simplification]");
}

//! Destructor.
gui_main_window_dmu::~gui_main_window_dmu()
{
  this->release();
}

//-----------------------------------------------------------------------------

//! Gets control on window close.
//! \param evt [in] event.
void gui_main_window_dmu::closeEvent(QCloseEvent* evt)
{
  // It seems that we have to destruct objects properly and manually in
  // order to avoid some side effects from VTK. E.g. if we don't kill the
  // widgets explicitly here, we may sometimes get a warning window of VTK
  // saying that it lacks some resources
  m_widgets.Release();
  //
  evt->accept();

  // Nullify resource in common facilities
  common_facilities::Instance()->ViewerDMU = NULL;
}

//-----------------------------------------------------------------------------

//! Creates main viewer.
void gui_main_window_dmu::createViewer()
{
  static Handle(OpenGl_GraphicDriver) aGraphicDriver;
  //
  if ( aGraphicDriver.IsNull() )
  {
    Handle(Aspect_DisplayConnection) aDisplayConnection;
#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
    aDisplayConnection = new Aspect_DisplayConnection( qgetenv("DISPLAY").constData() );
#endif
    aGraphicDriver = new OpenGl_GraphicDriver(aDisplayConnection);
  }

  // Create OpenCascade viewer
  TCollection_ExtendedString a3DName("Visu3D");
  m_viewer = new V3d_Viewer(aGraphicDriver,
                            a3DName.ToExtString(),
                            "",
                            1000.0,
                            V3d_XposYnegZpos,
                            Quantity_NOC_GRAY30,
                            V3d_ZBUFFER,
                            V3d_GOURAUD,
                            V3d_WAIT,
                            1,
                            1,
                            V3d_TEX_NONE);
  //
  Handle(V3d_DirectionalLight) LightDir   = new V3d_DirectionalLight (m_viewer, V3d_Zneg, Quantity_NOC_SNOW, 1);
  Handle(V3d_DirectionalLight) LightDirPT = new V3d_DirectionalLight (m_viewer, V3d_Zneg, Quantity_NOC_SNOW, 1);
  Handle(V3d_AmbientLight)     LightAmb   = new V3d_AmbientLight     (m_viewer);
  //
  LightDir  ->SetDirection ( 1.0, -2.0, -10.0);
  LightDirPT->SetDirection ( 1.0, -2.0, -10.0);
  LightDirPT->SetPosition  (-1.0,  1.0,   0.5);
  //
  LightDirPT->SetSmoothAngle(0.1);
  LightDirPT->SetIntensity(100.0);
  //
  m_viewer->SetLightOn(LightDir);
  m_viewer->SetLightOn(LightAmb);
  //
  m_context = new AIS_InteractiveContext(m_viewer);
  m_context->SelectionColor(Quantity_NOC_YELLOW);

  // Create OpenCascade viewer
  m_widgets.wViewer = new gui_viewer_dmu(m_context, this);
  //
  m_widgets.wViewer->GetView()->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_WHITE, 0.08, V3d_ZBUFFER);
  //
  Quantity_Color bg_Color1(0.15, 0.25, 0.40, Quantity_TOC_RGB);
  Quantity_Color bg_Color2(0.00, 0.00, 0.00, Quantity_TOC_RGB);
  //
  m_widgets.wViewer->GetView()->SetBgGradientColors(bg_Color1, bg_Color2, Aspect_GFM_CORNER1, 0);

  // Configure rendering parameters
  Graphic3d_RenderingParams& RenderParams = m_widgets.wViewer->GetView()->ChangeRenderingParams();
  RenderParams.NbMsaaSamples = 8; // Anti-aliasing by multi-sampling

  // Desktop used for sizing
  QDesktopWidget desktop;
  const int side   = std::min( desktop.height(), desktop.width() );
  const int width  = side*0.4;
  const int height = side*0.4;
  //
  m_widgets.wViewer->setMinimumSize(width, height);

  // Store in common facilities
  common_facilities::Instance()->ViewerDMU = m_widgets.wViewer;
}

//! Creates main dockable widgets.
void gui_main_window_dmu::createDockWindows()
{
  //---------------------------------------------------------------------------
  // Object browser
  {
    QDockWidget* pDock = new QDockWidget("XDE model browser", this);
    pDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wBrowser = new gui_object_browser_xde(pDock);
    pDock->setWidget(m_widgets.wBrowser);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDock);
  }
  //---------------------------------------------------------------------------
  // Tools
  {
    QDockWidget* pDock = new QDockWidget("Tools", this);
    pDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wTools = new gui_controls_dmu(pDock);
    pDock->setWidget(m_widgets.wTools);
    //
    this->addDockWidget(Qt::RightDockWidgetArea, pDock);
  }
  //---------------------------------------------------------------------------
  // Visualization properties
  {
    QDockWidget* pDock = new QDockWidget("Render", this);
    pDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wVisuTools = new gui_controls_dmu_visu(pDock);
    pDock->setWidget(m_widgets.wVisuTools);
    //
    this->addDockWidget(Qt::RightDockWidgetArea, pDock);
  }
}

//! Releases all resources.
void gui_main_window_dmu::release()
{
  m_viewer.Nullify();
  m_view.Nullify();
  m_context.Nullify();
}
