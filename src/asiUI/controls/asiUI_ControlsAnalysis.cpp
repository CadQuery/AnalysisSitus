//-----------------------------------------------------------------------------
// Created on: 31 March 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Sergey Slyadnev
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
#include <asiUI_ControlsAnalysis.h>

// asiUI includes
#include <asiUI_AAGAdaptor.h>
#include <asiUI_Common.h>
#include <asiUI_DialogEuler.h>
#include <asiUI_DialogSmoothAngleTol.h>
#include <asiUI_DialogTopoGraphDepth.h>
#include <asiUI_PartGraph.h>

// asiAlgo includes
#include <asiAlgo_AAGIterator.h>
#include <asiAlgo_CheckDihedralAngle.h>
#include <asiAlgo_CheckValidity.h>
#include <asiAlgo_FindFree.h>
#include <asiAlgo_FindNonmanifold.h>
#include <asiAlgo_PatchJointAdaptor.h>
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// asiVisu includes
#include <asiVisu_PartPrs.h>

// asiEngine includes
#include <asiEngine_Curve.h>
#include <asiEngine_Part.h>
#include <asiEngine_TolerantShapes.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <GProp_GProps.hxx>
#include <TopExp_Explorer.hxx>

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

asiUI_ControlsAnalysis::asiUI_ControlsAnalysis(const Handle(asiEngine_Model)& model,
                                               asiUI_ViewerPart*              pPartViewer,
                                               asiUI_ObjectBrowser*           pBrowser,
                                               ActAPI_ProgressEntry           notifier,
                                               ActAPI_PlotterEntry            plotter,
                                               QWidget*                       parent)
: QScrollArea  (parent),
  m_model      (model),
  m_partViewer (pPartViewer),
  m_browser    (pBrowser),
  m_notifier   (notifier),
  m_plotter    (plotter)
{
  // Main layout.
  m_pMainLayout = new QVBoxLayout;

  // Create buttons.
  m_widgets.Validity.pDiagnose             = new QPushButton("Diagnose");
  m_widgets.Validity.pCheckToler           = new QPushButton("Check tolerances");
  m_widgets.Validity.pCheckEuler           = new QPushButton("Check Euler-Poincare");
  m_widgets.Validity.pCheckContours        = new QPushButton("Check contours");
  m_widgets.Validity.pFindOpenEdges        = new QPushButton("Find open edges");
  m_widgets.Validity.pFindNonManifoldEdges = new QPushButton("Find non-manifold edges");
  //
  m_widgets.Features.pShowTopoGraph    = new QPushButton("Show topology graph");
  m_widgets.Features.pShowAAG          = new QPushButton("Show AAG");
  m_widgets.Features.pShowAAGWoSel     = new QPushButton("Show AAG w/o selected");
  m_widgets.Features.pCheckDihAngles   = new QPushButton("Classify dihedral angles");
  m_widgets.Features.pCheckG1Crossings = new QPushButton("Classify G1 crossings");
  m_widgets.Features.pFindSmoothEdges  = new QPushButton("Find smooth edges");
  m_widgets.Features.pFindConvexOnly   = new QPushButton("Find convex-only faces");
  //
  m_widgets.Properties.pEdgeCurvature = new QPushButton("Check edge curvature");
  m_widgets.Properties.pEdgeLength    = new QPushButton("Check edge length");
  m_widgets.Properties.pEdgeStrain    = new QPushButton("Check edge strain");
  m_widgets.Properties.pFaceArea      = new QPushButton("Check face area");
  m_widgets.Properties.pFaceEnergy    = new QPushButton("Check face bending");
  m_widgets.Properties.pAABB          = new QPushButton("Compute bounding box");

  // Group for validity checks.
  QGroupBox*   pValidityGroup = new QGroupBox("Validity");
  QVBoxLayout* pValidityLay   = new QVBoxLayout(pValidityGroup);
  //
  pValidityLay->addWidget(m_widgets.Validity.pDiagnose);
  pValidityLay->addWidget(m_widgets.Validity.pCheckToler);
  pValidityLay->addWidget(m_widgets.Validity.pCheckEuler);
  pValidityLay->addWidget(m_widgets.Validity.pCheckContours);
  pValidityLay->addWidget(m_widgets.Validity.pFindOpenEdges);
  pValidityLay->addWidget(m_widgets.Validity.pFindNonManifoldEdges);

  // Group for features.
  QGroupBox*   pFeaturesGroup = new QGroupBox("Features");
  QVBoxLayout* pFeaturesLay   = new QVBoxLayout(pFeaturesGroup);
  //
  pFeaturesLay->addWidget(m_widgets.Features.pShowTopoGraph);
  pFeaturesLay->addWidget(m_widgets.Features.pShowAAG);
  pFeaturesLay->addWidget(m_widgets.Features.pShowAAGWoSel);
  pFeaturesLay->addWidget(m_widgets.Features.pCheckDihAngles);
  pFeaturesLay->addWidget(m_widgets.Features.pCheckG1Crossings);
  pFeaturesLay->addWidget(m_widgets.Features.pFindSmoothEdges);
  pFeaturesLay->addWidget(m_widgets.Features.pFindConvexOnly);

  // Group for property checks.
  QGroupBox*   pPropsGroup = new QGroupBox("Properties");
  QVBoxLayout* pPropsLay   = new QVBoxLayout(pPropsGroup);
  //
  pPropsLay->addWidget(m_widgets.Properties.pEdgeCurvature);
  pPropsLay->addWidget(m_widgets.Properties.pEdgeLength);
  pPropsLay->addWidget(m_widgets.Properties.pEdgeStrain);
  pPropsLay->addWidget(m_widgets.Properties.pFaceArea);
  pPropsLay->addWidget(m_widgets.Properties.pFaceEnergy);
  pPropsLay->addWidget(m_widgets.Properties.pAABB);

  // Set layout.
  m_pMainLayout->addWidget(pValidityGroup);
  m_pMainLayout->addWidget(pFeaturesGroup);
  m_pMainLayout->addWidget(pPropsGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  QWidget* pMainWidget = new QWidget(this);
  pMainWidget->setLayout(m_pMainLayout);
  //
  this->setWidgetResizable(true);
  this->setFrameStyle(QFrame::NoFrame);
  this->setWidget(pMainWidget);

  // Connect signals to slots
  connect( m_widgets.Validity.pDiagnose,             SIGNAL( clicked() ), SLOT( onDiagnose             () ) );
  connect( m_widgets.Validity.pCheckToler,           SIGNAL( clicked() ), SLOT( onCheckToler           () ) );
  connect( m_widgets.Validity.pCheckEuler,           SIGNAL( clicked() ), SLOT( onCheckEuler           () ) );
  connect( m_widgets.Validity.pCheckContours,        SIGNAL( clicked() ), SLOT( onCheckContours        () ) );
  connect( m_widgets.Validity.pFindOpenEdges,        SIGNAL( clicked() ), SLOT( onFindOpenEdges        () ) );
  connect( m_widgets.Validity.pFindNonManifoldEdges, SIGNAL( clicked() ), SLOT( onFindNonManifoldEdges () ) );
  //
  connect( m_widgets.Features.pShowTopoGraph,    SIGNAL( clicked() ), SLOT( onShowTopoGraph    () ) );
  connect( m_widgets.Features.pShowAAG,          SIGNAL( clicked() ), SLOT( onShowAAG          () ) );
  connect( m_widgets.Features.pShowAAGWoSel,     SIGNAL( clicked() ), SLOT( onShowAAGWoSel     () ) );
  connect( m_widgets.Features.pCheckDihAngles,   SIGNAL( clicked() ), SLOT( onCheckDihAngles   () ) );
  connect( m_widgets.Features.pCheckG1Crossings, SIGNAL( clicked() ), SLOT( onCheckG1Crossings () ) );
  connect( m_widgets.Features.pFindSmoothEdges,  SIGNAL( clicked() ), SLOT( onFindSmoothEdges  () ) );
  connect( m_widgets.Features.pFindConvexOnly,   SIGNAL( clicked() ), SLOT( onFindConvexOnly   () ) );
  //
  connect( m_widgets.Properties.pEdgeCurvature, SIGNAL( clicked() ), SLOT( onEdgeCurvature () ) );
  connect( m_widgets.Properties.pEdgeLength,    SIGNAL( clicked() ), SLOT( onEdgeLength    () ) );
  connect( m_widgets.Properties.pEdgeStrain,    SIGNAL( clicked() ), SLOT( onEdgeStrain    () ) );
  connect( m_widgets.Properties.pFaceArea,      SIGNAL( clicked() ), SLOT( onFaceArea      () ) );
  connect( m_widgets.Properties.pFaceEnergy,    SIGNAL( clicked() ), SLOT( onFaceEnergy    () ) );
  connect( m_widgets.Properties.pAABB,          SIGNAL( clicked() ), SLOT( onAABB          () ) );
}

//-----------------------------------------------------------------------------

void asiUI_ControlsAnalysis::onDiagnose()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  asiAlgo_CheckValidity checker(m_notifier, m_plotter);

  // Get highlighted faces
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedSubShapes(selected);

  if ( selected.IsEmpty() )
  {
    if ( !checker.CheckBasic(part) )
    {
      m_notifier.SendLogMessage(LogErr(Normal) << "Basic validity checker returns false.");
      return;
    }
    m_notifier.SendLogMessage(LogInfo(Normal) << "Shape is correct.");
  }
  else
  {
    for ( int i = 1; i <= selected.Extent(); ++i )
    {
      if ( !checker.CheckBasic( selected(i) ) )
      {
        m_notifier.SendLogMessage(LogErr(Normal) << "Sub-shape is invalid.");
        return;
      }
      m_notifier.SendLogMessage(LogInfo(Normal) << "Sub-shape is correct.");
    }
  }
}

//-----------------------------------------------------------------------------

void asiUI_ControlsAnalysis::onCheckToler()
{
  // Attempt to get the highlighted sub-shapes. If no sub-shapes are selected,
  // we work on the full part.
  TopTools_IndexedMapOfShape selectedSubShapes;
  //
  asiEngine_Part PartAPI(m_model,
                         m_partViewer->PrsMgr(),
                         m_notifier, m_plotter);
  //
  PartAPI.GetHighlightedSubShapes(selectedSubShapes);

  // Choose shape to check.
  TopoDS_Shape shape2Check;
  //
  if ( selectedSubShapes.Extent() )
  {
    TopoDS_Compound comp;
    BRep_Builder().MakeCompound(comp);
    //
    for ( int k = 1; k <= selectedSubShapes.Extent(); ++k )
      BRep_Builder().Add( comp, selectedSubShapes(k) );

    shape2Check = comp;
  }
  else
  {
    shape2Check = m_model->GetPartNode()->GetShape();
  }

  // Prepare API to analyze tolerances.
  asiEngine_TolerantShapes TolInfo(m_model,
                                   m_partViewer->PrsMgr(),
                                   m_notifier, m_plotter);

  // Perform tolerance analysis.
  m_model->OpenCommand();
  {
    TolInfo.Clean_All();
    TolInfo.Populate(shape2Check, 10);
  }
  m_model->CommitCommand();

  // Output max tolerance.
  m_notifier.SendLogMessage( LogInfo(Normal) << "Max tolerance: %1."
                                             << asiAlgo_CheckValidity().MaxTolerance(shape2Check) );

  // Update UI.
  m_browser->Populate();
}

//-----------------------------------------------------------------------------

void asiUI_ControlsAnalysis::onCheckEuler()
{
  asiUI_DialogEuler* pEuler = new asiUI_DialogEuler(m_model,
                                                    m_notifier,
                                                    this);
  //
  pEuler->show();
}

//-----------------------------------------------------------------------------

void asiUI_ControlsAnalysis::onCheckContours()
{
  double globTolerance = 0.0;

  // Get Part Node.
  Handle(asiData_PartNode) part_n = m_model->GetPartNode();

  // Get Part shape.
  TopoDS_Shape partSh = part_n->GetShape();

  asiAlgo_CheckValidity checker;

  // Check each face individually.
  bool isOk = true;
  //
  for ( TopExp_Explorer exp(partSh, TopAbs_FACE); exp.More(); exp.Next() )
  {
    const TopoDS_Face& face = TopoDS::Face( exp.Current() );

    // Set default tolerance.
    double locTolerance;
    if ( globTolerance )
      locTolerance = globTolerance;
    else
      locTolerance = checker.MaxTolerance(face)*5.0;

    // Check closeness.
    if ( !checker.HasAllClosedWires(face, locTolerance) )
    {
      isOk = false;
      break;
    }
  }

  if ( isOk )
    m_notifier.SendLogMessage(LogInfo(Normal) << "All faces have closed contours.");
  else
    m_notifier.SendLogMessage(LogWarn(Normal) << "Some contours contain gaps.");
}

//-----------------------------------------------------------------------------

void asiUI_ControlsAnalysis::onFindOpenEdges()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Find free edges.
  TopTools_IndexedMapOfShape freeEdges;
  asiAlgo_FindFree::FindEdges(part, freeEdges, m_notifier);
  //
  m_notifier.SendLogMessage( LogInfo(Normal) << "Number of free edges: %1."
                                             << freeEdges.Extent() );

  // Prepare compound of edges.
  TopoDS_Compound freeEdgesComp;
  BRep_Builder BB;
  BB.MakeCompound(freeEdgesComp);
  //
  for ( int e = 1; e <= freeEdges.Extent(); ++e )
  {
    BB.Add( freeEdgesComp, freeEdges(e) );
  }

  // Save to model.
  Handle(asiData_BoundaryEdgesNode) BN = part_n->GetBoundaryEdgesRepresentation();
  //
  m_model->OpenCommand();
  {
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Red) )        ->SetShape( freeEdgesComp );
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Green) )      ->SetShape( TopoDS_Shape() );
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Ordinary)   ) ->SetShape( TopoDS_Shape() );
  }
  m_model->CommitCommand();

  // Update viewer.
  Handle(asiVisu_PartPrs)
    NPrs = Handle(asiVisu_PartPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation(part_n) );
  //
  if ( NPrs.IsNull() )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "There is no available presentation for part.");
    return;
  }
  //
  NPrs->MainActor()->GetProperty()->SetOpacity(0.5);
  NPrs->GetPipeline(asiVisu_PartPrs::Pipeline_Contour)->Actor()->SetVisibility(0);

  // Actualize presentation of edges.
  m_partViewer->PrsMgr()->Actualize(BN);
}

//-----------------------------------------------------------------------------

void asiUI_ControlsAnalysis::onFindNonManifoldEdges()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Find non-manifold edges.
  TopTools_IndexedMapOfShape nmEdges;
  asiAlgo_FindNonmanifold::FindEdges(part, nmEdges, m_notifier);
  //
  m_notifier.SendLogMessage( LogInfo(Normal) << "Number of non-manifold edges: %1."
                                             << nmEdges.Extent() );

  // Prepare compound of edges.
  TopoDS_Compound nmEdgesComp;
  BRep_Builder BB;
  BB.MakeCompound(nmEdgesComp);
  //
  for ( int e = 1; e <= nmEdges.Extent(); ++e )
  {
    BB.Add( nmEdgesComp, nmEdges(e) );
  }

  // Save to model.
  Handle(asiData_BoundaryEdgesNode) BN = part_n->GetBoundaryEdgesRepresentation();
  //
  m_model->OpenCommand();
  {
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Red) )        ->SetShape( nmEdgesComp );
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Green) )      ->SetShape( TopoDS_Shape() );
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Ordinary)   ) ->SetShape( TopoDS_Shape() );
  }
  m_model->CommitCommand();

  // Update viewer.
  Handle(asiVisu_PartPrs)
    NPrs = Handle(asiVisu_PartPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation(part_n) );
  //
  if ( NPrs.IsNull() )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "There is no available presentation for part.");
    return;
  }
  //
  NPrs->MainActor()->GetProperty()->SetOpacity(0.5);
  NPrs->GetPipeline(asiVisu_PartPrs::Pipeline_Contour)->Actor()->SetVisibility(0);

  // Actualize presentation of edges.
  m_partViewer->PrsMgr()->Actualize(BN);
}

//-----------------------------------------------------------------------------

//! Shows topology graph.
void asiUI_ControlsAnalysis::onShowTopoGraph()
{
  // Access Geometry Node.
  Handle(asiData_PartNode) N = m_model->GetPartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // Shape to visualize a graph for.
  TopoDS_Shape targetShape;

  // Access active face.
  Handle(asiData_FaceNode) FN = N->GetFaceRepresentation();
  if ( FN.IsNull() || !FN->IsWellFormed() || FN->GetSelectedFace() <= 0 )
    targetShape = N->GetShape();
  else
  {
    // Get selected faces.
    TColStd_PackedMapOfInteger faceIds;
    asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedFaces(faceIds);

    // Put all faces to compound.
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

  // No shape, no graph.
  if ( targetShape.IsNull() )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "Target shape is null.");
    return;
  }

  // Show graph.
  asiUI_DialogTopoGraphDepth*
    pDlg = new asiUI_DialogTopoGraphDepth(m_model, targetShape, m_partViewer, m_notifier, this);
  //
  pDlg->open();
}

//-----------------------------------------------------------------------------

//! Shows AA graph.
void asiUI_ControlsAnalysis::onShowAAG()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Access selected faces (if any).
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedSubShapes(selected);

  // Show graph.
  asiUI_PartGraph* pGraphView = new asiUI_PartGraph(m_model, m_partViewer, m_notifier);
  pGraphView->RenderAdjacency(part_n->GetAAG(), selected);
}

//-----------------------------------------------------------------------------

//! Eliminates selected faces from AAG.
void asiUI_ControlsAnalysis::onShowAAGWoSel()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get highlighted sub-shapes.
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedSubShapes(selected);

  // Build AAG
  Handle(asiAlgo_AAG) aag = new asiAlgo_AAG(part);

  // Remove highlighted sub-shapes.
  aag->Remove(selected);

  // Show graph.
  asiUI_PartGraph* pGraphView = new asiUI_PartGraph(m_model, m_partViewer, m_notifier);
  pGraphView->SetAAG(aag);
  //
  vtkSmartPointer<vtkMutableUndirectedGraph>
    undirected = asiUI_AAGAdaptor::Convert(aag);
  //
  pGraphView->Render(undirected, part, asiUI_PartGraph::Regime_AAG, false);
}

//-----------------------------------------------------------------------------

void asiUI_ControlsAnalysis::onCheckDihAngles()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get highlighted sub-shapes.
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedSubShapes(selected);

  // Holders for geometries.
  TopTools_IndexedMapOfShape convexEdges,     concaveEdges,     undefinedEdges,     smoothEdges;
  TopoDS_Compound            convexEdgesComp, concaveEdgesComp, undefinedEdgesComp, smoothEdgesComp;
  //
  BRep_Builder BB;
  BB.MakeCompound(convexEdgesComp);
  BB.MakeCompound(concaveEdgesComp);
  BB.MakeCompound(undefinedEdgesComp);
  BB.MakeCompound(smoothEdgesComp);

  // Perform either global or local analysis.
  if ( selected.IsEmpty() )
  {
    // Build AAG.
    Handle(asiAlgo_AAG) aag = new asiAlgo_AAG(part);

    TIMER_NEW
    TIMER_GO

    // Loop over the faces.
    Handle(asiAlgo_AAGRandomIterator) it = new asiAlgo_AAGRandomIterator(aag);
    for ( ; it->More(); it->Next() )
    {
      const int current_face_idx = it->GetFaceId();
      TColStd_PackedMapOfInteger neighbor_ids;
      it->GetNeighbors(neighbor_ids);
      const TopoDS_Face& current_face = aag->GetFace(current_face_idx);

      // Loop over the neighbors.
      for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbor_ids); nit.More(); nit.Next() )
      {
        const int          neighbor_face_idx = nit.Key();
        const TopoDS_Face& neighbor_face     = aag->GetFace(neighbor_face_idx);

        // Check angle between the two faces.
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
                               0.001);
      }
    }

    TIMER_FINISH
    TIMER_COUT_RESULT_NOTIFIER(m_notifier, "Classify dihedral angles")
  }
  else
  {
    m_notifier.SendLogMessage(LogNotice(Normal) << "Running local analysis of edges.");

    TopoDS_Face twin;
    if ( selected.Extent() < 2 )
    {
      m_notifier.SendLogMessage(LogWarn(Normal) << "At least 2 faces are expected.");

      // Analyze the same face twice.
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

          // Check angle between the two faces.
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
        // Check angle between the two faces.
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

  // Dump.
  m_notifier.SendLogMessage( LogInfo(Normal) << "Num. convex edges: %1."    << convexEdges.Extent()    );
  m_notifier.SendLogMessage( LogInfo(Normal) << "Num. concave edges: %1."   << concaveEdges.Extent()   );
  m_notifier.SendLogMessage( LogInfo(Normal) << "Num. undefined edges: %1." << undefinedEdges.Extent() );
  m_notifier.SendLogMessage( LogInfo(Normal) << "Num. smooth edges: %1."    << smoothEdges.Extent()    );

  // Save to model.
  Handle(asiData_BoundaryEdgesNode)
    BN = part_n->GetBoundaryEdgesRepresentation();
  //
  m_model->OpenCommand();
  {
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Green)    ) ->SetShape(convexEdgesComp);
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Red)      ) ->SetShape(concaveEdgesComp);
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Ordinary) ) ->SetShape(undefinedEdgesComp);
  }
  m_model->CommitCommand();

  // Update viewer
  Handle(asiVisu_PartPrs)
    NPrs = Handle(asiVisu_PartPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation(part_n) );
  //
  if ( NPrs.IsNull() )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "There is no available presentation for part.");
    return;
  }
  //
  NPrs->MainActor()->GetProperty()->SetOpacity(0.5);
  NPrs->GetPipeline(asiVisu_PartPrs::Pipeline_Contour)->Actor()->SetVisibility(0);

  // Actualize presentation of edges.
  m_partViewer->PrsMgr()->Actualize(BN);
}

//-----------------------------------------------------------------------------

void asiUI_ControlsAnalysis::onCheckG1Crossings()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get AAG to access the edges.
  Handle(asiAlgo_AAG) aag = part_n->GetAAG();
  //
  if ( aag.IsNull() )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "AAG is not initialized.");
    return;
  }

  // Holders for geometries.
  TopTools_IndexedMapOfShape smoothEdges, sharpEdges;
  TopoDS_Compound            smoothEdgesComp, sharpEdgesComp;
  //
  BRep_Builder BB;
  BB.MakeCompound(smoothEdgesComp);
  BB.MakeCompound(sharpEdgesComp);

  // Angular tolerance.
  const double angToler = 1.0*M_PI/180.;

  // Check edges.
  const TopTools_IndexedDataMapOfShapeListOfShape&
    efMap = aag->RequestMapOfEdgesFaces();
  //
  for ( int eidx = 1; eidx <= efMap.Extent(); ++eidx )
  {
    const TopoDS_Edge&          edge  = TopoDS::Edge( efMap.FindKey(eidx) );
    const TopTools_ListOfShape& faces = efMap.FindFromIndex(eidx);
    //
    if ( faces.Extent() != 2 )
      continue; // Skip non-manifold and open edges.

    const TopoDS_Face& F = TopoDS::Face( faces.First() );
    const TopoDS_Face& G = TopoDS::Face( faces.Last() );

    // Get edge curve.
    double ef, el;
    Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, ef, el);

    // Get host surfaces.
    Handle(Geom_BSplineSurface)
      SF = Handle(Geom_BSplineSurface)::DownCast( BRep_Tool::Surface(F) );
    //
    Handle(Geom_BSplineSurface)
      SG = Handle(Geom_BSplineSurface)::DownCast( BRep_Tool::Surface(G) );
    //
    if ( SF.IsNull() || SG.IsNull() )
    {
      m_notifier.SendLogMessage(LogWarn(Normal) << "Only B-surfaces are supported for G1/non-G1 check.");
      continue;
    }

    // Check transition smoothness.
    asiAlgo_PatchJointAdaptor adt(curve, SF, SG, m_notifier);
    //
    if ( adt.IsG1(angToler) )
    {
      smoothEdges.Add(edge);
      BB.Add(smoothEdgesComp, edge);
    }
    else
    {
      sharpEdges.Add(edge);
      BB.Add(sharpEdgesComp, edge);
    }
  }

  // Dump.
  m_notifier.SendLogMessage( LogInfo(Normal) << "Num. smooth edges: %1." << smoothEdges.Extent() );
  m_notifier.SendLogMessage( LogInfo(Normal) << "Num. sharp edges: %1."  << sharpEdges.Extent() );

  // Save to model.
  Handle(asiData_BoundaryEdgesNode)
    BN = part_n->GetBoundaryEdgesRepresentation();
  //
  m_model->OpenCommand();
  {
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Green) ) ->SetShape(smoothEdgesComp);
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Red)   ) ->SetShape(sharpEdgesComp);
  }
  m_model->CommitCommand();

  // Update viewer
  Handle(asiVisu_PartPrs)
    NPrs = Handle(asiVisu_PartPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation(part_n) );
  //
  if ( NPrs.IsNull() )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "There is no available presentation for part.");
    return;
  }
  //
  NPrs->MainActor()->GetProperty()->SetOpacity(0.5);
  NPrs->GetPipeline(asiVisu_PartPrs::Pipeline_Contour)->Actor()->SetVisibility(0);

  // Actualize presentation of edges.
  m_partViewer->PrsMgr()->Actualize(BN);
}

//-----------------------------------------------------------------------------

void asiUI_ControlsAnalysis::onFindSmoothEdges()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Ask user to provide a tolerance for detection of smooth angles.
  asiUI_DialogSmoothAngleTol* wDlg = new asiUI_DialogSmoothAngleTol(this);
  //
  wDlg->exec();
  //
  const double angTol = wDlg->Tolerance;

  // Get highlighted sub-shapes.
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedSubShapes(selected);

  // Holders for geometries.
  TopTools_IndexedMapOfShape convexEdges,     concaveEdges,     undefinedEdges,     smoothEdges;
  TopoDS_Compound            convexEdgesComp, concaveEdgesComp, undefinedEdgesComp, smoothEdgesComp;
  //
  BRep_Builder BB;
  BB.MakeCompound(convexEdgesComp);
  BB.MakeCompound(concaveEdgesComp);
  BB.MakeCompound(undefinedEdgesComp);
  BB.MakeCompound(smoothEdgesComp);

  // Perform either global or local analysis.
  if ( selected.IsEmpty() )
  {
    // Build AAG.
    Handle(asiAlgo_AAG) aag = new asiAlgo_AAG(part);

    // Loop over the faces.
    Handle(asiAlgo_AAGRandomIterator) it = new asiAlgo_AAGRandomIterator(aag);
    //
    for ( ; it->More(); it->Next() )
    {
      const int current_face_idx = it->GetFaceId();
      TColStd_PackedMapOfInteger neighbor_ids;
      it->GetNeighbors(neighbor_ids);
      const TopoDS_Face& current_face = aag->GetFace(current_face_idx);

      // Loop over the neighbors.
      for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbor_ids); nit.More(); nit.Next() )
      {
        const int          neighbor_face_idx = nit.Key();
        const TopoDS_Face& neighbor_face     = aag->GetFace(neighbor_face_idx);

        // Check angle between the two faces.
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
    m_notifier.SendLogMessage(LogNotice(Normal) << "Running local analysis of edges.");

    if ( selected.Extent() < 2 )
    {
      m_notifier.SendLogMessage(LogWarn(Normal) << "At least 2 faces are expected.");
      return;
    }

    for ( int f = 1; f <= selected.Extent(); ++f )
    {
      const TopoDS_Face& F = TopoDS::Face( selected(f) );
      //
      for ( int g = f + 1; g <= selected.Extent(); ++g )
      {
        const TopoDS_Face& G = TopoDS::Face( selected(g) );

        // Check angle between the two faces.
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

  // Dump.
  m_notifier.SendLogMessage( LogInfo(Normal) << "Num. convex edges: %1."    << convexEdges.Extent()    );
  m_notifier.SendLogMessage( LogInfo(Normal) << "Num. concave edges: %1."   << concaveEdges.Extent()   );
  m_notifier.SendLogMessage( LogInfo(Normal) << "Num. undefined edges: %1." << undefinedEdges.Extent() );
  m_notifier.SendLogMessage( LogInfo(Normal) << "Num. smooth edges: %1."    << smoothEdges.Extent()    );

  // Save to model.
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

  // Update viewer.
  Handle(asiVisu_PartPrs)
    NPrs = Handle(asiVisu_PartPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation(part_n) );
  //
  if ( NPrs.IsNull() )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "There is no available presentation for part.");
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
void asiUI_ControlsAnalysis::onFindConvexOnly()
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

void asiUI_ControlsAnalysis::onEdgeCurvature()
{
  /* ================
   *  Prepare inputs
   * ================ */

  // Get numerical values.
  int    numPts      = 100;
  double scaleFactor = 1.0;
  double amplFactor  = 1.0;

  // Get Part Node to access the selected edge.
  Handle(asiData_PartNode) partNode = m_model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return;
  }
  //
  TopoDS_Shape                      partShape = partNode->GetShape();
  const TopTools_IndexedMapOfShape& subShapes = partNode->GetAAG()->RequestMapOfSubShapes();

  // Curve Node is expected.
  Handle(asiData_CurveNode) curveNode = partNode->GetCurveRepresentation();
  //
  if ( curveNode.IsNull() || !curveNode->IsWellFormed() )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "Curve Node is null or ill-defined.");
    return;
  }

  // Get ID of the selected edge.
  const int edgeIdx = curveNode->GetSelectedEdge();
  //
  if ( edgeIdx <= 0 )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "Please, select edge first.");
    return;
  }

  // Get host curve of the selected edge.
  const TopoDS_Shape& edgeShape = subShapes(edgeIdx);
  //
  if ( edgeShape.ShapeType() != TopAbs_EDGE )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "Unexpected topological type of the selected edge.");
    return;
  }
  //
  double f, l;
  Handle(Geom_Curve) curve = BRep_Tool::Curve( TopoDS::Edge(edgeShape), f, l );

  /* ==========================
   *  Evaluate curvature combs
   * ========================== */

  // Create curvature combs.
  asiEngine_Curve CurveAPI(m_model, m_notifier, m_plotter);
  //
  Handle(asiData_CurvatureCombsNode) combsNode;
  //
  std::vector<gp_Pnt> points;
  std::vector<double> params;
  std::vector<double> curvatures;
  std::vector<gp_Vec> combs;
  std::vector<bool>   combsOk;
  //
  m_model->OpenCommand();
  {
    // Calculate curvature field.
    if ( !asiAlgo_Utils::CalculateCurvatureCombs(curve,
                                                 f,
                                                 l,
                                                 numPts,
                                                 amplFactor,
                                                 points,
                                                 params,
                                                 curvatures,
                                                 combs,
                                                 combsOk) )
    {
      m_notifier.SendLogMessage(LogErr(Normal) << "Cannot calculate curvature field.");
      return;
    }

    // Create persistent object.
    combsNode = CurveAPI.CreateOrUpdateCurvatureCombs(curveNode,
                                                      scaleFactor,
                                                      points,
                                                      combsOk,
                                                      params,
                                                      curvatures,
                                                      combs);
  }
  m_model->CommitCommand();

  // Actualize.
  m_browser->Populate();
  m_partViewer->PrsMgr()->Actualize(combsNode);
}

//-----------------------------------------------------------------------------

void asiUI_ControlsAnalysis::onEdgeLength()
{
  // Attempt to get the highlighted sub-shapes.
  TColStd_PackedMapOfInteger selectedEdgeIds;
  //
  asiEngine_Part PartAPI( m_model,
                          m_partViewer->PrsMgr(),
                          m_notifier,
                          m_plotter );
  //
  PartAPI.GetHighlightedEdges(selectedEdgeIds);

  // Get total length.
  double len = 0.0;
  for ( TColStd_MapIteratorOfPackedMapOfInteger eit(selectedEdgeIds); eit.More(); eit.Next() )
  {
    const int edgeId = eit.Key();

    // Get edge.
    const TopoDS_Shape&
      edge = m_model->GetPartNode()->GetAAG()->RequestMapOfEdges()(edgeId);

    // Calculate global properties.
    GProp_GProps props;
    BRepGProp::LinearProperties(edge, props);
    len += props.Mass();
  }

  m_notifier.SendLogMessage(LogInfo(Normal) << "Length: %1." << len);
}

//-----------------------------------------------------------------------------

void asiUI_ControlsAnalysis::onEdgeStrain()
{
  // Attempt to get the highlighted sub-shapes.
  TColStd_PackedMapOfInteger selectedEdgeIds;
  //
  asiEngine_Part PartAPI( m_model,
                          m_partViewer->PrsMgr(),
                          m_notifier,
                          m_plotter );
  //
  PartAPI.GetHighlightedEdges(selectedEdgeIds);

  // Get total strain.
  double totalEnergy = 0.0;
  for ( TColStd_MapIteratorOfPackedMapOfInteger eit(selectedEdgeIds); eit.More(); eit.Next() )
  {
    const int edgeId = eit.Key();

    // Get edge.
    const TopoDS_Shape&
      edge = m_model->GetPartNode()->GetAAG()->RequestMapOfEdges()(edgeId);

    // Get curve.
    double f, l;
    Handle(Geom_Curve) edgeCurve = BRep_Tool::Curve(TopoDS::Edge(edge), f, l);

    // Calculate strain energy.
    double energy = 0;
    if ( !asiAlgo_Utils::CalculateStrainEnergy(edgeCurve, energy) )
    {
      m_notifier.SendLogMessage(LogErr(Normal) << "Cannot calculate strain energy.");
      return;
    }

    totalEnergy += energy;
  }

  m_notifier.SendLogMessage(LogInfo(Normal) << "Approximate total strain energy: %1."
                                            << totalEnergy);
}

//-----------------------------------------------------------------------------

void asiUI_ControlsAnalysis::onFaceArea()
{
  // Attempt to get the highlighted sub-shapes.
  TColStd_PackedMapOfInteger selectedFaceIds;
  //
  asiEngine_Part PartAPI( m_model,
                          m_partViewer->PrsMgr(),
                          m_notifier,
                          m_plotter );
  //
  PartAPI.GetHighlightedFaces(selectedFaceIds);

  // Get total area.
  double area = 0.0;
  for ( TColStd_MapIteratorOfPackedMapOfInteger fit(selectedFaceIds); fit.More(); fit.Next() )
  {
    const int faceId = fit.Key();

    // Get face.
    const TopoDS_Shape&
      face = m_model->GetPartNode()->GetAAG()->GetMapOfFaces()(faceId);

    // Calculate global properties.
    GProp_GProps props;
    BRepGProp::SurfaceProperties(face, props);
    area += props.Mass();
  }

  m_notifier.SendLogMessage(LogInfo(Normal) << "Total area: %1." << area);
}

//-----------------------------------------------------------------------------

void asiUI_ControlsAnalysis::onFaceEnergy()
{
  // Attempt to get the highlighted sub-shapes.
  TColStd_PackedMapOfInteger selectedFaceIds;
  //
  asiEngine_Part PartAPI( m_model,
                          m_partViewer->PrsMgr(),
                          m_notifier,
                          m_plotter );
  //
  PartAPI.GetHighlightedFaces(selectedFaceIds);

  // Get total bending energy.
  double totalEnergy = 0.0;
  for ( TColStd_MapIteratorOfPackedMapOfInteger fit(selectedFaceIds); fit.More(); fit.Next() )
  {
    const int faceId = fit.Key();

    // Get face.
    const TopoDS_Shape&
      face = m_model->GetPartNode()->GetAAG()->GetMapOfFaces()(faceId);

    // Get surface.
    Handle(Geom_Surface) surf = BRep_Tool::Surface( TopoDS::Face(face) );

    // Calculate bending energy.
    double energy = 0;
    if ( !asiAlgo_Utils::CalculateBendingEnergy(surf, energy) )
    {
      m_notifier.SendLogMessage(LogErr(Normal) << "Cannot calculate bending energy.");
      return;
    }

    totalEnergy += energy;
  }

  m_notifier.SendLogMessage(LogInfo(Normal) << "Approximate total bending energy: %1."
                                            << totalEnergy);
}

//-----------------------------------------------------------------------------

void asiUI_ControlsAnalysis::onAABB()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  double xMin, yMin, zMin, xMax, yMax, zMax;
  asiAlgo_Utils::Bounds(part, xMin, yMin, zMin, xMax, yMax, zMax);
  //
  const double maxSide = Max(Abs(xMax - xMin), Max(Abs(yMax - yMin), Abs(zMax - zMin)));
  //
  m_notifier.SendLogMessage( LogInfo(Normal) << "Bounding box:\n"
                                                "\t X min = %1\n"
                                                "\t Y min = %2\n"
                                                "\t Z min = %3\n"
                                                "\t X max = %4\n"
                                                "\t Y max = %5\n"
                                                "\t Z max = %6\n"
                                                "\t Max side = %7\n"
                                                "\t 1 percent of max side = %8"
                                             << xMin << yMin << zMin
                                             << xMax << yMax << zMax
                                             << maxSide << 0.01*maxSide );

  const double inf = Precision::Infinite()*0.1;
  //
  if ( Abs(xMin) >= inf || Abs(yMin) >= inf || Abs(zMin) >= inf ||
       Abs(xMax) >= inf || Abs(yMax) >= inf || Abs(zMax) >= inf )
  {
    m_notifier.SendLogMessage(LogWarn(Normal) << "Bounding box is infitine.");
    return;
  }

  // Create bounding box to draw it.
  TopoDS_Shape bndbox = BRepPrimAPI_MakeBox( gp_Pnt(xMin, yMin, zMin), gp_Pnt(xMax, yMax, zMax) );
  //
  m_plotter.DRAW_SHAPE(bndbox, Color_Yellow, 1.0, true, "bounding box");
}

//-----------------------------------------------------------------------------

void asiUI_ControlsAnalysis::classifyDihAngle(const TopoDS_Face&          F,
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
  // Check angle between the two faces.
  TopTools_IndexedMapOfShape commonEdges;
  asiAlgo_CheckDihedralAngle dihAngle(m_notifier,
                                      verboseOutput ? m_plotter : nullptr);
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
  if ( angleType == FeatureAngleType_Convex ||
       angleType == FeatureAngleType_SmoothConvex)
  {
    pTargetMap  = &convexEdges;
    pTargetComp = &convexEdgesComp;
  }
  else if ( angleType == FeatureAngleType_Concave ||
            angleType == FeatureAngleType_SmoothConcave )
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
