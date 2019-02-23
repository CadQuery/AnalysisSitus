//-----------------------------------------------------------------------------
// Created on: 18 August 2016
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
