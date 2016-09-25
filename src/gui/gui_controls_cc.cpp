//-----------------------------------------------------------------------------
// Created on: 16 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_controls_cc.h>

// Common includes
#include <common_draw_test_suite.h>
#include <common_facilities.h>

// Geometry includes
#include <geom_bvh_facets.h>
#include <geom_bvh_iterator.h>
#include <geom_hit_facet.h>
#include <geom_utils.h>

// GUI includes
#include <gui_common.h>

// Engine includes
#include <engine_part.h>

// Visualization includes
#include <visu_create_contour_callback.h>
#include <visu_geom_prs.h>
#include <visu_pick_facet_callback.h>

// VTK includes
#include <vtkActor.h>
#include <vtkProperty.h>

// Qt includes
#include <QGroupBox>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

// SPE includes
#include <CC_ClassifyPointFace.h>

//-----------------------------------------------------------------------------

#define BTN_MIN_WIDTH 120

//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
gui_controls_cc::gui_controls_cc(QWidget* parent) : QWidget(parent), m_iPrevSelMode(0)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_widgets.pPickContour         = new QPushButton("Pick contour");
  m_widgets.pLoadContour         = new QPushButton("Import contour");
  m_widgets.pCheckVertexDistance = new QPushButton("Check distance at poles");
  m_widgets.pProjectVertices     = new QPushButton("Project poles to body");
  m_widgets.pBVH_SAH             = new QPushButton("BVH [SAH]");
  m_widgets.pBVH_Linear          = new QPushButton("BVH [linear]");
  m_widgets.pPickFacet           = new QPushButton("Pick facet");
  //
  m_widgets.pPickContour         -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pLoadContour         -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pCheckVertexDistance -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pProjectVertices     -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pBVH_SAH             -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pBVH_Linear          -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pPickFacet           -> setMinimumWidth(BTN_MIN_WIDTH);

  // Other configurations
  m_widgets.pPickContour ->setCheckable(true);
  m_widgets.pPickFacet   ->setCheckable(true);

  // Group of buttons for contour definition
  QGroupBox*   pContourGroup = new QGroupBox("Define contour");
  QVBoxLayout* pContourLay   = new QVBoxLayout(pContourGroup);
  //
  pContourLay->addWidget(m_widgets.pPickContour);
  pContourLay->addWidget(m_widgets.pLoadContour);

  // Group of buttons for contour validation
  QGroupBox*   pValidateGroup = new QGroupBox("Contour healing");
  QVBoxLayout* pValidateLay   = new QVBoxLayout(pValidateGroup);
  //
  pValidateLay->addWidget(m_widgets.pCheckVertexDistance);
  pValidateLay->addWidget(m_widgets.pProjectVertices);

  // Group of buttons for additional tests
  QGroupBox*   pAddendumGroup = new QGroupBox("Additional");
  QVBoxLayout* pAddendumLay   = new QVBoxLayout(pAddendumGroup);
  //
  pAddendumLay->addWidget(m_widgets.pPickFacet);
  pAddendumLay->addWidget(m_widgets.pBVH_SAH);
  pAddendumLay->addWidget(m_widgets.pBVH_Linear);

  // Set layout
  m_pMainLayout->addWidget(pContourGroup);
  m_pMainLayout->addWidget(pValidateGroup);
  m_pMainLayout->addWidget(pAddendumGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pPickContour,         SIGNAL( clicked() ), SLOT( onPickContour         () ) );
  connect( m_widgets.pLoadContour,         SIGNAL( clicked() ), SLOT( onLoadContour         () ) );
  connect( m_widgets.pCheckVertexDistance, SIGNAL( clicked() ), SLOT( onCheckVertexDistance () ) );
  connect( m_widgets.pProjectVertices,     SIGNAL( clicked() ), SLOT( onProjectVertices     () ) );
  connect( m_widgets.pBVH_SAH,             SIGNAL( clicked() ), SLOT( onBVH_SAH             () ) );
  connect( m_widgets.pBVH_Linear,          SIGNAL( clicked() ), SLOT( onBVH_Linear          () ) );
  connect( m_widgets.pPickFacet,           SIGNAL( clicked() ), SLOT( onPickFacet           () ) );
}

//-----------------------------------------------------------------------------

//! Destructor.
gui_controls_cc::~gui_controls_cc()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! Allows user to pick a contour interactively.
void gui_controls_cc::onPickContour()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  const bool isOn = m_widgets.pPickContour->isChecked();

  // Create the accelerating structure
  if ( m_bvh.IsNull() || part != m_bvh->GetShape() )
    m_bvh = new geom_bvh_facets(part,
                                geom_bvh_facets::Builder_Binned,
                                common_facilities::Instance()->Notifier,
                                common_facilities::Instance()->Plotter);

  // Get contour Node
  Handle(geom_contour_node)
    contour_n = common_facilities::Instance()->Model->GetPartNode()->GetContour();

  // Depending on the state of the control, either let user to pick some
  // points on the shape or finalize contour creation
  if ( isOn )
  {
    // Clear contour
    common_facilities::Instance()->Model->OpenCommand();
    {
      contour_n->Init();
    }
    common_facilities::Instance()->Model->CommitCommand();

    // Enable an appropriate selection mode
    m_iPrevSelMode = common_facilities::Instance()->Prs.Part->GetSelectionMode();
    common_facilities::Instance()->Prs.Part->SetSelectionMode(SelectionMode_Workpiece);

    // Add observer which takes responsibility to fill the data object with
    // the base points of the contour
    if ( !common_facilities::Instance()->Prs.Part->HasObserver(EVENT_PICK_WORLD_POINT) )
    {
      vtkSmartPointer<visu_create_contour_callback>
        cb = vtkSmartPointer<visu_create_contour_callback>::New();
      //
      cb->SetBVH(m_bvh);

      // Add observer
      common_facilities::Instance()->Prs.Part->AddObserver(EVENT_PICK_WORLD_POINT, cb);
    }
  }
  else
  {
    if ( !contour_n->IsClosed() )
    {
      common_facilities::Instance()->Model->OpenCommand();
      {
        contour_n->SetClosed(true);
      }
      common_facilities::Instance()->Model->CommitCommand();
    }

    common_facilities::Instance()->Prs.Part->Actualize( contour_n.get() );

    // Restore original selection mode
    common_facilities::Instance()->Prs.Part->SetSelectionMode(m_iPrevSelMode);
    //
    common_facilities::Instance()->Prs.Part->RemoveObserver(EVENT_PICK_WORLD_POINT);
  }
}

//-----------------------------------------------------------------------------

//! Allows user to load an externally defined contour.
void gui_controls_cc::onLoadContour()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  QString filename = gui_common::selectBRepFile(gui_common::OpenSaveAction_Open);

  // Read BREP
  TopoDS_Shape shape;
  if ( !geom_utils::ReadBRep(QStr2AsciiStr(filename), shape) )
  {
    std::cout << "Error: cannot read b-rep file" << std::endl;
    return;
  }

  if ( shape.ShapeType() != TopAbs_WIRE )
  {
    std::cout << "Error: loaded shape is not a WIRE" << std::endl;
    return;
  }

  // Create the accelerating structure
  if ( m_bvh.IsNull() || part != m_bvh->GetShape() )
    m_bvh = new geom_bvh_facets(part,
                                geom_bvh_facets::Builder_Binned,
                                common_facilities::Instance()->Notifier,
                                common_facilities::Instance()->Plotter);

  // Tool to find the owner face
  geom_hit_facet HitFacet(m_bvh,
                          common_facilities::Instance()->Notifier,
                          common_facilities::Instance()->Plotter);

  // Set geometry of contour
  Handle(geom_contour_node)
    contour_n = common_facilities::Instance()->Model->GetPartNode()->GetContour();
  //
  common_facilities::Instance()->Model->OpenCommand(); // tx start
  {
    // Clear contour
    contour_n->Init();

    // Iterate over the vertices to compose a contour data structure
    TopTools_IndexedMapOfShape traversed;
    int vidx = 1;
    //
    for ( TopExp_Explorer exp(shape, TopAbs_VERTEX); exp.More(); exp.Next(), ++vidx )
    {
      TopoDS_Vertex V = TopoDS::Vertex( exp.Current() );
      //
      if ( traversed.Contains(V) )
        continue;

      // Access the probe point
      traversed.Add(V);
      gp_Pnt P = BRep_Tool::Pnt(V);

      // Recover host face
      int facet_idx = -1;
      if ( !HitFacet(P, 1e-1, facet_idx) )
      {
        std::cout << "Cannot locate a host facet for a contour's point" << std::endl;
        common_facilities::Instance()->Plotter->DRAW_POINT(P, Color_Red, "Failure");
        /*common_facilities::Instance()->Model->AbortCommand();
        return;*/
        continue;
      }

      // Add point to the contour
      contour_n->AddPoint( P.XYZ(), m_bvh->GetFacet(facet_idx).FaceIndex );
    }
    contour_n->SetClosed(true);
  }
  common_facilities::Instance()->Model->CommitCommand(); // tx commit

  // Actualize
  common_facilities::Instance()->Prs.Part->DeletePresentation( contour_n.get() );
  common_facilities::Instance()->Prs.Part->Actualize( contour_n.get() );
}

//-----------------------------------------------------------------------------

//! Checks distance from the contour vertices to the host faces.
void gui_controls_cc::onCheckVertexDistance()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Get contour Node
  Handle(geom_contour_node)
    contour_n = common_facilities::Instance()->Model->GetPartNode()->GetContour();
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
  {
    std::cout << "Error: contour is not defined" << std::endl;
    return;
  }

  // Get all faces
  TopTools_IndexedMapOfShape faces;
  TopExp::MapShapes(part, TopAbs_FACE, faces);

  double maxGap = 0.0;

  // Get all points of the contour
  TColgp_SequenceOfPnt points;
  TColStd_SequenceOfInteger faceIndices;
  //
  contour_n->AsPointsOnFaces(points, faceIndices);
  //
  TColgp_SequenceOfPnt::Iterator pit(points);
  TColStd_SequenceOfInteger::Iterator fit(faceIndices);
  //
  for ( ; pit.More(); pit.Next(), fit.Next() )
  {
    const gp_Pnt& point    = pit.Value();
    const int     face_idx = fit.Value();

    // Get face
    const TopoDS_Face& face = TopoDS::Face( faces(face_idx) );

    // Point-face classifier
    gp_Pnt2d UV;
    double gap;
    //
    CC_ClassifyPointFace ClassPtFace( face, BRep_Tool::Tolerance(face), Precision::Confusion() );
    //
    ClassPtFace(point, false, UV, gap, NULL);

    if ( gap > maxGap )
      maxGap = gap;
  }

  std::cout << "Max gap is " << maxGap << std::endl;
}

//-----------------------------------------------------------------------------

//! Projects vertices of the contour to the part.
void gui_controls_cc::onProjectVertices()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Get contour Node
  Handle(geom_contour_node)
    contour_n = common_facilities::Instance()->Model->GetPartNode()->GetContour();
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
  {
    std::cout << "Error: contour is not defined" << std::endl;
    return;
  }

  // Get all faces
  TopTools_IndexedMapOfShape faces;
  TopExp::MapShapes(part, TopAbs_FACE, faces);

  // Get all points of the contour
  TColgp_SequenceOfPnt points;
  TColStd_SequenceOfInteger faceIndices;
  //
  contour_n->AsPointsOnFaces(points, faceIndices);
  //
  TColgp_SequenceOfPnt::Iterator pit(points);
  TColStd_SequenceOfInteger::Iterator fit(faceIndices);

  // Perform projection point-by-point
  common_facilities::Instance()->Model->OpenCommand();
  {
    int pointIndex = 0;
    for ( ; pit.More(); pit.Next(), fit.Next(), ++pointIndex )
    {
      const gp_Pnt& point    = pit.Value();
      const int     face_idx = fit.Value();

      // Get face
      const TopoDS_Face& face = TopoDS::Face( faces(face_idx) );

      // Point-face classifier
      gp_Pnt2d UV;
      double gap;
      //
      CC_ClassifyPointFace ClassPtFace( face, BRep_Tool::Tolerance(face), Precision::Confusion() );
      //
      ClassPtFace(point, false, UV, gap, NULL);

      // Reconstruct a point from UV
      gp_Pnt newPole = BRep_Tool::Surface(face)->Value( UV.X(), UV.Y() );
      //
      contour_n->ReplacePoint(pointIndex, newPole);
    }
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Actualize
  common_facilities::Instance()->Prs.Part->DeletePresentation( contour_n.get() );
  common_facilities::Instance()->Prs.Part->Actualize( contour_n.get() );
}

//-----------------------------------------------------------------------------

//! Constructs BVH with SAH builder.
void gui_controls_cc::onBVH_SAH()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Create the accelerating structure
  m_bvh = new geom_bvh_facets(part,
                              geom_bvh_facets::Builder_Binned,
                              common_facilities::Instance()->Notifier,
                              common_facilities::Instance()->Plotter);

  // Draw
  common_facilities::Instance()->Plotter->CLEAN();
  m_bvh->Dump(common_facilities::Instance()->Plotter);
}

//-----------------------------------------------------------------------------

//! Constructs BVH with linear builder.
void gui_controls_cc::onBVH_Linear()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Create the accelerating structure
  m_bvh = new geom_bvh_facets(part,
                              geom_bvh_facets::Builder_Linear,
                              common_facilities::Instance()->Notifier,
                              common_facilities::Instance()->Plotter);

  // Draw
  common_facilities::Instance()->Plotter->CLEAN();
  m_bvh->Dump(common_facilities::Instance()->Plotter);
}

//-----------------------------------------------------------------------------

//! Allows user to pick a single facet.
void gui_controls_cc::onPickFacet()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Create the accelerating structure
  if ( m_bvh.IsNull() || part != m_bvh->GetShape() )
    m_bvh = new geom_bvh_facets(part,
                                geom_bvh_facets::Builder_Binned,
                                common_facilities::Instance()->Notifier,
                                common_facilities::Instance()->Plotter);

  const bool isOn = m_widgets.pPickFacet->isChecked();

  // Depending on the state of the control, either let user to pick a
  // point on the shape or finalize picking
  if ( isOn )
  {
    // Enable an appropriate selection mode
    m_iPrevSelMode = common_facilities::Instance()->Prs.Part->GetSelectionMode();
    common_facilities::Instance()->Prs.Part->SetSelectionMode(SelectionMode_Workpiece);

    // Add observer which takes responsibility to interact with user
    if ( !common_facilities::Instance()->Prs.Part->HasObserver(EVENT_PICK_WORLD_POINT) )
    {
      vtkSmartPointer<visu_pick_facet_callback>
        cb = vtkSmartPointer<visu_pick_facet_callback>::New();
      //
      cb->SetBVH(m_bvh);

      // Add observer
      common_facilities::Instance()->Prs.Part->AddObserver(EVENT_PICK_WORLD_POINT, cb);
    }
  }
  else
  {
    // Restore original selection mode
    common_facilities::Instance()->Prs.Part->SetSelectionMode(m_iPrevSelMode);
    //
    common_facilities::Instance()->Prs.Part->RemoveObserver(EVENT_PICK_WORLD_POINT);
  }
}
