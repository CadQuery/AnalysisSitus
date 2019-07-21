//-----------------------------------------------------------------------------
// Created on: 06 November 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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
#include <asiVisu_ReEdgePrs.h>

// asiVisu includes
#include <asiVisu_BCurveKnotsPipeline.h>
#include <asiVisu_BCurvePolesPipeline.h>
#include <asiVisu_CurvePipeline.h>
#include <asiVisu_PointsPipeline.h>
#include <asiVisu_ReEdgeCurveDataProvider.h>
#include <asiVisu_ReEdgePolylineDataProvider.h>
#include <asiVisu_ReEdgeShapeDataProvider.h>
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

asiVisu_ReEdgePrs::asiVisu_ReEdgePrs(const Handle(ActAPI_INode)& N)
: asiVisu_DefaultPrs(N)
{
  Handle(asiData_ReEdgeNode) edge_n = Handle(asiData_ReEdgeNode)::DownCast(N);

  // Create Data Provider for polyline.
  Handle(asiVisu_ReEdgePolylineDataProvider)
    polyline_dp = new asiVisu_ReEdgePolylineDataProvider(edge_n);

  // Create Data Provider for edge as a geometric entity.
  Handle(asiVisu_ReEdgeCurveDataProvider)
    curve_dp = new asiVisu_ReEdgeCurveDataProvider(edge_n);

  // Pipeline for polyline.
  Handle(asiVisu_PointsPipeline) polyline_pl = new asiVisu_PointsPipeline(false);
  //
  this->addPipeline        ( Pipeline_Polyline, polyline_pl );
  this->assignDataProvider ( Pipeline_Polyline, polyline_dp );

  // Pipeline for curve.
  Handle(asiVisu_CurvePipeline) curve_pl = new asiVisu_CurvePipeline;
  curve_pl->Actor()->GetProperty()->SetColor(1.0, 0.0, 0.0);
  //
  this->addPipeline        ( Pipeline_Curve, curve_pl );
  this->assignDataProvider ( Pipeline_Curve, curve_dp );

  // Pipeline for poles of b-curves.
  this->addPipeline        ( Pipeline_CurvePoles, new asiVisu_BCurvePolesPipeline );
  this->assignDataProvider ( Pipeline_CurvePoles, curve_dp );

  // Pipeline for knots of b-curves.
  this->addPipeline        ( Pipeline_CurveKnots, new asiVisu_BCurveKnotsPipeline );
  this->assignDataProvider ( Pipeline_CurveKnots, curve_dp );

  // Configure presentation.
  curve_pl -> Actor()->GetProperty()->SetLineWidth(2.0f);
  curve_pl -> Actor()->GetProperty()->SetLighting(0);
  curve_pl -> Actor()->SetPickable(0);
  curve_pl -> Mapper()->SetScalarVisibility(0);
  //
  polyline_pl -> Actor()->GetProperty()->SetColor(0.0, 0.4, 1.0);
  polyline_pl -> Actor()->GetProperty()->SetLighting(0);
  polyline_pl -> Actor()->GetProperty()->SetPointSize(2.0f);
  polyline_pl -> Actor()->SetPickable(0);

  // Adjust props of poles and knots.
  this->GetPipeline(Pipeline_CurvePoles)->Actor()->GetProperty()->SetColor(0.6, 0.6, 0.6);
  this->GetPipeline(Pipeline_CurveKnots)->Actor()->GetProperty()->SetColor(0.0, 1.0, 0.0);
  //
  this->GetPipeline(Pipeline_CurvePoles)->Actor()->SetPickable(0);
  this->GetPipeline(Pipeline_CurveKnots)->Actor()->SetPickable(0);
  //
  this->GetPipeline(Pipeline_CurvePoles)->Actor()->SetVisibility(0);
  this->GetPipeline(Pipeline_CurveKnots)->Actor()->SetVisibility(0);

  //// Make edge be visualized always on top of the scene.
  //curve_pl->Mapper()->SetRelativeCoincidentTopologyLineOffsetParameters(0,-66000);
  //curve_pl->Mapper()->SetRelativeCoincidentTopologyPolygonOffsetParameters(0,-66000);
  //curve_pl->Mapper()->SetRelativeCoincidentTopologyPointOffsetParameter(-66000);
  ////
  //polyline_pl->Mapper()->SetRelativeCoincidentTopologyLineOffsetParameters(0,-66000);
  //polyline_pl->Mapper()->SetRelativeCoincidentTopologyPolygonOffsetParameters(0,-66000);
  //polyline_pl->Mapper()->SetRelativeCoincidentTopologyPointOffsetParameter(-66000);
}

//-----------------------------------------------------------------------------

Handle(asiVisu_Prs) asiVisu_ReEdgePrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new asiVisu_ReEdgePrs(N);
}
