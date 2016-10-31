//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_viewer_domain.h>

// Common includes
#include <common_draw_test_suite.h>
#include <common_facilities.h>

// Geometry includes
#include <asiAlgo_DeleteEdges.h>
#include <asiAlgo_JoinEdges.h>
#include <asiAlgo_Utils.h>

// Engine includes
#include <asiEngine_Domain.h>

// Visualization includes
#include <visu_utils.h>

// VTK includes
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTextRepresentation.h>

// Qt-VTK includes
#include <QVTKWidget.h>

// Qt includes
#pragma warning(push, 0)
#include <QVBoxLayout>
#pragma warning(pop)

// OCCT includes
#include <BRep_Tool.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

//! Creates a new instance of viewer.
//! \param parent [in] parent widget.
gui_viewer_domain::gui_viewer_domain(QWidget* parent) : gui_viewer(parent)
{
  // Store in common facilities
  common_facilities::Instance()->ViewerDomain = this;

  // Initialize Presentation Manager along with QVTK widget
  common_facilities::Instance()->Prs.Domain = vtkSmartPointer<visu_prs_manager>::New();
  m_prs_mgr = common_facilities::Instance()->Prs.Domain;
  //
  m_prs_mgr->Initialize(this);
  m_prs_mgr->SetInteractionMode(visu_prs_manager::InteractionMode_2D);
  m_prs_mgr->SetSelectionMode(SelectionMode_Workpiece);
  m_prs_mgr->GetCellPicker()->SetTolerance(0.005);

  // Widgets and layouts
  QVTKWidget*  pViewer     = m_prs_mgr->GetQVTKWidget();
  QHBoxLayout* pBaseLayout = new QHBoxLayout();

  // Configure layout
  pBaseLayout->setSpacing(0);
  pBaseLayout->addWidget(pViewer);
  pBaseLayout->setAlignment(Qt::AlignTop);
  pBaseLayout->setContentsMargins(0, 0, 0, 0);

  // Set central widget
  this->setLayout(pBaseLayout);

  /* ===================================
   *  Setting up picking infrastructure
   * =================================== */

  // Initialize Callback instance for Pick operation
  m_pickCallback = vtkSmartPointer<visu_pick_callback>::New();
  m_pickCallback->SetViewer(this);

  // Initialize Callback instance for Domain operations
  m_domainCallback = vtkSmartPointer<visu_pdomain_callback>::New();
  m_domainCallback->SetViewer(this);

  // Set observer for picking
  if ( !m_prs_mgr->GetImageInteractorStyle()->HasObserver(EVENT_PICK_DEFAULT) )
    m_prs_mgr->GetImageInteractorStyle()->AddObserver(EVENT_PICK_DEFAULT, m_pickCallback);

  // Set observer for detection
  if ( !m_prs_mgr->GetImageInteractorStyle()->HasObserver(EVENT_DETECT_DEFAULT) )
    m_prs_mgr->GetImageInteractorStyle()->AddObserver(EVENT_DETECT_DEFAULT, m_pickCallback);

  // Set observer for edge removal
  if ( !m_prs_mgr->GetImageInteractorStyle()->HasObserver(EVENT_DELETE) )
    m_prs_mgr->GetImageInteractorStyle()->AddObserver(EVENT_DELETE, m_domainCallback);

  // Set observer for edge joining
  if ( !m_prs_mgr->GetImageInteractorStyle()->HasObserver(EVENT_JOIN) )
    m_prs_mgr->GetImageInteractorStyle()->AddObserver(EVENT_JOIN, m_domainCallback);

  // Get notified once any sensitive is picked on a section
  connect( m_pickCallback, SIGNAL( domainPicked() ), this, SLOT( onDomainPicked() ) );

  // Get notified of edge removal
  connect( m_domainCallback, SIGNAL( killEdges() ), this, SLOT( onKillEdges() ) );

  // Get notified of edge joining
  connect( m_domainCallback, SIGNAL( joinEdges() ), this, SLOT( onJoinEdges() ) );

  /* =====================================
   *  Finalize initial state of the scene
   * ===================================== */

  // Initialize text widget used for annotations
  m_textWidget = vtkSmartPointer<vtkTextWidget>::New();
  m_textWidget->SelectableOff();
  //
  m_textWidget->SetInteractor      ( m_prs_mgr->GetRenderer()->GetRenderWindow()->GetInteractor() );
  m_textWidget->SetDefaultRenderer ( m_prs_mgr->GetRenderer() );
  m_textWidget->SetCurrentRenderer ( m_prs_mgr->GetRenderer() );
  //
  vtkTextRepresentation* textRep = vtkTextRepresentation::SafeDownCast( m_textWidget->GetRepresentation() );
  vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
  textRep->SetTextActor(textActor);
  //
  textRep->GetPositionCoordinate()->SetValue(0.3, 0.8);
  textRep->GetPosition2Coordinate()->SetValue(0.69, 0.19);
  //
  textActor->GetTextProperty()->SetJustificationToLeft();
  textActor->GetTextProperty()->SetVerticalJustificationToTop();

  this->onResetView();
}

//! Destructor.
gui_viewer_domain::~gui_viewer_domain()
{
}

//! Updates viewer.
void gui_viewer_domain::Repaint()
{
  m_prs_mgr->GetQVTKWidget()->repaint();
}

//! Resets view.
void gui_viewer_domain::onResetView()
{
  visu_utils::CameraOnTop( m_prs_mgr->GetRenderer() );
  this->Repaint();
}

//-----------------------------------------------------------------------------

//! Callback for picking event.
void gui_viewer_domain::onDomainPicked()
{
  Handle(asiData_PartNode) N = common_facilities::Instance()->Model->GetPartNode();
  if ( N.IsNull() || !N->IsWellFormed() || N->GetShape().IsNull() )
    return;

  // Build a map of shapes
  TopTools_IndexedMapOfShape FacesMap;
  TopExp::MapShapes(N->GetShape(), FacesMap);

  // Get face
  const int face_idx = N->GetFaceRepresentation()->GetSelectedFace();
  //
  TopoDS_Face F;
  if ( face_idx > 0 )
    F = TopoDS::Face( FacesMap.FindKey(face_idx) );
  //
  if ( F.IsNull() )
    return;

  //---------------------------------------------------------------------------
  // Retrieve current selection
  //---------------------------------------------------------------------------

  // Access picking results
  const visu_actual_selection& sel      = m_prs_mgr->GetCurrentSelection();
  const visu_pick_result&      pick_res = sel.PickResult(SelectionNature_Pick);
  const visu_actor_elem_map&   elem_map = pick_res.GetPickMap();

  // Check if there is anything selected
  if ( elem_map.IsEmpty() )
  {
    m_textWidget->Off();
    this->Repaint();
    return;
  }

  // Prepare cumulative set of all picked element IDs
  for ( visu_actor_elem_map::Iterator it(elem_map); it.More(); it.Next() )
  {
    const vtkSmartPointer<vtkActor>&  picked_actor = it.Key();
    const TColStd_PackedMapOfInteger& cellGIDs     = it.Value();

    // Access polygonal data mapper
    vtkPolyDataMapper* pMapper = vtkPolyDataMapper::SafeDownCast( picked_actor->GetMapper() );
    if ( !pMapper )
    {
      m_textWidget->Off();
      return;
    }

    // Access polygonal data
    vtkPolyData* pData = vtkPolyData::SafeDownCast( pMapper->GetInput() );
    if ( !pData )
    {
      m_textWidget->Off();
      return;
    }

    // Get edge. We make an exploration loop here in order not to miss seams
    const int   edgeId     = cellGIDs.GetMinimalMapped();
    int         current_id = 0;
    TopoDS_Edge edge;
    //
    for ( TopExp_Explorer eexp(F, TopAbs_EDGE); eexp.More(); eexp.Next() )
    {
      ++current_id;
      if ( current_id == edgeId )
      {
        edge = TopoDS::Edge( eexp.Current() );
        break;
      }
    }
    //
    if ( edge.IsNull() )
      return;

    // Prepare label
    TCollection_AsciiString TITLE  = "(Edge #";
                            TITLE += edgeId;
                            TITLE += ", ";
                            TITLE += asiAlgo_Utils::ShapeAddr(edge).c_str();
                            TITLE += ", ";
                            TITLE += asiAlgo_Utils::OrientationToString(edge);
                            TITLE += " in face)\n";
    //
    double f, l;
    Handle(Geom_Curve) c3d = BRep_Tool::Curve(edge, f, l);
    //
    TITLE += "3D: ";
    TITLE += c3d->DynamicType()->Name();
    TITLE += " [";
    TITLE += f;
    TITLE += ", ";
    TITLE += l;
    TITLE += " ]\n";

    double f2, l2;
    Handle(Geom2d_Curve) c2d = BRep_Tool::CurveOnSurface(edge, F, f2, l2);
    //
    TITLE += "2D: ";
    TITLE += c2d->DynamicType()->Name();
    TITLE += " [";
    TITLE += f2;
    TITLE += ", ";
    TITLE += l2;
    TITLE += " ]";

    // Update text on the annotation
    m_textWidget->GetTextActor()->SetInput( TITLE.ToCString() );
    m_textWidget->On();
  }
  this->Repaint();
}

//-----------------------------------------------------------------------------

//! Callback for edges removal.
void gui_viewer_domain::onKillEdges()
{
  Handle(asiData_PartNode) N = common_facilities::Instance()->Model->GetPartNode();
  if ( N.IsNull() || !N->IsWellFormed() || N->GetShape().IsNull() )
    return;

  TopoDS_Shape part = N->GetShape();

  // Get edges
  TopTools_IndexedMapOfShape selectedEdges;
  asiEngine_Domain::GetHighlightedEdges(selectedEdges);

  TIMER_NEW
  TIMER_GO

  // Delete selected edges
  asiAlgo_DeleteEdges eraser(part);
  if ( !eraser.Perform(selectedEdges, true) )
  {
    std::cout << "Error: cannot delete edges" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Edge ONLY deletion")

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
  common_facilities::Instance()->Prs.Part->InitializePickers();
  common_facilities::Instance()->Prs.Part->Actualize( N.get() );
}

//! Callback for edges joining.
void gui_viewer_domain::onJoinEdges()
{
  Handle(asiData_PartNode) N = common_facilities::Instance()->Model->GetPartNode();
  if ( N.IsNull() || !N->IsWellFormed() || N->GetShape().IsNull() )
    return;

  TopoDS_Shape part = N->GetShape();

  // Get edges
  TopoDS_Face face;
  TopTools_IndexedMapOfShape selectedEdges;
  asiEngine_Domain::GetHighlightedEdges(selectedEdges, face);

  TIMER_NEW
  TIMER_GO

  // Join selected edges
  asiAlgo_JoinEdges joiner(part);
  if ( !joiner.Perform(selectedEdges, face) )
  {
    std::cout << "Error: cannot join edges" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Edge joining")

  const TopoDS_Shape& result = joiner.Result();

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
  common_facilities::Instance()->Prs.Part->InitializePickers();
  common_facilities::Instance()->Prs.Part->Actualize( N.get() );
}
