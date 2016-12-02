//-----------------------------------------------------------------------------
// Created on: 21 June 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <exeAsBuilt_ControlsPCloud.h>

// exeAsBuilt includes
#include <exeAsBuilt_CommonFacilities.h>
#include <exeAsBuilt_DialogCloudify.h>
#include <exeAsBuilt_PickPointCallback.h>

// asiAlgo includes
#include <asiAlgo_PointCloudUtils.h>
#include <asiAlgo_Timer.h>

// asiUI includes
#include <asiUI_Common.h>

// Qt include
#include <QGroupBox>

// exeAsBuilt includes (FlannKdTree should be included after Qt includes)
#include <exeAsBuilt_FlannKdTree.h>

//-----------------------------------------------------------------------------

#define BTN_MIN_WIDTH 120

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
exeAsBuilt_ControlsPCloud::exeAsBuilt_ControlsPCloud(QWidget* parent)
: QWidget(parent), m_iPrevSelMode(0)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_widgets.pLoadPointsBtn   = new QPushButton("Load points");
  m_widgets.pCloudifyBtn     = new QPushButton("Cloudify CAD part");
  m_widgets.pEstimNormalsBtn = new QPushButton("Estimate normals");
  m_widgets.pProjPlaneBtn    = new QPushButton("Projection plane");
  m_widgets.pProjPlane       = vtkSmartPointer<vtkPlaneWidget>::New();
  m_widgets.pPickPointBtn    = new QPushButton("Pick point");
  //
  m_widgets.pLoadPointsBtn   -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pCloudifyBtn     -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pEstimNormalsBtn -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pProjPlaneBtn    -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pPickPointBtn    -> setMinimumWidth(BTN_MIN_WIDTH);

  // Other configurations
  m_widgets.pPickPointBtn->setCheckable(true);
  m_widgets.pProjPlaneBtn->setCheckable(true);
  m_widgets.pProjPlane->SetInteractor( exeAsBuilt_CommonFacilities::Instance()->Prs.Part->GetRenderWindow()->GetInteractor() );

  // Group for data exchange
  QGroupBox*   pDEGroup = new QGroupBox("Data source");
  QVBoxLayout* pDELay   = new QVBoxLayout(pDEGroup);
  //
  pDELay->addWidget(m_widgets.pLoadPointsBtn);
  pDELay->addWidget(m_widgets.pCloudifyBtn);

  // Group for analysis
  QGroupBox*   pAnalysisGroup = new QGroupBox("Analysis");
  QVBoxLayout* pAnalysisLay   = new QVBoxLayout(pAnalysisGroup);
  //
  //pAnalysisLay->addWidget(m_widgets.pEstimNormals);
  pAnalysisLay->addWidget(m_widgets.pProjPlaneBtn);
  pAnalysisLay->addWidget(m_widgets.pPickPointBtn);

  // Set layout
  m_pMainLayout->addWidget(pDEGroup);
  m_pMainLayout->addWidget(pAnalysisGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pLoadPointsBtn,   SIGNAL( clicked() ), SLOT( onLoadPoints   () ) );
  connect( m_widgets.pCloudifyBtn,     SIGNAL( clicked() ), SLOT( onCloudify     () ) );
  connect( m_widgets.pEstimNormalsBtn, SIGNAL( clicked() ), SLOT( onEstimNormals () ) );
  connect( m_widgets.pProjPlaneBtn,    SIGNAL( clicked() ), SLOT( onProjPlane    () ) );
  connect( m_widgets.pPickPointBtn,    SIGNAL( clicked() ), SLOT( onPickPoint    () ) );
}

//! Destructor.
exeAsBuilt_ControlsPCloud::~exeAsBuilt_ControlsPCloud()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! Reaction on user request to load points.
void exeAsBuilt_ControlsPCloud::onLoadPoints()
{
  TIMER_NEW
  TIMER_GO

  // Select file for point cloud data
  QString
    qfilename = asiUI_Common::selectFile(QStringList() << "*.pts" << "*.xyz",
                                        "Choose point cloud to load", "",
                                         asiUI_Common::OpenSaveAction_Open);
  //
  TCollection_AsciiString filename = QStr2AsciiStr(qfilename);

  // Load point cloud
  Handle(asiAlgo_PointCloud<double>) cloud = new asiAlgo_PointCloud<double>;
  //
  if ( !cloud->Load( filename.ToCString() ) )
  {
    std::cout << "Error: cannot load point cloud" << std::endl;
    return;
  }
  std::cout << "Point cloud was loaded successfully" << std::endl;

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Load point cloud")

  ActAPI_PlotterEntry IV(exeAsBuilt_CommonFacilities::Instance()->Plotter);
  IV.DRAW_POINTS( asiAlgo_PointCloudUtils::AsRealArray(cloud), Color_White, "Scan data" );
}

//-----------------------------------------------------------------------------

//! Reaction on estimation of normal field.
void exeAsBuilt_ControlsPCloud::onEstimNormals()
{
  // TODO: the following is just weird
  Handle(ActAPI_INode)
    N = exeAsBuilt_CommonFacilities::Instance()->Model->GetIVPointSetPartition()->GetNode(1);
  //
  Handle(asiData_IVPointSetNode) PN = Handle(asiData_IVPointSetNode)::DownCast(N);
  //
  if ( N.IsNull() || !PN->IsWellFormed() )
  {
    std::cout << "Error: point cloud is not ready" << std::endl;
    return;
  }

  // Calculate normals
  //asiAlgo_PointCloudNormals::Calculate( PN->GetPoints() );
  // TODO
}

//-----------------------------------------------------------------------------

//! Builds point cloud from the model.
void exeAsBuilt_ControlsPCloud::onCloudify()
{
  Handle(exeAsBuilt_CommonFacilities) cf = exeAsBuilt_CommonFacilities::Instance();
  Handle(asiData_PartNode)            part_n;
  TopoDS_Shape                        part;
  //
  if ( !asiUI_Common::PartShape(cf->Model, part_n, part) ) return;

  // Run dialog for cloudification
  exeAsBuilt_DialogCloudify*
    wCloudify = new exeAsBuilt_DialogCloudify(cf->Model, cf->Notifier, cf->Plotter, this);
  //
  wCloudify->exec();
}

//-----------------------------------------------------------------------------

//! Enables/disables a projection plane.
void exeAsBuilt_ControlsPCloud::onProjPlane()
{
  Handle(exeAsBuilt_CommonFacilities) cf = exeAsBuilt_CommonFacilities::Instance();

  const bool isOn = m_widgets.pProjPlaneBtn->isChecked();

  if ( isOn )
  {
    m_widgets.pProjPlane->On();
  }
  else
  {
    m_widgets.pProjPlane->Off();
  }
}

//-----------------------------------------------------------------------------

//! Allows user to pick a point.
void exeAsBuilt_ControlsPCloud::onPickPoint()
{
  Handle(exeAsBuilt_CommonFacilities) cf = exeAsBuilt_CommonFacilities::Instance();
  Handle(asiData_PartNode)            part_n;
  TopoDS_Shape                        part;
  //
  if ( !asiUI_Common::PartShape(cf->Model, part_n, part) ) return;

  // Build k-d tree if necessary
  if ( m_kdTree.IsNull() )
  {
    // Get the stored point cloud
    Handle(asiData_REPointsNode)      PointsNode = cf->Model->GetRENode()->Points();
    Handle(asiAlgo_PointCloud<float>) PointCloud = PointsNode->GetPointsf();
    //
    if ( PointCloud.IsNull() || !PointCloud->GetNumberOfPoints() )
    {
      std::cout << "Point cloud is null or empty" << std::endl;
      return;
    }
    std::cout << "Number of points in the point cloud: " << PointCloud->GetNumberOfPoints() << std::endl;

    TIMER_NEW
    TIMER_GO

    m_kdTree = new exeAsBuilt_FlannKdTree(PointCloud);

    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Construct k-d tree")
  }

  const bool isOn = m_widgets.pPickPointBtn->isChecked();

  // Depending on the state of the control, either let user pick a
  // point or finalize picking
  if ( isOn )
  {
    // Enable an appropriate selection mode
    m_iPrevSelMode = cf->ViewerPart->PrsMgr()->GetSelectionMode();
    cf->ViewerPart->PrsMgr()->SetSelectionMode(SelectionMode_Workpiece);

    // Configure workpiece picker
    cf->ViewerPart->GetPickCallback()->SetWorkpiecePicker(PickType_Point);

    // Add observer which takes responsibility to interact with user
    if ( !cf->ViewerPart->PrsMgr()->HasObserver(EVENT_PICK_WORLD_POINT) )
    {
      vtkSmartPointer<exeAsBuilt_PickPointCallback>
        cb = vtkSmartPointer<exeAsBuilt_PickPointCallback>::New();
      //
      cb->SetModel(cf->Model);
      cb->SetKdTree(m_kdTree);

      // Add observer
      cf->ViewerPart->PrsMgr()->AddObserver(EVENT_PICK_WORLD_POINT, cb);
    }
  }
  else
  {
    // Restore original selection mode
    cf->ViewerPart->PrsMgr()->SetSelectionMode(m_iPrevSelMode);
    //
    cf->ViewerPart->PrsMgr()->RemoveObserver(EVENT_PICK_WORLD_POINT);

    // Configure workpiece picker
    cf->ViewerPart->GetPickCallback()->SetWorkpiecePicker(PickType_World);
  }

  // TODO: NYI
}
