//-----------------------------------------------------------------------------
// Created on: 19 September 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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
#include <asiVisu_ContourPrs.h>

// asiVisu includes
#include <asiVisu_ContourDataProvider.h>
#include <asiVisu_ContourPointsDataProvider.h>
#include <asiVisu_ContourPolesDataProvider.h>
#include <asiVisu_PointsPipeline.h>
#include <asiVisu_ShapePipeline.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>

//-----------------------------------------------------------------------------

//! Creates a Presentation object for the passed Geometry Contour Node.
//! \param[in] N Contour Node to create a Presentation for.
asiVisu_ContourPrs::asiVisu_ContourPrs(const Handle(ActAPI_INode)& N)
: asiVisu_Prs(N)
{
  Handle(asiData_ContourNode) contour_n = Handle(asiData_ContourNode)::DownCast(N);

  // Create Data Provider for polyline
  Handle(asiVisu_ContourDataProvider)
    DP_contour = new asiVisu_ContourDataProvider(contour_n);

  // Create Data Provider for points
  Handle(asiVisu_ContourPointsDataProvider)
    DP_points = new asiVisu_ContourPointsDataProvider(contour_n);

  // Create Data Provider for poles
  Handle(asiVisu_ContourPolesDataProvider)
    DP_poles = new asiVisu_ContourPolesDataProvider(contour_n);

  // Pipeline for contour
  Handle(asiVisu_ShapePipeline) shape_pl = new asiVisu_ShapePipeline(false);
  shape_pl->GetDisplayModeFilter()->SetDisplayMode(ShapeDisplayMode_Wireframe);
  shape_pl->Actor()->GetProperty()->SetColor(1.0, 0.0, 0.0);
  //
  this->addPipeline        ( Pipeline_Main, shape_pl );
  this->assignDataProvider ( Pipeline_Main, DP_contour );

  // Pipeline for points
  Handle(asiVisu_PointsPipeline) points_pl = new asiVisu_PointsPipeline;
  //
  this->addPipeline        ( Pipeline_Points, points_pl );
  this->assignDataProvider ( Pipeline_Points, DP_points );

  // Pipeline for poles
  Handle(asiVisu_PointsPipeline) poles_pl = new asiVisu_PointsPipeline;
  //
  this->addPipeline        ( Pipeline_Poles, poles_pl );
  this->assignDataProvider ( Pipeline_Poles, DP_poles );

  // Configure presentation
  shape_pl  -> Actor()->GetProperty()->SetLineWidth(2.0f);
  shape_pl  -> Actor()->GetProperty()->SetLighting(0);
  shape_pl  -> Mapper()->SetScalarVisibility(0);
  //
  points_pl -> Actor()->GetProperty()->SetColor(0.0, 0.4, 1.0);
  poles_pl  -> Actor()->GetProperty()->SetLighting(0);
  points_pl -> Actor()->GetProperty()->SetPointSize(2.0f);
  //
  poles_pl -> Actor()->GetProperty()->SetColor(1.0, 0.0, 1.0);
  poles_pl -> Actor()->GetProperty()->SetLighting(0);
  poles_pl -> Actor()->GetProperty()->SetPointSize(5.0f);

  // Make contour be visualized always on top of the scene
  shape_pl->Mapper()->SetRelativeCoincidentTopologyLineOffsetParameters(0,-66000);
  shape_pl->Mapper()->SetRelativeCoincidentTopologyPolygonOffsetParameters(0,-66000);
  shape_pl->Mapper()->SetRelativeCoincidentTopologyPointOffsetParameter(-66000);
  //
  points_pl->Mapper()->SetRelativeCoincidentTopologyLineOffsetParameters(0,-66000);
  points_pl->Mapper()->SetRelativeCoincidentTopologyPolygonOffsetParameters(0,-66000);
  points_pl->Mapper()->SetRelativeCoincidentTopologyPointOffsetParameter(-66000);
  //
  poles_pl->Mapper()->SetRelativeCoincidentTopologyLineOffsetParameters(0,-66000);
  poles_pl->Mapper()->SetRelativeCoincidentTopologyPolygonOffsetParameters(0,-66000);
  poles_pl->Mapper()->SetRelativeCoincidentTopologyPointOffsetParameter(-66000);
}

//! Factory method for Presentation.
//! \param theNode [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_ContourPrs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new asiVisu_ContourPrs(theNode);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_ContourPrs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_ContourPrs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void asiVisu_ContourPrs::afterInitPipelines()
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_ContourPrs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_ContourPrs::afterUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection nature (picking or detecting).
void asiVisu_ContourPrs::highlight(vtkRenderer*                  theRenderer,
                                   const asiVisu_PickResult&     thePickRes,
                                   const asiVisu_SelectionNature theSelNature) const
{
  asiVisu_NotUsed(theRenderer);
  asiVisu_NotUsed(thePickRes);
  asiVisu_NotUsed(theSelNature);
}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void asiVisu_ContourPrs::unHighlight(vtkRenderer*                  theRenderer,
                                     const asiVisu_SelectionNature theSelNature) const
{
  asiVisu_NotUsed(theRenderer);
  asiVisu_NotUsed(theSelNature);
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void asiVisu_ContourPrs::renderPipelines(vtkRenderer* theRenderer) const
{
  asiVisu_NotUsed(theRenderer);
}

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void asiVisu_ContourPrs::deRenderPipelines(vtkRenderer* theRenderer) const
{
  asiVisu_NotUsed(theRenderer);
}
