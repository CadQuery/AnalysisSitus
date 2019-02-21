//-----------------------------------------------------------------------------
// Created on: 21 December (*) 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017-present, Sergey Slyadnev
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
#include <asiUI_HistoryGraph.h>

// asiUI includes
#include <asiUI_HistoryGraphAdaptor.h>
#include <asiUI_HistoryGraphItem.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// VTK includes
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCamera.h>
#include <vtkContextActor.h>
#include <vtkContextInteractorStyle.h>
#include <vtkContextScene.h>
#include <vtkContextTransform.h>
#include <vtkGraphLayout.h>
#include <vtkNew.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSimple2DLayoutStrategy.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTextRepresentation.h>
#include <vtkTextWidget.h>

#define LEGEND_TITLE_HISTORY "History graph"

//-----------------------------------------------------------------------------

#define VTK_CREATE(Type, Name) \
  vtkSmartPointer<Type> Name = vtkSmartPointer<Type>::New()

//-----------------------------------------------------------------------------

//! Constructor.
//! \param[in] model    Data Model instance.
//! \param[in] naming   naming service.
//! \param[in] progress progress notifier.
//! \param[in] plotter  imperative plotter.
asiUI_HistoryGraph::asiUI_HistoryGraph(const Handle(asiEngine_Model)& model,
                                       const Handle(asiAlgo_Naming)&  naming,
                                       ActAPI_ProgressEntry           progress,
                                       ActAPI_PlotterEntry            plotter)
: m_pWidget    ( NULL ),
  m_textWidget ( NULL ),
  m_model      ( model ),
  m_naming     ( naming ),
  m_history    ( naming->GetHistory() ),
  m_progress   ( progress ),
  m_plotter    ( plotter )
{}

//-----------------------------------------------------------------------------

//! Constructor.
//! \param[in] model    Data Model instance.
//! \param[in] history  modification history.
//! \param[in] progress progress notifier.
//! \param[in] plotter  imperative plotter.
asiUI_HistoryGraph::asiUI_HistoryGraph(const Handle(asiEngine_Model)& model,
                                       const Handle(asiAlgo_History)& history,
                                       ActAPI_ProgressEntry           progress,
                                       ActAPI_PlotterEntry            plotter)
: m_pWidget    (NULL),
  m_textWidget (NULL),
  m_model      (model),
  m_history    (history),
  m_progress   (progress),
  m_plotter    (plotter)
{}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_HistoryGraph::~asiUI_HistoryGraph()
{}

//-----------------------------------------------------------------------------

//! Renders history graph.
void asiUI_HistoryGraph::Render()
{
  /* ===================================
   *  Prepare structures for attributes
   * =================================== */

  // Populate graph data from history graph
  vtkSmartPointer<vtkGraph> graph = this->convertToGraph();

  // Layout strategy
  vtkNew<vtkSimple2DLayoutStrategy> simple2DStrategy;
  simple2DStrategy->SetIterationsPerLayout(10);

  // Layout
  vtkSmartPointer<vtkGraphLayout> graphLayout = vtkSmartPointer<vtkGraphLayout>::New();
  graphLayout->SetInputData(graph);
  graphLayout->SetLayoutStrategy( simple2DStrategy.GetPointer() );
  graphLayout->Update();

  // Graph item
  vtkSmartPointer<asiUI_HistoryGraphItem> graphItem = vtkSmartPointer<asiUI_HistoryGraphItem>::New();
  graphItem->SetGraph( graphLayout->GetOutput() );

  connect( graphItem, SIGNAL( vertexPicked(const int, const vtkIdType) ),
           this,      SLOT( onVertexPicked(const int, const vtkIdType) ) );

  // Context transform
  vtkSmartPointer<vtkContextTransform> trans = vtkSmartPointer<vtkContextTransform>::New();
  trans->SetInteractive(true);
  trans->AddItem(graphItem);

  // Context actor
  vtkSmartPointer<vtkContextActor> actor = vtkSmartPointer<vtkContextActor>::New();
  actor->GetScene()->AddItem(trans);

  /* ===============================================
   *  Prepare and initialize interaction facilities
   * =============================================== */

  // Renderer
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  renderer->SetBackground(0.15, 0.15, 0.15);

  // Render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderer->AddActor(actor);

  // Context interactor style
  vtkNew<vtkContextInteractorStyle> interactorStyle;
  interactorStyle->SetScene( actor->GetScene() );

  // Interactor
  vtkNew<QVTKInteractor> interactor;
  interactor->SetInteractorStyle( interactorStyle.GetPointer() );
  interactor->SetRenderWindow( renderWindow.GetPointer() );

  // Create Qt window. NOTICE that interactor should be already given to
  // the render window at this stage
  m_pWidget = new asiUI_VtkWindow();
  m_pWidget->SetRenderWindow( renderWindow.GetPointer() );

  connect( m_pWidget, SIGNAL( windowClosed() ), this, SLOT( onViewerClosed() ) );

  /* ========================
   *  Add legend and summary
   * ======================== */

  // Legend
  m_textWidget = vtkTextWidget::New();
  vtkTextRepresentation* textRep = vtkTextRepresentation::SafeDownCast( m_textWidget->GetRepresentation() );
  textRep->GetPositionCoordinate()->SetValue(0.2, 0.01);
  textRep->GetPosition2Coordinate()->SetValue(0.5, 0.06);
  m_textWidget->SelectableOff();
  //
  vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
  textRep->SetTextActor(textActor);
  //
  m_textWidget->GetTextActor()->SetInput(LEGEND_TITLE_HISTORY);
  m_textWidget->SetInteractor( m_pWidget->GetInteractor() );
  m_textWidget->SetDefaultRenderer( renderer );
  m_textWidget->SetCurrentRenderer( renderer );

  /* ===================
   *  Start interaction
   * =================== */

  renderWindow->SetLineSmoothing(true);
  renderWindow->SetWindowName("Modification history");
  //
  graphItem->StartLayoutAnimation( m_pWidget->GetInteractor() );
  //
  m_pWidget->GetInteractor()->Initialize();
  m_pWidget->resize(400, 400);
  m_pWidget->show();

  // Set callback on rendering
  m_pWidget->GetRenderWindow()->AddObserver(vtkCommand::RenderEvent, this, &asiUI_HistoryGraph::RenderEventCallback);
}

//-----------------------------------------------------------------------------

//! Callback to adjust text widgets.
void asiUI_HistoryGraph::RenderEventCallback()
{
  if ( !m_textWidget->GetEnabled() )
    m_textWidget->On();
}

//-----------------------------------------------------------------------------

//! Builds VTK graph from history graph.
//! \return graph instance.
vtkSmartPointer<vtkGraph> asiUI_HistoryGraph::convertToGraph()
{
  vtkSmartPointer<vtkGraph>
    result = asiUI_HistoryGraphAdaptor::Convert(m_history, m_naming, m_progress);

  return result;
}

//-----------------------------------------------------------------------------

//! Reaction on closing the viewer.
void asiUI_HistoryGraph::onViewerClosed()
{
  // NOTE: the important point is to remove widget after all items which
  //       may listen to it
  m_textWidget->Delete();

  delete m_pWidget;
  delete this;
}

//-----------------------------------------------------------------------------

//! Reaction on vertex picking.
//! \param[in] graphItemId ID of the algorithmic graph item corresponding
//!                        to the picked vertex.
//! \param[in] vid         graph vertex ID.
void asiUI_HistoryGraph::onVertexPicked(const int       graphItemId,
                                        const vtkIdType vid)
{
  asiVisu_NotUsed(vid);

  asiAlgo_History::t_item* pItem = m_history->GetNode(graphItemId);
  //
  if ( !pItem )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot find item for graph item ID %1."
                                             << graphItemId);
    return;
  }

  // Get part shape.
  Handle(asiData_PartNode) part_n = m_model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Part is null or invalid.");
    return;
  }
  //
  TopoDS_Shape masterCAD = part_n->GetShape();

  // Check if the picked sub-shape exists in the part shape.
  if ( asiAlgo_Utils::Contains(masterCAD, pItem->TransientPtr) )
  {
    m_plotter.REDRAW_SHAPE("historyItem", pItem->TransientPtr, Color_Green);
    m_progress.SendLogMessage(LogInfo(Normal) << "Sub-shape is ALIVE in the part shape.");
  }
  else
  {
    m_plotter.REDRAW_SHAPE("historyItem", pItem->TransientPtr, Color_White);
    m_progress.SendLogMessage(LogInfo(Normal) << "Sub-shape is ABSENT in the part shape.");
  }
}
