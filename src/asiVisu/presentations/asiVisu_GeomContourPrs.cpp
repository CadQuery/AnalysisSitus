//-----------------------------------------------------------------------------
// Created on: 19 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_GeomContourPrs.h>

// A-Situs (visualization) includes
#include <asiVisu_ContourDataProvider.h>
#include <asiData_ContourPointsDataProvider.h>
#include <asiVisu_PointsPipeline.h>
#include <asiVisu_ShapePipeline.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>

//-----------------------------------------------------------------------------

//! Creates a Presentation object for the passed Geometry Contour Node.
//! \param theNode [in] Geometry Contour Node to create a Presentation for.
asiVisu_GeomContourPrs::asiVisu_GeomContourPrs(const Handle(ActAPI_INode)& theNode)
: asiVisu_Prs(theNode)
{
  // Create Data Provider for polyline
  Handle(asiVisu_ContourDataProvider)
    DP_contour = new asiVisu_ContourDataProvider;

  // Create Data Provider for points
  Handle(asiData_ContourPointsDataProvider)
    DP_points = new asiData_ContourPointsDataProvider;

  // Pipeline for contour
  Handle(asiVisu_ShapePipeline) shape_pl = new asiVisu_ShapePipeline();
  shape_pl->WireframeModeOn();
  //
  this->addPipeline        ( Pipeline_Main, shape_pl );
  this->assignDataProvider ( Pipeline_Main, DP_contour );

  // Pipeline for points
  Handle(asiVisu_PointsPipeline) points_pl = new asiVisu_PointsPipeline;
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
Handle(asiVisu_Prs) asiVisu_GeomContourPrs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new asiVisu_GeomContourPrs(theNode);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_GeomContourPrs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_GeomContourPrs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void asiVisu_GeomContourPrs::afterInitPipelines()
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_GeomContourPrs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_GeomContourPrs::afterUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection nature (picking or detecting).
void asiVisu_GeomContourPrs::highlight(vtkRenderer*                 asiVisu_NotUsed(theRenderer),
                                      const asiVisu_PickResult&      asiVisu_NotUsed(thePickRes),
                                      const asiVisu_SelectionNature& asiVisu_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void asiVisu_GeomContourPrs::unHighlight(vtkRenderer*                 asiVisu_NotUsed(theRenderer),
                                        const asiVisu_SelectionNature& asiVisu_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void asiVisu_GeomContourPrs::renderPipelines(vtkRenderer* theRenderer) const
{
  // Do nothing...
}

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void asiVisu_GeomContourPrs::deRenderPipelines(vtkRenderer* asiVisu_NotUsed(theRenderer)) const
{
  // Do nothing...
}
