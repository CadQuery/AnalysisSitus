//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_geom_prs.h>

// A-Situs (visualization) includes
#include <visu_display_mode.h>
#include <visu_shape_data_provider.h>

// A-Situs (GUI) includes
#include <gui_common.h>

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

#define COUT_DEBUG

//! Creates a Presentation object for the passed Part Node.
//! \param N [in] Part Node to create a Presentation for.
visu_geom_prs::visu_geom_prs(const Handle(ActAPI_INode)& N) : visu_prs(N)
{
  // Create Data Provider
  Handle(visu_shape_data_provider) DP =
    new visu_shape_data_provider( N->GetId(),
                                  ActParamStream() << N->Parameter(geom_part_node::PID_Geometry) );

  // Main pipeline
  Handle(visu_shape_pipeline) pl = new visu_shape_pipeline(true, true, false, false);
  //
  this->addPipeline        ( Pipeline_Main, pl );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Set point size
  pl->Actor()->GetProperty()->SetPointSize(5.0f);
  pl->Actor()->GetProperty()->SetLineWidth(1.5f);

  /* ====================
   *  Pipeline for edges
   * ==================== */

  // Create pipeline for highlighting
  Handle(visu_shape_pipeline) contour_pl = new visu_shape_pipeline( true, true, true, false, pl->DataSource() );

  // Adjust props
  contour_pl->Actor()->GetProperty()->SetOpacity(0.5);
  contour_pl->Actor()->GetProperty()->SetLineWidth(1.5f);
  contour_pl->Actor()->SetPickable(0);
  //
  contour_pl->WireframeModeOn();
  //
  this->addPipeline        ( Pipeline_Contour, contour_pl );
  this->assignDataProvider ( Pipeline_Contour, DP->Clone() );

  /* ======================
   *  Pipeline for picking
   * ====================== */

  double pick_color[3];
  visu_utils::DefaultPickingColor(pick_color[0], pick_color[1], pick_color[2]);

  // Create pipeline for highlighting
  Handle(visu_shape_pipeline) pick_pl = new visu_shape_pipeline( false, true, true, false, pl->DataSource() );

  // Adjust props
  pick_pl->Actor()->GetProperty()->SetColor(pick_color[0], pick_color[1], pick_color[2]);
  pick_pl->Actor()->GetProperty()->SetLineWidth( visu_utils::DefaultPickLineWidth() );
  pick_pl->Actor()->GetProperty()->SetPointSize( visu_utils::DefaultHilightPointSize() );
  pick_pl->Actor()->SetPickable(0);
  pick_pl->Mapper()->ScalarVisibilityOff();
  //
  pick_pl->Actor()->GetProperty()->SetOpacity(1.0);
  pick_pl->Actor()->GetProperty()->SetAmbient(0.8);
  pick_pl->Actor()->GetProperty()->SetDiffuse(0.2);
  pick_pl->Actor()->GetProperty()->SetSpecular(0.9);
  pick_pl->Actor()->GetProperty()->SetSpecularPower(20);

  // Bind to the data provider
  this->installPickPipeline( pick_pl, DP->Clone() );

  /* ========================
   *  Pipeline for detecting
   * ======================== */

  double detect_color[3];
  visu_utils::DefaultDetectionColor(detect_color[0], detect_color[1], detect_color[2]);

  // Create pipeline for highlighting
  Handle(visu_shape_pipeline) detect_pl = new visu_shape_pipeline( false, true, true, false, pl->DataSource() );

  // Adjust props
  detect_pl->Actor()->GetProperty()->SetColor(detect_color[0], detect_color[1], detect_color[2]);
  detect_pl->Actor()->GetProperty()->SetLineWidth( visu_utils::DefaultDetectionLineWidth() );
  detect_pl->Actor()->GetProperty()->SetPointSize( visu_utils::DefaultHilightPointSize() );
  detect_pl->Actor()->SetPickable(0);
  detect_pl->Mapper()->ScalarVisibilityOff();
  //
  detect_pl->Actor()->GetProperty()->SetOpacity(1.0);
  detect_pl->Actor()->GetProperty()->SetAmbient(0.8);
  detect_pl->Actor()->GetProperty()->SetDiffuse(0.2);
  detect_pl->Actor()->GetProperty()->SetSpecular(0.9);
  detect_pl->Actor()->GetProperty()->SetSpecularPower(20);

  // Bind to the data provider
  this->installDetectPipeline( detect_pl, DP->Clone() );

  //---------------------------------------------------------------------------
  vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();
}

//! Factory method for Presentation.
//! \param N [in] Geometry Node to create a Presentation for.
//! \return new Presentation instance.
Handle(visu_prs) visu_geom_prs::Instance(const Handle(ActAPI_INode)& N)
{
  return new visu_geom_prs(N);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool visu_geom_prs::IsVisible() const
{
  return true;
}

//! Sets SHADING visualization mode.
void visu_geom_prs::DoShading() const
{
  Handle(visu_shape_pipeline)
    pl = Handle(visu_shape_pipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  pl->ShadingModeOn();
}

//! Sets WIREFRAME visualization mode.
void visu_geom_prs::DoWireframe() const
{
  Handle(visu_shape_pipeline)
    pl = Handle(visu_shape_pipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  pl->WireframeModeOn();
}

//! Sets custom color for the geometry.
//! \param theColor [in] color to set.
void visu_geom_prs::DoColor(const QColor& theColor) const
{
  if ( !theColor.isValid() )
    return;

  Handle(visu_shape_pipeline)
    pl = Handle(visu_shape_pipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  pl->Mapper()->ScalarVisibilityOff();
  pl->Actor()->GetProperty()->SetColor( theColor.redF(),
                                        theColor.greenF(),
                                        theColor.blueF() );
}

//! Unsets custom color for the geometry.
void visu_geom_prs::DoUnColor() const
{
  Handle(visu_shape_pipeline)
    pl = Handle(visu_shape_pipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  pl->Mapper()->ScalarVisibilityOn();
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void visu_geom_prs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void visu_geom_prs::afterInitPipelines()
{
  // Access pipelines dedicated for highlighting
  const Handle(visu_shape_pipeline)&
    pick_pl = Handle(visu_shape_pipeline)::DownCast( this->GetPickPipeline() ),
    detect_pl = Handle(visu_shape_pipeline)::DownCast( this->GetDetectPipeline() );

  const Handle(visu_shape_data_provider)&
    pick_dp = Handle(visu_shape_data_provider)::DownCast( this->dataProviderPick() ),
    detect_dp = Handle(visu_shape_data_provider)::DownCast( this->dataProviderDetect() );

  pick_dp->SetSubShapes(new TColStd_HPackedMapOfInteger);
  detect_dp->SetSubShapes(new TColStd_HPackedMapOfInteger);

  // Block selection pipelining initially
  pick_pl->VoidSubShapesOn();
  detect_pl->VoidSubShapesOn();

  // Init them as well (as selection pipelines are not automated)
  pick_pl->SetInput(pick_dp);
  detect_pl->SetInput(detect_dp);
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void visu_geom_prs::beforeUpdatePipelines() const
{
  Handle(geom_part_node) N = Handle(geom_part_node)::DownCast( this->GetNode() );

  visu_display_mode aDMode = (visu_display_mode) N->GetDisplayMode();
  if ( aDMode == DisplayMode_Undefined || aDMode == DisplayMode_Shading )
    this->DoShading();
  else
    this->DoWireframe();
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void visu_geom_prs::afterUpdatePipelines() const
{
  /* ====================================
   *  Update selection pipelines as well
   * ==================================== */

  // Access pipelines dedicated for highlighting
  const Handle(visu_shape_pipeline)&
    pick_pl = Handle(visu_shape_pipeline)::DownCast( this->GetPickPipeline() ),
    detect_pl = Handle(visu_shape_pipeline)::DownCast( this->GetDetectPipeline() );

  // IMPORTANT: We update our highlighting pipelines here just to make things
  // faster. The better place to do that is "highlight" method, because
  // we do not really need to build highlighting pipelines just after
  // the Nodal Presentation is created. Logically, we would better to prepare
  // this pipeline only on actual selection request from user. However, in the
  // latter case the reactivity of application might significantly slow down
  pick_pl->Update();
  detect_pl->Update();

  /* =================
   *  Actualize color
   * ================= */

  Handle(geom_part_node) N = Handle(geom_part_node)::DownCast( this->GetNode() );
  if ( N->HasColor() )
  {
    QColor color = gui_common::IntToColor( N->GetColor() );
    this->DoColor(color);
  }
  else
    this->DoUnColor();
}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection nature (picking or detecting).
void visu_geom_prs::highlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                              const visu_pick_result&      thePickRes,
                              const visu_selection_nature& theSelNature) const
{
  /* ==================
   *  Get target actor
   * ================== */

  Handle(visu_pipeline) main_pl = this->GetPipeline(Pipeline_Main);
  //
  vtkActor* main_actor = main_pl->Actor();

  /* ============================================================
   *  Get the list of sub-shape IDs corresponding to the picking
   *  results on the main actor
   * ============================================================ */

  TColStd_PackedMapOfInteger aSubShapeIDs;
  if ( thePickRes.IsSelectionSubShape() )
  {
    const visu_actor_elem_map& aPickMap = thePickRes.GetPickMap();
    for ( visu_actor_elem_map::Iterator it(aPickMap); it.More(); it.Next() )
    {
      const vtkSmartPointer<vtkActor>& aResActor = it.Key();
      if ( aResActor != main_actor )
        continue;

      aSubShapeIDs = it.Value();
    }
  }

  /* ===============================
   *  Update highlighting pipelines
   * =============================== */

  // Access pipeline for highlighting
  Handle(visu_shape_pipeline) hili_pl;
  //
  if ( theSelNature == SelectionNature_Pick )
    hili_pl = Handle(visu_shape_pipeline)::DownCast( this->GetPickPipeline() );
  else
    hili_pl = Handle(visu_shape_pipeline)::DownCast( this->GetDetectPipeline() );

  // No sub-shapes -> no selection
  if ( thePickRes.IsSelectionSubShape() && aSubShapeIDs.IsEmpty() )
  {
    hili_pl->VoidSubShapesOn();
    return;
  }

  // Access its Data Provider
  Handle(visu_shape_data_provider) hili_dp;
  //
  if ( theSelNature == SelectionNature_Pick )
    hili_dp = Handle(visu_shape_data_provider)::DownCast( this->dataProviderPick() );
  else
    hili_dp = Handle(visu_shape_data_provider)::DownCast( this->dataProviderDetect() );

  // Re-initialize highlighting pipeline
  hili_dp->GetSubShapes()->ChangeMap() = aSubShapeIDs;
  hili_pl->VoidSubShapesOff();
  hili_pl->SetInput(hili_dp);
}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void visu_geom_prs::unHighlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                const visu_selection_nature& theSelNature) const
{
  Handle(visu_shape_pipeline) hili_pl;
  //
  if ( theSelNature == SelectionNature_Pick )
    hili_pl = Handle(visu_shape_pipeline)::DownCast( this->GetPickPipeline() );
  else
    hili_pl = Handle(visu_shape_pipeline)::DownCast( this->GetDetectPipeline() );

  // Block data pipelining
  hili_pl->VoidSubShapesOn();
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void visu_geom_prs::renderPipelines(vtkRenderer* theRenderer) const
{
  Handle(visu_shape_pipeline)
    pick_pl = Handle(visu_shape_pipeline)::DownCast( this->GetPickPipeline() ),
    detect_pl = Handle(visu_shape_pipeline)::DownCast( this->GetDetectPipeline() );

  // Detection is initially blocked
  detect_pl->VoidSubShapesOn();

  // Picking pipeline must be added to renderer the LAST (!). Otherwise
  // we experience some strange coloring bug because of their coincidence
  /* (1) */ detect_pl->AddToRenderer(theRenderer);
  /* (2) */ pick_pl->AddToRenderer(theRenderer);
}

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void visu_geom_prs::deRenderPipelines(vtkRenderer* theRenderer) const
{
  Handle(visu_shape_pipeline)
    pick_pl = Handle(visu_shape_pipeline)::DownCast( this->GetPickPipeline() ),
    detect_pl = Handle(visu_shape_pipeline)::DownCast( this->GetDetectPipeline() );

  pick_pl->RemoveFromRenderer(theRenderer);
  detect_pl->RemoveFromRenderer(theRenderer);
}
