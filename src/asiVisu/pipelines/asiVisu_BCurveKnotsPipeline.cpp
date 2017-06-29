//-----------------------------------------------------------------------------
// Created on: 02 September 2016
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
#include <asiVisu_BCurveKnotsPipeline.h>

// Visualization includes
#include <asiVisu_BCurveKnotsSource.h>
#include <asiVisu_CurveDataProvider.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// VTK includes
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//! Creates new Pipeline initialized by default VTK mapper and actor.
asiVisu_BCurveKnotsPipeline::asiVisu_BCurveKnotsPipeline()
//
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(), vtkSmartPointer<vtkActor>::New() )
{
  this->Actor()->GetProperty()->SetPointSize(5.0f);
}

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void asiVisu_BCurveKnotsPipeline::SetInput(const Handle(asiVisu_DataProvider)& DP)
{
  Handle(asiVisu_CurveDataProvider) dp = Handle(asiVisu_CurveDataProvider)::DownCast(DP);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  Handle(Standard_Type) curve_type = dp->GetCurveType();
  //
  if ( curve_type.IsNull() )
  {
    // Pass empty data set in order to have valid pipeline
    vtkSmartPointer<vtkPolyData> dummyDS = vtkSmartPointer<vtkPolyData>::New();
    this->SetInputData(dummyDS);
    this->Modified(); // Update modification timestamp
    return; // Do nothing
  }

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( dp->MustExecute( this->GetMTime() ) )
  {
    // Access curve (3d or 2d)
    double f, l;
    Handle(Geom_Curve) c3d = dp->GetCurve(f, l);

    // B-curve knots
    if ( !c3d.IsNull() && c3d->IsKind( STANDARD_TYPE(Geom_BSplineCurve) ) )
    {
      Handle(Geom_BSplineCurve) BC = Handle(Geom_BSplineCurve)::DownCast(c3d);
      //
      vtkSmartPointer<asiVisu_BCurveKnotsSource>
        bpoles_src = vtkSmartPointer<asiVisu_BCurveKnotsSource>::New();
      //
      bpoles_src->SetInputCurve(BC);

      // Connect data source to the pipeline
      this->SetInputConnection( bpoles_src->GetOutputPort() );
    }
    else
    {
      // Pass empty data set in order to have valid pipeline
      vtkSmartPointer<vtkPolyData> dummyDS = vtkSmartPointer<vtkPolyData>::New();
      this->SetInputData(dummyDS);
    }
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_BCurveKnotsPipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void asiVisu_BCurveKnotsPipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void asiVisu_BCurveKnotsPipeline::callback_update()
{}
