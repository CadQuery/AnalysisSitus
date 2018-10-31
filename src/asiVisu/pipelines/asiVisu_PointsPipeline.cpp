//-----------------------------------------------------------------------------
// Created on: 06 April 2016
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
#include <asiVisu_PointsPipeline.h>

// asiVisu includes
#include <asiVisu_NodeInfo.h>
#include <asiVisu_PointsSource.h>
#include <asiVisu_PointsDataProvider.h>

// VTK includes
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

//! Creates new Pipeline initialized by default VTK mapper and actor.
asiVisu_PointsPipeline::asiVisu_PointsPipeline()
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                    vtkSmartPointer<vtkActor>::New() )
{
  // Default visual style
  this->Actor()->GetProperty()->SetPointSize(3.0);
  this->Actor()->GetProperty()->SetColor(1.0, 0.0, 0.0);
  this->Actor()->GetProperty()->SetOpacity(0.5);
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void asiVisu_PointsPipeline::SetInput(const Handle(asiVisu_DataProvider)& DP)
{
  const Handle(asiVisu_PointsDataProvider)&
    provider = Handle(asiVisu_PointsDataProvider)::DownCast(DP);
  //
  if ( provider.IsNull() )
    return;

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  Handle(asiAlgo_BaseCloud<double>) points = provider->GetPoints();
  //
  if ( points.IsNull() || points->IsEmpty() )
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

  if ( provider->MustExecute( this->GetMTime() ) )
  {
    // Source for points
    vtkSmartPointer< asiVisu_PointsSource<double> >
      src = vtkSmartPointer< asiVisu_PointsSource<double> >::New();
    //
    src->SetInputPoints(points);
    src->SetFilter( provider->GetIndices() );

    // Bind to a Data Node using information key
    asiVisu_NodeInfo::Store( provider->GetNodeID(), this->Actor() );

    // Initialize pipeline
    this->SetInputConnection( src->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_PointsPipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void asiVisu_PointsPipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void asiVisu_PointsPipeline::callback_update()
{}
