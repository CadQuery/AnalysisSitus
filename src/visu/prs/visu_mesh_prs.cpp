//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_mesh_prs.h>

// Visualization includes
#include <visu_display_mode.h>
#include <visu_mesh_contour_pipeline.h>
#include <visu_mesh_data_provider.h>
#include <visu_mesh_pipeline.h>
#include <visu_utils.h>

// GUI includes
#include <gui_common.h>

// VTK includes
#include <vtkActor.h>
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object for the passed Mesh Node.
//! \param theNode [in] Mesh Node to create a Presentation for.
visu_mesh_prs::visu_mesh_prs(const Handle(ActAPI_INode)& theNode) : visu_prs(theNode)
{
  // Create Data Provider
  Handle(visu_mesh_data_provider)
    DP = new visu_mesh_data_provider( theNode->GetId(),
                                      ActAPI_ParameterStream() << theNode->Parameter(mesh_node::PID_Mesh) );

  // Pipeline for mesh
  this->addPipeline(Pipeline_Mesh, new visu_mesh_pipeline);
  this->assignDataProvider(Pipeline_Mesh, DP);

  // Pipeline for mesh contour
  this->addPipeline(Pipeline_MeshContour, new visu_mesh_contour_pipeline);
  this->assignDataProvider(Pipeline_MeshContour, DP);

  // We use CONTOUR mesh pipeline along with an ordinary one. Thus it is
  // really necessary to resolve coincident primitives to avoid blinking
  // on mesh edges
  vtkMapper::SetResolveCoincidentTopology(1);

  /* =====================================
   *  Prepare a pipeline for highlighting
   * ===================================== */

  Handle(visu_mesh_contour_pipeline) aHiliPipeline = new visu_mesh_contour_pipeline();

  // Set color, opacity and line width
  double aHiliColor[3];
  visu_utils::DefaultPickingColor(aHiliColor[0], aHiliColor[1], aHiliColor[2]);
  aHiliPipeline->SetColor(aHiliColor[0], aHiliColor[1], aHiliColor[2]);
  aHiliPipeline->SetOpacity(1.0);
  aHiliPipeline->SetLineWidth( visu_utils::DefaultPickLineWidth() );

  // Set picking pipeline
  this->installPickPipeline( aHiliPipeline, DP->Clone() );
}

//! Factory method for Node's Presentation.
//! \param theNode [in] Mesh Node to create a Presentation for.
//! \return new Presentation instance.
Handle(visu_prs) visu_mesh_prs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new visu_mesh_prs(theNode);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool visu_mesh_prs::IsVisible() const
{
  return true; // TODO: make visibility controllable
}

//! Sets SHADING visualization mode.
void visu_mesh_prs::doShading() const
{
  Handle(visu_mesh_pipeline) aMeshPL =
    Handle(visu_mesh_pipeline)::DownCast( this->GetPipeline(Pipeline_Mesh) );
  Handle(visu_mesh_pipeline) aMeshContourPL =
    Handle(visu_mesh_pipeline)::DownCast( this->GetPipeline(Pipeline_MeshContour) );

  aMeshPL->Actor()->GetProperty()->EdgeVisibilityOff();
  aMeshPL->Actor()->GetProperty()->SetRepresentationToSurface();
  aMeshContourPL->Actor()->VisibilityOn();
}

//! Sets WIREFRAME visualization mode.
void visu_mesh_prs::doWireframe() const
{
  Handle(visu_mesh_pipeline) aMeshPL =
    Handle(visu_mesh_pipeline)::DownCast( this->GetPipeline(Pipeline_Mesh) );
  Handle(visu_mesh_pipeline) aMeshContourPL =
    Handle(visu_mesh_pipeline)::DownCast( this->GetPipeline(Pipeline_MeshContour) );

  aMeshPL->Actor()->GetProperty()->EdgeVisibilityOn();
  aMeshPL->Actor()->GetProperty()->SetRepresentationToWireframe();
  aMeshContourPL->Actor()->VisibilityOff();
}

//! Sets custom color for the Mesh.
//! \param theColor [in] color to set.
void visu_mesh_prs::doColor(const QColor& theColor) const
{
  if ( !theColor.isValid() )
    return;

  Handle(visu_mesh_pipeline) aMeshPL =
    Handle(visu_mesh_pipeline)::DownCast( this->GetPipeline(Pipeline_Mesh) );

  aMeshPL->Mapper()->ScalarVisibilityOff();
  aMeshPL->Actor()->GetProperty()->SetColor( theColor.redF(),
                                             theColor.greenF(),
                                             theColor.blueF() );
}

//! Unsets custom color for the Mesh.
void visu_mesh_prs::doUnColor() const
{
  Handle(visu_mesh_pipeline) aMeshPL =
    Handle(visu_mesh_pipeline)::DownCast( this->GetPipeline(Pipeline_Mesh) );

  aMeshPL->Mapper()->ScalarVisibilityOn();
}

//! Callback for initialization of Presentation pipelines.
void visu_mesh_prs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void visu_mesh_prs::afterInitPipelines()
{
  // Access a dedicated pipeline for highlighting
  const Handle(visu_mesh_contour_pipeline)& aHiliPL =
    Handle(visu_mesh_contour_pipeline)::DownCast( this->GetPickPipeline() );

  // Initialize the pipeline's input
  aHiliPL->SetInput( this->dataProvider(Pipeline_Mesh) );
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void visu_mesh_prs::beforeUpdatePipelines() const
{
  Handle(mesh_node) MeshNode = Handle(mesh_node)::DownCast( this->GetNode() );

  visu_display_mode aDMode = (visu_display_mode) MeshNode->GetDisplayMode();
  if ( aDMode == DisplayMode_Undefined || aDMode == DisplayMode_Shading )
    this->doShading();
  else if ( aDMode == DisplayMode_Wireframe )
    this->doWireframe();
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void visu_mesh_prs::afterUpdatePipelines() const
{
  /* ======================================
   *  Update highlighting pipeline as well
   * ====================================== */

  // Access a dedicated pipeline for highlighting
  const Handle(visu_mesh_contour_pipeline)& aHiliPL =
    Handle(visu_mesh_contour_pipeline)::DownCast( this->GetPickPipeline() );

  // IMPORTANT: We update our highlighting pipeline here just to make things
  // faster. The better place to do that is "highlight" method, because
  // we do not really need to build highlighting pipeline just after
  // the Nodal Presentation is created. Logically, we would better to prepare
  // this pipeline only on actual pick request from user. However, in the
  // latter case the reactivity of application might significantly slow down
  aHiliPL->Update();

  /* =================
   *  Actualize color
   * ================= */

  Handle(mesh_node) MeshNode = Handle(mesh_node)::DownCast( this->GetNode() );
  if ( MeshNode->HasColor() )
  {
    QColor aColor = gui_common::IntToColor( MeshNode->GetColor() );
    this->doColor(aColor);
  }
  else
    this->doUnColor();
}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection kind.
void visu_mesh_prs::highlight(vtkRenderer*                 theRenderer,
                              const visu_pick_result&      ASitus_NotUsed(thePickRes),
                              const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Access the pipeline for highlighting
  const Handle(visu_mesh_contour_pipeline)&
    aHiliPL = Handle(visu_mesh_contour_pipeline)::DownCast( this->GetPickPipeline() );

  // Now render (pipeline is already built in "initPipelines" callback)
  aHiliPL->AddToRenderer(theRenderer);
}

//! Callback for un-highlighting.
//! \param theRenderer  [in] renderer.
//! \param theSelNature [in] selection kind.
void visu_mesh_prs::unHighlight(vtkRenderer*                 theRenderer,
                                const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Access the pipeline for highlighting
  const Handle(visu_mesh_contour_pipeline)&
    aHiliPL = Handle(visu_mesh_contour_pipeline)::DownCast( this->GetPickPipeline() );

  // Remove the pipeline from renderer
  aHiliPL->RemoveFromRenderer(theRenderer);
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void visu_mesh_prs::renderPipelines(vtkRenderer* ASitus_NotUsed(theRenderer)) const
{
}

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void visu_mesh_prs::deRenderPipelines(vtkRenderer* theRenderer) const
{
  this->unHighlight(theRenderer, SelectionNature_None);
}
