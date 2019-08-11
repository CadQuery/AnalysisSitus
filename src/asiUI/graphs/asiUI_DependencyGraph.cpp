//-----------------------------------------------------------------------------
// Created on: 11 August 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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
#include <asiUI_DependencyGraph.h>

// asiUI includes
#include <asiUI_DependencyGraphItem.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// Active Data includes
#include <ActData_DependencyGraph.h>

// VTK includes
#pragma warning(push, 0)
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCamera.h>
#include <vtkContextActor.h>
#include <vtkContextInteractorStyle.h>
#include <vtkContextScene.h>
#include <vtkContextTransform.h>
#include <vtkGraphLayout.h>
#include <vtkMutableDirectedGraph.h>
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
//
#include <vtkGraphLayoutView.h>
#include <vtkGraphToPolyData.h>
#include <vtkGlyphSource2D.h>
#include <vtkGlyph3D.h>
#include <vtkPolyDataMapper.h>
#pragma warning(pop)

//-----------------------------------------------------------------------------

#define LEGEND_TITLE "Dependency graph"
#define COLOR_NORMAL 0.0, 1.0, 0.0
#define COLOR_HEAVY 1.0, 0.0, 0.0

//-----------------------------------------------------------------------------

//! Constructor.
//! \param[in] model Data Model instance.
asiUI_DependencyGraph::asiUI_DependencyGraph(const Handle(asiEngine_Model)& model)
: m_model      (model),
  m_textWidget (NULL)
{}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_DependencyGraph::~asiUI_DependencyGraph()
{}

//-----------------------------------------------------------------------------

//! Renders graph.
void asiUI_DependencyGraph::Render()
{
  vtkSmartPointer<vtkGraph> graph = this->convertToGraph();

  // Do layout manually before handing graph to the view.
  // This allows us to know the positions of edge arrows.
  vtkSmartPointer<vtkGraphLayoutView>
    graphLayoutView = vtkSmartPointer<vtkGraphLayoutView>::New();

  vtkSmartPointer<vtkGraphLayout>
    layout = vtkSmartPointer<vtkGraphLayout>::New();

  vtkSmartPointer<vtkSimple2DLayoutStrategy>
    strategy = vtkSmartPointer<vtkSimple2DLayoutStrategy>::New();

  layout->SetInputData(graph);
  layout->SetLayoutStrategy(strategy);

  // Tell the view to use the vertex layout we provide
  graphLayoutView->SetLayoutStrategyToPassThrough();

  // The arrows will be positioned on a straight line between two
  // vertices so tell the view not to draw arcs for parallel edges
  graphLayoutView->SetEdgeLayoutStrategyToPassThrough();

  // Add the graph to the view. This will render vertices and edges,
  // but not edge arrows.
  graphLayoutView->AddRepresentationFromInputConnection(layout->GetOutputPort());

  // Manually create an actor containing the glyphed arrows.
  vtkSmartPointer<vtkGraphToPolyData>
    graphToPoly = vtkSmartPointer<vtkGraphToPolyData>::New();
  //
  graphToPoly->SetInputConnection(layout->GetOutputPort());
  graphToPoly->EdgeGlyphOutputOn();

  // Set the position (0: edge start, 1: edge end) where
  // the edge arrows should go.
  graphToPoly->SetEdgeGlyphPosition(0.98);

  // Make a simple edge arrow for glyphing.
  vtkSmartPointer<vtkGlyphSource2D>
    arrowSource = vtkSmartPointer<vtkGlyphSource2D>::New();
  //
  arrowSource->SetGlyphTypeToEdgeArrow();
  arrowSource->SetScale(0.05);
  arrowSource->Update();

  // Use Glyph3D to repeat the glyph on all edges.
  vtkSmartPointer<vtkGlyph3D>
    arrowGlyph = vtkSmartPointer<vtkGlyph3D>::New();
  //
  arrowGlyph->SetInputConnection(0, graphToPoly->GetOutputPort(1));
  arrowGlyph->SetInputConnection(1, arrowSource->GetOutputPort());

  // Add the edge arrow actor to the view.
  vtkSmartPointer<vtkPolyDataMapper>
    arrowMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  //
  arrowMapper->SetInputConnection(arrowGlyph->GetOutputPort());

  vtkSmartPointer<vtkActor>
    arrowActor = vtkSmartPointer<vtkActor>::New();
  //
  arrowActor->SetMapper(arrowMapper);
  graphLayoutView->GetRenderer()->AddActor(arrowActor);

  graphLayoutView->SetVertexLabelVisibility(true);
  graphLayoutView->SetVertexLabelArrayName(ARRNAME_LABELS);

  graphLayoutView->ResetCamera();
  graphLayoutView->Render();
  graphLayoutView->GetInteractor()->Start();
}

//-----------------------------------------------------------------------------

//! Callback to adjust text widgets.
void asiUI_DependencyGraph::RenderEventCallback()
{
  if ( !m_textWidget->GetEnabled() )
    m_textWidget->On();
}

//-----------------------------------------------------------------------------

vtkSmartPointer<vtkGraph> asiUI_DependencyGraph::convertToGraph()
{
  vtkSmartPointer<vtkMutableDirectedGraph>
    result = vtkSmartPointer<vtkMutableDirectedGraph>::New();

  /* ===================================
   *  Prepare structures for attributes
   * =================================== */

  // Array for groups
  vtkSmartPointer<vtkStringArray> groupArr = vtkSmartPointer<vtkStringArray>::New();
  groupArr->SetNumberOfComponents(1);
  groupArr->SetName(ARRNAME_GROUP);

  // Array for vertex labels
  vtkSmartPointer<vtkStringArray> labelArr = vtkSmartPointer<vtkStringArray>::New();
  labelArr->SetNumberOfComponents(1);
  labelArr->SetName(ARRNAME_LABELS);

  /* ================================================
   *  Populate graph data from Model Execution Graph
   * ================================================ */

  Handle(ActData_DependencyGraph) depGraph = new ActData_DependencyGraph(m_model);

  // Get graph data
  const ActData_DependencyGraph::VertexDataMap& vertices = depGraph->Vertices();
  const ActData_DependencyGraph::EdgeMap&       arcs     = depGraph->Edges();

  // Populate VTK graph data with vertices
  NCollection_DataMap<int, vtkIdType> VTKVertMap;
  for ( ActData_DependencyGraph::VertexDataMap::Iterator vIt(vertices); vIt.More(); vIt.Next() )
  {
    // Prepare domain-VTK correspondence pair
    int       V    = vIt.Key();
    vtkIdType vtkV = result->AddVertex();
    //
    VTKVertMap.Bind(V, vtkV);

    // Generate label for vertex
    const ActData_DependencyGraph::VertexData& VData = vIt.Value();
    ActAPI_DataObjectId nodeId = VData.Parameter->GetNode()->GetId();
    TCollection_AsciiString
      name = TCollection_AsciiString( VData.TreeFunction->GetName() ).Cat(" [").Cat(nodeId).Cat("]");
    //
    labelArr->InsertNextValue( name.ToCString() );

    // Set color scalar for vertex
    if ( VData.TreeFunction->IsHeavy() )
      groupArr->InsertNextValue(ARRNAME_GROUP_TF_HEAVY);
    else
      groupArr->InsertNextValue(ARRNAME_GROUP_TF_LIGHT);
  }

  // Populate VTK graph data with edges
  for ( ActData_DependencyGraph::EdgeMap::Iterator arcIt(arcs); arcIt.More(); arcIt.Next() )
  {
    const ActData_DependencyGraph::OriEdge& arc = arcIt.Value();
    result->AddEdge( VTKVertMap.Find(arc.V1), VTKVertMap(arc.V2) );
  }

  result->GetVertexData()->AddArray(labelArr);
  result->GetVertexData()->AddArray(groupArr);

  return result;
}

//-----------------------------------------------------------------------------

//! Reaction on closing the viewer.
void asiUI_DependencyGraph::onViewerClosed()
{
  // NOTE: the important point is to remove widget after all items which
  //       may listen to it
  m_textWidget->Delete();

  delete m_pWidget;
  delete this;
}

//-----------------------------------------------------------------------------

void asiUI_DependencyGraph::onVertexPicked(const vtkIdType vid)
{
  // NYI
}
