//-----------------------------------------------------------------------------
// Created on: 27 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sergey Slyadnev nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_ViewerPart.h>

// asiVisu includes
#include <asiVisu_NodeInfo.h>
#include <asiVisu_Utils.h>

// asiUI includes
#include <asiUI_ControlsPart.h>
#include <asiUI_DialogFindEdge.h>
#include <asiUI_DialogFindFace.h>
#include <asiUI_DialogRefineTessellation.h>

// VTK includes
#include <vtkAssembly.h>
#include <vtkRenderWindow.h>

// Qt-VTK includes
#include <QVTKWidget.h>

// Qt includes
#pragma warning(push, 0)
#include <QDesktopWidget>
#include <QVBoxLayout>
#pragma warning(pop)

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

void GetPickedSubshapeIds(const Handle(asiVisu_PickerResult)& pick_res,
                          std::vector<int>&                   picked_subshape_IDs,
                          std::vector<ActAPI_DataObjectId>&   picked_node_IDs)
{
  Handle(asiVisu_CellPickerResult)
    cellPickRes = Handle(asiVisu_CellPickerResult)::DownCast(pick_res);

  const vtkSmartPointer<vtkActor>&  picked_actor  = cellPickRes->GetPickedActor();
  const TColStd_PackedMapOfInteger& subshape_mask = cellPickRes->GetPickedElementIds();

  if ( !picked_actor.GetPointer() )
    return; // Nothing selected

  // Retrieve Node information
  asiVisu_NodeInfo* pNodeInfo = asiVisu_NodeInfo::Retrieve(picked_actor);
  //
  if ( !pNodeInfo )
    return;

  // Retrieve the corresponding Node ID by picked Actor
  ActAPI_DataObjectId picked_node_id = pNodeInfo->GetNodeId();

  // Fill coherent collections of references: sub-shape IDs against owning Nodes
  for ( TColStd_MapIteratorOfPackedMapOfInteger maskIt(subshape_mask); maskIt.More(); maskIt.Next() )
  {
    picked_subshape_IDs.push_back( maskIt.Key() );
    picked_node_IDs.push_back(picked_node_id);
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
//! \param[in] model             Data Model instance.
//! \param[in] enableInteraction enables/disables interaction mechanisms.
//! \param[in] parent            parent widget.
asiUI_ViewerPart::asiUI_ViewerPart(const Handle(asiEngine_Model)& model,
                                   const bool                     enableInteraction,
                                   QWidget*                       parent)
: asiUI_Viewer(parent), m_model(model)
{
  // Initialize presentation manager along with QVTK widget
  m_prs_mgr = vtkSmartPointer<asiVisu_PrsManager>::New();
  //
  m_prs_mgr->SetModel(model);
  m_prs_mgr->Initialize(this);
  m_prs_mgr->SetBlackAndWhiteIntensity(0.8, 0.9);
  m_prs_mgr->SetInteractionMode(asiVisu_PrsManager::InteractionMode_3D);
  m_prs_mgr->SetSelectionMode(SelectionMode_Face);

  // Widgets and layouts
  QVTKWidget*  pViewer     = m_prs_mgr->GetQVTKWidget();
  QVBoxLayout* pBaseLayout = new QVBoxLayout();

  // Create toolbar
  m_toolBar = new QToolBar(this);
  m_toolBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  // Create actions
  this->createActions();

  // Configure layout
  pBaseLayout->setSpacing(0);
  pBaseLayout->addWidget(m_toolBar);
  pBaseLayout->addWidget(pViewer);
  pBaseLayout->setAlignment(Qt::AlignTop);
  pBaseLayout->setContentsMargins(0, 0, 0, 0);

  // Set central widget
  this->setLayout(pBaseLayout);

  /* ===================================
   *  Setting up picking infrastructure
   * =================================== */

  if ( enableInteraction )
  {
    // Initialize Callback instance for Pick operation
    m_pickCallback = vtkSmartPointer<asiUI_PickCallback>::New();
    m_pickCallback->SetViewer(this);
    m_pickCallback->SetModel(m_model);
    m_pickCallback->SetPickerTypes(PickerType_Cell);

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

    // Set observer for tessellation refinement
    if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_REFINE_TESSELLATION) )
      m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_REFINE_TESSELLATION, m_partCallback);

    // Get notified once a sub-shape is picked
    connect( m_pickCallback, SIGNAL( picked() ), this, SLOT( onSubShapesPicked() ) );
    connect( m_pickCallback, SIGNAL( picked() ), this, SLOT( onWhateverPicked() ) );

    // Get notified about part events
    connect( m_partCallback, SIGNAL( findFace() ),           this, SLOT( onFindFace() ) );
    connect( m_partCallback, SIGNAL( findEdge() ),           this, SLOT( onFindEdge() ) );
    connect( m_partCallback, SIGNAL( refineTessellation() ), this, SLOT( onRefineTessellation() ) );

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
  }

  /* ========================
   *  Initialize axes widget
   * ======================== */

  vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
  vtkSmartPointer<vtkAssembly>  assm = vtkSmartPointer<vtkAssembly>::New();
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
{}

//-----------------------------------------------------------------------------

//! \return size hint.
QSize asiUI_ViewerPart::sizeHint() const
{
  QDesktopWidget desktop;
  const int side   = std::min( desktop.height(), desktop.width() );
  const int width  = (int) (side*0.25);
  const int height = (int) (side*0.5);

  QSize s(width, height);
  return s;
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
  //
  this->Repaint();
}

//-----------------------------------------------------------------------------

//! Fits view to the contents.
void asiUI_ViewerPart::onFitAll()
{
  asiVisu_Utils::AdjustCamera( m_prs_mgr->GetRenderer(),
                               m_prs_mgr->PropsByTrihedron() );
  //
  this->Repaint();
}

//-----------------------------------------------------------------------------

//! Callback for picking event.
void asiUI_ViewerPart::onWhateverPicked()
{
  // Access picking results
  const asiVisu_ActualSelection&          sel      = m_prs_mgr->GetCurrentSelection();
  const Handle(asiVisu_CellPickerResult)& pick_res = sel.GetCellPickerResult(SelectionNature_Persistent);

  // Get picked position without any attempt to interpret what's happening
  double x, y, z;
  pick_res->GetPickedPos(x, y, z);

  // We don't care of picking logic here and let the listener react
  emit pointPicked(x, y, z);
}

//-----------------------------------------------------------------------------

//! Callback for picking event.
void asiUI_ViewerPart::onSubShapesPicked()
{
  // Access picking results
  const asiVisu_ActualSelection&      sel      = m_prs_mgr->GetCurrentSelection();
  const Handle(asiVisu_PickerResult)& pick_res = sel.GetCellPickerResult(SelectionNature_Persistent);

  // Special processing for Part Node
  Handle(asiData_PartNode) geom_n = m_model->GetPartNode();
  //
  if ( geom_n.IsNull() || !geom_n->IsWellFormed() )
  {
    if ( pick_res->IsSelectionFace() )
      emit facePicked( pick_res.get() );
    //
    else if ( pick_res->IsSelectionEdge() )
      emit edgePicked( pick_res.get() );
    //
    else if ( pick_res->IsSelectionVertex() )
      emit vertexPicked( pick_res.get() );

    std::cout << "Geometry Node is not accessible" << std::endl;
    return; // No target Node to proceed with
  }

  if ( !m_prs_mgr->IsPresented(geom_n) )
    return;

  //---------------------------------------------------------------------------
  // Retrieve current selection
  //---------------------------------------------------------------------------

  if ( pick_res->IsSelectionFace() )
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
        geom_n->GetNormsRepresentation()   ->SetSelectedFace(picked_face_IDs[0]);
        geom_n->GetSurfaceRepresentation() ->SetSelectedFace(picked_face_IDs[0]);
        geom_n->GetContourRepresentation() ->SetSelectedFace(picked_face_IDs[0]);
        //
        std::cout << "Active face has been stored..." << std::endl;
      }
      else // Reset stored indices
      {
        geom_n->GetFaceRepresentation()    ->SetSelectedFace(0);
        geom_n->GetNormsRepresentation()   ->SetSelectedFace(0);
        geom_n->GetSurfaceRepresentation() ->SetSelectedFace(0);
        geom_n->GetContourRepresentation() ->SetSelectedFace(0);
        //
        std::cout << "Active face has been reset..." << std::endl;
      }
    }
    m_model->CommitCommand(); // tx commit

    //-------------------------------------------------------------------------
    // Notify
    //-------------------------------------------------------------------------

    emit facePicked( pick_res.get() );
  }
  else if ( pick_res->IsSelectionEdge() )
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

    emit edgePicked( pick_res.get() );
  }
  else if ( pick_res->IsSelectionVertex() )
  {
    // Prepare arrays for selected elements
    std::vector<int>                 picked_vertex_IDs;
    std::vector<ActAPI_DataObjectId> picked_node_IDs;

    // Prepare cumulative set of all picked element IDs
    GetPickedSubshapeIds(pick_res, picked_vertex_IDs, picked_node_IDs);

    //-------------------------------------------------------------------------
    // Store active selection in the Data Model
    //-------------------------------------------------------------------------

    m_model->OpenCommand(); // tx start
    {
      // Store index of the active edge
      if ( picked_vertex_IDs.size() )
      {
        geom_n->GetVertexRepresentation()->SetSelectedVertex(picked_vertex_IDs[0]);
        //
        std::cout << "Active vertex has been stored..." << std::endl;
      }
      else // Reset stored indices
      {
        geom_n->GetVertexRepresentation()->SetSelectedVertex(0);
        //
        std::cout << "Active vertex has been reset..." << std::endl;
      }
    }
    m_model->CommitCommand(); // tx commit

    //-------------------------------------------------------------------------
    // Notify
    //-------------------------------------------------------------------------

    emit vertexPicked( pick_res.get() );
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

//! Callback for tessellation refinement.
void asiUI_ViewerPart::onRefineTessellation()
{
  // Run dialog
  asiUI_DialogRefineTessellation*
    wRefineTessellation = new asiUI_DialogRefineTessellation(m_model, this->PrsMgr(), this);
  //
  wRefineTessellation->show();
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPart::onContextMenu(const QPoint& pos)
{
  QVTKWidget* pViewer   = m_prs_mgr->GetQVTKWidget();
  QPoint      globalPos = pViewer->mapToGlobal(pos);

  emit contextMenu(globalPos);
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPart::onDefaultView()
{
  asiVisu_Utils::ResetCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  asiVisu_Utils::AdjustCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  //
  this->Repaint();
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPart::onBackView()
{
  asiVisu_Utils::CameraOnBack( m_prs_mgr->GetRenderer() );
  asiVisu_Utils::AdjustCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  //
  this->Repaint();
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPart::onBottomView()
{
  asiVisu_Utils::CameraOnBottom( m_prs_mgr->GetRenderer() );
  asiVisu_Utils::AdjustCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  //
  this->Repaint();
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPart::onFrontView()
{
  asiVisu_Utils::CameraOnFront( m_prs_mgr->GetRenderer() );
  asiVisu_Utils::AdjustCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  //
  this->Repaint();
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPart::onLeftView()
{
  asiVisu_Utils::CameraOnLeft( m_prs_mgr->GetRenderer() );
  asiVisu_Utils::AdjustCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  //
  this->Repaint();
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPart::onRightView()
{
  asiVisu_Utils::CameraOnRight( m_prs_mgr->GetRenderer() );
  asiVisu_Utils::AdjustCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  //
  this->Repaint();
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPart::onTopView()
{
  asiVisu_Utils::CameraOnTop( m_prs_mgr->GetRenderer() );
  asiVisu_Utils::AdjustCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  //
  this->Repaint();
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPart::createActions()
{
  QAction* pFitAllAction     = new QAction("Fit",    this);
  QAction* pResetViewAction  = new QAction("Reset",  this);
  QAction* pBackViewAction   = new QAction("Back",   this);
  QAction* pBottomViewAction = new QAction("Bottom", this);
  QAction* pFrontViewAction  = new QAction("Front",  this);
  QAction* pLeftViewAction   = new QAction("Left",   this);
  QAction* pRightViewAction  = new QAction("Right",  this);
  QAction* pTopViewAction    = new QAction("Top",    this);
  //
  connect( pFitAllAction,     SIGNAL( triggered() ), this, SLOT( onFitAll      () ) );
  connect( pResetViewAction,  SIGNAL( triggered() ), this, SLOT( onDefaultView () ) );
  connect( pBackViewAction,   SIGNAL( triggered() ), this, SLOT( onBackView    () ) );
  connect( pBottomViewAction, SIGNAL( triggered() ), this, SLOT( onBottomView  () ) );
  connect( pFrontViewAction,  SIGNAL( triggered() ), this, SLOT( onFrontView   () ) );
  connect( pLeftViewAction,   SIGNAL( triggered() ), this, SLOT( onLeftView    () ) );
  connect( pRightViewAction,  SIGNAL( triggered() ), this, SLOT( onRightView   () ) );
  connect( pTopViewAction,    SIGNAL( triggered() ), this, SLOT( onTopView     () ) );

  // Add action to the toolbar
  m_toolBar->addAction(pFitAllAction);
  m_toolBar->addAction(pResetViewAction);
  m_toolBar->addAction(pBackViewAction);
  m_toolBar->addAction(pBottomViewAction);
  m_toolBar->addAction(pFrontViewAction);
  m_toolBar->addAction(pLeftViewAction);
  m_toolBar->addAction(pRightViewAction);
  m_toolBar->addAction(pTopViewAction);
}
