//-----------------------------------------------------------------------------
// Created on: 27 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_ViewerPart.h>

// Visualization includes
#include <asiVisu_NodeInfo.h>
#include <asiVisu_Utils.h>

// GUI includes
#include <asiUI_ControlsPart.h>
#include <asiUI_DialogFindEdge.h>
#include <asiUI_DialogFindFace.h>

// VTK includes
#include <vtkAssembly.h>
#include <vtkRenderWindow.h>

// Qt-VTK includes
#include <QVTKWidget.h>

// Qt includes
#pragma warning(push, 0)
#include <QVBoxLayout>
#pragma warning(pop)

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

#define COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

void GetPickedSubshapeIds(const asiVisu_PickResult&         pick_res,
                          std::vector<int>&                 picked_subshape_IDs,
                          std::vector<ActAPI_DataObjectId>& picked_node_IDs)
{
  const asiVisu_ActorElemMap& elem_map = pick_res.GetPickMap();

  // Prepare cumulative set of all picked element IDs
  for ( asiVisu_ActorElemMap::Iterator it(elem_map); it.More(); it.Next() )
  {
    const vtkSmartPointer<vtkActor>&  picked_actor  = it.Key();
    const TColStd_PackedMapOfInteger& subshape_mask = it.Value();

    // Retrieve the corresponding Node ID by picked Actor
    ActAPI_DataObjectId
      picked_node_id = asiVisu_NodeInfo::Retrieve(picked_actor)->GetNodeId();

    // Fill coherent collections of references: sub-shape IDs against owning Nodes
    for ( TColStd_MapIteratorOfPackedMapOfInteger maskIt(subshape_mask); maskIt.More(); maskIt.Next() )
    {
      picked_subshape_IDs.push_back( maskIt.Key() );
      picked_node_IDs.push_back(picked_node_id);
    }
  }

#if defined COUT_DEBUG
  if ( picked_subshape_IDs.size() )
  {
    std::cout << "Picked sub-shapes:";
    for ( size_t k = 0; k < picked_subshape_IDs.size(); ++k )
    {
      std::cout << " " << picked_subshape_IDs[k] << " [" << picked_node_IDs[k].ToCString() << "]";
    }
    std::cout << std::endl;
  }
#endif
}

//-----------------------------------------------------------------------------

//! Creates a new instance of viewer.
//! \param model  [in] Data Model instance.
//! \param parent [in] parent widget.
asiUI_ViewerPart::asiUI_ViewerPart(const Handle(asiEngine_Model)& model,
                                   QWidget*                       parent)
: asiUI_Viewer(parent), m_model(model)
{
  // Initialize presentation manager along with QVTK widget
  m_prs_mgr = vtkSmartPointer<asiVisu_PrsManager>::New();
  //
  m_prs_mgr->Initialize(this);
  m_prs_mgr->SetBlackAndWhiteIntensity(0.4, 0.7);
  m_prs_mgr->SetInteractionMode(asiVisu_PrsManager::InteractionMode_3D);
  m_prs_mgr->SetSelectionMode(SelectionMode_Face);

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
  m_pickCallback = vtkSmartPointer<asiUI_PickCallback>::New();
  m_pickCallback->SetViewer(this);

  // Initialize Callback instance for handling events related to Part viewer
  m_partCallback = vtkSmartPointer<asiUI_PartCallback>::New();
  m_partCallback->SetViewer(this);

  // Set observer for picking
  if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_PICK_DEFAULT) )
    m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_PICK_DEFAULT, m_pickCallback);

  // Set observer for detection
  if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_DETECT_DEFAULT) )
    m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_DETECT_DEFAULT, m_pickCallback);

  // Set observer for finding face
  if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_FIND_FACE) )
    m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_FIND_FACE, m_partCallback);

  // Set observer for finding edge
  if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_FIND_EDGE) )
    m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_FIND_EDGE, m_partCallback);

  // Get notified once a sub-shape is picked
  connect( m_pickCallback, SIGNAL( picked() ), this, SLOT( onSubShapesPicked() ) );

  // Get notified about part events
  connect( m_partCallback, SIGNAL( findFace() ), this, SLOT( onFindFace() ) );
  connect( m_partCallback, SIGNAL( findEdge() ), this, SLOT( onFindEdge() ) );

  /* ===============================
   *  Setting up rotation callbacks
   * =============================== */

  // Initialize Callback instance for rotation
  m_rotoCallback = vtkSmartPointer<asiUI_RotationCallback>::New();
  m_rotoCallback->SetViewer(this);

  // Set observer for starting rotation
  if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_ROTATION_START) )
    m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_ROTATION_START, m_rotoCallback);

  // Set observer for ending rotation
  if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_ROTATION_END) )
    m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_ROTATION_END, m_rotoCallback);

  /* ========================
   *  Initialize axes widget
   * ======================== */

  vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
  vtkSmartPointer<vtkAssembly> assm = vtkSmartPointer<vtkAssembly>::New();
  assm->AddPart(axes);
  //
  m_axesWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
  m_axesWidget->SetOrientationMarker(assm);
  //
  vtkRenderer* renderer = m_prs_mgr->GetRenderer();
  renderer->SetRenderWindow( m_prs_mgr->GetQVTKWidget()->GetRenderWindow() );
  //
  m_axesWidget->SetCurrentRenderer( m_prs_mgr->GetRenderer() );
  m_axesWidget->SetInteractor( m_prs_mgr->GetQVTKWidget()->GetRenderWindow()->GetInteractor() );
  m_axesWidget->SetEnabled(1);
  m_axesWidget->SetInteractive(0);
  m_axesWidget->SetViewport(0, 0, 0.25, 0.25);

  /* =====================================
   *  Finalize initial state of the scene
   * ===================================== */

  pViewer->setContextMenuPolicy(Qt::CustomContextMenu);
  connect( pViewer, SIGNAL ( customContextMenuRequested(const QPoint&) ),
           this,    SLOT   ( onContextMenu(const QPoint&) ) );

  this->onResetView();
}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_ViewerPart::~asiUI_ViewerPart()
{
}

//-----------------------------------------------------------------------------

//! Updates viewer.
void asiUI_ViewerPart::Repaint()
{
  m_prs_mgr->GetQVTKWidget()->repaint();
}

//-----------------------------------------------------------------------------

//! Resets view.
void asiUI_ViewerPart::onResetView()
{
  asiVisu_Utils::ResetCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  this->Repaint();
}

//-----------------------------------------------------------------------------

//! Callback for picking event.
void asiUI_ViewerPart::onSubShapesPicked()
{
  Handle(asiData_PartNode) geom_n = m_model->GetPartNode();
  //
  if ( geom_n.IsNull() || !geom_n->IsWellFormed() )
  {
    std::cout << "Geometry Node is not accessible" << std::endl;
    return; // No target Node to proceed with
  }

  if ( !m_prs_mgr->IsPresented(geom_n) )
    return;

  //---------------------------------------------------------------------------
  // Retrieve current selection
  //---------------------------------------------------------------------------

  // Access picking results
  const asiVisu_ActualSelection& sel      = m_prs_mgr->GetCurrentSelection();
  const asiVisu_PickResult&      pick_res = sel.PickResult(SelectionNature_Pick);

  if ( pick_res.IsSelectionFace() )
  {
    // Prepare arrays for selected elements
    std::vector<int>                 picked_face_IDs;
    std::vector<ActAPI_DataObjectId> picked_node_IDs;

    // Prepare cumulative set of all picked element IDs
    GetPickedSubshapeIds(pick_res, picked_face_IDs, picked_node_IDs);

    //-------------------------------------------------------------------------
    // Store active selection in the Data Model
    //-------------------------------------------------------------------------

    m_model->OpenCommand(); // tx start
    {
      // Store index of the active face
      if ( picked_face_IDs.size() )
      {
        geom_n->GetFaceRepresentation()    ->SetSelectedFace(picked_face_IDs[0]);
        geom_n->GetSurfaceRepresentation() ->SetSelectedFace(picked_face_IDs[0]);
        //
        std::cout << "Active face has been stored..." << std::endl;
      }
      else // Reset stored indices
      {
        geom_n->GetFaceRepresentation()    ->SetSelectedFace(0);
        geom_n->GetSurfaceRepresentation() ->SetSelectedFace(0);
        //
        std::cout << "Active face has been reset..." << std::endl;
      }
    }
    m_model->CommitCommand(); // tx commit

    //-------------------------------------------------------------------------
    // Notify
    //-------------------------------------------------------------------------

    emit facePicked();
  }
  else if ( pick_res.IsSelectionEdge() )
  {
    // Prepare arrays for selected elements
    std::vector<int>                 picked_edge_IDs;
    std::vector<ActAPI_DataObjectId> picked_node_IDs;

    // Prepare cumulative set of all picked element IDs
    GetPickedSubshapeIds(pick_res, picked_edge_IDs, picked_node_IDs);

    //-------------------------------------------------------------------------
    // Store active selection in the Data Model
    //-------------------------------------------------------------------------

    m_model->OpenCommand(); // tx start
    {
      // Store index of the active edge
      if ( picked_edge_IDs.size() )
      {
        geom_n->GetEdgeRepresentation()  ->SetSelectedEdge(picked_edge_IDs[0]);
        geom_n->GetCurveRepresentation() ->SetSelectedEdge(picked_edge_IDs[0]);
        //
        std::cout << "Active edge has been stored..." << std::endl;
      }
      else // Reset stored indices
      {
        geom_n->GetEdgeRepresentation()  ->SetSelectedEdge(0);
        geom_n->GetCurveRepresentation() ->SetSelectedEdge(0);
        //
        std::cout << "Active edge has been reset..." << std::endl;
      }
    }
    m_model->CommitCommand(); // tx commit

    //-------------------------------------------------------------------------
    // Notify
    //-------------------------------------------------------------------------

    emit edgePicked();
  }
}

//-----------------------------------------------------------------------------

//! Callback for face find request.
void asiUI_ViewerPart::onFindFace()
{
  // Run dialog
  asiUI_DialogFindFace* wFindFace = new asiUI_DialogFindFace(m_model, this->PrsMgr(), this);
  wFindFace->show();
}

//-----------------------------------------------------------------------------

//! Callback for edge find request.
void asiUI_ViewerPart::onFindEdge()
{
  // Run dialog
  asiUI_DialogFindEdge* wFindEdge = new asiUI_DialogFindEdge(m_model, this->PrsMgr(), this);
  wFindEdge->show();
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPart::onContextMenu(const QPoint& pos)
{
  QVTKWidget* pViewer   = m_prs_mgr->GetQVTKWidget();
  QPoint      globalPos = pViewer->mapToGlobal(pos);

  emit contextMenu(globalPos);
}
