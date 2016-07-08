//-----------------------------------------------------------------------------
// Created on: 03 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_controls_dmu.h>

// A-Situs (GUI) includes
#include <gui_common.h>
#include <gui_dialog_mesh.h>
#include <gui_dialog_sewing.h>

// A-Situs (XDE) includes
#include <xde_IGES.h>
#include <xde_shape_id.h>
#include <xde_STEP.h>

// Feature includes
#include <feature_delete_faces.h>
#include <feature_detect_choles.h>

// A-Situs (geometry) includes
#include <geom_utils.h>

// A-Situs (mesh) includes
#include <tess_obj.h>

// A-Situs (visualization) includes
#include <visu_topo_graph.h>

// DF Browser includes
#include <DFBrowser.hxx>

// OCCT includes
#include <AIS_ConnectedInteractive.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <BRep_Builder.hxx>
#include <OSD_Environment.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <TDF_Tool.hxx>
#include <XCAFDoc_ColorTool.hxx>

// Qt includes
#include <QGroupBox>
#include <QSpacerItem>

//-----------------------------------------------------------------------------

#define BTN_MIN_WIDTH 180

//-----------------------------------------------------------------------------

#define FEATURE_HOLE_RADIUS 20.0

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
gui_controls_dmu::gui_controls_dmu(QWidget* parent) : QWidget(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Get directory containing resources
  TCollection_AsciiString ico_dir = OSD_Environment("AS_RESOURCES").Value();
  ico_dir += "/";

  // Button for geometric stitching
  m_widgets.pSewing = new QToolButton();
  m_widgets.pSewing->setIcon( QIcon( AsciiStr2QStr(ico_dir) + "toolbar_geom_sewing.png" ) );
  m_widgets.pSewing->setIconSize( QSize(32, 32) );
  m_widgets.pSewing->setToolTip("Stitch geometric faces");
  m_widgets.pSewing->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );

  // Button for maximizing faces
  m_widgets.pMergeFaces = new QToolButton();
  m_widgets.pMergeFaces->setIcon( QIcon( AsciiStr2QStr(ico_dir) + "toolbar_geom_maximize.png" ) );
  m_widgets.pMergeFaces->setIconSize( QSize(32, 32) );
  m_widgets.pMergeFaces->setToolTip("Maximize geometric faces");
  m_widgets.pMergeFaces->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );

  // Button for holes identification
  m_widgets.pIdentifyHoles = new QToolButton();
  m_widgets.pIdentifyHoles->setIcon( QIcon( AsciiStr2QStr(ico_dir) + "toolbar_geom_identify_holes.png" ) );
  m_widgets.pIdentifyHoles->setIconSize( QSize(32, 32) );
  m_widgets.pIdentifyHoles->setToolTip("Identify holes");
  m_widgets.pIdentifyHoles->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );

  // Button for holes suppression
  m_widgets.pRemoveHoles = new QToolButton();
  m_widgets.pRemoveHoles->setIcon( QIcon( AsciiStr2QStr(ico_dir) + "toolbar_geom_remove_holes.png" ) );
  m_widgets.pRemoveHoles->setIconSize( QSize(32, 32) );
  m_widgets.pRemoveHoles->setToolTip("Remove holes");
  m_widgets.pRemoveHoles->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );

  // Button for mesh generation
  m_widgets.pMesh = new QToolButton();
  m_widgets.pMesh->setIcon( QIcon( AsciiStr2QStr(ico_dir) + "toolbar_tess_gen.png" ) );
  m_widgets.pMesh->setIconSize( QSize(32, 32) );
  m_widgets.pMesh->setToolTip("Generate mesh");
  m_widgets.pMesh->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );

  // Button to show topological graph
  m_widgets.pShowTopoGraph = new QToolButton();
  m_widgets.pShowTopoGraph->setIcon( QIcon( AsciiStr2QStr(ico_dir) + "toolbar_show_topo_graph.png" ) );
  m_widgets.pShowTopoGraph->setIconSize( QSize(32, 32) );
  m_widgets.pShowTopoGraph->setToolTip("Show topology graph");
  m_widgets.pShowTopoGraph->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );

  // Button to show AAG
  m_widgets.pShowAAG = new QToolButton();
  m_widgets.pShowAAG->setIcon( QIcon( AsciiStr2QStr(ico_dir) + "toolbar_show_aag.png" ) );
  m_widgets.pShowAAG->setIconSize( QSize(32, 32) );
  m_widgets.pShowAAG->setToolTip("Show adjacency graph");
  m_widgets.pShowAAG->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );

  // Button to show OCAF
  m_widgets.pShowOCAF = new QToolButton();
  m_widgets.pShowOCAF->setIcon( QIcon( AsciiStr2QStr(ico_dir) + "toolbar_show_ocaf.png" ) );
  m_widgets.pShowOCAF->setIconSize( QSize(32, 32) );
  m_widgets.pShowOCAF->setToolTip("Show detailed OCAF");
  m_widgets.pShowOCAF->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );

  // Buttons
  m_widgets.pLoadIGES = new QPushButton("Load IGES");
  m_widgets.pLoadSTEP = new QPushButton("Load STEP");
  m_widgets.pSaveXBF  = new QPushButton("Save to XDE");
  m_widgets.pSaveBRep = new QPushButton("Save to B-Rep");
  m_widgets.pSaveOBJ  = new QPushButton("Save to OBJ");

  // Group box for import
  QGroupBox*   pImportGroup = new QGroupBox("Import");
  QVBoxLayout* pImportLay   = new QVBoxLayout(pImportGroup);
  //
  pImportLay->addWidget(m_widgets.pLoadIGES);
  pImportLay->addWidget(m_widgets.pLoadSTEP);

  // Group box for geometry
  QGroupBox*   pGeometryGroup = new QGroupBox("Geometry");
  QGridLayout* pGeometryLay   = new QGridLayout(pGeometryGroup);
  //
  pGeometryLay->addWidget(m_widgets.pSewing,        0, 0, Qt::AlignLeft);
  pGeometryLay->addWidget(m_widgets.pMergeFaces,    0, 1, Qt::AlignLeft);
  //pGeometryLay->addWidget(m_widgets.pIdentifyHoles, 1, 0);
  //pGeometryLay->addWidget(m_widgets.pRemoveHoles,   1, 1);

  // Group box for mesh
  QGroupBox*   pMeshGroup = new QGroupBox("Mesh");
  QGridLayout* pMeshLay   = new QGridLayout(pMeshGroup);
  //
  pMeshLay->addWidget(m_widgets.pMesh, 0, 0, Qt::AlignLeft);

  // Group box for export
  QGroupBox*   pExportGroup = new QGroupBox("Export");
  QVBoxLayout* pExportLay   = new QVBoxLayout(pExportGroup);
  //
  pExportLay->addWidget(m_widgets.pSaveXBF);
  pExportLay->addWidget(m_widgets.pSaveBRep);
  pExportLay->addWidget(m_widgets.pSaveOBJ);

  // Group box for analysis
  QGroupBox*   pAnalysisGroup = new QGroupBox("Analysis");
  QGridLayout* pAnalysisLay   = new QGridLayout(pAnalysisGroup);
  //
  /*pAnalysisLay->addWidget(m_widgets.pShowTopoGraph, 0, 0, Qt::AlignLeft);
  pAnalysisLay->addWidget(m_widgets.pShowAAG,       0, 1, Qt::AlignLeft);*/
  pAnalysisLay->addWidget(m_widgets.pShowOCAF,      0, 0, Qt::AlignLeft);

  // Set layout
  m_pMainLayout->setSpacing(0);
  //
  m_pMainLayout->addWidget(pImportGroup);
  m_pMainLayout->addWidget(pGeometryGroup);
  m_pMainLayout->addWidget(pMeshGroup);
  m_pMainLayout->addWidget(pExportGroup);
  m_pMainLayout->addWidget(pAnalysisGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pLoadIGES,      SIGNAL( clicked() ), SLOT( onLoadIGES      () ) );
  connect( m_widgets.pLoadSTEP,      SIGNAL( clicked() ), SLOT( onLoadSTEP      () ) );
  connect( m_widgets.pSewing,        SIGNAL( clicked() ), SLOT( onSewing        () ) );
  connect( m_widgets.pMergeFaces,    SIGNAL( clicked() ), SLOT( onMergeFaces    () ) );
  connect( m_widgets.pIdentifyHoles, SIGNAL( clicked() ), SLOT( onIdentifyHoles () ) );
  connect( m_widgets.pRemoveHoles,   SIGNAL( clicked() ), SLOT( onRemoveHoles   () ) );
  connect( m_widgets.pMesh,          SIGNAL( clicked() ), SLOT( onMesh          () ) );
  connect( m_widgets.pSaveXBF,       SIGNAL( clicked() ), SLOT( onSaveXBF       () ) );
  connect( m_widgets.pSaveBRep,      SIGNAL( clicked() ), SLOT( onSaveBRep      () ) );
  connect( m_widgets.pSaveOBJ,       SIGNAL( clicked() ), SLOT( onSaveOBJ       () ) );
  connect( m_widgets.pShowTopoGraph, SIGNAL( clicked() ), SLOT( onShowTopoGraph () ) );
  connect( m_widgets.pShowAAG,       SIGNAL( clicked() ), SLOT( onShowAAG       () ) );
  connect( m_widgets.pShowOCAF,      SIGNAL( clicked() ), SLOT( onShowOCAF      () ) );
}

//! Destructor.
gui_controls_dmu::~gui_controls_dmu()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------
// Slots
//-----------------------------------------------------------------------------

//! On sewing (stitching of geometric faces).
void gui_controls_dmu::onSewing()
{
  // Run dialog for sewing properties
  gui_dialog_sewing* wSewing = new gui_dialog_sewing(this);
  wSewing->show();
}

//-----------------------------------------------------------------------------

//! On maximization of geometric faces.
void gui_controls_dmu::onMergeFaces()
{
  //---------------------------------------------------------------------------
  // Merging
  //---------------------------------------------------------------------------

  // Get part labels
  TDF_LabelSequence labels;
  common_facilities::Instance()->Model_XDE->GetLabelsOfParts(labels);

  // Get Shape Tool
  Handle(XCAFDoc_ShapeTool) ShapeTool = common_facilities::Instance()->Model_XDE->GetShapeTool();

  // Sew each part
  common_facilities::Instance()->Model_XDE->OpenCommand();
  {
    for ( int l = 1; l <= labels.Length(); ++l )
    {
      TopoDS_Shape shape = ShapeTool->GetShape( labels(l) );

      // Perform merge
      if ( !geom_utils::MaximizeFaces(shape) )
      {
        std::cout << "Error: face merging failed" << std::endl;
      }
      //
      std::cout << "Face merging done. Visualizing..." << std::endl;

      // Set shape back to model
      ShapeTool->SetShape(labels(l), shape);
    }
  }
  common_facilities::Instance()->Model_XDE->CommitCommand();

  //---------------------------------------------------------------------------
  // UI updates
  //---------------------------------------------------------------------------

  // Update shape
  common_facilities::Instance()->ViewerDMU->GetContext()->EraseAll();
  common_facilities::Instance()->ViewerDMU->Visualize(common_facilities::Instance()->Model_XDE, AIS_WireFrame);
}

//-----------------------------------------------------------------------------

//! On holes identification.
void gui_controls_dmu::onIdentifyHoles()
{
  //// Target shape
  //if ( common_facilities::Instance()->aisDMU.IsNull() )
  //{
  //  std::cout << "Error: no working shape" << std::endl;
  //  return;
  //}
  //const TopoDS_Shape& shape = common_facilities::Instance()->aisDMU->Shape();

  //// Identify holes
  //const double R = FEATURE_HOLE_RADIUS;
  //geom_detect_holes detector(shape);
  //if ( !detector.Perform(R) )
  //{
  //  std::cout << "Error: cannot identify holes" << std::endl;
  //  return;
  //}

  //// Get detected holes
  //const NCollection_Sequence<TopoDS_Face>& result = detector.Result();
  //if ( result.IsEmpty() )
  //{
  //  std::cout << "No holes detected with radius not greater than " << R << std::endl;
  //  return;
  //}
  //else
  //  std::cout << result.Length() << " hole(s) detected with radius not greater than " << R << std::endl;

  //// Access presentation
  //Handle(AIS_InteractiveContext) ctx = common_facilities::Instance()->ViewerDMU->GetContext();
  ////
  //Handle(SelectMgr_IndexedMapOfOwner) prsOwners;
  //ctx->EntityOwners(prsOwners, common_facilities::Instance()->aisDMU);
  ////
  //if ( prsOwners.IsNull() || !prsOwners->Extent() )
  //{
  //  std::cout << "Error: no entity owners" << std::endl;
  //  return;
  //}
  //else
  //  std::cout << "Got " << prsOwners->Extent() << " entity owner(s)" << std::endl;

  //// Highlight
  //ctx->ClearSelected();
  ////
  //for ( int i = 1; i <= prsOwners->Extent(); ++i )
  //{
  //  const Handle(SelectMgr_EntityOwner)& owner = prsOwners->FindKey(i);
  //  //
  //  if ( owner->IsKind( STANDARD_TYPE(StdSelect_BRepOwner) ) )
  //  {
  //    Handle(StdSelect_BRepOwner) brepOwner = Handle(StdSelect_BRepOwner)::DownCast(owner);
  //    const TopoDS_Shape& ownedShape = brepOwner->Shape();

  //    // Try to find the hole shape among sensitivities
  //    for ( int h = 1; h <= result.Length(); ++h )
  //    {
  //      if ( ownedShape.IsSame( result(h) ) )
  //        ctx->AddOrRemoveSelected(owner, 0);
  //    }
  //  }
  //}
  //ctx->UpdateCurrentViewer();
}

//-----------------------------------------------------------------------------

//! On holes suppression.
void gui_controls_dmu::onRemoveHoles()
{
  // Get part labels
  TDF_LabelSequence labels;
  common_facilities::Instance()->Model_XDE->GetLabelsOfParts(labels);

  // Get Shape Tool
  Handle(XCAFDoc_ShapeTool) ShapeTool = common_facilities::Instance()->Model_XDE->GetShapeTool();

  // Sew each part
  common_facilities::Instance()->Model_XDE->OpenCommand();
  {
    for ( int l = 1; l <= labels.Length(); ++l )
    {
      TopoDS_Shape shape = ShapeTool->GetShape( labels(l) );

      // Identify holes
      const double R = FEATURE_HOLE_RADIUS;
      feature_detect_choles detector(shape, R, NULL,
                                     common_facilities::Instance()->Notifier,
                                     common_facilities::Instance()->Plotter);
      if ( !detector.Perform() )
      {
        std::cout << "Error: cannot identify holes" << std::endl;
        return;
      }

      // Get detected holes
      const TopTools_IndexedMapOfShape& holes = detector.GetResultFaces();
      if ( holes.IsEmpty() )
      {
        std::cout << "No holes detected with radius not greater than " << R << std::endl;
        return;
      }
      else
        std::cout << holes.Extent() << " hole(s) detected with radius not greater than " << R << std::endl;

      // Remove holes
      feature_delete_faces eraser(shape);
      if ( !eraser.Perform(holes, false) )
      {
        std::cout << "Error: cannot remove holes" << std::endl;
        return;
      }
      TopoDS_Shape result = eraser.GetResult();

      // Set shape back to model
      ShapeTool->SetShape(labels(l), result);
    }
  }
  common_facilities::Instance()->Model_XDE->CommitCommand();

  //---------------------------------------------------------------------------
  // UI updates
  //---------------------------------------------------------------------------

  // Update shape
  common_facilities::Instance()->ViewerDMU->GetContext()->EraseAll();
  common_facilities::Instance()->ViewerDMU->Visualize(common_facilities::Instance()->Model_XDE, AIS_WireFrame);
}

//-----------------------------------------------------------------------------

//! On IGES loading.
void gui_controls_dmu::onLoadIGES()
{
  QString filename = gui_common::selectIGESFile(gui_common::OpenSaveAction_Open);
  //
  if ( filename.isEmpty() )
    return;

  // Read IGES to XDE model
  common_cad_properties props;
  if ( !xde_IGES::Read(QStr2AsciiStr(filename), common_facilities::Instance()->Model_XDE, props) )
  {
    std::cout << "Error: cannot read IGES file" << std::endl;
    return;
  }

  std::cout << "IGES file was imported" << std::endl;

  // Adjust scene and other UI elements
  this->adjustUI();
}

//-----------------------------------------------------------------------------

//! On STEP loading.
void gui_controls_dmu::onLoadSTEP()
{
  QString filename = gui_common::selectSTEPFile(gui_common::OpenSaveAction_Open);
  //
  if ( filename.isEmpty() )
    return;

  // Read STEP to XDE model
  common_cad_properties props;
  if ( !xde_STEP::Read(QStr2AsciiStr(filename), common_facilities::Instance()->Model_XDE, props) )
  {
    std::cout << "Error: cannot read STEP file" << std::endl;
    return;
  }

  std::cout << "STEP file was imported" << std::endl;

  // Adjust scene and other UI elements
  this->adjustUI();
}

//-----------------------------------------------------------------------------

//! On mesh generation.
void gui_controls_dmu::onMesh()
{
  // Run dialog for mesh properties
  gui_dialog_mesh* wMesh = new gui_dialog_mesh(this);
  wMesh->show();
}

//-----------------------------------------------------------------------------

//! On XBF save.
void gui_controls_dmu::onSaveXBF()
{
  QString filename = gui_common::selectXBFFile(gui_common::OpenSaveAction_Save);

  // Save as XDE document
  if ( common_facilities::Instance()->Model_XDE->SaveAs( QStr2AsciiStr(filename) ) )
    std::cout << "XBF saved successfully" << std::endl;
  else
    std::cout << "Error: cannot save XBF" << std::endl;
}

//-----------------------------------------------------------------------------

//! On B-Rep save.
void gui_controls_dmu::onSaveBRep()
{
  QString filename = gui_common::selectBRepFile(gui_common::OpenSaveAction_Save);

  // Working model
  if ( common_facilities::Instance()->Model_XDE.IsNull() )
  {
    std::cout << "Error: no working model" << std::endl;
    return;
  }

  Handle(XCAFDoc_ShapeTool)
    shapeTool = common_facilities::Instance()->Model_XDE->GetShapeTool();

  TopoDS_Compound C;
  BRep_Builder BB;
  BB.MakeCompound(C);

  // Get displayed shapes
  Handle(AIS_InteractiveContext) ctx = common_facilities::Instance()->ViewerDMU->GetContext();
  AIS_ListOfInteractive displayed;
  ctx->DisplayedObjects(displayed);
  //
  for ( AIS_ListIteratorOfListOfInteractive it(displayed); it.More(); it.Next() )
  {
    Handle(xde_shape_id)
      ID = Handle(xde_shape_id)::DownCast( it.Value()->GetOwner() );

    const TCollection_AsciiString& entry = ID->Top2Bottom().Last();

    TDF_Label L;
    TDF_Tool::Label( shapeTool->Label().Data(), entry, L);
    BB.Add( C, shapeTool->GetShape(L) );
  }



  // Save as B-Rep geometry
  if ( geom_utils::WriteBRep( C, QStr2AsciiStr(filename) ) )
    std::cout << "B-Rep saved successfully" << std::endl;
  else
    std::cout << "Error: cannot save B-Rep" << std::endl;
}

//-----------------------------------------------------------------------------

//! On OBJ save.
void gui_controls_dmu::onSaveOBJ()
{
  QString filename = gui_common::selectOBJFile(gui_common::OpenSaveAction_Save);

  // XDE model
  if ( common_facilities::Instance()->Model_XDE.IsNull() )
  {
    std::cout << "Error: no working model" << std::endl;
    return;
  }
  TopoDS_Shape shape = common_facilities::Instance()->Model_XDE->GetOneShape();

  // Save as OBJ geometry
  if ( tess_obj::Write( shape, QStr2AsciiStr(filename) ) )
    std::cout << "OBJ saved successfully" << std::endl;
  else
    std::cout << "Error: cannot save OBJ" << std::endl;
}

//-----------------------------------------------------------------------------

//! On show topology graph.
void gui_controls_dmu::onShowTopoGraph()
{
  if ( common_facilities::Instance()->Model_XDE.IsNull() )
  {
    std::cout << "Error: no active XDE model available" << std::endl;
    return;
  }

  // Target shape
  TopoDS_Shape shape = common_facilities::Instance()->Model_XDE->GetOneShape();

  // Show graph
  visu_topo_graph* pGraphView = new visu_topo_graph;
  pGraphView->RenderFull(shape, TopAbs_FACE);
}

//-----------------------------------------------------------------------------

//! On show AAG.
void gui_controls_dmu::onShowAAG()
{
  //// Target shape
  //if ( common_facilities::Instance()->aisDMU.IsNull() )
  //{
  //  std::cout << "Error: no working shape" << std::endl;
  //  return;
  //}
  //const TopoDS_Shape& shape = common_facilities::Instance()->aisDMU->Shape();

  //// Get selected faces
  //TopTools_ListOfShape selectedFaces;
  ////
  //Handle(AIS_InteractiveContext) ctx = common_facilities::Instance()->ViewerDMU->GetContext();
  ////
  //ctx->InitSelected();
  //while ( ctx->MoreSelected() )
  //{
  //  TopoDS_Shape sh = ctx->SelectedShape();
  //  //
  //  if ( sh.ShapeType() == TopAbs_FACE )
  //  {
  //    selectedFaces.Append(sh);
  //  }
  //  ctx->NextSelected();
  //}

  //// Show graph
  //visu_topo_graph* pGraphView = new visu_topo_graph;
  //pGraphView->RenderAdjacency(shape, selectedFaces);
}

//-----------------------------------------------------------------------------

//! On show OCAF.
void gui_controls_dmu::onShowOCAF()
{
  if ( common_facilities::Instance()->Model_XDE.IsNull() )
  {
    std::cout << "Error: XDE model is not yet initialized" << std::endl;
    return;
  }

  // Show CAF Browser
  DFBrowser::DFBrowserCall( common_facilities::Instance()->Model_XDE->GetDocument() );
}

//-----------------------------------------------------------------------------

//! Adjusts UI after CAD import.
void gui_controls_dmu::adjustUI()
{
  // Clean up viewer
  common_facilities::Instance()->ViewerDMU->Clear();

  // Access default drawer
  Handle(Prs3d_Drawer) drawer = common_facilities::Instance()->ViewerDMU->GetContext()->DefaultDrawer();

  // NOTICE: the following option is very important. We disable meshing
  //         from the very beginning, so a CAD model will be visualized
  //         in wireframe regardless of whether its preferred visualization
  //         mode is shading or not. We proceed like this in order to give
  //         the user full control over the meshing parameters.
  drawer->SetAutoTriangulation(0);

  // Visualize model
  common_facilities::Instance()->ViewerDMU->Visualize(common_facilities::Instance()->Model_XDE, AIS_WireFrame);
  common_facilities::Instance()->ViewerDMU->fitAll();

  // Populate tree view
  common_facilities::Instance()->ObjectBrowser_XDE->Populate();
}
