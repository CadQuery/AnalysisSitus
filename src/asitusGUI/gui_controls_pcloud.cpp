//-----------------------------------------------------------------------------
// Created on: 21 June 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_controls_pcloud.h>

// Common includes
#include <common_draw_test_suite.h>
#include <common_facilities.h>

// GUI includes
#include <gui_common.h>

// Geometry includes
//#include <geom_point_cloud_normals.h>

// Qt include
#include <QGroupBox>

//-----------------------------------------------------------------------------

#define BTN_MIN_WIDTH 120

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
gui_controls_pcloud::gui_controls_pcloud(QWidget* parent) : QWidget(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_widgets.pLoadPoints   = new QPushButton("Load points");
  m_widgets.pEstimNormals = new QPushButton("Estimate normals");
  //
  m_widgets.pLoadPoints   -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pEstimNormals -> setMinimumWidth(BTN_MIN_WIDTH);

  // Group for data exchange
  QGroupBox*   pDEGroup = new QGroupBox("Data exchange");
  QVBoxLayout* pDELay   = new QVBoxLayout(pDEGroup);
  //
  pDELay->addWidget(m_widgets.pLoadPoints);

  // Group for analysis
  QGroupBox*   pAnalysisGroup = new QGroupBox("Analysis");
  QVBoxLayout* pAnalysisLay   = new QVBoxLayout(pAnalysisGroup);
  //
  pAnalysisLay->addWidget(m_widgets.pEstimNormals);

  // Set layout
  m_pMainLayout->addWidget(pDEGroup);
  m_pMainLayout->addWidget(pAnalysisGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pLoadPoints,   SIGNAL( clicked() ), SLOT( onLoadPoints   () ) );
  connect( m_widgets.pEstimNormals, SIGNAL( clicked() ), SLOT( onEstimNormals () ) );
}

//! Destructor.
gui_controls_pcloud::~gui_controls_pcloud()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! Reaction on user request to load points.
void gui_controls_pcloud::onLoadPoints()
{
  TIMER_NEW
  TIMER_GO

  // Select file for point cloud data
  QString
    qfilename = gui_common::selectFile(QStringList() << "*.pts" << "*.xyz",
                                       "Choose point cloud to load", "",
                                       gui_common::OpenSaveAction_Open);
  //
  TCollection_AsciiString filename = QStr2AsciiStr(qfilename);

  // Load point cloud
  Handle(geom_point_cloud) cloud = new geom_point_cloud(common_facilities::Instance()->Notifier,
                                                        common_facilities::Instance()->Plotter);
  if ( !cloud->Load(filename) )
  {
    std::cout << "Error: cannot load point cloud" << std::endl;
    return;
  }
  std::cout << "Point cloud was loaded successfully" << std::endl;

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Load point cloud")

  ActAPI_PlotterEntry IV(common_facilities::Instance()->Plotter);
  IV.DRAW_POINTS( cloud->GetPoints(), Color_White, "Scan data" );
}

//-----------------------------------------------------------------------------

//! Reaction on estimation of normal field.
void gui_controls_pcloud::onEstimNormals()
{
  // TODO: the following is just weird
  Handle(ActAPI_INode)
    N = common_facilities::Instance()->Model->GetIVPointSetPartition()->GetNode(1);
  //
  Handle(visu_iv_point_set_node) PN = Handle(visu_iv_point_set_node)::DownCast(N);
  //
  if ( N.IsNull() || !PN->IsWellFormed() )
  {
    std::cout << "Error: point cloud is not ready" << std::endl;
    return;
  }

  // Calculate normals
  //geom_point_cloud_normals::Calculate( PN->GetPoints() );
  // TODO
}
