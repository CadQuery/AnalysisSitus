//-----------------------------------------------------------------------------
// Created on: 05 April 2016
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
#include <asiVisu_RESurfacesPipeline.h>

// Visualization includes
#include <asiVisu_RESurfacesDataProvider.h>
#include <asiVisu_SurfaceSource.h>

// VTK includes
#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

//! Creates new Pipeline initialized by default VTK mapper and actor.
asiVisu_RESurfacesPipeline::asiVisu_RESurfacesPipeline()
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                 vtkSmartPointer<vtkActor>::New() ),
  m_iStepsNumber(20)
{
  this->Actor()->GetProperty()->SetLineWidth(1.0);
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void asiVisu_RESurfacesPipeline::SetInput(const Handle(asiVisu_DataProvider)& DP)
{
  if ( !m_iStepsNumber )
    return;

  Handle(asiVisu_RESurfacesDataProvider)
    provider = Handle(asiVisu_RESurfacesDataProvider)::DownCast(DP);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  const int numSurfaces = provider->GetNumOfSurfaces();
  //
  if ( !numSurfaces )
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
    // Append filter
    vtkSmartPointer<vtkAppendPolyData>
      appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();

    // Convert each surface to polygonal form one by one
    for ( int s = 1; s <= numSurfaces; ++s )
    {
      double uLimit, vLimit;
      Handle(Geom_Surface) S = provider->GetSurface(s, uLimit, vLimit);

      vtkSmartPointer<asiVisu_SurfaceSource>
        src = vtkSmartPointer<asiVisu_SurfaceSource>::New();
      //
      src->SetInputSurface  (S);
      src->SetNumberOfSteps (m_iStepsNumber);
      src->SetTrimValues    (uLimit, vLimit);
      //
      appendFilter->AddInputConnection( src->GetOutputPort() );
    }

    // Initialize pipeline
    this->SetInputConnection( appendFilter->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_RESurfacesPipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void asiVisu_RESurfacesPipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void asiVisu_RESurfacesPipeline::callback_update()
{}
