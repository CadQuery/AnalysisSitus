//-----------------------------------------------------------------------------
// Created on: 14 August 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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
#include <asiUI_ControlsMesh.h>

#include <asiUI_PickContourCallback.h>

// asiVisu includes
#include <asiVisu_TriangulationPrs.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

#include <asiEngine_Part.h>
#include <asiEngine_Triangulation.h>
#include <asiEngine_RE.h>

// Qt includes
#include <QGroupBox>

// Mobius includes
#include <mobius/cascade_BSplineCurve3D.h>
#include <mobius/geom_FairBCurve.h>

//-----------------------------------------------------------------------------

#define BTN_MIN_WIDTH 120

//-----------------------------------------------------------------------------

//! Constructor.
//! \param[in] model       Data Model instance.
//! \param[in] pPartViewer part viewer.
//! \param[in] notifier    progress notifier.
//! \param[in] plotter     imperative plotter.
//! \param[in] parent      parent widget.
asiUI_ControlsMesh::asiUI_ControlsMesh(const Handle(asiEngine_Model)& model,
                                       asiUI_ViewerPart*              pPartViewer,
                                       asiUI_ObjectBrowser*           pBrowser,
                                       ActAPI_ProgressEntry           notifier,
                                       ActAPI_PlotterEntry            plotter,
                                       QWidget*                       parent)
: QWidget         (parent),
  m_model         (model),
  m_partViewer    (pPartViewer),
  m_objectBrowser (pBrowser),
  m_notifier      (notifier),
  m_plotter       (plotter)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_widgets.pLoadStl           = new QPushButton("Load STL");
  m_widgets.pDefineContour     = new QPushButton("Define contour");
  m_widgets.pBuildContourLines = new QPushButton("Build contour lines");
  m_widgets.pBuildPatches      = new QPushButton("Build patches");
  m_widgets.pFairContourLines  = new QPushButton("Fair contour lines");
  m_widgets.pLambda            = new QLineEdit();
  m_widgets.pLambda->setText("1.0");
  m_widgets.pUndo              = new QPushButton("Undo");
  m_widgets.pRedo              = new QPushButton("Redo");
  //
  m_widgets.pLoadPly        = new QPushButton("Load PLY");
  m_widgets.pSaveStl        = new QPushButton("Save STL");
  m_widgets.pSelectFaces    = new QPushButton("Select faces");
  m_widgets.pSelectEdges    = new QPushButton("Select edges");
  //
  m_widgets.pLoadStl        -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pLoadPly        -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSaveStl        -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSelectFaces    -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSelectEdges    -> setMinimumWidth(BTN_MIN_WIDTH);

  m_widgets.pSelectFaces->setCheckable(true);
  m_widgets.pSelectEdges->setCheckable(true);

  // Group box for data interoperability
  QGroupBox*   pExchangeGroup = new QGroupBox("Control panel");
  QVBoxLayout* pExchangeLay   = new QVBoxLayout(pExchangeGroup);
  //
  pExchangeLay->addWidget(m_widgets.pLoadStl);
  pExchangeLay->addWidget(m_widgets.pDefineContour);
  pExchangeLay->addWidget(m_widgets.pBuildContourLines);
  pExchangeLay->addWidget(m_widgets.pBuildPatches);
  pExchangeLay->addWidget(m_widgets.pFairContourLines);
  pExchangeLay->addWidget(m_widgets.pLambda);
  pExchangeLay->addWidget(m_widgets.pUndo);
  pExchangeLay->addWidget(m_widgets.pRedo);
  //pExchangeLay->addWidget(m_widgets.pLoadPly);
  //pExchangeLay->addWidget(m_widgets.pSaveStl);

  // Group box for interactive selection
  QGroupBox*   pSelectionGroup = new QGroupBox("Selection");
  QVBoxLayout* pSelectionLay   = new QVBoxLayout(pSelectionGroup);
  //
  pSelectionLay->addWidget(m_widgets.pSelectFaces);
  pSelectionLay->addWidget(m_widgets.pSelectEdges);

  // Set layout
  m_pMainLayout->addWidget(pExchangeGroup);
  //m_pMainLayout->addWidget(pSelectionGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pLoadStl,           SIGNAL( clicked() ), SLOT( onLoadStl() ) );
  connect( m_widgets.pDefineContour,     SIGNAL( clicked() ), SLOT( onDefineContour() ) );
  connect( m_widgets.pBuildContourLines, SIGNAL( clicked() ), SLOT( onBuildContourLines() ) );
  connect( m_widgets.pBuildPatches,      SIGNAL( clicked() ), SLOT( onBuildPatches() ) );
  connect( m_widgets.pFairContourLines,  SIGNAL( clicked() ), SLOT( onFairContourLines() ) );
  connect( m_widgets.pUndo,              SIGNAL( clicked() ), SLOT( onUndo() ) );
  connect( m_widgets.pRedo,              SIGNAL( clicked() ), SLOT( onRedo() ) );
  //
  connect( m_widgets.pLoadPly,     SIGNAL( clicked() ), SLOT( onLoadPly() ) );
  connect( m_widgets.pSaveStl,     SIGNAL( clicked() ), SLOT( onSaveStl() ) );
  connect( m_widgets.pSelectFaces, SIGNAL( clicked() ), SLOT( onSelectFaces() ) );
  connect( m_widgets.pSelectEdges, SIGNAL( clicked() ), SLOT( onSelectEdges() ) );
}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_ControlsMesh::~asiUI_ControlsMesh()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! On STL loading.
void asiUI_ControlsMesh::onLoadStl()
{
  // Select filename
  QString filename = asiUI_Common::selectSTLFile(asiUI_Common::OpenSaveAction_Open);

  // Load mesh
  Handle(Poly_Triangulation) triangulation;
  //
  if ( !asiAlgo_Utils::ReadStl(QStr2AsciiStr(filename), triangulation, m_notifier) )
  {
    m_notifier.SendLogMessage( LogErr(Normal) << "Cannot read STL file" );
    return;
  }
  //
  m_notifier.SendLogMessage( LogInfo(Normal) << "Loaded mesh from %1" << QStr2AsciiStr(filename) );

  //---------------------------------------------------------------------------
  // Initialize Triangulation Node
  //---------------------------------------------------------------------------

  // Set mesh
  Handle(asiData_TriangulationNode) triangulation_n = m_model->GetTriangulationNode();
  //
  m_model->OpenCommand(); // tx start
  {
    triangulation_n->SetTriangulation(triangulation);
  }
  m_model->CommitCommand(); // tx commit

  //---------------------------------------------------------------------------
  // Update UI
  //---------------------------------------------------------------------------

  m_partViewer->PrsMgr()->Actualize(triangulation_n.get(), false, true);
}

//-----------------------------------------------------------------------------

//! On PLY loading.
void asiUI_ControlsMesh::onLoadPly()
{
  // Select filename
  QString filename = asiUI_Common::selectPLYFile(asiUI_Common::OpenSaveAction_Open);

  // Load mesh
  Handle(ActData_Mesh) mesh;
  //
  if ( !asiAlgo_Utils::ReadPly(QStr2AsciiStr(filename), mesh, m_notifier) )
  {
    m_notifier.SendLogMessage( LogErr(Normal) << "Cannot read PLY file" );
    return;
  }
  //
  m_notifier.SendLogMessage( LogInfo(Normal) << "Loaded mesh from %1" << QStr2AsciiStr(filename) );

  //---------------------------------------------------------------------------
  // Initialize Triangulation Node
  //---------------------------------------------------------------------------

  // Set mesh
  Handle(asiData_TessNode) tess_n = m_model->GetTessellationNode();
  //
  m_model->OpenCommand(); // tx start
  {
    tess_n->SetMesh(mesh);
  }
  m_model->CommitCommand(); // tx commit

  //---------------------------------------------------------------------------
  // Update UI
  //---------------------------------------------------------------------------

  m_partViewer->PrsMgr()->Actualize(tess_n.get(), false, true);
}

//-----------------------------------------------------------------------------

//! On STL saving.
void asiUI_ControlsMesh::onSaveStl()
{
  // Select filename
  QString filename = asiUI_Common::selectSTLFile(asiUI_Common::OpenSaveAction_Save);

  // Get mesh
  Handle(asiData_TriangulationNode) triangulation_n = m_model->GetTriangulationNode();
  Handle(Poly_Triangulation)        triangulation   = triangulation_n->GetTriangulation();

  // Save
  if ( !asiAlgo_Utils::WriteStl( triangulation, QStr2AsciiStr(filename) ) )
  {
    m_notifier.SendLogMessage( LogErr(Normal) << "Cannot write STL file" );
    return;
  }
  //
  m_notifier.SendLogMessage( LogInfo(Normal) << "Mesh was saved to %1." << QStr2AsciiStr(filename) );
}

//-----------------------------------------------------------------------------

//! On selection of faces.
void asiUI_ControlsMesh::onSelectFaces()
{
  m_widgets.pSelectEdges->setChecked(false);

  // Get presentation for Mesh Node
  Handle(asiVisu_TriangulationPrs)
    prs = Handle(asiVisu_TriangulationPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation( m_model->GetTriangulationNode() ) );
  //
  if ( prs.IsNull() )
    return;

  prs->MainActor()->SetPickable( m_widgets.pSelectFaces->isChecked() );
  prs->ContourActor()->SetPickable(0);
}

//-----------------------------------------------------------------------------

//! On selection of edges.
void asiUI_ControlsMesh::onSelectEdges()
{
  m_widgets.pSelectFaces->setChecked(false);

  // Get presentation for Mesh Node
  Handle(asiVisu_TriangulationPrs)
    prs = Handle(asiVisu_TriangulationPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation( m_model->GetTriangulationNode() ) );
  //
  if ( prs.IsNull() )
    return;

  prs->MainActor()->SetPickable(0);
  prs->ContourActor()->SetPickable( m_widgets.pSelectEdges->isChecked() );
}

//-----------------------------------------------------------------------------

void asiUI_ControlsMesh::onDefineContour()
{
  // Get Part & Triangulaion Nodes.
  Handle(asiData_PartNode)          part_n = m_model->GetPartNode();
  Handle(asiData_TriangulationNode) tris_n = m_model->GetTriangulationNode();

  // Contour Node.
  Handle(asiData_ContourNode) contour_n;

  // Prepare data model.
  asiEngine_Part          partAPI ( m_model, m_notifier, m_plotter );
  asiEngine_Triangulation trisAPI ( m_model, m_notifier, m_plotter );
  asiEngine_RE            reAPI   ( m_model, m_notifier, m_plotter );
  //
  m_model->OpenCommand();
  {
    // Build BVH for CAD-agnostic mesh.
    trisAPI.BuildBVH();
  }
  m_model->CommitCommand();

  // Get Part presentation manager.
  const vtkSmartPointer<asiVisu_PrsManager>& PM = m_partViewer->PrsMgr();

  // Set picker type to world picker.
  m_partViewer->GetPickCallback()->SetPickerTypes(PickerType_World | PickerType_Cell);

  // Set selection mode.
  PM->SetSelectionMode(SelectionMode_Workpiece);

  // Prepare callback which will manage the interaction process with user.
  vtkSmartPointer<asiUI_PickContourCallback>
    cb = vtkSmartPointer<asiUI_PickContourCallback>::New();
  //
  cb->SetViewer        ( m_partViewer );
  cb->SetModel         ( m_model );
  cb->SetObjectBrowser ( m_objectBrowser );
  //
  if ( !tris_n->GetTriangulation().IsNull() )
    cb->SetBVH( tris_n->GetBVH() );
  //
  cb->SetDiagnosticTools ( m_notifier, m_plotter );

  // Remove previously defined observers.
  if ( PM->HasObserver(EVENT_SELECT_WORLD_POINT) )
    PM->RemoveObservers(EVENT_SELECT_WORLD_POINT);
  //
  if ( PM->HasObserver(EVENT_SELECT_CELL) )
    PM->RemoveObservers(EVENT_SELECT_CELL);

  // Add observer which takes responsibility to interact with the user.
  PM->AddObserver(EVENT_SELECT_WORLD_POINT, cb);
  PM->AddObserver(EVENT_SELECT_CELL,        cb);
}

//-----------------------------------------------------------------------------

void asiUI_ControlsMesh::onBuildContourLines()
{
  asiEngine_RE reApi( m_model,
                      m_notifier,
                      m_plotter );

  // Find Edges Node.
  Handle(asiData_ReEdgesNode) edgesNode = reApi.Get_Edges();
  //
  if ( edgesNode.IsNull() )
  {
    return;
  }

  m_model->OpenCommand();

  // Approximate every edge individually.
  for ( Handle(ActAPI_IChildIterator) eit = edgesNode->GetChildIterator(); eit->More(); eit->Next() )
  {
    Handle(asiData_ReEdgeNode)
      edgeNode = Handle(asiData_ReEdgeNode)::DownCast( eit->Value() );
    
    std::vector<Handle(Geom_BSplineCurve)> curves;

    // Approximate with parametric curve.
    std::vector<gp_XYZ> pts;
    edgeNode->GetPolyline(pts);
    //
    Handle(Geom_BSplineCurve) curve;
    if ( !asiAlgo_Utils::ApproximatePoints(pts, 3, 0.1, curve) )
    {
      m_model->AbortCommand();
      return;
    }

    // Update Data Model.
    edgeNode->SetCurve(curve);

    // Add curve to the collection for filling.
    curves.push_back(curve);

    // Update scene.
    m_partViewer->PrsMgr()->Actualize(edgeNode);
  }

  m_model->CommitCommand();
}

//-----------------------------------------------------------------------------

void asiUI_ControlsMesh::onBuildPatches()
{
  asiEngine_RE reApi( m_model,
                      m_notifier,
                      m_plotter );

  // Find Patches Node.
  Handle(asiData_RePatchesNode) patchesNode = reApi.Get_Patches();
  //
  if ( patchesNode.IsNull() )
  {
    return;
  }

  m_model->OpenCommand();

  // Reconstruct every patch individually.
  for ( Handle(ActAPI_IChildIterator) pit = patchesNode->GetChildIterator(); pit->More(); pit->Next() )
  {
    Handle(asiData_RePatchNode)
      patchNode = Handle(asiData_RePatchNode)::DownCast( pit->Value() );

    std::vector<Handle(Geom_BSplineCurve)> curves;

    // Get all edges and approximate them.
    for ( Handle(ActAPI_IChildIterator) cit = patchNode->GetChildIterator();
          cit->More(); cit->Next() )
    {
      Handle(ActAPI_INode) childNode = cit->Value();
      //
      if ( !childNode->IsKind( STANDARD_TYPE(asiData_ReCoEdgeNode) ) )
      {
        m_model->AbortCommand();
        return;
      }
      //
      const Handle(asiData_ReCoEdgeNode)&
        coedgeNode = Handle(asiData_ReCoEdgeNode)::DownCast(childNode);
      //
      Handle(asiData_ReEdgeNode) edgeNode = coedgeNode->GetEdge();

      // Get B-curve from edge.
      Handle(Geom_BSplineCurve)
        bcurve = Handle(Geom_BSplineCurve)::DownCast( edgeNode->GetCurve() );
      //
      if ( bcurve.IsNull() )
      {
        return;
      }

      // Add curve to the collection for filling.
      curves.push_back(bcurve);

      // Update scene.
      m_partViewer->PrsMgr()->Actualize(edgeNode);
    }

    // Build patch.
    Handle(Geom_BSplineSurface) patchSurf;
    //
    if ( !asiAlgo_Utils::Fill4Contour(curves, patchSurf) )
    {
      m_model->AbortCommand();
      return;
    }
    //
    TCollection_AsciiString patchName("patch_"); patchName += patchNode->GetId();
    //
    m_plotter.REDRAW_SURFACE(patchName, patchSurf, Color_White);
  }

  m_model->CommitCommand();
}

//-----------------------------------------------------------------------------

Handle(Geom_BSplineCurve) FairCurve(const Handle(Geom_BSplineCurve)& curve,
                                    const double                     lambda,
                                    ActAPI_ProgressEntry             progress)
{
  // Convert to Mobius curve.
  mobius::cascade_BSplineCurve3D toMobius(curve);
  toMobius.DirectConvert();
  const mobius::ptr<mobius::bcurve>& mobCurve = toMobius.GetMobiusCurve();

  // Perform fairing from Mobius.
  mobius::geom_FairBCurve fairing(mobCurve, lambda, NULL, NULL);
  //
  if ( !fairing.Perform() )
  {
    progress.SendLogMessage(LogErr(Normal) << "Fairing failed.");
    return NULL;
  }

  // Get the faired curve.
  const mobius::ptr<mobius::bcurve>& mobResult = fairing.GetResult();

  // Convert to OpenCascade curve.
  mobius::cascade_BSplineCurve3D toOpenCascade(mobResult);
  toOpenCascade.DirectConvert();
  Handle(Geom_BSplineCurve) result = toOpenCascade.GetOpenCascadeCurve();

  return result;
}

void asiUI_ControlsMesh::onFairContourLines()
{
  // Get fairing coefficient.
  const double lambda = m_widgets.pLambda->text().toDouble();

  asiEngine_RE reApi( m_model,
                      m_notifier,
                      m_plotter );

  // Find Edges Node.
  Handle(asiData_ReEdgesNode) edgesNode = reApi.Get_Edges();
  //
  if ( edgesNode.IsNull() )
  {
    return;
  }

  m_model->OpenCommand();

  // Approximate every edge individually.
  for ( Handle(ActAPI_IChildIterator) eit = edgesNode->GetChildIterator(); eit->More(); eit->Next() )
  {
    Handle(asiData_ReEdgeNode)
      edgeNode = Handle(asiData_ReEdgeNode)::DownCast( eit->Value() );

    // Get B-curve from edge.
    Handle(Geom_BSplineCurve)
      bcurve = Handle(Geom_BSplineCurve)::DownCast( edgeNode->GetCurve() );
    //
    if ( bcurve.IsNull() )
    {
      return;
    }

    // Fair curve.
    Handle(Geom_BSplineCurve) fairedBCurve = FairCurve( bcurve,
                                                        lambda,
                                                        m_notifier );
    //
    if ( fairedBCurve.IsNull() )
    {
      return;
    }

    // Update Data Model.
    edgeNode->SetCurve(fairedBCurve);

    // Update scene.
    m_partViewer->PrsMgr()->Actualize(edgeNode);
  }

  m_model->CommitCommand();
}

//-----------------------------------------------------------------------------

void asiUI_ControlsMesh::onUndoRedo(const Handle(ActAPI_TxRes)& txRes)
{
  if ( txRes.IsNull() )
    return;

  // Loop over the affected Parameters to get the affected Nodes. These Nodes
  // are placed into a map to have them unique.
  ActAPI_NodeIdMap modifiedNodes, deletedNodes;
  //
  for ( int k = 1; k <= txRes->parameterRefs.Extent(); ++k )
  {
    const ActAPI_TxRes::t_parameterRef& objRef = txRes->parameterRefs(k);

    // Get ID of the Node.
    ActAPI_NodeId nodeId = ActData_Common::NodeIdByParameterId(objRef.id);

    // Add Node ID.
    if ( objRef.isAlive )
    {
      if ( !modifiedNodes.Contains(nodeId) )
      {
        modifiedNodes.Add(nodeId);
      }
    }
    else
    {
      if ( !deletedNodes.Contains(nodeId) )
      {
        deletedNodes.Add(nodeId);
      }
    }
  }

  // Get all presentation managers
  const vtkSmartPointer<asiVisu_PrsManager>& partPM = m_partViewer->PrsMgr();

  // Loop over the deleted Nodes to derender them
  for ( int k = 1; k <= deletedNodes.Extent(); ++k )
  {
    const ActAPI_DataObjectId& id = deletedNodes(k);

    if ( partPM->IsPresented(id) )
      partPM->DeRenderPresentation(id);
  }

  // Loop over the modified Nodes to actualize them
  for ( int k = 1; k <= modifiedNodes.Extent(); ++k )
  {
    const ActAPI_DataObjectId& id = modifiedNodes(k);

    // Actualize
    if ( partPM->IsPresented(id) )
      partPM->Actualize( m_model->FindNode(id) );
  }

  // Update object browser
  m_objectBrowser->Populate();
}

void asiUI_ControlsMesh::onUndo()
{
  this->onUndoRedo( m_model->Undo() );
}

//-----------------------------------------------------------------------------

void asiUI_ControlsMesh::onRedo()
{
  this->onUndoRedo( m_model->Redo() );
}
