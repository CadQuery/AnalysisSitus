//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_GeomPrs.h>

// A-Situs (visualization) includes
#include <asiVisu_DisplayMode.h>
#include <asiVisu_PartDataProvider.h>
#include <asiVisu_PartPipeline.h>

// VTK includes
#include <vtkCellData.h>
#include <vtkIdTypeArray.h>
#include <vtkMapper.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

// VIS includes
#pragma warning(push, 0)
#include <IVtkTools_ShapeObject.hxx>
#pragma warning(pop)

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

//! Convert integer value to a color.
//! \param color [in] integer value.
//! \return converted value
static QColor IntToColor(const int color)
{
  unsigned char red   = ( color >> 16 ) & 0xFF;
  unsigned char green = ( color >>  8 ) & 0xFF;
  unsigned char blue  =   color         & 0xFF;
  return QColor(red, green, blue);
}

//! Creates a Presentation object for the passed Part Node.
//! \param N [in] Part Node to create a Presentation for.
asiVisu_GeomPrs::asiVisu_GeomPrs(const Handle(ActAPI_INode)& N) : asiVisu_Prs(N)
{
  Handle(asiData_PartNode) partNode = Handle(asiData_PartNode)::DownCast(N);

  // Create Data Provider
  Handle(asiVisu_PartDataProvider) dp = new asiVisu_PartDataProvider(partNode);

  // Main pipeline
  Handle(asiVisu_PartPipeline) pl = new asiVisu_PartPipeline(true, true);
  //
  this->addPipeline        ( Pipeline_Main, pl );
  this->assignDataProvider ( Pipeline_Main, dp );

  // Set point size and line width
  pl->Actor()->GetProperty()->SetPointSize(5.0f);
  pl->Actor()->GetProperty()->SetLineWidth(1.5f);

  ///* ====================
  // *  Pipeline for edges
  // * ==================== */

  //// Create pipeline for highlighting
  //Handle(asiVisu_ShapePipeline)
  //  contour_pl = new asiVisu_ShapePipeline( true, true, true, false, pl->DataSource() );

  //// Adjust props
  //contour_pl->Actor()->GetProperty()->SetOpacity(0.5);
  //contour_pl->Actor()->GetProperty()->SetLineWidth(1.5f);
  //contour_pl->Actor()->SetPickable(0);
  ////
  //contour_pl->WireframeModeOn();
  ////
  //this->addPipeline        ( Pipeline_Contour, contour_pl );
  //this->assignDataProvider ( Pipeline_Contour, DP );

  ///* ==================================
  // *  Pipeline for robust presentation
  // * ================================== */

  //// Create pipeline for highlighting
  //Handle(asiVisu_ShapeRobustPipeline)
  //  robust_pl = new asiVisu_ShapeRobustPipeline();
  ////
  //this->addPipeline        ( Pipeline_Robust, robust_pl );
  //this->assignDataProvider ( Pipeline_Robust, DP );

  //// Hide by default
  //this->DoRobust(false);

  ///* ======================
  // *  Pipeline for picking
  // * ====================== */

  //double pick_color[3];
  //asiVisu_Utils::DefaultPickingColor(pick_color[0], pick_color[1], pick_color[2]);

  //// Create pipeline for highlighting
  //Handle(asiVisu_ShapePipeline) pick_pl = new asiVisu_ShapePipeline( false, true, true, false, pl->DataSource() );

  //// Adjust props
  //pick_pl->Actor()->GetProperty()->SetColor(pick_color[0], pick_color[1], pick_color[2]);
  //pick_pl->Actor()->GetProperty()->SetLineWidth( asiVisu_Utils::DefaultPickLineWidth() );
  //pick_pl->Actor()->GetProperty()->SetPointSize( asiVisu_Utils::DefaultHilightPointSize() );
  //pick_pl->Actor()->SetPickable(0);
  //pick_pl->Mapper()->ScalarVisibilityOff();
  ////
  //pick_pl->Actor()->GetProperty()->SetOpacity(1.0);
  //pick_pl->Actor()->GetProperty()->SetAmbient(0.8);
  //pick_pl->Actor()->GetProperty()->SetDiffuse(0.2);
  //pick_pl->Actor()->GetProperty()->SetSpecular(0.9);
  //pick_pl->Actor()->GetProperty()->SetSpecularPower(20);

  //// Bind to the data provider
  //this->installPickPipeline( pick_pl, DP );

  ///* ========================
  // *  Pipeline for detecting
  // * ======================== */

  //double detect_color[3];
  //asiVisu_Utils::DefaultDetectionColor(detect_color[0], detect_color[1], detect_color[2]);

  //// Create pipeline for highlighting
  //Handle(asiVisu_ShapePipeline)
  //  detect_pl = new asiVisu_ShapePipeline( false, true, true, false, pl->DataSource() );

  //// Adjust props
  //detect_pl->Actor()->GetProperty()->SetColor(detect_color[0], detect_color[1], detect_color[2]);
  //detect_pl->Actor()->GetProperty()->SetLineWidth( asiVisu_Utils::DefaultDetectionLineWidth() );
  //detect_pl->Actor()->GetProperty()->SetPointSize( asiVisu_Utils::DefaultHilightPointSize() );
  //detect_pl->Actor()->SetPickable(0);
  //detect_pl->Mapper()->ScalarVisibilityOff();
  ////
  //detect_pl->Actor()->GetProperty()->SetOpacity(1.0);
  //detect_pl->Actor()->GetProperty()->SetAmbient(0.8);
  //detect_pl->Actor()->GetProperty()->SetDiffuse(0.2);
  //detect_pl->Actor()->GetProperty()->SetSpecular(0.9);
  //detect_pl->Actor()->GetProperty()->SetSpecularPower(20);

  //// Bind to the data provider
  //this->installDetectPipeline( detect_pl, DP );

  //---------------------------------------------------------------------------
  vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();
}

//! Factory method for Presentation.
//! \param N [in] Geometry Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_GeomPrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new asiVisu_GeomPrs(N);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_GeomPrs::IsVisible() const
{
  return true;
}

//! Sets SHADING visualization mode.
void asiVisu_GeomPrs::DoShading() const
{
  /*Handle(asiVisu_PartPipeline)
    pl = Handle(asiVisu_PartPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  if ( pl.IsNull() )
    return;

  pl->ShadingModeOn();*/
}

//! Sets WIREFRAME visualization mode.
void asiVisu_GeomPrs::DoWireframe() const
{
  /*Handle(asiVisu_PartPipeline)
    pl = Handle(asiVisu_PartPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  if ( pl.IsNull() )
    return;

  pl->WireframeModeOn();*/
}

//! Sets custom color for the geometry.
//! \param color [in] color to set.
void asiVisu_GeomPrs::DoColor(const QColor& color) const
{
  if ( !color.isValid() )
    return;

  Handle(asiVisu_PartPipeline)
    pl = Handle(asiVisu_PartPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  if ( pl.IsNull() )
    return;

  pl->Mapper()->ScalarVisibilityOff();
  pl->Actor()->GetProperty()->SetColor( color.redF(),
                                        color.greenF(),
                                        color.blueF() );
}

//! Unsets custom color for the geometry.
void asiVisu_GeomPrs::DoUnColor() const
{
  Handle(asiVisu_PartPipeline)
    pl = Handle(asiVisu_PartPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  if ( pl.IsNull() )
    return;

  pl->Mapper()->ScalarVisibilityOn();
}

//! Enables/disables visualization of vertices depending on the passed flag.
//! \param on [in] true/false.
void asiVisu_GeomPrs::DoVertices(const bool on) const
{
  /*Handle(asiVisu_PartPipeline)
    pl = Handle(asiVisu_PartPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  if ( pl.IsNull() )
    return;

  on ? pl->SharedVerticesOn() : pl->SharedVerticesOff();*/
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_GeomPrs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void asiVisu_GeomPrs::afterInitPipelines()
{
  //// Access pipelines dedicated for highlighting
  //const Handle(asiVisu_ShapePipeline)&
  //  pick_pl = Handle(asiVisu_ShapePipeline)::DownCast( this->GetPickPipeline() ),
  //  detect_pl = Handle(asiVisu_ShapePipeline)::DownCast( this->GetDetectPipeline() );
  ////
  //const Handle(asiVisu_ShapeDataProvider)&
  //  pick_dp = Handle(asiVisu_ShapeDataProvider)::DownCast( this->dataProviderPick() ),
  //  detect_dp = Handle(asiVisu_ShapeDataProvider)::DownCast( this->dataProviderDetect() );

  //if ( !pick_dp.IsNull() )
  //{
  //  pick_dp->SetSubShapes(new TColStd_HPackedMapOfInteger);
  //  pick_pl->VoidSubShapesOn(); // Block selection pipelining initially
  //  pick_pl->SetInput(pick_dp); // Init them as well (as selection pipelines are not automated)
  //}

  //if ( !detect_dp.IsNull() )
  //{
  //  detect_dp->SetSubShapes(new TColStd_HPackedMapOfInteger);
  //  detect_pl->VoidSubShapesOn(); // Block selection pipelining initially
  //  detect_pl->SetInput(detect_dp); // Init them as well (as selection pipelines are not automated)
  //}
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_GeomPrs::beforeUpdatePipelines() const
{
  /*Handle(asiData_PartNode) N = Handle(asiData_PartNode)::DownCast( this->GetNode() );

  asiVisu_DisplayMode aDMode = (asiVisu_DisplayMode) N->GetDisplayMode();
  if ( aDMode == DisplayMode_Undefined || aDMode == DisplayMode_Shading )
    this->DoShading();
  else
    this->DoWireframe();*/
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_GeomPrs::afterUpdatePipelines() const
{
  ///* ====================================
  // *  Update selection pipelines as well
  // * ==================================== */

  //// Access pipelines dedicated for highlighting
  //const Handle(asiVisu_ShapePipeline)&
  //  pick_pl = Handle(asiVisu_ShapePipeline)::DownCast( this->GetPickPipeline() ),
  //  detect_pl = Handle(asiVisu_ShapePipeline)::DownCast( this->GetDetectPipeline() );

  //// IMPORTANT: We update our highlighting pipelines here just to make things
  //// faster. The better place to do that is "highlight" method, because
  //// we do not really need to build highlighting pipelines just after
  //// the Nodal Presentation is created. Logically, we would better to prepare
  //// this pipeline only on actual selection request from user. However, in the
  //// latter case the reactivity of application might significantly slow down
  //if ( !pick_pl.IsNull() )
  //  pick_pl->Update();
  ////
  //if ( !detect_pl.IsNull() )
  //  detect_pl->Update();

  ///* ====================================
  // *  Actualize visualization properties
  // * ==================================== */

  //Handle(asiData_PartNode) N = Handle(asiData_PartNode)::DownCast( this->GetNode() );

  //// Custom color (if any)
  //if ( N->HasColor() )
  //{
  //  QColor color = ::IntToColor( N->GetColor() );
  //  this->DoColor(color);
  //}
  //else
  //  this->DoUnColor();

  //// Visualization of vertices
  //this->DoVertices( N->HasVertices() );
}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection nature (picking or detecting).
void asiVisu_GeomPrs::highlight(vtkRenderer*                   theRenderer,
                                const asiVisu_PickResult&      thePickRes,
                                const asiVisu_SelectionNature& theSelNature) const
{
  //asiVisu_NotUsed(theRenderer);

  ///* ==================
  // *  Get target actor
  // * ================== */

  //Handle(asiVisu_Pipeline) main_pl = this->GetPipeline(Pipeline_Main);
  ////
  //vtkActor* main_actor = main_pl->Actor();

  ///* ============================================================
  // *  Get the list of sub-shape IDs corresponding to the picking
  // *  results on the main actor
  // * ============================================================ */

  //TColStd_PackedMapOfInteger aSubShapeIDs;
  //if ( thePickRes.IsSelectionSubShape() )
  //{
  //  const asiVisu_ActorElemMap& aPickMap = thePickRes.GetPickMap();
  //  for ( asiVisu_ActorElemMap::Iterator it(aPickMap); it.More(); it.Next() )
  //  {
  //    const vtkSmartPointer<vtkActor>& aResActor = it.Key();
  //    if ( aResActor != main_actor )
  //      continue;

  //    aSubShapeIDs = it.Value();
  //  }
  //}

  ///* ===============================
  // *  Update highlighting pipelines
  // * =============================== */

  //// Access pipeline for highlighting
  //Handle(asiVisu_ShapePipeline) hili_pl;
  ////
  //if ( theSelNature == SelectionNature_Pick )
  //  hili_pl = Handle(asiVisu_ShapePipeline)::DownCast( this->GetPickPipeline() );
  //else
  //  hili_pl = Handle(asiVisu_ShapePipeline)::DownCast( this->GetDetectPipeline() );

  //if ( hili_pl.IsNull() )
  //  return;

  //// No sub-shapes -> no selection
  //if ( thePickRes.IsSelectionSubShape() && aSubShapeIDs.IsEmpty() )
  //{
  //  hili_pl->VoidSubShapesOn();
  //  return;
  //}

  //// Access its Data Provider
  //Handle(asiVisu_ShapeDataProvider) hili_dp;
  ////
  //if ( theSelNature == SelectionNature_Pick )
  //  hili_dp = Handle(asiVisu_ShapeDataProvider)::DownCast( this->dataProviderPick() );
  //else
  //  hili_dp = Handle(asiVisu_ShapeDataProvider)::DownCast( this->dataProviderDetect() );

  //if ( hili_dp.IsNull() )
  //  return;

  //// For edges, we have to switch to a wireframe mode
  //if ( thePickRes.IsSelectionEdge() )
  //  hili_pl->WireframeModeOn();
  //else
  //  hili_pl->ShadingModeOn();

  //// Re-initialize highlighting pipeline
  //hili_dp->GetSubShapes()->ChangeMap() = aSubShapeIDs;
  //hili_pl->VoidSubShapesOff();
  //hili_pl->SetInput(hili_dp);
}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void asiVisu_GeomPrs::unHighlight(vtkRenderer*                   theRenderer,
                                  const asiVisu_SelectionNature& theSelNature) const
{
  //asiVisu_NotUsed(theRenderer);

  //Handle(asiVisu_ShapePipeline) hili_pl;
  ////
  //if ( theSelNature == SelectionNature_Pick )
  //  hili_pl = Handle(asiVisu_ShapePipeline)::DownCast( this->GetPickPipeline() );
  //else
  //  hili_pl = Handle(asiVisu_ShapePipeline)::DownCast( this->GetDetectPipeline() );

  //if ( hili_pl.IsNull() )
  //  return;

  //// Block data pipelining
  //hili_pl->VoidSubShapesOn();
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void asiVisu_GeomPrs::renderPipelines(vtkRenderer* theRenderer) const
{
  //Handle(asiVisu_ShapePipeline)
  //  pick_pl = Handle(asiVisu_ShapePipeline)::DownCast( this->GetPickPipeline() ),
  //  detect_pl = Handle(asiVisu_ShapePipeline)::DownCast( this->GetDetectPipeline() );

  //if ( pick_pl.IsNull() || detect_pl.IsNull() )
  //  return;

  //// Detection is initially blocked
  //detect_pl->VoidSubShapesOn();

  //// Picking pipeline must be added to renderer the LAST (!). Otherwise
  //// we experience some strange coloring bug because of their coincidence
  ///* (1) */ detect_pl->AddToRenderer(theRenderer);
  ///* (2) */ pick_pl->AddToRenderer(theRenderer);
}

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void asiVisu_GeomPrs::deRenderPipelines(vtkRenderer* theRenderer) const
{
  /*Handle(asiVisu_ShapePipeline)
    pick_pl = Handle(asiVisu_ShapePipeline)::DownCast( this->GetPickPipeline() ),
    detect_pl = Handle(asiVisu_ShapePipeline)::DownCast( this->GetDetectPipeline() );

  if ( !pick_pl.IsNull() )
    pick_pl->RemoveFromRenderer(theRenderer);

  if ( !detect_pl.IsNull() )
    detect_pl->RemoveFromRenderer(theRenderer);*/
}
