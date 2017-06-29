//-----------------------------------------------------------------------------
// Created on: 18 August 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_EdgeCurvePipeline.h>

// Visualization includes
#include <asiVisu_CurveSource.h>
#include <asiVisu_EdgeDataProvider.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkAppendPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

// OCCT includes
#include <BRep_Tool.hxx>

//-----------------------------------------------------------------------------

//! Creates new Edge Curve Pipeline initialized by default VTK mapper and actor.
asiVisu_EdgeCurvePipeline::asiVisu_EdgeCurvePipeline()
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                 vtkSmartPointer<vtkActor>::New() ),
  m_iStepsNumber(20)
{
  this->Actor()->GetProperty()->SetLineWidth(1.0);
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void asiVisu_EdgeCurvePipeline::SetInput(const Handle(asiVisu_DataProvider)& DP)
{
  if ( !m_iStepsNumber )
    return;

  Handle(asiVisu_EdgeDataProvider)
    edgeProvider = Handle(asiVisu_EdgeDataProvider)::DownCast(DP);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  const int edge_idx = edgeProvider->GetEdgeIndexAmongSubshapes();
  if ( !edge_idx )
  {
    // Pass empty data set in order to have valid pipeline
    vtkSmartPointer<vtkPolyData> aDummyDS = vtkSmartPointer<vtkPolyData>::New();
    this->SetInputData(aDummyDS);
    this->Modified(); // Update modification timestamp
    return; // Do nothing
  }

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( edgeProvider->MustExecute( this->GetMTime() ) )
  {
    TopoDS_Edge E = edgeProvider->ExtractEdge();
    //
    if ( E.IsNull() )
      return;

    // Allocate Data Source
    vtkSmartPointer<asiVisu_CurveSource>
      curveSource = vtkSmartPointer<asiVisu_CurveSource>::New();

    // Set geometry to be converted to VTK polygonal DS
    if ( !curveSource->SetInputEdge(E) )
      return; // No poly data produced

    // Initialize pipeline
    this->SetInputConnection( curveSource->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_EdgeCurvePipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void asiVisu_EdgeCurvePipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void asiVisu_EdgeCurvePipeline::callback_update()
{}
