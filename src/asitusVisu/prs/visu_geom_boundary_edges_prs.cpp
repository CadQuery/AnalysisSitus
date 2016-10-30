//-----------------------------------------------------------------------------
// Created on: 22 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_geom_boundary_edges_prs.h>

// A-Situs (visualization) includes
#include <visu_shape_data_provider.h>
#include <visu_shape_pipeline.h>

// A-Situs (GUI) includes
#include <gui_common.h>

// VTK includes
#include <vtkActor.h>
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object for the passed Node.
//! \param N [in] Node to create a Presentation for.
visu_geom_boundary_edges_prs::visu_geom_boundary_edges_prs(const Handle(ActAPI_INode)& N) : visu_prs(N)
{
  // Create Data Providers
  Handle(visu_shape_data_provider) DP_convex =
    new visu_shape_data_provider( N->GetId(),
                                  ActParamStream() << N->Parameter(geom_boundary_edges_node::PID_Green) );
  //
  Handle(visu_shape_data_provider) DP_concave =
    new visu_shape_data_provider( N->GetId(),
                                  ActParamStream() << N->Parameter(geom_boundary_edges_node::PID_Red) );
  //
  Handle(visu_shape_data_provider) DP_undefined =
    new visu_shape_data_provider( N->GetId(),
                                  ActParamStream() << N->Parameter(geom_boundary_edges_node::PID_Ordinary) );

  //---------------------------------------------------------------------------
  // Pipeline for convex edges
  Handle(visu_shape_pipeline) pl_convex = new visu_shape_pipeline(false, false, false, false);
  //
  this->addPipeline        ( Pipeline_Convex, pl_convex );
  this->assignDataProvider ( Pipeline_Convex, DP_convex );
  //
  pl_convex->Actor()->GetProperty()->SetLineWidth(2.0f);
  pl_convex->Actor()->GetProperty()->SetColor(0.2, 1.0, 0.0);
  pl_convex->Mapper()->ScalarVisibilityOff();
  pl_convex->WireframeModeOn();

  //---------------------------------------------------------------------------
  // Pipeline for concave edges
  Handle(visu_shape_pipeline) pl_concave = new visu_shape_pipeline(false, false, false, false);
  //
  this->addPipeline        ( Pipeline_Concave, pl_concave );
  this->assignDataProvider ( Pipeline_Concave, DP_concave );
  //
  pl_concave->Actor()->GetProperty()->SetLineWidth(2.0f);
  pl_concave->Actor()->GetProperty()->SetColor(1.0, 0.0, 0.0);
  pl_concave->Mapper()->ScalarVisibilityOff();
  pl_concave->WireframeModeOn();

  //---------------------------------------------------------------------------
  // Pipeline for undefined edges
  Handle(visu_shape_pipeline) pl_undefined = new visu_shape_pipeline(false, false, false, false);
  //
  this->addPipeline        ( Pipeline_Undefined, pl_undefined );
  this->assignDataProvider ( Pipeline_Undefined, DP_undefined );
  //
  pl_undefined->Actor()->GetProperty()->SetLineWidth(2.0f);
  pl_undefined->Actor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
  pl_undefined->Mapper()->ScalarVisibilityOff();
  pl_undefined->WireframeModeOn();

  //---------------------------------------------------------------------------
  vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();
}

//! Factory method for Presentation.
//! \param N [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(visu_prs) visu_geom_boundary_edges_prs::Instance(const Handle(ActAPI_INode)& N)
{
  return new visu_geom_boundary_edges_prs(N);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool visu_geom_boundary_edges_prs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void visu_geom_boundary_edges_prs::beforeInitPipelines()
{
}

//! Callback for initialization of Presentation pipelines.
void visu_geom_boundary_edges_prs::afterInitPipelines()
{
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void visu_geom_boundary_edges_prs::beforeUpdatePipelines() const
{
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void visu_geom_boundary_edges_prs::afterUpdatePipelines() const
{
}

//! Callback for highlighting.
void visu_geom_boundary_edges_prs::highlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                             const visu_pick_result&      ASitus_NotUsed(thePickRes),
                                             const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
}

//! Callback for highlighting reset.
void visu_geom_boundary_edges_prs::unHighlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                               const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
}

//! Callback for rendering.
void visu_geom_boundary_edges_prs::renderPipelines(vtkRenderer* ASitus_NotUsed(theRenderer)) const
{
}

//! Callback for de-rendering.
void visu_geom_boundary_edges_prs::deRenderPipelines(vtkRenderer* ASitus_NotUsed(theRenderer)) const
{
}
