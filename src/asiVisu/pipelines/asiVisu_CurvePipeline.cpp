//-----------------------------------------------------------------------------
// Created on: 10 February 2016
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
#include <asiVisu_CurvePipeline.h>

// asiVisu includes
#include <asiVisu_CurveSource.h>
#include <asiVisu_CurveDataProvider.h>

// VTK includes
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//! Creates new Curve Pipeline initialized by default VTK mapper and actor.
asiVisu_CurvePipeline::asiVisu_CurvePipeline()
//
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(), vtkSmartPointer<vtkActor>::New() )
{
  this->Actor()->GetProperty()->SetLineWidth(2.0);
}

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void asiVisu_CurvePipeline::SetInput(const Handle(asiVisu_DataProvider)& DP)
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
    // Curve source
    double f, l;
    vtkSmartPointer<asiVisu_CurveSource> src = vtkSmartPointer<asiVisu_CurveSource>::New();
    //
    if ( curve_type->SubType( STANDARD_TYPE(Geom2d_Curve) ) )
    {
      Handle(Geom2d_Curve) curve = dp->GetCurve2d(f, l);
      src->SetInputCurve2d(curve, f, l);

      // Set tip size
      const double tipSize = Max( 0.1, (curve->Value(l).XY() - curve->Value(f).XY() ).Modulus()*0.1 );
      //
      src->SetTipSize(tipSize);
    }
    else if ( curve_type->SubType( STANDARD_TYPE(Geom_Curve) ) )
    {
      Handle(Geom_Curve) curve = dp->GetCurve(f, l);
      src->SetInputCurve(curve, f, l);

      // Set tip size
      const double tipSize = Max( 0.1, (curve->Value(l).XYZ() - curve->Value(f).XYZ() ).Modulus()*0.1 );
      //
      src->SetTipSize(0/*tipSize*/);
    }
    else
      Standard_ProgramError::Raise("Not yet implemented");

    // Initialize pipeline
    this->SetInputConnection( src->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_CurvePipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void asiVisu_CurvePipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void asiVisu_CurvePipeline::callback_update()
{}
