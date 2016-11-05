//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_ControlsPart.h>

// asiUI includes
#include <asiUI_Common.h>
#include <asiUI_DialogCloudify.h>
#include <asiUI_DialogSewing.h>
#include <asiUI_DialogSTEP.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asiAlgo includes
#include <asiAlgo_MeshConvert.h>
#include <asiAlgo_PLY.h>
#include <asiAlgo_STEP.h>
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// asiVisu includes
#include <asiVisu_GeomPrs.h>

// OCCT includes
#include <BRepBndLib.hxx>
#include <BRepTools.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

// Qt include
#include <QGroupBox>

// VTK includes
#include <vtkActor.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

#define BTN_MIN_WIDTH 120

//-----------------------------------------------------------------------------

//! Constructor.
//! \param model      [in] Data Model instance.
//! \param partPrsMgr [in] presentation manager for Part Node.
//! \param notifier   [in] progress notifier.
//! \param plotter    [in] imperative plotter.
//! \param parent     [in] parent widget.
asiUI_ControlsPart::asiUI_ControlsPart(const Handle(asiEngine_Model)&             model,
                                       const vtkSmartPointer<asiVisu_PrsManager>& partPrsMgr,
                                       ActAPI_ProgressEntry                       notifier,
                                       ActAPI_PlotterEntry                        plotter,
                                       QWidget*                                   parent)
//
: QWidget      (parent),
  m_model      (model),
  m_partPrsMgr (partPrsMgr),
  m_notifier   (notifier),
  m_plotter    (plotter)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_widgets.pLoadBRep      = new QPushButton("Load b-rep");
  m_widgets.pLoadSTEP      = new QPushButton("Load STEP");
  m_widgets.pSaveSTEP      = new QPushButton("Save STEP");
  m_widgets.pSavePly       = new QPushButton("Save mesh (ply)");
  m_widgets.pSaveBRep      = new QPushButton("Save b-rep");
  //
  m_widgets.pCheckShape    = new QPushButton("Check shape");
  m_widgets.pTolerance     = new QPushButton("Tolerance");
  //
  m_widgets.pSewing        = new QPushButton("Sewing");
  m_widgets.pMaximizeFaces = new QPushButton("Maximize faces");
  //
  m_widgets.pShowVertices  = new QPushButton("Show vertices");
  m_widgets.pShowNormals   = new QPushButton("Show normals");
  m_widgets.pSelectFaces   = new QPushButton("Select faces");
  m_widgets.pSelectEdges   = new QPushButton("Select edges");
  //
  m_widgets.pLoadBRep      -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pLoadSTEP      -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSaveSTEP      -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSavePly       -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSaveBRep      -> setMinimumWidth(BTN_MIN_WIDTH);
  //
  m_widgets.pCheckShape    -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pTolerance     -> setMinimumWidth(BTN_MIN_WIDTH);
  //
  m_widgets.pSewing        -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pMaximizeFaces -> setMinimumWidth(BTN_MIN_WIDTH);
  //
  m_widgets.pShowVertices  -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pShowNormals   -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSelectFaces   -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSelectEdges   -> setMinimumWidth(BTN_MIN_WIDTH);

  // Other configurations
  m_widgets.pShowVertices->setCheckable(true);

  // Group box for data interoperability
  QGroupBox*   pExchangeGroup = new QGroupBox("Data Exchange");
  QVBoxLayout* pExchangeLay   = new QVBoxLayout(pExchangeGroup);
  //
  pExchangeLay->addWidget(m_widgets.pLoadBRep);
  pExchangeLay->addWidget(m_widgets.pLoadSTEP);
  pExchangeLay->addWidget(m_widgets.pSaveSTEP);
  pExchangeLay->addWidget(m_widgets.pSavePly);
  pExchangeLay->addWidget(m_widgets.pSaveBRep);

  // Group for analysis
  QGroupBox*   pAnalysisGroup = new QGroupBox("Analysis");
  QVBoxLayout* pAnalysisLay   = new QVBoxLayout(pAnalysisGroup);
  //
  pAnalysisLay->addWidget(m_widgets.pCheckShape);
  pAnalysisLay->addWidget(m_widgets.pTolerance);

  // Group for processing
  QGroupBox*   pProcessingGroup = new QGroupBox("Processing");
  QVBoxLayout* pProcessingLay   = new QVBoxLayout(pProcessingGroup);
  //
  pProcessingLay->addWidget(m_widgets.pSewing);
  pProcessingLay->addWidget(m_widgets.pMaximizeFaces);

  // Group for visualization
  QGroupBox*   pVisuGroup = new QGroupBox("Visualization");
  QVBoxLayout* pVisuLay   = new QVBoxLayout(pVisuGroup);
  //
  pVisuLay->addWidget(m_widgets.pShowVertices);
  pVisuLay->addWidget(m_widgets.pShowNormals);
  pVisuLay->addWidget(m_widgets.pSelectFaces);
  pVisuLay->addWidget(m_widgets.pSelectEdges);

  // Set layout
  m_pMainLayout->addWidget(pExchangeGroup);
  m_pMainLayout->addWidget(pAnalysisGroup);
  m_pMainLayout->addWidget(pProcessingGroup);
  m_pMainLayout->addWidget(pVisuGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pLoadBRep,      SIGNAL( clicked() ), SLOT( onLoadBRep      () ) );
  connect( m_widgets.pLoadSTEP,      SIGNAL( clicked() ), SLOT( onLoadSTEP      () ) );
  connect( m_widgets.pSaveSTEP,      SIGNAL( clicked() ), SLOT( onSaveSTEP      () ) );
  connect( m_widgets.pSavePly,       SIGNAL( clicked() ), SLOT( onSavePly       () ) );
  connect( m_widgets.pSaveBRep,      SIGNAL( clicked() ), SLOT( onSaveBRep      () ) );
  //
  connect( m_widgets.pCheckShape,    SIGNAL( clicked() ), SLOT( onCheckShape    () ) );
  connect( m_widgets.pTolerance,     SIGNAL( clicked() ), SLOT( onTolerance     () ) );
  //
  connect( m_widgets.pSewing,        SIGNAL( clicked() ), SLOT( onSewing        () ) );
  connect( m_widgets.pMaximizeFaces, SIGNAL( clicked() ), SLOT( onMaximizeFaces () ) );
  //
  connect( m_widgets.pShowVertices,  SIGNAL( clicked() ), SLOT( onShowVertices  () ) );
  connect( m_widgets.pShowNormals,   SIGNAL( clicked() ), SLOT( onShowNormals   () ) );
  connect( m_widgets.pSelectFaces,   SIGNAL( clicked() ), SLOT( onSelectFaces   () ) );
  connect( m_widgets.pSelectEdges,   SIGNAL( clicked() ), SLOT( onSelectEdges   () ) );
}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_ControlsPart::~asiUI_ControlsPart()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! On BREP loading.
void asiUI_ControlsPart::onLoadBRep()
{
  QString filename = asiUI_Common::selectBRepFile(asiUI_Common::OpenSaveAction_Open);

  // Read BREP
  TopoDS_Shape shape;
  if ( !asiAlgo_Utils::ReadBRep(QStr2AsciiStr(filename), shape) )
  {
    std::cout << "Error: cannot read b-rep file" << std::endl;
    return;
  }

  // Clean up the Model
  m_model->Clear();

  // Set part geometry
  Handle(asiData_PartNode) geom_n = m_model->GetPartNode();
  //
  m_model->OpenCommand(); // tx start
  {
    geom_n->SetShape(shape);
  }
  m_model->CommitCommand(); // tx commit

  // Notify
  emit partLoaded();
}

//-----------------------------------------------------------------------------

//! Loads model from STEP file.
void asiUI_ControlsPart::onLoadSTEP()
{
  Handle(asiData_PartNode) geom_n = m_model->GetPartNode();

  // Dialog for reading STEP
  asiUI_DialogSTEP* pDlg = new asiUI_DialogSTEP(m_model, geom_n, asiUI_DialogSTEP::Mode_Read, this);
  pDlg->exec();

  // Notify
  emit partLoaded();
}

//-----------------------------------------------------------------------------

//! Saves model to STEP file.
void asiUI_ControlsPart::onSaveSTEP()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Dialog for STEP save
  asiUI_DialogSTEP* pDlg = new asiUI_DialogSTEP(m_model, part_n, asiUI_DialogSTEP::Mode_Write, this);
  pDlg->show();

  // Notify
  emit partSaved();
}

//-----------------------------------------------------------------------------

//! Saves mesh to PLY file.
void asiUI_ControlsPart::onSavePly()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;
  //
  QString filename = asiUI_Common::selectPlyFile(asiUI_Common::OpenSaveAction_Save);

  // Convert shape's inherent mesh to a storable mesh
  Handle(Mesh) storedMesh;
  if ( !asiAlgo_MeshConvert::ToPersistent(part, storedMesh) )
  {
    std::cout << "Error: cannot convert mesh to persistent form" << std::endl;
    return;
  }

  // Save mesh to ply files
  if ( !asiAlgo_PLY::Write( storedMesh, QStr2AsciiStr(filename) ) )
  {
    std::cout << "Error: cannot save mesh" << std::endl;
    return;
  }

  // Notify
  emit partSaved();
}

//-----------------------------------------------------------------------------

//! Saves model to BREP file.
void asiUI_ControlsPart::onSaveBRep()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;
  //
  QString filename = asiUI_Common::selectBRepFile(asiUI_Common::OpenSaveAction_Save);

  // Save shape
  if ( !asiAlgo_Utils::WriteBRep( part, QStr2AsciiStr(filename) ) )
  {
    std::cout << "Error: cannot save shape" << std::endl;
    return;
  }

  // Notify
  emit partSaved();
}

//-----------------------------------------------------------------------------

//! Runs shape checker.
void asiUI_ControlsPart::onCheckShape()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get highlighted faces
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part(m_model, m_partPrsMgr).GetHighlightedSubShapes(selected);

  if ( selected.IsEmpty() )
  {
    if ( !asiAlgo_Utils::CheckShape(part, m_notifier) )
    {
      std::cout << "Error: shape is invalid" << std::endl;
      return;
    }
    std::cout << "Shape is correct" << std::endl;
  }
  else
  {
    for ( int i = 1; i <= selected.Extent(); ++i )
    {
      if ( !asiAlgo_Utils::CheckShape(selected(i), m_notifier) )
      {
        std::cout << "Error: sub-shape is invalid" << std::endl;
        return;
      }
      std::cout << "Sub-shape is correct" << std::endl;
    }
  }
}

//-----------------------------------------------------------------------------

//! Consults tolerance.
void asiUI_ControlsPart::onTolerance()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get highlighted faces
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part(m_model, m_partPrsMgr).GetHighlightedSubShapes(selected);

  if ( selected.IsEmpty() )
  {
    const double tol = asiAlgo_Utils::MaxTolerance(part);
    std::cout << "Tolerance: " << tol << std::endl;
  }
  else
  {
    double maxTol = 0.0;
    for ( int i = 1; i <= selected.Extent(); ++i )
    {
      const double tol = asiAlgo_Utils::MaxTolerance( selected(i) );
      maxTol = std::max(tol, maxTol);
    }
    std::cout << "Tolerance (sub-shapes): " << maxTol << std::endl;
  }
}

//-----------------------------------------------------------------------------

//! Runs sewing.
void asiUI_ControlsPart::onSewing()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Run dialog for sewing properties
  asiUI_DialogSewing* wSewing = new asiUI_DialogSewing(m_model, part_n, this);
  wSewing->exec();

  // Notify
  emit partModified();
}

//-----------------------------------------------------------------------------

//! Runs face maximization.
void asiUI_ControlsPart::onMaximizeFaces()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  m_model->OpenCommand();
  {
    TIMER_NEW
    TIMER_GO

    // Perform merge
    if ( !asiAlgo_Utils::MaximizeFaces(part) )
    {
      std::cout << "Error: face merging failed" << std::endl;
      m_model->AbortCommand();
      return;
    }

    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Face Maximization")

    //
    std::cout << "Face merging done. Visualizing..." << std::endl;
    //
    part_n->SetShape(part);
  }
  m_model->CommitCommand();

  // Notify
  emit partModified();
}

//-----------------------------------------------------------------------------

//! Switches visualization of vertices.
void asiUI_ControlsPart::onShowVertices()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  const bool isOn = m_widgets.pShowVertices->isChecked();

  // Modify data
  m_model->OpenCommand();
  {
    part_n->SetHasVertices(isOn);
  }
  m_model->CommitCommand();

  // Notify
  isOn ? emit verticesOn() : emit verticesOff();
}

//-----------------------------------------------------------------------------

//! Switches visualization of normals.
void asiUI_ControlsPart::onShowNormals()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Calculate boundary box to choose a nice modulus for normal
  Bnd_Box B;
  BRepBndLib::Add(part, B);
  //
  const double modulus = B.CornerMin().Distance( B.CornerMax() ) / 50;

  const bool isOn = m_widgets.pShowNormals->isChecked();

  // Iterate over the face and sample the vector field defined by their
  // orientations
  for ( TopExp_Explorer fexp(part, TopAbs_FACE); fexp.More(); fexp.Next() )
  {
    const TopoDS_Face& face = TopoDS::Face( fexp.Current() );

    double uMin, uMax, vMin, vMax;
    BRepTools::UVBounds(face, uMin, uMax, vMin, vMax);

    // Get middle point
    const double um = (uMin + uMax)*0.5;
    const double vm = (vMin + vMax)*0.5;

    Handle(Geom_Surface) surf = BRep_Tool::Surface(face);
    gp_Pnt P;
    gp_Vec D1U, D1V;
    //
    surf->D1(um, vm, P, D1U, D1V);

    // Calculate normal
    gp_Vec norm = D1U ^ D1V;
    if ( norm.Magnitude() < RealEpsilon() )
    {
      std::cout << "Error: degenerated normal" << std::endl;
      m_plotter.DRAW_POINT(P, Color_Black, "Degenerated Face Norm");
      continue;
    }

    norm.Normalize();
    norm *= modulus;

    if ( face.Orientation() == TopAbs_REVERSED )
    {
      m_plotter.DRAW_VECTOR_AT(P, norm, Color_Blue, "Face Norm");
    }
    else
    {
      m_plotter.DRAW_VECTOR_AT(P, norm, Color_Red, "Face Norm");
    }
  }

  // Notify
  isOn ? emit normalsOn() : emit normalsOff();
}

//-----------------------------------------------------------------------------

//! Switches to selection by faces.
void asiUI_ControlsPart::onSelectFaces()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Enable the corresponding selection mode
  m_partPrsMgr->SetSelectionMode(SelectionMode_Face);

  // Notify
  emit selectionFacesOn();
}

//-----------------------------------------------------------------------------

//! Switches to selection by edges.
void asiUI_ControlsPart::onSelectEdges()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Enable the corresponding selection mode
  m_partPrsMgr->SetSelectionMode(SelectionMode_Edge);

  // Notify
  emit selectionEdgesOn();
}
