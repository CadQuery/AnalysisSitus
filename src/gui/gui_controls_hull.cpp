//-----------------------------------------------------------------------------
// Created on: 03 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_controls_hull.h>

// GUI includes
#include <gui_common.h>

// Visualization includes
#include <visu_display_mode.h>
#include <visu_geom_prs.h>
#include <visu_geom_surf_prs.h>

// Geometry includes
#include <geom_point_cloud.h>
#include <geom_STEP.h>
#include <geom_utils.h>

// Common includes
#include <common_draw_test_suite.h>
#include <common_facilities.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BSplCLib.hxx>
#include <Geom_BSplineCurve.hxx>
#include <math_Matrix.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp.hxx>

//-----------------------------------------------------------------------------

#define CONTROL_BTN_WIDTH 150

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
gui_controls_hull::gui_controls_hull(QWidget* parent) : QWidget(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Load points
  m_widgets.pLoadPoints = new QPushButton("Load points");
  m_widgets.pLoadPoints->setMinimumWidth(CONTROL_BTN_WIDTH);
  //
  connect( m_widgets.pLoadPoints, SIGNAL( clicked() ), this, SLOT( onLoadPoints() ) );

  // Interpolate sections
  m_widgets.pInterpSections = new QPushButton("Interp. sections");
  m_widgets.pInterpSections->setMinimumWidth(CONTROL_BTN_WIDTH);
  //
  connect( m_widgets.pInterpSections, SIGNAL( clicked() ), this, SLOT( onInterpSections() ) );

  // Interpolate columns
  m_widgets.pInterpColumns = new QPushButton("Interp. columns");
  m_widgets.pInterpColumns->setMinimumWidth(CONTROL_BTN_WIDTH);
  //
  connect( m_widgets.pInterpColumns, SIGNAL( clicked() ), this, SLOT( onInterpColumns() ) );

  // Skin surface
  m_widgets.pSkinSurface = new QPushButton("Skin surface");
  m_widgets.pSkinSurface->setMinimumWidth(CONTROL_BTN_WIDTH);
  //
  connect( m_widgets.pSkinSurface, SIGNAL( clicked() ), this, SLOT( onSkinSurface() ) );

  // Save geometry to STEP
  m_widgets.pSaveSTEP = new QPushButton("Export to STEP");
  m_widgets.pSaveSTEP->setMinimumWidth(CONTROL_BTN_WIDTH);
  //
  connect( m_widgets.pSaveSTEP, SIGNAL( clicked() ), this, SLOT( onSaveSTEP() ) );

  // Show curvature field
  m_widgets.pCurvature = new QPushButton("Curvature analysis");
  m_widgets.pCurvature->setMinimumWidth(CONTROL_BTN_WIDTH);
  //
  connect( m_widgets.pCurvature, SIGNAL( clicked() ), this, SLOT( onCurvature() ) );

  // Set layout
  m_pMainLayout->addWidget(m_widgets.pLoadPoints);
  m_pMainLayout->addWidget(m_widgets.pInterpSections);
  m_pMainLayout->addWidget(m_widgets.pInterpColumns);
  m_pMainLayout->addWidget(m_widgets.pSkinSurface);
  m_pMainLayout->addWidget(m_widgets.pSaveSTEP);
  m_pMainLayout->addWidget(m_widgets.pCurvature);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  this->setLayout(m_pMainLayout);
}

//! Destructor.
gui_controls_hull::~gui_controls_hull()
{
  delete m_pMainLayout;
  //
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! Load points.
void gui_controls_hull::onLoadPoints()
{
  QString filename = gui_common::selectXYZFile(gui_common::OpenSaveAction_Open);

  // Read point cloud as topological shape. This is completely stupid, but
  // it allows us to reuse standard visualization techniques
  TopoDS_Shape topoCloud;
  if ( !geom_point_cloud::ReadSections(QStr2AsciiStr(filename), topoCloud) )
  {
    std::cout << "Error: cannot read point cloud" << std::endl;
    return;
  }

  common_facilities::Instance()->Model->Clear();

  // Set part geometry
  Handle(geom_part_node) geom_n = common_facilities::Instance()->Model->PartNode();
  //
  common_facilities::Instance()->Model->OpenCommand();
  {
    geom_n->SetShape(topoCloud);
  }
  common_facilities::Instance()->Model->CommitCommand();

  //---------------------------------------------------------------------------
  // Update UI
  //---------------------------------------------------------------------------

  common_facilities::Instance()->Prs.Part->DeleteAllPresentations();
  common_facilities::Instance()->Prs.Part->Actualize(geom_n.get(), false, true);
  //
  if ( common_facilities::Instance()->ObjectBrowser )
    common_facilities::Instance()->ObjectBrowser->Populate();
}

//! Interpolate sections.
void gui_controls_hull::onInterpSections()
{
  /* =============================================
   *  Fill Model-independent collection of points
   * ============================================= */

  // Get point cloud
  Handle(geom_part_node) part = common_facilities::Instance()->Model->PartNode();
  //
  TopoDS_Shape partShape = part->GetShape();

  // Part shape should have COMPOUND type by construction
  if ( partShape.ShapeType() != TopAbs_COMPOUND )
  {
    std::cout << "Error: part should be of COMPOUND type" << std::endl;
    return;
  }

  // Collection of section points
  std::vector< std::vector<gp_Pnt> > sections;

  // Loop over the part compound extracting sub-compounds for section points
  for ( TopoDS_Iterator it(partShape); it.More(); it.Next() )
  {
    const TopoDS_Shape& sectionShape = it.Value();
    if ( sectionShape.ShapeType() != TopAbs_COMPOUND )
    {
      std::cout << "Error: section should be of COMPOUND type" << std::endl;
      return;
    }

    std::vector<gp_Pnt> section;

    // Fill section with points
    for ( TopoDS_Iterator it2(sectionShape); it2.More(); it2.Next() )
    {
      const TopoDS_Shape& pointShape = it2.Value();
      if ( pointShape.ShapeType() != TopAbs_VERTEX )
      {
        std::cout << "Error: point should be of VERTEX type" << std::endl;
        return;
      }

      // Add point to the section
      section.push_back( BRep_Tool::Pnt( TopoDS::Vertex(pointShape) ) );
    }

    // Add section to the collection of sections
    sections.push_back(section);
  }

  /* ===============
   *  Interpolation
   * =============== */

  TopoDS_Compound result;
  BRep_Builder().MakeCompound(result);

  //---------------------------------------------------------------------------
  // Choose degree
  const int p = 3;

  TIMER_NEW
  TIMER_GO

  // Interpolate sections one by one
  for ( size_t section_idx = 0; section_idx < sections.size(); ++section_idx )
  {
    const std::vector<gp_Pnt>& section = sections[section_idx];

    // Interpolate points
    Handle(Geom_BSplineCurve) uIso;
    //
    if ( !geom_utils::InterpolatePoints(section, p, uIso) || uIso.IsNull() )
    {
      std::cout << "Error: interpolation failed" << std::endl;
      return;
    }

    // Create shape
    TopoDS_Edge E = BRepBuilderAPI_MakeEdge(uIso);
    //
    BRep_Builder().Add(result, E);
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Interpolation of sections")

  /* ==============
   *  Finalization
   * ============== */

  common_facilities::Instance()->Model->Clear();

  // Save geometry to data model
  common_facilities::Instance()->Model->OpenCommand();
  {
    common_facilities::Instance()->Model->PartNode()->SetShape(result);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Update presentation
  common_facilities::Instance()->Prs.Part->DeleteAllPresentations();
  vtkSmartPointer<visu_prs_manager> prsMgr = common_facilities::Instance()->Prs.Part;
  prsMgr->Actualize( common_facilities::Instance()->Model->PartNode().get(), false, true );
}

//! Interpolate columns.
void gui_controls_hull::onInterpColumns()
{
  // Get sections edges
  Handle(geom_part_node) part = common_facilities::Instance()->Model->PartNode();
  //
  TopoDS_Shape partShape = part->GetShape();
  //
  if ( partShape.IsNull() )
  {
    std::cout << "Error: shape is NULL" << std::endl;
    return;
  }

  // Extract curves from the edges
  std::vector<Handle(Geom_BSplineCurve)> sections;
  //
  for ( TopoDS_Iterator it(partShape); it.More(); it.Next() )
  {
    if ( it.Value().ShapeType() != TopAbs_EDGE )
    {
      std::cout << "Error: unexpected nested type" << std::endl;
      return;
    }
    
    const TopoDS_Edge& E = TopoDS::Edge( it.Value() );
    //
    double f, l;
    sections.push_back( Handle(Geom_BSplineCurve)::DownCast( BRep_Tool::Curve(E, f, l) ) );
  }
  //
  if ( sections.size() < 2 )
  {
    std::cout << "Error: poor number of sections" << std::endl;
    return;
  }

  /* ===============
   *  Interpolation
   * =============== */

  TopoDS_Compound result;
  BRep_Builder().MakeCompound(result);

  // Choose degree
  const int q = 3;

  // Number of columns to interpolate
  const int nColumns = sections[0]->NbPoles();
  const int m        = nColumns - 1;

  TIMER_NEW
  TIMER_GO

  // Loop over the columns and interpolate each one
  for ( int col = 0; col <= m; ++col )
  {
    std::vector<gp_Pnt> columnPoles;

    // Collect poles of cross-sectional curves in each column
    for ( size_t icurve = 0; icurve < sections.size(); ++icurve )
      columnPoles.push_back( sections[icurve]->Pole(col + 1) ); // Indexation is 1-based in OCCT

    // Interpolate columns
    Handle(Geom_BSplineCurve) vIso;
    //
    if ( !geom_utils::InterpolatePoints(columnPoles, q, vIso) || vIso.IsNull() )
    {
      std::cout << "Error: interpolation failed" << std::endl;
      return;
    }

    // Create shape
    TopoDS_Edge E = BRepBuilderAPI_MakeEdge(vIso);
    //
    BRep_Builder().Add(result, E);
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Interpolation of columns")

  // Add columns shape next to rows
  TopoDS_Compound fullComp;
  BRep_Builder().MakeCompound(fullComp);
  BRep_Builder().Add(fullComp, partShape);
  BRep_Builder().Add(fullComp, result);

  /* ==============
   *  Finalization
   * ============== */

  common_facilities::Instance()->Model->Clear();

  // Save geometry to data model
  common_facilities::Instance()->Model->OpenCommand();
  {
    common_facilities::Instance()->Model->PartNode()->SetShape(fullComp);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Update presentation
  common_facilities::Instance()->Prs.Part->DeleteAllPresentations();
  vtkSmartPointer<visu_prs_manager> prsMgr = common_facilities::Instance()->Prs.Part;
  prsMgr->Actualize( common_facilities::Instance()->Model->PartNode().get(), false, true );
}

//! Skin surface.
void gui_controls_hull::onSkinSurface()
{
  // Get sections edges
  Handle(geom_part_node) part = common_facilities::Instance()->Model->PartNode();
  //
  TopoDS_Shape partShape = part->GetShape();
  //
  if ( partShape.IsNull() )
  {
    std::cout << "Error: shape is NULL" << std::endl;
    return;
  }

  // Get compounds for rows and columns
  TopoDS_Iterator topIt(partShape);
  TopoDS_Shape rowsComp = topIt.Value();
  topIt.Next();
  TopoDS_Shape colsComp = topIt.Value();

  // Extract rows
  std::vector<Handle(Geom_BSplineCurve)> rows;
  //
  for ( TopoDS_Iterator it(rowsComp); it.More(); it.Next() )
  {
    if ( it.Value().ShapeType() != TopAbs_EDGE )
    {
      std::cout << "Error: unexpected nested type" << std::endl;
      return;
    }

    const TopoDS_Edge& E = TopoDS::Edge( it.Value() );
    //
    double f, l;
    rows.push_back( Handle(Geom_BSplineCurve)::DownCast( BRep_Tool::Curve(E, f, l) ) );
  }

  // Extract columns
  std::vector<Handle(Geom_BSplineCurve)> cols;
  //
  for ( TopoDS_Iterator it(colsComp); it.More(); it.Next() )
  {
    if ( it.Value().ShapeType() != TopAbs_EDGE )
    {
      std::cout << "Error: unexpected nested type" << std::endl;
      return;
    }

    const TopoDS_Edge& E = TopoDS::Edge( it.Value() );
    //
    double f, l;
    cols.push_back( Handle(Geom_BSplineCurve)::DownCast( BRep_Tool::Curve(E, f, l) ) );
  }

  /* =====================================
   *  Build surface from rows and columns
   * ===================================== */

  const int nPoles_U = cols[0]->NbPoles();
  const int nPoles_V = rows[0]->NbPoles();

  // Populate poles
  TColgp_Array2OfPnt Poles(1, nPoles_U, 1, nPoles_V);
  for ( int uIdx = 1; uIdx <= nPoles_U; ++uIdx )
  {
    for ( int vIdx = 1; vIdx <= nPoles_V; ++vIdx )
    {
      Poles(uIdx, vIdx) = cols[vIdx - 1]->Pole(uIdx);
    }
  }

  // Populate knots
  const TColStd_Array1OfReal&    UKnots = cols[0]->Knots();
  const TColStd_Array1OfReal&    VKnots = rows[0]->Knots();
  const TColStd_Array1OfInteger& UMults = cols[0]->Multiplicities();
  const TColStd_Array1OfInteger& VMults = rows[0]->Multiplicities();
  //
  const int UDegree = cols[0]->Degree();
  const int VDegree = rows[0]->Degree();

  // Construct surface
  Handle(Geom_BSplineSurface)
    surf = new Geom_BSplineSurface(Poles, UKnots, VKnots, UMults, VMults, UDegree, VDegree);

  // Build topology
  TopoDS_Face F = BRepBuilderAPI_MakeFace( surf, Precision::Confusion() );

  /* ==============
   *  Finalization
   * ============== */

  common_facilities::Instance()->Model->Clear();

  // Save geometry to data model
  common_facilities::Instance()->Model->OpenCommand();
  {
    common_facilities::Instance()->Model->PartNode()->SetShape(F);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Update presentation
  common_facilities::Instance()->Prs.Part->DeleteAllPresentations();
  vtkSmartPointer<visu_prs_manager> prsMgr = common_facilities::Instance()->Prs.Part;
  prsMgr->Actualize( common_facilities::Instance()->Model->PartNode().get(), false, true );
}

//! Save geometry to STEP.
void gui_controls_hull::onSaveSTEP()
{
  QString filename = gui_common::selectSTEPFile(gui_common::OpenSaveAction_Save);

  // Access Part Node
  Handle(geom_part_node) part = common_facilities::Instance()->Model->PartNode();
  //
  TopoDS_Shape partShape = part->GetShape();

  // Save STEP
  if ( !geom_STEP::Write( partShape, QStr2AsciiStr(filename) ) )
  {
    std::cout << "Error: cannot save geometry to STEP file" << std::endl;
    return;
  }
}

//! Shows curvature view.
void gui_controls_hull::onCurvature()
{
  // Clean viewer
  common_facilities::Instance()->Prs.Part->DeleteAllPresentations();

  // Get Part Node with the underlying Surface Node
  Handle(geom_part_node) part_n = common_facilities::Instance()->Model->PartNode();
  Handle(geom_surf_node) surf_n = part_n->SurfaceRepresentation();
  Handle(geom_face_node) face_n = part_n->FaceRepresentation();

  // Check shape type
  if ( part_n->GetShape().ShapeType() != TopAbs_FACE )
  {
    std::cout << "Error: shape is not a face" << std::endl;
    return;
  }

  // Get face corresponding to the reconstructed surface
  TopoDS_Face F = TopoDS::Face( part_n->GetShape() );
  if ( F.IsNull() )
  {
    std::cout << "Error: no available face" << std::endl;
    return;
  }

  // Find index of the face
  TopTools_IndexedMapOfShape M;
  TopExp::MapShapes(F, M);
  //
  common_facilities::Instance()->Model->OpenCommand();
  {
    const int face_idx = M.FindIndex(F);
    surf_n->SetSelectedFace(face_idx);
    face_n->SetSelectedFace(face_idx);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Actualize
  vtkSmartPointer<visu_prs_manager> prsMgr = common_facilities::Instance()->Prs.Part;
  //
  prsMgr->Actualize( surf_n.get(), false, true );
}
