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
#include <asiVisu_PointsFromListDataProvider.h>
#include <asiVisu_ShapePipeline.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiVisu_ContourPrs::asiVisu_ContourPrs(const Handle(ActAPI_INode)& N)
: asiVisu_DefaultPrs(N)
{
  Handle(asiData_ContourNode) contour_n = Handle(asiData_ContourNode)::DownCast(N);

  // Create Data Provider for polyline
  Handle(asiVisu_ContourDataProvider)
    shape_dp = new asiVisu_ContourDataProvider(contour_n);

  // Create Data Provider for points
  Handle(asiVisu_ContourPointsDataProvider)
    points_dp = new asiVisu_ContourPointsDataProvider(contour_n);

  // Create Data Provider for poles
  Handle(asiVisu_ContourPolesDataProvider)
    poles_dp = new asiVisu_ContourPolesDataProvider(contour_n);

  // Create Data Provider for the selected points
  Handle(asiVisu_PointsFromListDataProvider)
    poles_from_list_dp = new asiVisu_PointsFromListDataProvider;

  // Pipeline for contour
  Handle(asiVisu_ShapePipeline) shape_pl = new asiVisu_ShapePipeline(false);
  shape_pl->GetDisplayModeFilter()->SetDisplayMode(ShapeDisplayMode_Wireframe);
  shape_pl->Actor()->GetProperty()->SetColor(1.0, 0.0, 0.0);
  //
  this->addPipeline        ( Pipeline_Main, shape_pl );
  this->assignDataProvider ( Pipeline_Main, shape_dp );

  // Pipeline for points
  Handle(asiVisu_PointsPipeline) points_pl = new asiVisu_PointsPipeline(false);
  //
  this->addPipeline        ( Pipeline_Points, points_pl );
  this->assignDataProvider ( Pipeline_Points, points_dp );

  // Configure presentation
  shape_pl  -> Actor()->GetProperty()->SetLineWidth(2.0f);
  shape_pl  -> Actor()->GetProperty()->SetLighting(0);
  shape_pl  -> Actor()->SetPickable(0);
  shape_pl  -> Mapper()->SetScalarVisibility(0);
  //
  points_pl -> Actor()->GetProperty()->SetColor(0.0, 0.4, 1.0);
  points_pl -> Actor()->GetProperty()->SetLighting(0);
  points_pl -> Actor()->GetProperty()->SetPointSize(2.0f);
  points_pl -> Actor()->SetPickable(0);

  // Make contour be visualized always on top of the scene
  shape_pl->Mapper()->SetRelativeCoincidentTopologyLineOffsetParameters(0,-66000);
  shape_pl->Mapper()->SetRelativeCoincidentTopologyPolygonOffsetParameters(0,-66000);
  shape_pl->Mapper()->SetRelativeCoincidentTopologyPointOffsetParameter(-66000);
  //
  points_pl->Mapper()->SetRelativeCoincidentTopologyLineOffsetParameters(0,-66000);
  points_pl->Mapper()->SetRelativeCoincidentTopologyPolygonOffsetParameters(0,-66000);
  points_pl->Mapper()->SetRelativeCoincidentTopologyPointOffsetParameter(-66000);
}

//-----------------------------------------------------------------------------

Handle(asiVisu_Prs) asiVisu_ContourPrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new asiVisu_ContourPrs(N);
}
