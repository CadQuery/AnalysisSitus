//-----------------------------------------------------------------------------
// Created on: 31 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_controls_features.h>

// Common includes
#include <common_draw_test_suite.h>
#include <common_facilities.h>

// GUI includes
#include <gui_common.h>
#include <gui_dialog_detect_holes.h>
#include <gui_dialog_sewing.h>

// Engine includes
#include <engine_part.h>

// Feature includes
#include <feature_aag_iterator.h>
#include <feature_suppress_faces.h>
#include <feature_suppress.h>
#include <feature_detect_fillets.h>
#include <feature_detect_choles.h>
#include <feature_detect_ordinary_blends.h>
#include <feature_detect_pholes.h>
#include <feature_detect_pockets.h>
#include <feature_solid_angle.h>
#include <feature_utils.h>

// Geometry includes
#include <geom_aag_vtk.h>

// Geometry includes
#include <geom_detach_faces.h>
#include <geom_find_same_hosts.h>
#include <geom_rehost_faces.h>
#include <geom_STEP.h>
#include <geom_utils.h>

// A-Situs (mesh) includes
#include <tess_convert.h>
#include <tess_ply.h>

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

void ClassifySolidAngle(const TopoDS_Face&          F,
                        const TopoDS_Face&          G,
                        TopTools_IndexedMapOfShape& convexEdges,
                        TopTools_IndexedMapOfShape& concaveEdges,
                        TopTools_IndexedMapOfShape& undefinedEdges,
                        TopTools_IndexedMapOfShape& smoothEdges,
                        TopoDS_Compound&            convexEdgesComp,
                        TopoDS_Compound&            concaveEdgesComp,
                        TopoDS_Compound&            undefinedEdgesComp,
                        TopoDS_Compound&            smoothEdgesComp,
                        const bool                  usePlotter,
                        const bool                  allowSmooth)
{
  // Check angle between the two faces
  TopTools_IndexedMapOfShape commonEdges;
  feature_solid_angle solid_angle(common_facilities::Instance()->Notifier,
                                  usePlotter ? common_facilities::Instance()->Plotter : NULL);
  //
  feature_angle angle = solid_angle.AngleBetweenFaces(F, G, allowSmooth, commonEdges);
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
  else if ( angle == Angle_Smooth )
  {
    pTargetMap  = &smoothEdges;
    pTargetComp = &smoothEdgesComp;
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
    BRep_Builder().Add( *pTargetComp, commonEdges(i) );
  }
}

//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
gui_controls_features::gui_controls_features(QWidget* parent) : QWidget(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_widgets.pCheckSolidAngles    = new QPushButton("Classify solid angles");
  m_widgets.pFindSmoothEdges     = new QPushButton("Find smooth edges");
  m_widgets.pFindConvexOnly      = new QPushButton("Find convex-only");
  m_widgets.pFindSlots           = new QPushButton("Find slots");
  m_widgets.pFindHoles           = new QPushButton("Find holes");
  m_widgets.pSuppressHoles       = new QPushButton("Suppress holes");
  m_widgets.pFindPlanarHoles     = new QPushButton("Find planar holes");
  m_widgets.pFindFillets         = new QPushButton("Find fillets");
  //
  m_widgets.pDetachSelected      = new QPushButton("Detach sel. faces");
  m_widgets.pDeleteSelected      = new QPushButton("Delete sel. faces only");
  m_widgets.pDeleteSelectedFull  = new QPushButton("Delete sel. faces FULL");
  m_widgets.pFindSameHosts       = new QPushButton("Find same hosts");
  m_widgets.pRehostFaces         = new QPushButton("Re-host faces");
  m_widgets.pUnperiodizeSelected = new QPushButton("Unperiodize sel. faces");
  m_widgets.pCheckAlongCurvature = new QPushButton("Check along curvature");
  //
  m_widgets.pCheckSolidAngles    -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pFindSmoothEdges     -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pFindConvexOnly      -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pFindSlots           -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pFindHoles           -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSuppressHoles       -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pFindPlanarHoles     -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pFindFillets         -> setMinimumWidth(BTN_MIN_WIDTH);
  //
  m_widgets.pDetachSelected      -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pDeleteSelected      -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pDeleteSelectedFull  -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pFindSameHosts       -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pRehostFaces         -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pUnperiodizeSelected -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pCheckAlongCurvature -> setMinimumWidth(BTN_MIN_WIDTH);

  // Group for features
  QGroupBox*   pFeaturesGroup = new QGroupBox("Features");
  QVBoxLayout* pFeaturesLay   = new QVBoxLayout(pFeaturesGroup);
  //
  pFeaturesLay->addWidget(m_widgets.pCheckSolidAngles);
  pFeaturesLay->addWidget(m_widgets.pFindSmoothEdges);
  pFeaturesLay->addWidget(m_widgets.pFindConvexOnly);
  pFeaturesLay->addWidget(m_widgets.pFindSlots);
  pFeaturesLay->addWidget(m_widgets.pFindHoles);
  pFeaturesLay->addWidget(m_widgets.pSuppressHoles);
  pFeaturesLay->addWidget(m_widgets.pFindPlanarHoles);
  pFeaturesLay->addWidget(m_widgets.pFindFillets);

  // Group for features
  QGroupBox*   pDyModelingGroup = new QGroupBox("Dynamic Modeling");
  QVBoxLayout* pDyModelingLay   = new QVBoxLayout(pDyModelingGroup);
  //
  pDyModelingLay->addWidget(m_widgets.pDetachSelected);
  pDyModelingLay->addWidget(m_widgets.pDeleteSelected);
  pDyModelingLay->addWidget(m_widgets.pDeleteSelectedFull);
  pDyModelingLay->addWidget(m_widgets.pFindSameHosts);
  pDyModelingLay->addWidget(m_widgets.pRehostFaces);
  pDyModelingLay->addWidget(m_widgets.pUnperiodizeSelected);
  pDyModelingLay->addWidget(m_widgets.pCheckAlongCurvature);

  // Set layout
  m_pMainLayout->addWidget(pFeaturesGroup);
  m_pMainLayout->addWidget(pDyModelingGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pCheckSolidAngles,    SIGNAL( clicked() ), SLOT( onCheckSolidAngles    () ) );
  connect( m_widgets.pFindSmoothEdges,     SIGNAL( clicked() ), SLOT( onFindSmoothEdges     () ) );
  connect( m_widgets.pFindConvexOnly,      SIGNAL( clicked() ), SLOT( onFindConvexOnly      () ) );
  connect( m_widgets.pFindSlots,           SIGNAL( clicked() ), SLOT( onFindSlots           () ) );
  connect( m_widgets.pFindHoles,           SIGNAL( clicked() ), SLOT( onFindHoles           () ) );
  connect( m_widgets.pSuppressHoles,       SIGNAL( clicked() ), SLOT( onSuppressHoles       () ) );
  connect( m_widgets.pFindPlanarHoles,     SIGNAL( clicked() ), SLOT( onFindPlanarHoles     () ) );
  connect( m_widgets.pFindFillets,         SIGNAL( clicked() ), SLOT( onFindFillets         () ) );
  //
  connect( m_widgets.pDetachSelected,      SIGNAL( clicked() ), SLOT( onDetachSelected      () ) );
  connect( m_widgets.pDeleteSelected,      SIGNAL( clicked() ), SLOT( onDeleteSelected      () ) );
  connect( m_widgets.pDeleteSelectedFull,  SIGNAL( clicked() ), SLOT( onDeleteSelectedFull  () ) );
  connect( m_widgets.pFindSameHosts,       SIGNAL( clicked() ), SLOT( onFindSameHosts       () ) );
  connect( m_widgets.pRehostFaces,         SIGNAL( clicked() ), SLOT( onRehostFaces         () ) );
  connect( m_widgets.pUnperiodizeSelected, SIGNAL( clicked() ), SLOT( onUnperiodizeSelected () ) );
  connect( m_widgets.pCheckAlongCurvature, SIGNAL( clicked() ), SLOT( onCheckAlongCurvature () ) );
}

//-----------------------------------------------------------------------------

//! Destructor.
gui_controls_features::~gui_controls_features()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! Classifies solid angles as concave / convex.
void gui_controls_features::onCheckSolidAngles()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Get highlighted sub-shapes
  TopTools_IndexedMapOfShape selected;
  engine_part::GetHighlightedSubShapes(selected);

  // Holders for geometries
  TopTools_IndexedMapOfShape convexEdges,     concaveEdges,     undefinedEdges,     smoothEdges;
  TopoDS_Compound            convexEdgesComp, concaveEdgesComp, undefinedEdgesComp, smoothEdgesComp;
  //
  BRep_Builder BB;
  BB.MakeCompound(convexEdgesComp);
  BB.MakeCompound(concaveEdgesComp);
  BB.MakeCompound(undefinedEdgesComp);
  BB.MakeCompound(smoothEdgesComp);

  // Perform either global or local analysis
  if ( selected.IsEmpty() )
  {
    // Build AAG
    Handle(feature_aag) aag = new feature_aag(part);

    // Loop over the faces
    //
    Handle(feature_aag_random_iterator) it = new feature_aag_random_iterator(aag);
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
        ClassifySolidAngle(current_face,
                           neighbor_face,
                           convexEdges,
                           concaveEdges,
                           undefinedEdges,
                           smoothEdges,
                           convexEdgesComp,
                           concaveEdgesComp,
                           undefinedEdgesComp,
                           smoothEdgesComp,
                           false,
                           false);
      }
    }
  }
  else
  {
    std::cout << "Info: local analysis of edges" << std::endl;

    TopoDS_Face twin;
    if ( selected.Extent() < 2 )
    {
      std::cout << "Warning: at least 2 faces are expected" << std::endl;

      // Analyze the same face twice
      twin = TopoDS::Face( selected.FindKey(1) );
    }

    for ( int f = 1; f <= selected.Extent(); ++f )
    {
      const TopoDS_Face& F = TopoDS::Face( selected(f) );
      //
      if ( twin.IsNull() )
      {
        for ( int g = f + 1; g <= selected.Extent(); ++g )
        {
          const TopoDS_Face& G = TopoDS::Face( selected(g) );

          // Check angle between the two faces
          ClassifySolidAngle(F,
                             G,
                             convexEdges,
                             concaveEdges,
                             undefinedEdges,
                             smoothEdges,
                             convexEdgesComp,
                             concaveEdgesComp,
                             undefinedEdgesComp,
                             smoothEdgesComp,
                             true,
                             false);
        }
      }
      else
      {
        // Check angle between the two faces
        ClassifySolidAngle(F,
                           twin,
                           convexEdges,
                           concaveEdges,
                           undefinedEdges,
                           smoothEdges,
                           convexEdgesComp,
                           concaveEdgesComp,
                           undefinedEdgesComp,
                           smoothEdgesComp,
                           true,
                           false);
      }
    }
  }

  // Dump
  std::cout << "\tNum. convex  edges: "   << convexEdges.Extent()    << std::endl;
  std::cout << "\tNum. concave edges: "   << concaveEdges.Extent()   << std::endl;
  std::cout << "\tNum. undefined edges: " << undefinedEdges.Extent() << std::endl;
  std::cout << "\tNum. smooth edges: "    << smoothEdges.Extent()    << std::endl;

  // Save to model
  Handle(geom_boundary_edges_node)
    BN = common_facilities::Instance()->Model->GetPartNode()->GetBoundaryEdgesRepresentation();
  //
  common_facilities::Instance()->Model->OpenCommand();
  {
    ActParamTool::AsShape( BN->Parameter(geom_boundary_edges_node::PID_Green)    )->SetShape(convexEdgesComp);
    ActParamTool::AsShape( BN->Parameter(geom_boundary_edges_node::PID_Red)   )->SetShape(concaveEdgesComp);
    ActParamTool::AsShape( BN->Parameter(geom_boundary_edges_node::PID_Ordinary) )->SetShape(undefinedEdgesComp);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Update viewer
  Handle(visu_geom_prs)
    NPrs = Handle(visu_geom_prs)::DownCast(
             common_facilities::Instance()->Prs.Part->GetPresentation( common_facilities::Instance()->Model->GetPartNode() )
           );
  if ( NPrs.IsNull() )
  {
    std::cout << "Error: there is no available presentation for part" << std::endl;
    return;
  }
  //
  NPrs->MainActor()->GetProperty()->SetOpacity(0.5);
  //
  NPrs->GetPipeline(visu_geom_prs::Pipeline_Contour)->Actor()->SetVisibility(0);
  //
  common_facilities::Instance()->Prs.Part->Actualize( BN.get() );
}

//-----------------------------------------------------------------------------

//! Finds smooth edges.
void gui_controls_features::onFindSmoothEdges()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Get highlighted sub-shapes
  TopTools_IndexedMapOfShape selected;
  engine_part::GetHighlightedSubShapes(selected);

  // Holders for geometries
  TopTools_IndexedMapOfShape convexEdges,     concaveEdges,     undefinedEdges,     smoothEdges;
  TopoDS_Compound            convexEdgesComp, concaveEdgesComp, undefinedEdgesComp, smoothEdgesComp;
  //
  BRep_Builder BB;
  BB.MakeCompound(convexEdgesComp);
  BB.MakeCompound(concaveEdgesComp);
  BB.MakeCompound(undefinedEdgesComp);
  BB.MakeCompound(smoothEdgesComp);

  // Perform either global or local analysis
  if ( selected.IsEmpty() )
  {
    // Build AAG
    Handle(feature_aag) aag = new feature_aag(part);

    // Loop over the faces
    //
    Handle(feature_aag_random_iterator) it = new feature_aag_random_iterator(aag);
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
        ClassifySolidAngle(current_face,
                           neighbor_face,
                           convexEdges,
                           concaveEdges,
                           undefinedEdges,
                           smoothEdges,
                           convexEdgesComp,
                           concaveEdgesComp,
                           undefinedEdgesComp,
                           smoothEdgesComp,
                           false,
                           true);
      }
    }
  }
  else
  {
    std::cout << "Info: local analysis of edges" << std::endl;
    if ( selected.Extent() < 2 )
    {
      std::cout << "Error: at least 2 faces are expected" << std::endl;
      return;
    }

    for ( int f = 1; f <= selected.Extent(); ++f )
    {
      const TopoDS_Face& F = TopoDS::Face( selected(f) );
      //
      for ( int g = f + 1; g <= selected.Extent(); ++g )
      {
        const TopoDS_Face& G = TopoDS::Face( selected(g) );

        // Check angle between the two faces
        ClassifySolidAngle(F,
                           G,
                           convexEdges,
                           concaveEdges,
                           undefinedEdges,
                           smoothEdges,
                           convexEdgesComp,
                           concaveEdgesComp,
                           undefinedEdgesComp,
                           smoothEdgesComp,
                           true,
                           true);
      }
    }
  }

  // Dump
  std::cout << "\tNum. convex  edges: "   << convexEdges.Extent()    << std::endl;
  std::cout << "\tNum. concave edges: "   << concaveEdges.Extent()   << std::endl;
  std::cout << "\tNum. undefined edges: " << undefinedEdges.Extent() << std::endl;
  std::cout << "\tNum. smooth edges: "    << smoothEdges.Extent()    << std::endl;

  // Save to model
  Handle(geom_boundary_edges_node)
    BN = common_facilities::Instance()->Model->GetPartNode()->GetBoundaryEdgesRepresentation();
  //
  common_facilities::Instance()->Model->OpenCommand();
  {
    ActParamTool::AsShape( BN->Parameter(geom_boundary_edges_node::PID_Ordinary) ) ->SetShape( TopoDS_Shape() );
    ActParamTool::AsShape( BN->Parameter(geom_boundary_edges_node::PID_Red) )      ->SetShape( TopoDS_Shape() );
    ActParamTool::AsShape( BN->Parameter(geom_boundary_edges_node::PID_Green) )    ->SetShape(smoothEdgesComp);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Update viewer
  Handle(visu_geom_prs)
    NPrs = Handle(visu_geom_prs)::DownCast(
             common_facilities::Instance()->Prs.Part->GetPresentation( common_facilities::Instance()->Model->GetPartNode() )
           );
  if ( NPrs.IsNull() )
  {
    std::cout << "Error: there is no available presentation for part" << std::endl;
    return;
  }
  //
  NPrs->MainActor()->GetProperty()->SetOpacity(0.5);
  //
  NPrs->GetPipeline(visu_geom_prs::Pipeline_Contour)->Actor()->SetVisibility(0);
  //
  common_facilities::Instance()->Prs.Part->Actualize( BN.get() );
}

//-----------------------------------------------------------------------------

//! Identifies convex-only faces.
void gui_controls_features::onFindConvexOnly()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Build AAG
  Handle(feature_aag) aag = new feature_aag(part);

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

//-----------------------------------------------------------------------------

//! Identifies slot features.
void gui_controls_features::onFindSlots()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Prepare detector
  feature_detect_pockets detector(part, 50,
                                  common_facilities::Instance()->Notifier,
                                  common_facilities::Instance()->Plotter);
  if ( !detector.Perform() )
  {
    std::cout << "Error: cannot detect slots" << std::endl;
    return;
  }

  const TopTools_IndexedMapOfShape& slotFaces = detector.GetResultFaces();
  std::cout << "Number of detected faces: " << slotFaces.Extent() << std::endl;

  // Highlight
  engine_part::HighlightSubShapes(slotFaces);
}

//-----------------------------------------------------------------------------

//! Finds holes.
void gui_controls_features::onFindHoles()
{
  // Run dialog
  gui_dialog_detect_holes* wDetectHoles = new gui_dialog_detect_holes(this);
  wDetectHoles->show();
}

//-----------------------------------------------------------------------------

//! Suppresses holes.
void gui_controls_features::onSuppressHoles()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Get highlighted faces
  TColStd_PackedMapOfInteger selected;
  engine_part::GetHighlightedFaces(selected);

  TIMER_NEW
  TIMER_GO

  // Delete faces as holes
  feature_suppress suppressor(part, NULL, NULL, common_facilities::Instance()->Plotter);
  //
  if ( !suppressor.Perform(selected) )
  {
    std::cout << "Error: cannot suppress holes" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Suppress holes")

  const TopoDS_Shape& result = suppressor.GetResult();

  // Save to model
  common_facilities::Instance()->Model->Clear();
  //
  common_facilities::Instance()->Model->OpenCommand();
  {
    common_facilities::Instance()->Model->GetPartNode()->SetShape(result);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Update viewer
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->InitializePickers();
  common_facilities::Instance()->Prs.Part->Actualize( common_facilities::Instance()->Model->GetPartNode().get() );
}

//-----------------------------------------------------------------------------

//! Finds planar holes.
void gui_controls_features::onFindPlanarHoles()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Identify polyhedral holes
  feature_detect_pholes detector(part,
                                 common_facilities::Instance()->Notifier,
                                 common_facilities::Instance()->Plotter);
  if ( !detector.Perform() )
  {
    std::cout << "Error: cannot identify planar holes" << std::endl;
    return;
  }

  // Get detected holes
  const TopTools_IndexedMapOfShape& holes = detector.GetResultFaces();
  if ( holes.IsEmpty() )
  {
    std::cout << "No holes detected" << std::endl;
    return;
  }
  else
    std::cout << holes.Extent() << " hole(s) detected" << std::endl;

  engine_part::HighlightSubShapes(holes);
}

//-----------------------------------------------------------------------------

//! Finds fillets.
void gui_controls_features::onFindFillets()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Get highlighted faces
  TColStd_PackedMapOfInteger selectedFaceIndices;
  engine_part::GetHighlightedFaces(selectedFaceIndices);

  // Identify fillets
  const double R = 20.0;
  feature_detect_ordinary_blends detector(part, NULL,
                                          common_facilities::Instance()->Notifier,
                                          common_facilities::Instance()->Plotter);
  //
  detector.SetFaces(selectedFaceIndices);
  //
  if ( !detector.Perform() )
  {
    std::cout << "Error: cannot identify fillets" << std::endl;
    return;
  }

  // Update viewer
  Handle(visu_geom_prs)
    NPrs = Handle(visu_geom_prs)::DownCast(
      common_facilities::Instance()->Prs.Part->GetPresentation( common_facilities::Instance()->Model->GetPartNode() )
    );
  //
  NPrs->MainActor()->GetProperty()->SetOpacity(0.5);
  //
  NPrs->GetPipeline(visu_geom_prs::Pipeline_Contour)->Actor()->SetVisibility(0);

  // Get detected fillets
  const TopTools_IndexedMapOfShape& fillets = detector.GetResultFaces();
  if ( fillets.IsEmpty() )
  {
    std::cout << "No fillets detected with radius not greater than " << R << std::endl;
    return;
  }
  else
    std::cout << fillets.Extent() << " hole(s) detected with radius not greater than " << R << std::endl;

  // Highlight fillet faces
  engine_part::HighlightSubShapes(fillets);
}

//-----------------------------------------------------------------------------

//! Finds faces with the same host geometry.
void gui_controls_features::onFindSameHosts()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Prepare plotter
  ActAPI_PlotterEntry IV(common_facilities::Instance()->Plotter);

  // Get all faces of the part
  TopTools_IndexedMapOfShape faces;
  TopExp::MapShapes(part, TopAbs_FACE, faces);

  TIMER_NEW
  TIMER_GO

  geom_find_same_hosts::t_groups groups;
  geom_find_same_hosts finder(faces);
  finder(groups);

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Detection of same-host faces")

  IV.CLEAN();

  // Highlight groups
  for ( int g = 1; g <= groups.Length(); ++g )
  {
    TopoDS_Compound groupComp;
    BRep_Builder().MakeCompound(groupComp);

    const TColStd_PackedMapOfInteger& group = groups(g);
    for ( TColStd_MapIteratorOfPackedMapOfInteger fit(group); fit.More(); fit.Next() )
    {
      BRep_Builder().Add( groupComp, faces( fit.Key() ) );
    }

    TCollection_AsciiString name("Same Host Group");
    name += " ["; name += group.Extent(); name += " surfaces]";
    IV.DRAW_SHAPE(groupComp, Quantity_Color( ActAPI_IPlotter::Color_Sparse(g) ), name);
  }
}

//-----------------------------------------------------------------------------

//! Makes the selected faces share the same host geometry.
void gui_controls_features::onRehostFaces()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

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
    common_facilities::Instance()->Model->GetPartNode()->SetShape(result);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Update viewer
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->InitializePickers();
  common_facilities::Instance()->Prs.Part->Actualize( common_facilities::Instance()->Model->GetPartNode().get() );
}

//-----------------------------------------------------------------------------

//! Detaches selected faces.
void gui_controls_features::onDetachSelected()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

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
    common_facilities::Instance()->Model->GetPartNode()->SetShape(result);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Update viewer
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->InitializePickers();
  common_facilities::Instance()->Prs.Part->Actualize( common_facilities::Instance()->Model->GetPartNode().get() );
}

//-----------------------------------------------------------------------------

//! Deletes selected faces.
void gui_controls_features::onDeleteSelected()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Get highlighted faces
  TColStd_PackedMapOfInteger selected;
  engine_part::GetHighlightedFaces(selected);

  TIMER_NEW
  TIMER_GO

  // Delete selected faces
  feature_suppress_faces eraser(part);
  if ( !eraser.Perform(selected, true) )
  {
    std::cout << "Error: cannot delete faces" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Faces ONLY deletion")

  const TopoDS_Shape& result = eraser.GetResult();

  // Save to model
  common_facilities::Instance()->Model->Clear();
  //
  common_facilities::Instance()->Model->OpenCommand();
  {
    common_facilities::Instance()->Model->GetPartNode()->SetShape(result);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Update viewer
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->InitializePickers();
  common_facilities::Instance()->Prs.Part->Actualize( common_facilities::Instance()->Model->GetPartNode().get() );
}

//-----------------------------------------------------------------------------

//! Deletes selected faces with all underlying topology.
void gui_controls_features::onDeleteSelectedFull()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Get highlighted faces
  TColStd_PackedMapOfInteger selected;
  engine_part::GetHighlightedFaces(selected);

  TIMER_NEW
  TIMER_GO

  // Delete selected faces
  feature_suppress_faces eraser(part);
  if ( !eraser.Perform(selected, false) )
  {
    std::cout << "Error: cannot delete faces" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("FULL faces deletion")

  const TopoDS_Shape& result = eraser.GetResult();

  // Save to model
  common_facilities::Instance()->Model->Clear();
  //
  common_facilities::Instance()->Model->OpenCommand();
  {
    common_facilities::Instance()->Model->GetPartNode()->SetShape(result);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Update viewer
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->InitializePickers();
  common_facilities::Instance()->Prs.Part->Actualize( common_facilities::Instance()->Model->GetPartNode().get() );
}

//-----------------------------------------------------------------------------

//! Performs unperiodization of the selected faces.
void gui_controls_features::onUnperiodizeSelected()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Get highlighted faces
  TopTools_IndexedMapOfShape selected;
  engine_part::GetHighlightedSubShapes(selected);

  for ( int f = 1; f <= selected.Extent(); ++f )
  {
    const TopoDS_Shape& shape = selected(f);
    //
    if ( shape.ShapeType() != TopAbs_FACE )
      continue;

    // Extract host surface
    const TopoDS_Face& face = TopoDS::Face(shape);
    Handle(Geom_Surface) surf = BRep_Tool::Surface(face);
    //
    if ( !surf->IsKind( STANDARD_TYPE(Geom_BSplineSurface) ) )
      continue;

    // Convert to B-surface
    Handle(Geom_BSplineSurface) bsurf = Handle(Geom_BSplineSurface)::DownCast(surf);
    if ( bsurf->IsUPeriodic() )
    {
      std::cout << "Surface is U-periodic -> unperiodize in U" << std::endl;
      bsurf->SetUNotPeriodic();
    }
    if ( bsurf->IsVPeriodic() )
    {
      std::cout << "Surface is V-periodic -> unperiodize in V" << std::endl;
      bsurf->SetVNotPeriodic();
    }
  }

  // Update viewer
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->InitializePickers();
  common_facilities::Instance()->Prs.Part->Actualize( common_facilities::Instance()->Model->GetPartNode().get() );
}

//-----------------------------------------------------------------------------

//! Evaluate curvature along the selected edge.
void gui_controls_features::onCheckAlongCurvature()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Get highlighted edges
  TopTools_IndexedMapOfShape selected;
  engine_part::GetHighlightedSubShapes(selected);

  for ( int e = 1; e <= selected.Extent(); ++e )
  {
    const TopoDS_Edge& edge = TopoDS::Edge( selected(e) );
    //
    if ( edge.IsNull() )
      continue;

    // Get owner faces
    TopTools_IndexedDataMapOfShapeListOfShape M;
    TopExp::MapShapesAndAncestors(part, TopAbs_EDGE, TopAbs_FACE, M);
    //
    const TopTools_ListOfShape& faces = M.FindFromKey(edge);
    //
    for ( TopTools_ListIteratorOfListOfShape fit(faces); fit.More(); fit.Next() )
    {
      const TopoDS_Face& face = TopoDS::Face( fit.Value() );

      // Evaluate along curvature
      double k;
      if ( !feature_utils::EvaluateAlongCurvature(face, edge, k) )
      {
        std::cout << "Error: cannot evaluate along curvature" << std::endl;
        continue;
      }
      else
      {
        std::cout << "\tAlong curvature: " << k << std::endl;

        if ( Abs(k) < 1.e-5 )
          std::cout << "\t~ Curvature radius: infinite" << std::endl;
        else
          std::cout << "\t~ Curvature radius: " << Abs(1.0 / k) << std::endl;
      }
    }
  }
}
