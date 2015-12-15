//-----------------------------------------------------------------------------
// Created on: 05 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <visu_topo_graph.h>

// Visualization includes
#include <visu_utils.h>

// Geometry includes
#include <geom_utils.h>

// OCCT includes
#include <TopoDS_Iterator.hxx>

// VTK includes
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkAppendPolyData.h>
#include <vtkDataSetAttributes.h>
#include <vtkGlyph3D.h>
#include <vtkGlyphSource2D.h>
#include <vtkGraphLayout.h>
#include <vtkGraphToPolyData.h>
#include <vtkInteractorStyleImage.h>
#include <vtkLabeledDataMapper.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkForceDirectedLayoutStrategy.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTextRepresentation.h>
#include <vtkTextWidget.h>
#include <vtkVertexGlyphFilter.h>

#define ARRNAME_LABELS "Labels"
#define ARRNAME_COLORS "Colors"
#define LEGEND_TITLE "Topology graph down to vertices"
#define COLOR_NORMAL 0.0, 1.0, 0.0
#define COLOR_HEAVY 1.0, 0.0, 0.0

#define VTK_CREATE(Type, Name) \
  vtkSmartPointer<Type> Name = vtkSmartPointer<Type>::New()

//! Constructor.
visu_topo_graph::visu_topo_graph() {}

//! Renders topology graph.
//! \param shape [in] target shape.
void visu_topo_graph::Render(const TopoDS_Shape& shape)
{
  /* ===================================
   *  Prepare structures for attributes
   * =================================== */

  // Array for vertex colors
  VTK_CREATE(vtkIntArray, aColorArr);
  aColorArr->SetNumberOfComponents(1);
  aColorArr->SetName(ARRNAME_COLORS);

  // Array for vertex labels
  VTK_CREATE(vtkStringArray, aLabelArr);
  aLabelArr->SetNumberOfComponents(1);
  aLabelArr->SetName(ARRNAME_LABELS);

  // Lookup table
  VTK_CREATE(vtkLookupTable, aLookup);
  aLookup->SetNumberOfTableValues(2);
  aLookup->SetTableValue(0, COLOR_NORMAL); // Normal Tree Functions
  aLookup->SetTableValue(1, COLOR_HEAVY); // Heavy Tree Functions
  aLookup->Build();

  /* =========================================
   *  Populate graph data from topology graph
   * ========================================= */

  // Create VTK data set for graph data
  VTK_CREATE(vtkMutableDirectedGraph, aGraphData);

  const vtkIdType root_vid = aGraphData->AddVertex();
  TopTools_DataMapOfShapeInteger M;
  M.Bind(shape, root_vid);
  aLabelArr->InsertNextValue( ShapeAddrWithPrefix(shape).c_str() );
  aColorArr->InsertNextValue(1);
  //
  this->buildRecursively(shape, root_vid, aGraphData, aLabelArr, aColorArr, M);

  aGraphData->GetVertexData()->AddArray(aLabelArr);
  aGraphData->GetVertexData()->AddArray(aColorArr);

  /* ==============================
   *  Build visualization pipeline
   * ============================== */

  // Do layout manually before handing graph to the view.
  // This allows us to know the positions of edge arrows
  VTK_CREATE(vtkGraphLayout, aGraphLayout);
  VTK_CREATE(vtkForceDirectedLayoutStrategy, aGraphLayoutStrategy);
  aGraphLayout->SetInputData(aGraphData);
  aGraphLayout->SetLayoutStrategy(aGraphLayoutStrategy);

  // Manually create an actor containing the arrows
  VTK_CREATE(vtkGraphToPolyData, aGraphToPoly);
  aGraphToPoly->SetInputConnection( aGraphLayout->GetOutputPort() );
  aGraphToPoly->EdgeGlyphOutputOn();
  aGraphToPoly->SetEdgeGlyphPosition(0.99); // 0: edge start, 1: edge end

  // Manually create an actor containing the arrows
  VTK_CREATE(vtkGraphToPolyData, aGraphToPolyLinks);
  aGraphToPolyLinks->SetInputConnection( aGraphLayout->GetOutputPort() );
  aGraphToPolyLinks->Update(); // Update as we want to access point data

  // Polydata for labels
  VTK_CREATE(vtkPolyData, aPolyDataForLabels);
  aPolyDataForLabels->DeepCopy( aGraphToPolyLinks->GetOutput() );
  aPolyDataForLabels->GetPointData()->AddArray(aLabelArr);

  // Polydata for node markers
  VTK_CREATE(vtkPolyData, aPolyDataForNodes);
  aPolyDataForNodes->DeepCopy( aGraphToPolyLinks->GetOutput() );
  aPolyDataForNodes->GetPointData()->SetScalars(aColorArr);
  VTK_CREATE(vtkVertexGlyphFilter, aVertexFilter);
  aVertexFilter->SetInputData(aPolyDataForNodes);

  // Make a simple edge arrow to play as a glyph
  VTK_CREATE(vtkGlyphSource2D, anArrowSource);
  anArrowSource->SetGlyphTypeToEdgeArrow();
  anArrowSource->SetScale(0.01);
  anArrowSource->Update();

  // Use Glyph3D to repeat the glyph on all edges
  VTK_CREATE(vtkGlyph3D, anArrowGlyph);
  anArrowGlyph->SetInputConnection( 0, aGraphToPoly->GetOutputPort(1) );
  anArrowGlyph->SetInputConnection( 1, anArrowSource->GetOutputPort() );

  // Manually create an actor containing the arrows
  VTK_CREATE(vtkAppendPolyData, aGraphPolyFull);
  aGraphPolyFull->AddInputConnection( anArrowGlyph->GetOutputPort() );
  aGraphPolyFull->AddInputConnection( aGraphToPolyLinks->GetOutputPort() );

  // Text properties for labels
  VTK_CREATE(vtkTextProperty, aTextProps);
  aTextProps->BoldOff();
  aTextProps->ShadowOff();
  aTextProps->SetFontSize(12);

  // Add labels
  VTK_CREATE(vtkLabeledDataMapper, aLabelMapper);
  VTK_CREATE(vtkActor2D, aLabelActor);
  aLabelMapper->SetFieldDataName(ARRNAME_LABELS);
  aLabelMapper->SetInputData(aPolyDataForLabels);
  aLabelMapper->SetLabelTextProperty(aTextProps);
  aLabelMapper->SetLabelModeToLabelFieldData();
  aLabelActor->SetMapper(aLabelMapper);

  // Add arrows
  VTK_CREATE(vtkPolyDataMapper, anArrowMapper);
  VTK_CREATE(vtkActor, anArrowActor);
  anArrowMapper->SetInputConnection( aGraphPolyFull->GetOutputPort() );
  anArrowActor->SetMapper(anArrowMapper);

  // Add vertices
  VTK_CREATE(vtkPolyDataMapper, aVertexMapper);
  VTK_CREATE(vtkActor, aVertexActor);
  aVertexMapper->SetLookupTable(aLookup);
  aVertexMapper->SetInputConnection( aVertexFilter->GetOutputPort() );
  aVertexActor->SetMapper(aVertexMapper);
  aVertexActor->GetProperty()->SetPointSize(5);

  /* ================================
   *  Finalize and start interaction
   * ================================ */

  // Create a renderer, render window and interactor
  VTK_CREATE(vtkRenderer, renderer);
  VTK_CREATE(vtkRenderWindow, renderWindow);
  VTK_CREATE(vtkRenderWindowInteractor, renderWindowInteractor);
  VTK_CREATE(vtkInteractorStyleImage, interactorStyle);

  renderWindow->SetSize(800, 800);
  renderWindow->AddRenderer(renderer);
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Set interactor style
  renderWindowInteractor->SetInteractorStyle(interactorStyle);

  // Add actors to the scene
  renderer->AddActor(anArrowActor);
  renderer->AddActor(aLabelActor);
  renderer->AddActor(aVertexActor);
  renderer->SetBackground(0.0, 0.0, 0.0); // Background color

  // Shape summary
  VTK_CREATE(vtkTextWidget, aTextWidget);
  visu_utils::InitTextWidget(aTextWidget);
  //
  TCollection_AsciiString shapeInfo;
  geom_utils::ShapeSummary(shape, shapeInfo);
  aTextWidget->GetTextActor()->SetInput( shapeInfo.ToCString() );
  //
  aTextWidget->SetInteractor      ( renderer->GetRenderWindow()->GetInteractor() );
  aTextWidget->SetDefaultRenderer ( renderer );
  aTextWidget->SetCurrentRenderer ( renderer );
  aTextWidget->On                 ( );

  /* ============
   *  Add legend
   * ============ */

  VTK_CREATE(vtkTextWidget, textWidget);
  vtkTextRepresentation* aRep = vtkTextRepresentation::SafeDownCast( textWidget->GetRepresentation() );
  aRep->GetPositionCoordinate()->SetValue(0.3, 0.01);
  aRep->GetPosition2Coordinate()->SetValue(0.4, 0.04);
  textWidget->SelectableOff();

  VTK_CREATE(vtkTextActor, aTextActor);
  aRep->SetTextActor(aTextActor);

  textWidget->GetTextActor()->SetInput(LEGEND_TITLE);
  textWidget->SetInteractor(renderWindowInteractor);
  textWidget->SetDefaultRenderer(renderer);
  textWidget->SetCurrentRenderer(renderer);
  textWidget->SetEnabled(1);

  /* =====================
   *  Render and interact
   * ===================== */

  renderWindow->Render();
  renderWindow->SetWindowName("Topology Graph");
  renderWindowInteractor->Start();
}

//! Builds data structures for visualization recursively.
//! \param rootShape     [in]     root shape.
//! \param rootId        [in]     ID of the root vertex.
//! \param pDS           [in/out] data structure being filled.
//! \param pLabelArr     [in/out] array for labels associated with vertices.
//! \param pColorArr     [in/out] array for colors for vertices.
//! \param shapeVertices [in/out] map of shapes against their registered graph vertices.
void visu_topo_graph::buildRecursively(const TopoDS_Shape&             rootShape,
                                       const vtkIdType                 rootId,
                                       vtkMutableDirectedGraph*        pDS,
                                       vtkStringArray*                 pLabelArr,
                                       vtkIntArray*                    pColorArr,
                                       TopTools_DataMapOfShapeInteger& shapeVertices)
{
  for ( TopoDS_Iterator it(rootShape, 0, 0); it.More(); it.Next() )
  {
    const TopoDS_Shape& subShape = it.Value();

    vtkIdType childId;
    if ( shapeVertices.IsBound(subShape) )
    {
      childId = shapeVertices.Find(subShape);
    }
    else
    {
      childId = pDS->AddVertex();
      shapeVertices.Bind(subShape, childId);

      pLabelArr->InsertNextValue( ShapeAddrWithPrefix(subShape).c_str() );
      pColorArr->InsertNextValue(1);
    }
    //
    pDS->AddEdge(rootId, childId);
    //
    this->buildRecursively(subShape, childId, pDS, pLabelArr, pColorArr, shapeVertices);
  }
}
