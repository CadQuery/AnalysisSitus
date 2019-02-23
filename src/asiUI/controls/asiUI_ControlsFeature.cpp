//-----------------------------------------------------------------------------
// Created on: 31 March 2016
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
//    * Neither the name of the copyright holder(s) nor the
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
#include <asiUI_ControlsFeature.h>

// asiUI includes
#include <asiUI_AAGAdaptor.h>
#include <asiUI_Common.h>
#include <asiUI_DialogSmoothAngleTol.h>
#include <asiUI_DialogTopoGraphDepth.h>
#include <asiUI_PartGraph.h>

// asiAlgo includes
#include <asiAlgo_AAGIterator.h>
#include <asiAlgo_CheckDihedralAngle.h>
#include <asiAlgo_DetachFaces.h>
#include <asiAlgo_FindFree.h>
#include <asiAlgo_FindNonmanifold.h>
#include <asiAlgo_FindSameHosts.h>
#include <asiAlgo_MeshConvert.h>
#include <asiAlgo_PLY.h>
#include <asiAlgo_RehostFaces.h>
#include <asiAlgo_SuppressFaces.h>
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// asiVisu includes
#include <asiVisu_PartPrs.h>

// asiEngine includes
#include <asiEngine_Part.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Precision.hxx>
#include <ShapeUpgrade_ShapeDivideAngle.hxx>
#include <ShapeUpgrade_ShapeDivideClosedEdges.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

// Qt include
#pragma warning(push, 0)
#include <QGroupBox>
#pragma warning(pop)

// VTK includes
#pragma warning(push, 0)
#include <vtkActor.h>
#include <vtkProperty.h>
#pragma warning(pop)

//-----------------------------------------------------------------------------

#define BTN_MIN_WIDTH 120

//-----------------------------------------------------------------------------

//! Constructor.
//! \param model       [in] Data Model instance.
//! \param pPartViewer [in] Part viewer.
//! \param notifier    [in] progress notifier.
//! \param plotter     [in] imperative plotter.
//! \param parent      [in] parent widget.
asiUI_ControlsFeature::asiUI_ControlsFeature(const Handle(asiEngine_Model)& model,
                                             asiUI_ViewerPart*              pPartViewer,
                                             ActAPI_ProgressEntry           notifier,
                                             ActAPI_PlotterEntry            plotter,
                                             QWidget*                       parent)
: QWidget      (parent),
  m_model      (model),
  m_partViewer (pPartViewer),
  m_notifier   (notifier),
  m_plotter    (plotter)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_widgets.pShowTOPOGraph       = new QPushButton("Show TOPO graph");
  m_widgets.pShowAAG             = new QPushButton("Show AA graph");
  m_widgets.pElimSelected        = new QPushButton("Show AA graph w/o selected");
  //
  m_widgets.pFreeEdges           = new QPushButton("Find free edges");
  m_widgets.pNonManifoldEdges    = new QPushButton("Find non-manifold edges");
  m_widgets.pCheckDihAngles      = new QPushButton("Classify dihedral angles");
  m_widgets.pFindSmoothEdges     = new QPushButton("Find smooth edges");
  m_widgets.pFindConvexOnly      = new QPushButton("Find convex-only");
  //
  m_widgets.pDetachSelected      = new QPushButton("Detach selected faces");
  m_widgets.pDeleteSelected      = new QPushButton("Delete selected faces");
  m_widgets.pDeleteSelectedFull  = new QPushButton("Delete selected faces (FULL)");
  m_widgets.pDeleteSelectedSmart = new QPushButton("Delete selected faces (smart)");
  m_widgets.pDivideClosedEdges   = new QPushButton("Divide all closed edges");
  m_widgets.pDivideAngle         = new QPushButton("Divide faces by angle");
  m_widgets.pBoundingBox         = new QPushButton("Compute bounding box");
  //
  m_widgets.pShowTOPOGraph       -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pShowAAG             -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pElimSelected        -> setMinimumWidth(BTN_MIN_WIDTH);
  //
  m_widgets.pFreeEdges           -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pNonManifoldEdges    -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pCheckDihAngles      -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pFindSmoothEdges     -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pFindConvexOnly      -> setMinimumWidth(BTN_MIN_WIDTH);
  //
  m_widgets.pDetachSelected      -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pDeleteSelected      -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pDeleteSelectedFull  -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pDivideClosedEdges   -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pDivideAngle         -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pBoundingBox         -> setMinimumWidth(BTN_MIN_WIDTH);

  // Group for analysis
  QGroupBox*   pAnalysisGroup = new QGroupBox("Analysis");
  QVBoxLayout* pAnalysisLay   = new QVBoxLayout(pAnalysisGroup);
  //
  pAnalysisLay->addWidget(m_widgets.pShowTOPOGraph);
  pAnalysisLay->addWidget(m_widgets.pShowAAG);
  pAnalysisLay->addWidget(m_widgets.pElimSelected);

  // Group for features
  QGroupBox*   pFeaturesGroup = new QGroupBox("Features");
  QVBoxLayout* pFeaturesLay   = new QVBoxLayout(pFeaturesGroup);
  //
  pFeaturesLay->addWidget(m_widgets.pFreeEdges);
  pFeaturesLay->addWidget(m_widgets.pNonManifoldEdges);
  pFeaturesLay->addWidget(m_widgets.pCheckDihAngles);
  pFeaturesLay->addWidget(m_widgets.pFindSmoothEdges);
  pFeaturesLay->addWidget(m_widgets.pFindConvexOnly);

  // Group for features
  QGroupBox*   pDyModelingGroup = new QGroupBox("Dynamic Modeling");
  QVBoxLayout* pDyModelingLay   = new QVBoxLayout(pDyModelingGroup);
  //
  pDyModelingLay->addWidget(m_widgets.pDeleteSelected);
  pDyModelingLay->addWidget(m_widgets.pDeleteSelectedFull);
  pDyModelingLay->addWidget(m_widgets.pDeleteSelectedSmart);
  pDyModelingLay->addWidget(m_widgets.pDetachSelected);
  pDyModelingLay->addWidget(m_widgets.pDivideClosedEdges);
  pDyModelingLay->addWidget(m_widgets.pDivideAngle);
  pDyModelingLay->addWidget(m_widgets.pBoundingBox);

  // Set layout
  m_pMainLayout->addWidget(pAnalysisGroup);
  m_pMainLayout->addWidget(pFeaturesGroup);
  m_pMainLayout->addWidget(pDyModelingGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pShowTOPOGraph,       SIGNAL( clicked() ), SLOT( onShowTOPOGraph       () ) );
  connect( m_widgets.pShowAAG,             SIGNAL( clicked() ), SLOT( onShowAAG             () ) );
  connect( m_widgets.pElimSelected,        SIGNAL( clicked() ), SLOT( onElimSelected        () ) );
  //
  connect( m_widgets.pFreeEdges,           SIGNAL( clicked() ), SLOT( onFreeEdges           () ) );
  connect( m_widgets.pNonManifoldEdges,    SIGNAL( clicked() ), SLOT( onNonManifoldEdges    () ) );
  connect( m_widgets.pCheckDihAngles,      SIGNAL( clicked() ), SLOT( onCheckDihedralAngles () ) );
  connect( m_widgets.pFindSmoothEdges,     SIGNAL( clicked() ), SLOT( onFindSmoothEdges     () ) );
  connect( m_widgets.pFindConvexOnly,      SIGNAL( clicked() ), SLOT( onFindConvexOnly      () ) );
  //
  connect( m_widgets.pDetachSelected,      SIGNAL( clicked() ), SLOT( onDetachSelected      () ) );
  connect( m_widgets.pDeleteSelected,      SIGNAL( clicked() ), SLOT( onDeleteSelected      () ) );
  connect( m_widgets.pDeleteSelectedFull,  SIGNAL( clicked() ), SLOT( onDeleteSelectedFull  () ) );
  connect( m_widgets.pDeleteSelectedSmart, SIGNAL( clicked() ), SLOT( onDeleteSelectedSmart () ) );
  connect( m_widgets.pDivideClosedEdges,   SIGNAL( clicked() ), SLOT( onDivideClosedEdges   () ) );
  connect( m_widgets.pDivideAngle,         SIGNAL( clicked() ), SLOT( onDivideAngle         () ) );
  connect( m_widgets.pBoundingBox,         SIGNAL( clicked() ), SLOT( onBoundingBox         () ) );
}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_ControlsFeature::~asiUI_ControlsFeature()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! Shows topology graph.
void asiUI_ControlsFeature::onShowTOPOGraph()
{
  // Access Geometry Node
  Handle(asiData_PartNode) N = m_model->GetPartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // Shape to visualize a graph for
  TopoDS_Shape targetShape;

  // Access active face
  Handle(asiData_FaceNode) FN = N->GetFaceRepresentation();
  if ( FN.IsNull() || !FN->IsWellFormed() || FN->GetSelectedFace() <= 0 )
    targetShape = N->GetShape();
  else
  {
    // Get selected faces
    TColStd_PackedMapOfInteger faceIds;
    asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedFaces(faceIds);

    // Put all faces to compound
    TopoDS_Compound comp;
    BRep_Builder().MakeCompound(comp);
    //
    for ( TColStd_MapIteratorOfPackedMapOfInteger fit(faceIds); fit.More(); fit.Next() )
    {
      TopoDS_Face face = m_model->GetPartNode()->GetAAG()->GetFace( fit.Key() );
      BRep_Builder().Add(comp, face);
    }

    targetShape = comp;
  }

  // No shape, no graph
  if ( targetShape.IsNull() )
  {
    std::cout << "Error: target shape is null" << std::endl;
    return;
  }

  // Show graph
  asiUI_DialogTopoGraphDepth*
    pDlg = new asiUI_DialogTopoGraphDepth(m_model, targetShape, m_partViewer, m_notifier, this);
  //
  pDlg->open();
}

//-----------------------------------------------------------------------------

//! Shows AA graph.
void asiUI_ControlsFeature::onShowAAG()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Access selected faces (if any)
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedSubShapes(selected);

  // Show graph
  asiUI_PartGraph* pGraphView = new asiUI_PartGraph(m_model, m_partViewer);
  pGraphView->RenderAdjacency(part_n->GetAAG(), selected);
}

//-----------------------------------------------------------------------------

//! Eliminates selected faces from AAG.
void asiUI_ControlsFeature::onElimSelected()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get highlighted sub-shapes
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedSubShapes(selected);

  // Build AAG
  Handle(asiAlgo_AAG) aag = new asiAlgo_AAG(part);

  // Remove highlighted sub-shapes
  aag->Remove(selected);

  // Show graph
  asiUI_PartGraph* pGraphView = new asiUI_PartGraph(m_model, m_partViewer);
  pGraphView->SetAAG(aag);
  //
  vtkSmartPointer<vtkMutableUndirectedGraph>
    undirected = asiUI_AAGAdaptor::Convert(aag);
  //
  pGraphView->Render(undirected, part, asiUI_PartGraph::Regime_AAG, false);
}

//-----------------------------------------------------------------------------

//! Finds free edges.
void asiUI_ControlsFeature::onFreeEdges()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Find free edges
  TopTools_IndexedMapOfShape freeEdges;
  asiAlgo_FindFree::FindEdges(part, freeEdges, m_notifier);
  //
  std::cout << "Number of free edges: " << freeEdges.Extent() << std::endl;

  // Prepare compound of edges
  TopoDS_Compound freeEdgesComp;
  BRep_Builder BB;
  BB.MakeCompound(freeEdgesComp);
  //
  for ( int e = 1; e <= freeEdges.Extent(); ++e )
  {
    BB.Add( freeEdgesComp, freeEdges(e) );
  }

  // Save to model
  Handle(asiData_BoundaryEdgesNode) BN = part_n->GetBoundaryEdgesRepresentation();
  //
  m_model->OpenCommand();
  {
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Red) )        ->SetShape( freeEdgesComp );
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Green) )      ->SetShape( TopoDS_Shape() );
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Ordinary)   ) ->SetShape( TopoDS_Shape() );
  }
  m_model->CommitCommand();

  // Update viewer
  Handle(asiVisu_PartPrs)
    NPrs = Handle(asiVisu_PartPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation(part_n) );
  //
  if ( NPrs.IsNull() )
  {
    std::cout << "Error: there is no available presentation for part" << std::endl;
    return;
  }
  //
  NPrs->MainActor()->GetProperty()->SetOpacity(0.5);
  NPrs->GetPipeline(asiVisu_PartPrs::Pipeline_Contour)->Actor()->SetVisibility(0);

  // Actualize presentation of edges
  m_partViewer->PrsMgr()->Actualize( BN.get() );
}

//-----------------------------------------------------------------------------

//! Finds non-manifold edges.
void asiUI_ControlsFeature::onNonManifoldEdges()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Find non-manifold edges
  TopTools_IndexedMapOfShape nmEdges;
  asiAlgo_FindNonmanifold::FindEdges(part, nmEdges, m_notifier);
  //
  std::cout << "Number of non-manifold edges: " << nmEdges.Extent() << std::endl;

  // Prepare compound of edges
  TopoDS_Compound nmEdgesComp;
  BRep_Builder BB;
  BB.MakeCompound(nmEdgesComp);
  //
  for ( int e = 1; e <= nmEdges.Extent(); ++e )
  {
    BB.Add( nmEdgesComp, nmEdges(e) );
  }

  // Save to model
  Handle(asiData_BoundaryEdgesNode) BN = part_n->GetBoundaryEdgesRepresentation();
  //
  m_model->OpenCommand();
  {
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Red) )        ->SetShape( nmEdgesComp );
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Green) )      ->SetShape( TopoDS_Shape() );
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Ordinary)   ) ->SetShape( TopoDS_Shape() );
  }
  m_model->CommitCommand();

  // Update viewer
  Handle(asiVisu_PartPrs)
    NPrs = Handle(asiVisu_PartPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation(part_n) );
  //
  if ( NPrs.IsNull() )
  {
    std::cout << "Error: there is no available presentation for part" << std::endl;
    return;
  }
  //
  NPrs->MainActor()->GetProperty()->SetOpacity(0.5);
  NPrs->GetPipeline(asiVisu_PartPrs::Pipeline_Contour)->Actor()->SetVisibility(0);

  // Actualize presentation of edges
  m_partViewer->PrsMgr()->Actualize( BN.get() );
}

//-----------------------------------------------------------------------------

//! Classifies solid angles as concave / convex.
void asiUI_ControlsFeature::onCheckDihedralAngles()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get highlighted sub-shapes
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedSubShapes(selected);

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
    Handle(asiAlgo_AAG) aag = new asiAlgo_AAG(part);

    TIMER_NEW
    TIMER_GO

    // Loop over the faces
    Handle(asiAlgo_AAGRandomIterator) it = new asiAlgo_AAGRandomIterator(aag);
    for ( ; it->More(); it->Next() )
    {
      const int current_face_idx = it->GetFaceId();
      TColStd_PackedMapOfInteger neighbor_ids;
      it->GetNeighbors(neighbor_ids);
      const TopoDS_Face& current_face = aag->GetFace(current_face_idx);

      // Loop over the neighbors
      for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbor_ids); nit.More(); nit.Next() )
      {
        const int          neighbor_face_idx = nit.Key();
        const TopoDS_Face& neighbor_face     = aag->GetFace(neighbor_face_idx);

        // Check angle between the two faces
        this->classifyDihAngle(current_face,
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
                               false,
                               0.0);
      }
    }

    TIMER_FINISH
    TIMER_COUT_RESULT_NOTIFIER(m_notifier, "Classify dihedral angles")
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
          this->classifyDihAngle(F,
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
                                 false,
                                 0.0);
        }
      }
      else
      {
        // Check angle between the two faces
        this->classifyDihAngle(F,
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
                               false,
                               0.0);
      }
    }
  }

  // Dump
  std::cout << "\tNum. convex  edges: "   << convexEdges.Extent()    << std::endl;
  std::cout << "\tNum. concave edges: "   << concaveEdges.Extent()   << std::endl;
  std::cout << "\tNum. undefined edges: " << undefinedEdges.Extent() << std::endl;
  std::cout << "\tNum. smooth edges: "    << smoothEdges.Extent()    << std::endl;

  // Save to model
  Handle(asiData_BoundaryEdgesNode)
    BN = part_n->GetBoundaryEdgesRepresentation();
  //
  m_model->OpenCommand();
  {
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Green)    ) ->SetShape(convexEdgesComp);
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Red)   )    ->SetShape(concaveEdgesComp);
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Ordinary) ) ->SetShape(undefinedEdgesComp);
  }
  m_model->CommitCommand();

  // Update viewer
  Handle(asiVisu_PartPrs)
    NPrs = Handle(asiVisu_PartPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation(part_n) );
  //
  if ( NPrs.IsNull() )
  {
    std::cout << "Error: there is no available presentation for part" << std::endl;
    return;
  }
  //
  NPrs->MainActor()->GetProperty()->SetOpacity(0.5);
  NPrs->GetPipeline(asiVisu_PartPrs::Pipeline_Contour)->Actor()->SetVisibility(0);

  // Actualize presentation of edges
  m_partViewer->PrsMgr()->Actualize( BN.get() );
}

//-----------------------------------------------------------------------------

//! Finds smooth edges.
void asiUI_ControlsFeature::onFindSmoothEdges()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Ask user to provide a tolerance for detection of smooth angles
  asiUI_DialogSmoothAngleTol* wDlg = new asiUI_DialogSmoothAngleTol(this);
  //
  wDlg->exec();
  //
  const double angTol = wDlg->Tolerance;

  // Get highlighted sub-shapes
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedSubShapes(selected);

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
    Handle(asiAlgo_AAG) aag = new asiAlgo_AAG(part);

    // Loop over the faces
    Handle(asiAlgo_AAGRandomIterator) it = new asiAlgo_AAGRandomIterator(aag);
    //
    for ( ; it->More(); it->Next() )
    {
      const int current_face_idx = it->GetFaceId();
      TColStd_PackedMapOfInteger neighbor_ids;
      it->GetNeighbors(neighbor_ids);
      const TopoDS_Face& current_face = aag->GetFace(current_face_idx);

      // Loop over the neighbors
      for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbor_ids); nit.More(); nit.Next() )
      {
        const int          neighbor_face_idx = nit.Key();
        const TopoDS_Face& neighbor_face     = aag->GetFace(neighbor_face_idx);

        // Check angle between the two faces
        this->classifyDihAngle(current_face,
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
                               true,
                               angTol);
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
        this->classifyDihAngle(F,
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
                               true,
                               angTol);
      }
    }
  }

  // Dump
  std::cout << "\tNum. convex  edges: "   << convexEdges.Extent()    << std::endl;
  std::cout << "\tNum. concave edges: "   << concaveEdges.Extent()   << std::endl;
  std::cout << "\tNum. undefined edges: " << undefinedEdges.Extent() << std::endl;
  std::cout << "\tNum. smooth edges: "    << smoothEdges.Extent()    << std::endl;

  // Save to model
  Handle(asiData_BoundaryEdgesNode)
    BN = part_n->GetBoundaryEdgesRepresentation();
  //
  m_model->OpenCommand();
  {
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Ordinary) ) ->SetShape( TopoDS_Shape() );
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Red) )      ->SetShape( TopoDS_Shape() );
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Green) )    ->SetShape(smoothEdgesComp);
  }
  m_model->CommitCommand();

  // Update viewer
  Handle(asiVisu_PartPrs)
    NPrs = Handle(asiVisu_PartPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation(part_n) );
  //
  if ( NPrs.IsNull() )
  {
    std::cout << "Error: there is no available presentation for part" << std::endl;
    return;
  }
  //
  NPrs->MainActor()->GetProperty()->SetOpacity(0.5);
  //
  NPrs->GetPipeline(asiVisu_PartPrs::Pipeline_Contour)->Actor()->SetVisibility(0);
  //
  m_partViewer->PrsMgr()->Actualize( BN.get() );
}

//-----------------------------------------------------------------------------

//! Identifies convex-only faces.
void asiUI_ControlsFeature::onFindConvexOnly()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Build AAG
  Handle(asiAlgo_AAG) aag = new asiAlgo_AAG(part);

  // Find convex-only faces
  TopTools_IndexedMapOfShape convex;
  if ( !aag->FindConvexOnly(convex) )
  {
    std::cout << "There are no convex-only faces in AAG" << std::endl;
    return;
  }

  // Highlight sub-shape
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).HighlightSubShapes(convex);
}

//-----------------------------------------------------------------------------

//! Detaches selected faces.
void asiUI_ControlsFeature::onDetachSelected()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get highlighted faces
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedSubShapes(selected);

  TIMER_NEW
  TIMER_GO

  // Deatch selected faces
  asiAlgo_DetachFaces detacher(part);
  if ( !detacher.Perform(selected) )
  {
    std::cout << "Error: cannot detach faces" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Detach faces")

  const TopoDS_Shape& result = detacher.Result();

  // Update part
  m_model->OpenCommand(); // tx start
  {
    asiEngine_Part( m_model, m_partViewer->PrsMgr() ).Update(result);
  }
  m_model->CommitCommand(); // tx commit

  // Update viewer
  m_partViewer->PrsMgr()->Actualize( part_n.get() );
}

//-----------------------------------------------------------------------------

//! Deletes selected faces.
void asiUI_ControlsFeature::onDeleteSelected()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get highlighted faces
  TColStd_PackedMapOfInteger selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedFaces(selected);

  TIMER_NEW
  TIMER_GO

  // Delete selected faces
  asiAlgo_SuppressFaces eraser(part);
  if ( !eraser.Perform(selected, true) )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "Cannot delete faces.");
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(m_notifier, "Faces deletion");

  const TopoDS_Shape& result = eraser.GetResult();

  // Update part
  m_model->OpenCommand(); // tx start
  {
    asiEngine_Part( m_model, m_partViewer->PrsMgr() ).Update(result);
  }
  m_model->CommitCommand(); // tx commit

  // Update viewer
  m_partViewer->PrsMgr()->Actualize( part_n.get() );
}

//-----------------------------------------------------------------------------

//! Deletes selected faces with all underlying topology.
void asiUI_ControlsFeature::onDeleteSelectedFull()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get highlighted faces
  TColStd_PackedMapOfInteger selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedFaces(selected);

  TIMER_NEW
  TIMER_GO

  // Delete selected faces
  asiAlgo_SuppressFaces eraser(part);
  if ( !eraser.Perform(selected, false) )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "Cannot delete faces.");
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(m_notifier, "FULL faces deletion");

  const TopoDS_Shape& result = eraser.GetResult();

  // Update part
  m_model->OpenCommand(); // tx start
  {
    asiEngine_Part( m_model, m_partViewer->PrsMgr() ).Update(result);
  }
  m_model->CommitCommand(); // tx commit

  // Update viewer
  m_partViewer->PrsMgr()->Actualize( part_n.get() );
}

//-----------------------------------------------------------------------------

//! Deletes selected faces and stitches the neighbor ones.
void asiUI_ControlsFeature::onDeleteSelectedSmart()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get highlighted faces.
  TColStd_PackedMapOfInteger selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedFaces(selected);

  // Repack faces.
  TopTools_ListOfShape faces2Kill;
  for ( TColStd_MapIteratorOfPackedMapOfInteger fit(selected); fit.More(); fit.Next() )
  {
    const TopoDS_Shape& faceSh = part_n->GetAAG()->GetMapOfFaces()( fit.Key() );
    faces2Kill.Append(faceSh);
  }

  TIMER_NEW
  TIMER_GO

  // Perform Boolean operation for face removal.
  TopoDS_Shape result;
  //
  if ( !asiAlgo_Utils::BooleanRemoveFaces( part,
                                           faces2Kill,
                                           false, // Parallel mode.
                                           true, // Whether to track history.
                                           result,
                                           m_notifier ) )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "Smart face removal failed.");
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(m_notifier, "Smart faces deletion");

  // Update part.
  m_model->OpenCommand(); // tx start.
  {
    asiEngine_Part( m_model, m_partViewer->PrsMgr() ).Update(result);
  }
  m_model->CommitCommand(); // tx commit.

  // Update viewer.
  m_partViewer->PrsMgr()->Actualize( part_n.get() );
}

//-----------------------------------------------------------------------------

//! Divides all closed edges.
void asiUI_ControlsFeature::onDivideClosedEdges()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  TIMER_NEW
  TIMER_GO

  // Divide closed edges
  ShapeUpgrade_ShapeDivideClosedEdges divider(part);
  divider.SetNbSplitPoints(1);
  //
  if ( !divider.Perform() )
  {
    std::cout << "Error: cannot divide edges" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Divide closed edges")

  const TopoDS_Shape& result = divider.Result();

  // Update part
  m_model->OpenCommand(); // tx start
  {
    asiEngine_Part( m_model, m_partViewer->PrsMgr() ).Update(result);
  }
  m_model->CommitCommand(); // tx commit

  // Update viewer
  m_partViewer->PrsMgr()->Actualize( part_n.get() );
}

//-----------------------------------------------------------------------------

//! Divides faces by angle.
void asiUI_ControlsFeature::onDivideAngle()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  TIMER_NEW
  TIMER_GO

  // Divide
  ShapeUpgrade_ShapeDivideAngle divider(45.0, part);
  //
  if ( !divider.Perform() )
  {
    std::cout << "Error: cannot divide by angle" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Divide by angle")

  const TopoDS_Shape& result = divider.Result();

  // Update part
  m_model->OpenCommand(); // tx start
  {
    asiEngine_Part( m_model, m_partViewer->PrsMgr() ).Update(result);
  }
  m_model->CommitCommand(); // tx commit

  // Update viewer
  m_partViewer->PrsMgr()->Actualize( part_n.get() );
}

//-----------------------------------------------------------------------------

//! Computes bounding box.
void asiUI_ControlsFeature::onBoundingBox()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  double xMin, yMin, zMin, xMax, yMax, zMax;
  asiAlgo_Utils::Bounds(part, xMin, yMin, zMin, xMax, yMax, zMax);
  //
  m_notifier.SendLogMessage( LogInfo(Normal) << "Bounding box:\n"
                                                "\t X min = %1\n"
                                                "\t Y min = %2\n"
                                                "\t Z min = %3\n"
                                                "\t X max = %4\n"
                                                "\t Y max = %5\n"
                                                "\t Z max = %6"
                                             << xMin << yMin << zMin
                                             << xMax << yMax << zMax );

  const double inf = Precision::Infinite()*0.1;
  //
  if ( Abs(xMin) >= inf || Abs(yMin) >= inf || Abs(zMin) >= inf ||
       Abs(xMax) >= inf || Abs(yMax) >= inf || Abs(zMax) >= inf )
  {
    m_notifier.SendLogMessage(LogWarn(Normal) << "Bounding box is infitine.");
    return;
  }

  // Create bounding box to draw it
  TopoDS_Shape bndbox = BRepPrimAPI_MakeBox( gp_Pnt(xMin, yMin, zMin), gp_Pnt(xMax, yMax, zMax) );
  //
  m_plotter.DRAW_SHAPE(bndbox, Color_Yellow, 1.0, true, "bounding box");
}

//-----------------------------------------------------------------------------

void asiUI_ControlsFeature::classifyDihAngle(const TopoDS_Face&          F,
                                             const TopoDS_Face&          G,
                                             TopTools_IndexedMapOfShape& convexEdges,
                                             TopTools_IndexedMapOfShape& concaveEdges,
                                             TopTools_IndexedMapOfShape& undefinedEdges,
                                             TopTools_IndexedMapOfShape& smoothEdges,
                                             TopoDS_Compound&            convexEdgesComp,
                                             TopoDS_Compound&            concaveEdgesComp,
                                             TopoDS_Compound&            undefinedEdgesComp,
                                             TopoDS_Compound&            smoothEdgesComp,
                                             const bool                  verboseOutput,
                                             const bool                  allowSmooth,
                                             const double                smoothAngularTol) const
{
  // Check angle between the two faces
  TopTools_IndexedMapOfShape commonEdges;
  asiAlgo_CheckDihedralAngle dihAngle(m_notifier,
                                      verboseOutput ? m_plotter : NULL);
  //
  double angRad = 0.0;

  asiAlgo_FeatureAngleType
    angleType = dihAngle.AngleBetweenFaces(F,
                                           G,
                                           allowSmooth,
                                           smoothAngularTol,
                                           commonEdges,
                                           angRad);

  TopTools_IndexedMapOfShape* pTargetMap;
  TopoDS_Compound*            pTargetComp;
  //
  if ( angleType == FeatureAngleType_Convex )
  {
    pTargetMap  = &convexEdges;
    pTargetComp = &convexEdgesComp;
  }
  else if ( angleType == FeatureAngleType_Concave )
  {
    pTargetMap  = &concaveEdges;
    pTargetComp = &concaveEdgesComp;
  }
  else if ( angleType == FeatureAngleType_Smooth )
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

  if ( verboseOutput )
    m_notifier.SendLogMessage(LogInfo(Normal) << "Angle between faces: %1." << angRad*180.0/M_PI);
}
