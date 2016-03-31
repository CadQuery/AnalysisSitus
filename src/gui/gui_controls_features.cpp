//-----------------------------------------------------------------------------
// Created on: 31 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_controls_features.h>

// A-Situs (common) includes
#include <common_draw_test_suite.h>
#include <common_facilities.h>

// A-Situs (GUI) includes
#include <gui_common.h>
#include <gui_dialog_sewing.h>

// A-Situs (engine) includes
#include <engine_part.h>

// A-Situs (modeling) includes
#include <geom_aag_iterator.h>
#include <geom_aag_vtk.h>
#include <geom_delete_faces.h>
#include <geom_detach_faces.h>
#include <geom_detect_slots.h>
#include <geom_rehost_faces.h>
#include <geom_STEP.h>
#include <geom_utils.h>

// A-Situs (mesh) includes
#include <mesh_convert.h>
#include <mesh_ply.h>

// A-Situs (visualization) includes
#include <visu_geom_prs.h>
#include <visu_topo_graph.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <Precision.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

// Qt include
#include <QGroupBox>

// VTK includes
#include <vtkActor.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

#define BTN_MIN_WIDTH 120

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
gui_controls_features::gui_controls_features(QWidget* parent) : QWidget(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_widgets.pCheckSolidAngles   = new QPushButton("Classify solid angles");
  m_widgets.pFindConvexOnly     = new QPushButton("Find convex-only");
  m_widgets.pFindSlots          = new QPushButton("Find slots");
  m_widgets.pFindSameHosts      = new QPushButton("Find same hosts");
  m_widgets.pRehostFaces        = new QPushButton("Re-host faces");
  //
  m_widgets.pDetachSelected     = new QPushButton("Detach sel. faces");
  m_widgets.pDeleteSelected     = new QPushButton("Delete sel. faces only");
  m_widgets.pDeleteSelectedFull = new QPushButton("Delete sel. faces FULL");
  //
  m_widgets.pCheckSolidAngles   -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pFindConvexOnly     -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pFindSlots          -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pFindSameHosts      -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pRehostFaces        -> setMinimumWidth(BTN_MIN_WIDTH);
  //
  m_widgets.pDetachSelected     -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pDeleteSelected     -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pDeleteSelectedFull -> setMinimumWidth(BTN_MIN_WIDTH);

  // Group for features
  QGroupBox*   pFeaturesGroup = new QGroupBox("Features [SLOTS]");
  QVBoxLayout* pFeaturesLay   = new QVBoxLayout(pFeaturesGroup);
  //
  pFeaturesLay->addWidget(m_widgets.pCheckSolidAngles);
  pFeaturesLay->addWidget(m_widgets.pFindConvexOnly);
  pFeaturesLay->addWidget(m_widgets.pFindSlots);
  pFeaturesLay->addWidget(m_widgets.pFindSameHosts);
  pFeaturesLay->addWidget(m_widgets.pRehostFaces);

  // Group for features
  QGroupBox*   pAllFeaturesGroup = new QGroupBox("Features");
  QVBoxLayout* pAllFeaturesLay   = new QVBoxLayout(pAllFeaturesGroup);
  //
  pAllFeaturesLay->addWidget(m_widgets.pDetachSelected);
  pAllFeaturesLay->addWidget(m_widgets.pDeleteSelected);
  pAllFeaturesLay->addWidget(m_widgets.pDeleteSelectedFull);

  // Set layout
  m_pMainLayout->addWidget(pFeaturesGroup);
  m_pMainLayout->addWidget(pAllFeaturesGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pCheckSolidAngles,   SIGNAL( clicked() ), SLOT( onCheckSolidAngles   () ) );
  connect( m_widgets.pFindConvexOnly,     SIGNAL( clicked() ), SLOT( onFindConvexOnly     () ) );
  connect( m_widgets.pFindSlots,          SIGNAL( clicked() ), SLOT( onFindSlots          () ) );
  connect( m_widgets.pFindSameHosts,      SIGNAL( clicked() ), SLOT( onFindSameHosts      () ) );
  connect( m_widgets.pRehostFaces,        SIGNAL( clicked() ), SLOT( onRehostFaces        () ) );
  //
  connect( m_widgets.pDetachSelected,     SIGNAL( clicked() ), SLOT( onDetachSelected     () ) );
  connect( m_widgets.pDeleteSelected,     SIGNAL( clicked() ), SLOT( onDeleteSelected     () ) );
  connect( m_widgets.pDeleteSelectedFull, SIGNAL( clicked() ), SLOT( onDeleteSelectedFull () ) );
}

//! Destructor.
gui_controls_features::~gui_controls_features()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//! Classifies solid angles as concave / convex.
void gui_controls_features::onCheckSolidAngles()
{
  // Access Geometry Node
  Handle(geom_part_node) N = common_facilities::Instance()->Model->PartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // Working shape
  TopoDS_Shape part = N->GetShape();
  //
  if ( part.IsNull() )
  {
    std::cout << "Error: part shape is null" << std::endl;
    return;
  }

  // Build AAG
  Handle(geom_aag) aag = new geom_aag(part);

  // Loop over the faces
  TopTools_IndexedMapOfShape convexEdges,     concaveEdges,     undefinedEdges;
  TopoDS_Compound            convexEdgesComp, concaveEdgesComp, undefinedEdgesComp;
  //
  BRep_Builder BB;
  BB.MakeCompound(convexEdgesComp);
  BB.MakeCompound(concaveEdgesComp);
  BB.MakeCompound(undefinedEdgesComp);
  //
  Handle(geom_aag_random_iterator) it = new geom_aag_random_iterator(aag);
  for ( ; it->More(); it->Next() )
  {
    const int                         current_face_idx = it->GetFaceId();
    const TColStd_PackedMapOfInteger& neighbor_ids     = it->GetNeighbors();
    const TopoDS_Face&                current_face     = aag->GetFace(current_face_idx);

    // Loop over the neighbors
    for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbor_ids); nit.More(); nit.Next() )
    {
      const int          neighbor_face_idx = nit.Key();
      const TopoDS_Face& neighbor_face     = aag->GetFace(neighbor_face_idx);

      // Check angle between the two faces
      TopTools_IndexedMapOfShape commonEdges;
      geom_angle angle = geom_utils::AngleBetweenFaces(current_face, neighbor_face, commonEdges);
      //
      TopTools_IndexedMapOfShape* pTargetMap;
      TopoDS_Compound*            pTargetComp;
      //
      if ( angle == Angle_Convex )
      {
        pTargetMap  = &convexEdges;
        pTargetComp = &convexEdgesComp;
      }
      else if ( angle == Angle_Concave )
      {
        pTargetMap  = &concaveEdges;
        pTargetComp = &concaveEdgesComp;
      }
      else
      {
        pTargetMap  = &undefinedEdges;
        pTargetComp = &undefinedEdgesComp;
      }
      //
      for ( int i = 1; i <= commonEdges.Extent(); ++i )
      {
        pTargetMap->Add( commonEdges(i) );
        BB.Add( *pTargetComp, commonEdges(i) );
      }
    }
  }

  // Dump
  std::cout << "\tNum. convex  edges: "   << convexEdges.Extent()    << std::endl;
  std::cout << "\tNum. concave edges: "   << concaveEdges.Extent()   << std::endl;
  std::cout << "\tNum. undefined edges: " << undefinedEdges.Extent() << std::endl;

  // Save to model
  Handle(geom_boundary_edges_node) BN = N->BoundaryEdgesRepresentation();
  //
  common_facilities::Instance()->Model->OpenCommand();
  {
    ActParamTool::AsShape( BN->Parameter(geom_boundary_edges_node::PID_Convex)    )->SetShape(convexEdgesComp);
    ActParamTool::AsShape( BN->Parameter(geom_boundary_edges_node::PID_Concave)   )->SetShape(concaveEdgesComp);
    ActParamTool::AsShape( BN->Parameter(geom_boundary_edges_node::PID_Undefined) )->SetShape(undefinedEdgesComp);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Update viewer
  Handle(visu_geom_prs)
    NPrs = Handle(visu_geom_prs)::DownCast( common_facilities::Instance()->Prs.Part->GetPresentation(N) );
  //
  NPrs->MainActor()->GetProperty()->SetOpacity(0.5);
  //
  NPrs->GetPipeline(visu_geom_prs::Pipeline_Contour)->Actor()->SetVisibility(0);
  //
  common_facilities::Instance()->Prs.Part->Actualize( BN.get() );
}

//! Identifies convex-only faces.
void gui_controls_features::onFindConvexOnly()
{
  // Access Geometry Node
  Handle(geom_part_node) N = common_facilities::Instance()->Model->PartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // Working shape
  TopoDS_Shape part = N->GetShape();
  //
  if ( part.IsNull() )
  {
    std::cout << "Error: part shape is null" << std::endl;
    return;
  }

  // Build AAG
  Handle(geom_aag) aag = new geom_aag(part);

  // Find convex-only faces
  TopTools_IndexedMapOfShape convex;
  if ( !aag->FindConvexOnly(convex) )
  {
    std::cout << "There are no convex-only faces in AAG" << std::endl;
    return;
  }

  // Highlight sub-shape
  engine_part::HighlightSubShapes(convex);
}

//! Identifies slot features.
void gui_controls_features::onFindSlots()
{
  // Access Geometry Node
  Handle(geom_part_node) N = common_facilities::Instance()->Model->PartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // Working shape
  TopoDS_Shape part = N->GetShape();
  //
  if ( part.IsNull() )
  {
    std::cout << "Error: part shape is null" << std::endl;
    return;
  }

  TIMER_NEW
  TIMER_GO

  // Prepare detector
  geom_detect_slots detector(part);
  if ( !detector.Perform() )
  {
    std::cout << "Error: cannot detect slots" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Slots identification")

  const TopTools_IndexedMapOfShape& slotFaces = detector.Result();
  std::cout << "Number of detected faces: " << slotFaces.Extent() << std::endl;

  // Highlight
  engine_part::HighlightSubShapes(slotFaces);
}

//! Finds faces with the same host geometry.
void gui_controls_features::onFindSameHosts()
{
  // Access Geometry Node
  Handle(geom_part_node) N = common_facilities::Instance()->Model->PartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // Working shape
  TopoDS_Shape part = N->GetShape();
  //
  if ( part.IsNull() )
  {
    std::cout << "Error: part shape is null" << std::endl;
    return;
  }

  // Accumulate faces to highlight
  NCollection_Sequence<TopTools_IndexedMapOfShape> selected;
  TopTools_IndexedMapOfShape selectedSame;

  TIMER_NEW
  TIMER_GO

  const double anglePrec = Precision::Angular();
  const double distPrec  = Precision::Confusion();
  //
  for ( TopExp_Explorer exp1(part, TopAbs_FACE); exp1.More(); exp1.Next() )
  {
    const TopoDS_Face& F1 = TopoDS::Face( exp1.Current() );
    Handle(Geom_Plane) P1 = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface(F1) );
    //
    if ( P1.IsNull() )
      continue;

    const gp_Ax3& Ax_P1   = P1->Position();
    const gp_Dir& xDir_P1 = Ax_P1.XDirection();
    const gp_Dir& yDir_P1 = Ax_P1.YDirection();
    gp_Pln        pln_P1  = P1->Pln();

    // Second loop
    for ( TopExp_Explorer exp2(part, TopAbs_FACE); exp2.More(); exp2.Next() )
    {
      const TopoDS_Face& F2 = TopoDS::Face( exp2.Current() );
      if ( F1.IsSame(F2) )
        continue;

      Handle(Geom_Plane) P2 = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface(F2) );
      //
      if ( P2.IsNull() )
        continue;

      const gp_Ax3& Ax_P2   = P2->Position();
      const gp_Dir& xDir_P2 = Ax_P2.XDirection();
      const gp_Dir& yDir_P2 = Ax_P2.YDirection();
      gp_Pln        pln_P2  = P2->Pln();

      if ( !xDir_P1.IsParallel(xDir_P2, anglePrec) )
        continue;
      //
      if ( !yDir_P1.IsParallel(yDir_P2, anglePrec) )
        continue;
      //
      if ( pln_P1.Distance(pln_P2) > distPrec )
        continue;

      ///
      selectedSame.Add(F1);
      selectedSame.Add(F2);
    }
    //
    selected.Append(selectedSame);
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Detection of same-host faces")

  // Highlight
  engine_part::HighlightSubShapes( selectedSame, 100 );
  /*if ( selected.Length() >= 1 )
    engine_part::HighlightSubShapes( selected(1) );*/
}

//! Makes the selected faces share the same host geometry.
void gui_controls_features::onRehostFaces()
{
  // Access Geometry Node
  Handle(geom_part_node) N = common_facilities::Instance()->Model->PartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // Working shape
  TopoDS_Shape part = N->GetShape();
  //
  if ( part.IsNull() )
  {
    std::cout << "Error: part shape is null" << std::endl;
    return;
  }

  // Get highlighted faces
  TopTools_IndexedMapOfShape selected;
  engine_part::GetHighlightedSubShapes(selected);

  TIMER_NEW
  TIMER_GO

  // Re-host selected faces
  geom_rehost_faces rehoster(part);
  if ( !rehoster.Perform(selected) )
  {
    std::cout << "Error: cannot re-host faces" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Re-host faces")

  const TopoDS_Shape& result = rehoster.Result();

  // Save to model
  common_facilities::Instance()->Model->Clear();
  //
  common_facilities::Instance()->Model->OpenCommand();
  {
    N->SetShape(result);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Update viewer
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->InitializePicker();
  common_facilities::Instance()->Prs.Part->Actualize( N.get() );
}

//! Detaches selected faces.
void gui_controls_features::onDetachSelected()
{
  // Access Geometry Node
  Handle(geom_part_node) N = common_facilities::Instance()->Model->PartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // Working shape
  TopoDS_Shape part = N->GetShape();
  //
  if ( part.IsNull() )
  {
    std::cout << "Error: part shape is null" << std::endl;
    return;
  }

  // Get highlighted faces
  TopTools_IndexedMapOfShape selected;
  engine_part::GetHighlightedSubShapes(selected);

  TIMER_NEW
  TIMER_GO

  // Delete selected faces
  geom_detach_faces detacher(part);
  if ( !detacher.Perform(selected) )
  {
    std::cout << "Error: cannot detach faces" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Detach faces")

  const TopoDS_Shape& result = detacher.Result();

  // Save to model
  common_facilities::Instance()->Model->Clear();
  //
  common_facilities::Instance()->Model->OpenCommand();
  {
    N->SetShape(result);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Update viewer
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->InitializePicker();
  common_facilities::Instance()->Prs.Part->Actualize( N.get() );
}

//! Deletes selected faces.
void gui_controls_features::onDeleteSelected()
{
  // Access Geometry Node
  Handle(geom_part_node) N = common_facilities::Instance()->Model->PartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // Working shape
  TopoDS_Shape part = N->GetShape();
  //
  if ( part.IsNull() )
  {
    std::cout << "Error: part shape is null" << std::endl;
    return;
  }

  // Get highlighted faces
  TopTools_IndexedMapOfShape selected;
  engine_part::GetHighlightedSubShapes(selected);

  TIMER_NEW
  TIMER_GO

  // Delete selected faces
  geom_delete_faces eraser(part);
  if ( !eraser.Perform(selected, true) )
  {
    std::cout << "Error: cannot delete faces" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Faces ONLY deletion")

  const TopoDS_Shape& result = eraser.Result();

  // Save to model
  common_facilities::Instance()->Model->Clear();
  //
  common_facilities::Instance()->Model->OpenCommand();
  {
    N->SetShape(result);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Update viewer
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->InitializePicker();
  common_facilities::Instance()->Prs.Part->Actualize( N.get() );
}

//! Deletes selected faces with all underlying topology.
void gui_controls_features::onDeleteSelectedFull()
{
  // Access Geometry Node
  Handle(geom_part_node) N = common_facilities::Instance()->Model->PartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // Working shape
  TopoDS_Shape part = N->GetShape();
  //
  if ( part.IsNull() )
  {
    std::cout << "Error: part shape is null" << std::endl;
    return;
  }

  // Get highlighted faces
  TopTools_IndexedMapOfShape selected;
  engine_part::GetHighlightedSubShapes(selected);

  TIMER_NEW
  TIMER_GO

  // Delete selected faces
  geom_delete_faces eraser(part);
  if ( !eraser.Perform(selected, false) )
  {
    std::cout << "Error: cannot delete faces" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("FULL faces deletion")

  const TopoDS_Shape& result = eraser.Result();

  // Save to model
  common_facilities::Instance()->Model->Clear();
  //
  common_facilities::Instance()->Model->OpenCommand();
  {
    N->SetShape(result);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Update viewer
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->InitializePicker();
  common_facilities::Instance()->Prs.Part->Actualize( N.get() );
}
