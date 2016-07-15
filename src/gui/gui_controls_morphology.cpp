//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_controls_morphology.h>

// Common includes
#include <common_draw_test_suite.h>
#include <common_facilities.h>

// GUI includes
#include <gui_common.h>

// Feature includes
#include <feature_build_obb.h>

// Geometry includes
#include <geom_build_contours.h>
#include <geom_build_stock.h>
#include <geom_classify_point_solid.h>
#include <geom_face_point_cloud.h>
#include <geom_find_same_hosts.h>
#include <geom_intersect_curves.h>
#include <geom_intersect_ray_shape.h>
#include <geom_intersect_surfaces.h>
#include <geom_quick_hull_2d.h>
#include <geom_utils.h>

// Engine includes
#include <engine_part.h>
#include <engine_volume.h>

// Visualization includes
#include <visu_geom_prs.h>
#include <visu_prs.h>
#include <visu_re_surfaces_prs.h>

// Feature includes
#include <feature_aag.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepTools.hxx>
#include <Geom_Line.hxx>
#include <Precision.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

// Qt include
#include <QGroupBox>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

#define BTN_MIN_WIDTH 120

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
gui_controls_morphology::gui_controls_morphology(QWidget* parent) : QWidget(parent), m_fFuzzy(0.0)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_widgets.pSetAsPart        = new QPushButton("Set as part");
  m_widgets.pBuildMajorant    = new QPushButton("Build stock shape");
  m_widgets.pBuildMajorantOBB = new QPushButton("Build stock shape [OBB]");
  m_widgets.pBuildDelta       = new QPushButton("Build delta volume");
  m_widgets.pFindSeedFaces    = new QPushButton("Find seed faces");
  m_widgets.pFindExtrusions   = new QPushButton("Extract extrusions");
  m_widgets.pClassifyFeatures = new QPushButton("Classify features");
  //
  m_widgets.pSetAsPart        -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pBuildMajorant    -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pBuildMajorantOBB -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pBuildDelta       -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pFindSeedFaces    -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pFindExtrusions   -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pClassifyFeatures -> setMinimumWidth(BTN_MIN_WIDTH);

  // Decomposition browser
  m_widgets.wDecompBrowser = new gui_decomposition_browser();

  // Group for browser
  QGroupBox*   pBrowserGroup = new QGroupBox("Feature Model");
  QVBoxLayout* pBrowserLay   = new QVBoxLayout(pBrowserGroup);
  //
  pBrowserLay->addWidget(m_widgets.wDecompBrowser);

  // Group for stock volumes
  QGroupBox*   pVolumeGroup = new QGroupBox("Volume Decomposition");
  QVBoxLayout* pVolumeLay   = new QVBoxLayout(pVolumeGroup);
  //
  pVolumeLay->addWidget(m_widgets.pBuildMajorant);
  pVolumeLay->addWidget(m_widgets.pBuildMajorantOBB);
  pVolumeLay->addWidget(m_widgets.pBuildDelta);
  pVolumeLay->addWidget(m_widgets.pFindSeedFaces);
  pVolumeLay->addWidget(m_widgets.pFindExtrusions);
  pVolumeLay->addWidget(m_widgets.pClassifyFeatures);

  // Group for utilities
  QGroupBox*   pUtilitiesGroup = new QGroupBox("Utilities");
  QVBoxLayout* pUtilitiesLay   = new QVBoxLayout(pUtilitiesGroup);
  //
  pUtilitiesLay->addWidget(m_widgets.pSetAsPart);

  // Set layout
  m_pMainLayout->addWidget(pBrowserGroup);
  m_pMainLayout->addWidget(pVolumeGroup);
  m_pMainLayout->addWidget(pUtilitiesGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pSetAsPart,        SIGNAL( clicked() ), SLOT( onSetAsPart        () ) );
  connect( m_widgets.pBuildMajorant,    SIGNAL( clicked() ), SLOT( onBuildMajorant    () ) );
  connect( m_widgets.pBuildMajorantOBB, SIGNAL( clicked() ), SLOT( onBuildMajorantOBB () ) );
  connect( m_widgets.pBuildDelta,       SIGNAL( clicked() ), SLOT( onBuildDelta       () ) );
  connect( m_widgets.pFindSeedFaces,    SIGNAL( clicked() ), SLOT( onFindSeedFaces    () ) );
  connect( m_widgets.pFindExtrusions,   SIGNAL( clicked() ), SLOT( onFindExtrusions   () ) );
  connect( m_widgets.pClassifyFeatures, SIGNAL( clicked() ), SLOT( onClassifyFeatures () ) );
}

//! Destructor.
gui_controls_morphology::~gui_controls_morphology()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! Sets the selected volume as a part.
void gui_controls_morphology::onSetAsPart()
{
  ActAPI_DataObjectId id = m_widgets.wDecompBrowser->GetSelected();
  //
  if ( id.IsEmpty() )
  {
    std::cout << "Error: object to transfer is not selected" << std::endl;
    return;
  }

  // Get Model
  const Handle(common_model)& M = common_facilities::Instance()->Model;

  // Access Volume Node
  Handle(geom_volume_node) volume_n = Handle(geom_volume_node)::DownCast( M->FindNode(id) );
  //
  if ( volume_n.IsNull() || !volume_n->IsWellFormed() )
  {
    std::cout << "Error: selected object is not a valid volume" << std::endl;
    return;
  }

  // Transfer shape to part
  M->OpenCommand();
  {
    engine_part::Clean_Part();
    //
    M->PartNode()->SetShape( volume_n->GetShape() );
  }
  M->CommitCommand();

  // Update viewer
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->InitializePickers();
  common_facilities::Instance()->Prs.Part->Actualize( M->PartNode().get() );
}

//-----------------------------------------------------------------------------

//! Builds stock volume.
void gui_controls_morphology::onBuildMajorant()
{
  m_fFuzzy = 0.0;
  //
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  TIMER_NEW
  TIMER_GO

  // Prepare tool
  geom_build_stock stock_builder(common_facilities::Instance()->Notifier,
                                 common_facilities::Instance()->Plotter);

  if ( !stock_builder(part) )
  {
    std::cout << "Error: cannot build stock" << std::endl;
    return;
  }

  // Get result
  const TopoDS_Solid& stockSolid = stock_builder.Result();

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Stock reconstruction")

  /* ==============
   *  Finalization
   * ============== */

  // Create a volume item for stock
  common_facilities::Instance()->Model->OpenCommand();
  {
    m_stock_node = engine_volume::Create_Volume(stockSolid, "Stock");
  }
  common_facilities::Instance()->Model->CommitCommand();
  //
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->Actualize( m_stock_node.get() );
  m_widgets.wDecompBrowser->Populate();
}

//-----------------------------------------------------------------------------

//! Builds stock volume as OBB.
void gui_controls_morphology::onBuildMajorantOBB()
{
  m_fFuzzy = 0.1;
  //
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  TIMER_NEW
  TIMER_GO

  // Prepare tool
  feature_build_obb obb_builder(part, feature_build_obb::Mode_Inertia,
                                common_facilities::Instance()->Notifier,
                                common_facilities::Instance()->Plotter);

  if ( !obb_builder.Perform() )
  {
    std::cout << "Error: cannot build stock (OBB)" << std::endl;
    return;
  }

  // Get result
  TopoDS_Shape stockSolid = obb_builder.GetResult()->BuildSolid();

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Stock (OBB) reconstruction")

  /* ==============
   *  Finalization
   * ============== */

  // Create a volume item for stock
  common_facilities::Instance()->Model->OpenCommand();
  {
    m_stock_node = engine_volume::Create_Volume(stockSolid, "Stock");
  }
  common_facilities::Instance()->Model->CommitCommand();
  //
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->Actualize( m_stock_node.get() );
  m_widgets.wDecompBrowser->Populate();
}

//-----------------------------------------------------------------------------

//! Builds delta volume.
void gui_controls_morphology::onBuildDelta()
{
  // Get part shape
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Get stock volume which has to exist when user clicks that button
  if ( m_stock_node.IsNull() || !m_stock_node->IsWellFormed() )
  {
    std::cout << "Error: stock model is not available" << std::endl;
    return;
  }

  // Get stock shape
  TopoDS_Shape stock = m_stock_node->GetShape();
  //
  if ( stock.IsNull() || stock.ShapeType() != TopAbs_SOLID )
  {
    std::cout << "Error: stock is null or not a solid";
    return;
  }

  /* =====================
   *  Perform Boolean Cut
   * ===================== */

  TIMER_NEW
  TIMER_GO

  TopoDS_Shape deltaVolume = geom_utils::BooleanCut(stock, part, m_fFuzzy);

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Delta volume")

  /* ==============
   *  Finalization
   * ============== */

  // Create a volume item for delta and part
  common_facilities::Instance()->Model->OpenCommand();
  {
    m_delta_node = engine_volume::Create_Volume(deltaVolume, "Delta Volume", m_stock_node);
    m_part_node  = engine_volume::Create_Volume(part, "Part Volume", m_stock_node);
  }
  common_facilities::Instance()->Model->CommitCommand();
  //
  common_facilities::Instance()->Prs.Part->DeleteAllPresentations();
  common_facilities::Instance()->Prs.Part->Actualize( m_delta_node.get() );
  m_widgets.wDecompBrowser->Populate();
}

//-----------------------------------------------------------------------------

//! Searches for seed faces.
void gui_controls_morphology::onFindSeedFaces()
{
  // Get part shape
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Build AAG in order to have access to adjacency information
  Handle(feature_aag) aag = new feature_aag(part);

  // Find convex-only faces
  TopTools_IndexedMapOfShape convex;
  //
  if ( !aag->FindConvexOnly(convex) )
  {
    std::cout << "Error: cannot find convex-only faces" << std::endl;
    return;
  }

  // Prepare imperative viewer
  ActAPI_PlotterEntry IV(common_facilities::Instance()->Plotter);

  // Choose tolerance to compare angles
  const double tol_ang = Precision::Angular();

  // Keep only those having orthogonal edges in neighbors
  for ( int f = 1; f <= convex.Extent(); ++f )
  {
    // Get current face, its ID and its host surface
    const TopoDS_Face&   face    = TopoDS::Face( convex(f) );
    const int            face_id = aag->GetFaceId(face);
    Handle(Geom_Surface) surface = BRep_Tool::Surface(face);

    // We expect host surface to be a plane
    Handle(Geom_Plane) plane = Handle(Geom_Plane)::DownCast(surface);
    if ( plane.IsNull() )
    {
      std::cout << "Error: hey, this stuff works for planar faces only!" << std::endl;
      return;
    }

    // Get normal to the surface. As we are working with planes, any parameter
    // is suitable, so let's take just zeroes as U and V
    gp_Pnt plane_P;
    gp_Vec plane_DU, plane_DV;
    plane->D1(0.0, 0.0, plane_P, plane_DU, plane_DV);
    //
    gp_Dir plane_N = plane_DU ^ plane_DV;

    // Get neighbor faces
    const TColStd_PackedMapOfInteger& neighbors = aag->GetNeighbors(face_id);

    // Loop over the neighbor faces to check their edges
    int numOrtoEdges = 0;
    //
    for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbors); nit.More(); nit.Next() )
    {
      const int          neighbor_id = nit.Key();
      const TopoDS_Face& neighbor    = aag->GetFace(neighbor_id);

      // Loop over the edges of the neighbor face
      for ( TopExp_Explorer exp(neighbor, TopAbs_EDGE); exp.More(); exp.Next() )
      {
        // Get current neighbor edge and its curve
        const TopoDS_Edge& neighbor_edge = TopoDS::Edge( exp.Current() );
        double f, l;
        Handle(Geom_Curve) neighbor_curve = BRep_Tool::Curve(neighbor_edge, f, l);

        // We expect host curve to be a straight line
        Handle(Geom_Line) neighbor_line = Handle(Geom_Line)::DownCast(neighbor_curve);
        if ( neighbor_line.IsNull() )
        {
          std::cout << "Error: hey, this stuff works for planar edges only!" << std::endl;
          return;
        }

        // Get direction of the edge
        const gp_Dir& neighbor_line_dir = neighbor_line->Lin().Direction();

        // Finally check if that line is orthogonal to our plane, so it is
        // parallel to the surface normal
        if ( neighbor_line_dir.IsParallel(plane_N, tol_ang) )
          ++numOrtoEdges;
      }
    }

    if ( numOrtoEdges >= 3 )
    {
      std::cout << "Current face seems to be suitable" << std::endl;
      IV.DRAW_SHAPE(face, Quantity_Color(Quantity_NOC_RED), 0.5, "Seed Face");
    }
    else
      continue;

    // Now we have a candidate face which is promising in sense to be
    // a base face for extrusion. Let's check now if this face has a convex
    // domain or not. The idea is as follows.
    //
    // 1. For our face we build a convex hull.
    //
    // 2. We use many different convex combinations of hull points and classify
    //    each one with respect to our face. If any membership classification
    //    gives OUT, then domain is not convex, so we halt at false condition.
    //    Notice that we assume a face to have a sole outer wire without
    //    internal holes.
    //
    // NOTICE: we cannot use AABBs instead of convex hulls because AABBs
    //         will produce empty space for convex domains as well as for
    //         non-convex ones. Therefore, our calculation should be more
    //         accurate.

    // Build a two-dimensional point cloud of face vertices
    // in its parametric domain
    Handle(geom_face_point_cloud) pts2d = new geom_face_point_cloud;
    pts2d->InitWithVertices(face);

    //IV.CLEAN();
    //IV.DRAW_SHAPE(face);

    // Build convex hull
    geom_quick_hull_2d qHull(pts2d);
    //
    if ( !qHull.Perform() )
    {
      std::cout << "Error: cannot build convex hull" << std::endl;
      return;
    }
    const Handle(TColStd_HSequenceOfInteger)& hull = qHull.Hull();
    for ( int k = 1; k <= hull->Length(); ++k )
    {
      const int     hull_pt_idx = hull->Value(k);
      const gp_XYZ& hull_pt     = pts2d->Value3d(hull_pt_idx);

      IV.DRAW_POINT( hull_pt, Quantity_Color(Quantity_NOC_GREEN) );
      if ( k > 1 )
      {
        IV.DRAW_LINK( pts2d->Value3d( hull->Value(k - 1) ),
                      pts2d->Value3d( hull->Value(k) ),
                      Quantity_Color(Quantity_NOC_GREEN) );
      }
      if ( k == hull->Length() )
      {
        IV.DRAW_LINK( pts2d->Value3d( hull->Value(k) ),
                      pts2d->Value3d( hull->Value(1) ),
                      Quantity_Color(Quantity_NOC_GREEN) );
      }
    }

    //Sleep(1000);

    // TODO
  }
}

//-----------------------------------------------------------------------------

//! Searches for extrusions.
void gui_controls_morphology::onFindExtrusions()
{
  // Get part shape
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Get selected faces
  TopTools_IndexedMapOfShape faces;
  engine_part::GetHighlightedSubShapes(faces);
  //
  if ( faces.IsEmpty() )
  {
    std::cout << "Error: no faces to proceed" << std::endl;
    return;
  }

  // Prepare imperative viewer
  ActAPI_PlotterEntry IV(common_facilities::Instance()->Plotter);

  TIMER_NEW
  TIMER_GO

  TopoDS_Compound CutTool;
  BRep_Builder().MakeCompound(CutTool);
  //
  for ( int f = 1; f <= faces.Extent(); ++f )
  {
    const TopoDS_Face& face = TopoDS::Face( faces(f) );

    // Get normal to the face
    Handle(Geom_Surface) S = BRep_Tool::Surface(face);
    double uMin, uMax, vMin, vMax;
    BRepTools::UVBounds(face, uMin, uMax, vMin, vMax);
    //
    gp_Pnt P;
    gp_Vec D1U, D1V;
    S->D1( (uMin + uMax)*0.5, (vMin + vMax)*0.5, P, D1U, D1V);
    //
    gp_Vec N = D1U.Crossed(D1V);
    if ( N.Magnitude() < RealEpsilon() )
    {
      std::cout << "Error: cannot evaluate surface normal" << std::endl;
      return;
    }
    if ( face.Orientation() == TopAbs_REVERSED )
      N.Reverse();

    // Attempt to intersect the inverted surface normal with the rest of body
    geom_intersect_ray_shape inter_rs( part, Precision::Confusion() );
    //
    gp_Ax1 ray( P, N.Reversed().XYZ() );
    geom_intersection_points_ray_face inters;
    inter_rs(ray, inters);
    //
    if ( inters.Length() != 2 )
      continue; // We expect two intersection points for primitive extrusions

    // Draw
    for ( int k = 1; k <= inters.Length(); ++k )
    {
      const geom_intersection_point_ray_face& ipt = inters(k);
      //
      IV.DRAW_POINT( ipt.P.XYZ(), Quantity_Color(Quantity_NOC_BLUE1), "ipt" );
    }

    // Distance
    const double dist = ( inters(2).P.XYZ() - inters(1).P.XYZ() ).Modulus();

    // Build the recognized primitive explicitly
    BRepPrimAPI_MakePrism mkPrism( face, gp_Vec(ray.Direction().XYZ()*dist) );
    const TopoDS_Shape& extrusion = mkPrism.Shape();
    //
    IV.DRAW_SHAPE(extrusion, Quantity_Color(Quantity_NOC_RED), 0.5, "extrusion");

    BRep_Builder().Add(CutTool, extrusion);
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Detection of extrusions")

  /* =====================================
   *  Populate hierarchy of delta volumes
   * ===================================== */

  // Create a volume item for delta
  common_facilities::Instance()->Model->OpenCommand();
  {
    TIMER_NEW
    TIMER_GO

    // Add recognized extrusions and compute the rest of delta volume
    TopoDS_Shape deltaRest = part;
    for ( TopoDS_Iterator it(CutTool); it.More(); it.Next() )
    {
      const TopoDS_Shape& volume = it.Value();
      engine_volume::Create_Volume(volume, "Extrusion", m_delta_node);
      //
      deltaRest = geom_utils::BooleanCut(deltaRest, volume, m_fFuzzy);
    }

    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Cut out extrusions")

    // Create Volume Node for the rest of material
    Handle(geom_volume_node)
      nonDecomposed_n = engine_volume::Create_Volume(deltaRest, "Non-decomposed", m_delta_node);
    //
    for ( TopExp_Explorer exp(deltaRest, TopAbs_SOLID); exp.More(); exp.Next() )
    {
      // Create Volume Node for the rest of material
      engine_volume::Create_Volume(exp.Current(), "Primitive", nonDecomposed_n);
    }
  }
  common_facilities::Instance()->Model->CommitCommand();

  /* ==============
   *  Finalization
   * ============== */

  // Populate list of features
  common_facilities::Instance()->Model->OpenCommand();
  {
    m_part_node->SetFeatures2Keep( engine_volume::Get_LeafVolumesExcept(m_part_node) );
    m_part_node->SetFeatures2Exclude( NULL );
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Update viewer
  Handle(visu_geom_prs)
    NPrs = Handle(visu_geom_prs)::DownCast(
             common_facilities::Instance()->Prs.Part->GetPresentation( common_facilities::Instance()->Model->PartNode() )
           );
  if ( NPrs.IsNull() )
  {
    std::cout << "Error: there is no available presentation for part" << std::endl;
    return;
  }
  //
  NPrs->MainActor()->GetProperty()->SetOpacity(0.5);

  // Populate feature browser
  m_widgets.wDecompBrowser->Populate();
}

//-----------------------------------------------------------------------------

//! Performs classification of features.
void gui_controls_morphology::onClassifyFeatures()
{
  if ( m_part_node.IsNull() || !m_part_node->IsWellFormed() )
  {
    std::cout << "Error: no part available" << std::endl;
    return;
  }

  TopoDS_Solid partSolid;
  TopoDS_Shape part = m_part_node->GetShape();
  //
  if ( part.ShapeType() == TopAbs_SOLID )
    partSolid = TopoDS::Solid(part);
  else
  {
    TopExp_Explorer pexp(part, TopAbs_SOLID);
    if ( !pexp.More() )
    {
      std::cout << "Error: part is not a solid" << std::endl;
      return;
    }

    partSolid = TopoDS::Solid( pexp.Current() );
  }

  // Get all volumetric primitives representing the features
  Handle(ActAPI_HDataCursorList) features = engine_volume::Get_LeafVolumesExcept(m_part_node);
  if ( features.IsNull() || features->IsEmpty() )
  {
    std::cout << "Error: no features available" << std::endl;
    return;
  }

  /* =============================
   *  Actualize in viewer [BEGIN]
   * ============================= */

  // Take presentation manager
  const vtkSmartPointer<visu_prs_manager>& prs_mgr = common_facilities::Instance()->Prs.Part;

  // Clear the scene
  prs_mgr->DeRenderAllPresentations();

  // Configure presentation of a selected feature
  if ( !prs_mgr->IsPresented(m_part_node) )
    prs_mgr->SetPresentation(m_part_node);
  //
  Handle(visu_prs)             feature_prs = prs_mgr->GetPresentation(m_part_node);
  Handle(h_visu_pipeline_list) pipelines   = feature_prs->GetPipelineList();
  //
  for ( int p = 1; p <= pipelines->Length(); ++p )
  {
    pipelines->Value(p)->Actor()->GetProperty()->SetOpacity(0.15);
  }
  //
  prs_mgr->Actualize( m_part_node.get() );

  /* ===========================
   *  Actualize in viewer [END]
   * =========================== */

  // Prepare imperative viewer
  ActAPI_PlotterEntry IV(common_facilities::Instance()->Plotter);

  TIMER_NEW
  TIMER_GO

  // Solid classifier
  geom_classify_point_solid csPointSolid(partSolid, true);

  // For each feature and each face in a feature classify whether this face
  // can be attributed to the original part or not
  for ( int v = 1; v <= features->Length(); ++v )
  {
    Handle(geom_volume_node) volume_n = Handle(geom_volume_node)::DownCast( features->Value(v) );
    if ( volume_n.IsNull() || !volume_n->IsWellFormed() )
      continue;

    TopoDS_Shape volume = volume_n->GetShape();
    if ( volume.IsNull() )
      continue;

    // Loop over the feature faces
    for ( TopExp_Explorer fexp(volume, TopAbs_FACE); fexp.More(); fexp.Next() )
    {
      const TopoDS_Face& face = TopoDS::Face( fexp.Current() );

      // Get midpoint of the face
      Handle(Geom_Surface) S = BRep_Tool::Surface(face);
      double uMin, uMax, vMin, vMax;
      BRepTools::UVBounds(face, uMin, uMax, vMin, vMax);
      //
      gp_Pnt P;
      S->D0( (uMin + uMax)*0.5, (vMin + vMax)*0.5, P );

      IV.DRAW_POINT(P.XYZ(), Quantity_Color(Quantity_NOC_YELLOW), "midpt");

      // Classify this point with respect to part solid
      geom_membership mem = csPointSolid(P, NULL);

      if ( mem == Membership_In )
      {
        std::cout << "Classification result: IN" << std::endl;

        IV.DRAW_SHAPE(face, Quantity_Color(Quantity_NOC_BLUE1), 0.5);
      }
      else if ( mem == Membership_On )
      {
        std::cout << "Classification result: ON" << std::endl;

        IV.DRAW_SHAPE(face, Quantity_Color(Quantity_NOC_BLUE1), 0.5);
      }
      else if ( mem == Membership_Out )
      {
        std::cout << "Classification result: OUT" << std::endl;

        //IV.DRAW_SHAPE(face, Quantity_Color(Quantity_NOC_RED), 0.5);
      }
    }
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("PMC")

}
