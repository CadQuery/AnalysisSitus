//-----------------------------------------------------------------------------
// Created on: 19 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_geom_contour_prs.h>

// A-Situs (visualization) includes
#include <visu_contour_data_provider.h>
#include <visu_contour_points_data_provider.h>
#include <visu_points_pipeline.h>
#include <visu_shape_pipeline.h>
#include <visu_utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>

//-----------------------------------------------------------------------------

//! Creates a Presentation object for the passed Geometry Contour Node.
//! \param theNode [in] Geometry Contour Node to create a Presentation for.
visu_geom_contour_prs::visu_geom_contour_prs(const Handle(ActAPI_INode)& theNode)
: visu_prs(theNode)
{
  // Create Data Provider for polyline
  Handle(visu_contour_data_provider)
    DP_contour = new visu_contour_data_provider;

  // Create Data Provider for points
  Handle(visu_contour_points_data_provider)
    DP_points = new visu_contour_points_data_provider;

  // Pipeline for contour
  Handle(visu_shape_pipeline) shape_pl = new visu_shape_pipeline();
  shape_pl->WireframeModeOn();
  //
  this->addPipeline        ( Pipeline_Main, shape_pl );
  this->assignDataProvider ( Pipeline_Main, DP_contour );

  // Pipeline for points
  Handle(visu_points_pipeline) points_pl = new visu_points_pipeline;
  //
  this->addPipeline        ( Pipeline_Points, points_pl );
  this->assignDataProvider ( Pipeline_Points, DP_points );

  // Configure presentation
  shape_pl  -> Actor()->GetProperty()->SetLineWidth(2.0f);
  shape_pl  -> Actor()->GetProperty()->SetLighting(0);
  shape_pl  -> Mapper()->SetScalarVisibility(0);
  points_pl -> Actor()->GetProperty()->SetColor(0.0, 0.0, 1.0);
  points_pl -> Actor()->GetProperty()->SetPointSize(5.0f);

  // Make contour be visualized always on top of the scene
  shape_pl->Mapper()->SetRelativeCoincidentTopologyLineOffsetParameters(0,-66000);
  shape_pl->Mapper()->SetRelativeCoincidentTopologyPolygonOffsetParameters(0,-66000);
  shape_pl->Mapper()->SetRelativeCoincidentTopologyPointOffsetParameter(-66000);
  //
  points_pl->Mapper()->SetRelativeCoincidentTopologyLineOffsetParameters(0,-66000);
  points_pl->Mapper()->SetRelativeCoincidentTopologyPolygonOffsetParameters(0,-66000);
  points_pl->Mapper()->SetRelativeCoincidentTopologyPointOffsetParameter(-66000);
}

//! Factory method for Presentation.
//! \param theNode [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(visu_prs) visu_geom_contour_prs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new visu_geom_contour_prs(theNode);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool visu_geom_contour_prs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void visu_geom_contour_prs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void visu_geom_contour_prs::afterInitPipelines()
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void visu_geom_contour_prs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void visu_geom_contour_prs::afterUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection nature (picking or detecting).
void visu_geom_contour_prs::highlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                      const visu_pick_result&      ASitus_NotUsed(thePickRes),
                                      const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void visu_geom_contour_prs::unHighlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                        const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void visu_geom_contour_prs::renderPipelines(vtkRenderer* theRenderer) const
{
  // Do nothing...
}

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void visu_geom_contour_prs::deRenderPipelines(vtkRenderer* ASitus_NotUsed(theRenderer)) const
{
  // Do nothing...
}
