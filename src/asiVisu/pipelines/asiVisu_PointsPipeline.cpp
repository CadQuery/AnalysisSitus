//-----------------------------------------------------------------------------
// Created on: 06 April 2016
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
  if ( points.IsNull() )
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
