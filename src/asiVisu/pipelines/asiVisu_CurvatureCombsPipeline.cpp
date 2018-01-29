//-----------------------------------------------------------------------------
// Created on: 29 January 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2017-2018, Sergey Slyadnev
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
#include <asiVisu_CurvatureCombsPipeline.h>

// asiVisu includes
#include <asiVisu_CurvatureCombsSource.h>
#include <asiVisu_CurveDataProvider.h>

// VTK includes
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//! Creates new Pipeline initialized by default VTK mapper and actor.
asiVisu_CurvatureCombsPipeline::asiVisu_CurvatureCombsPipeline()
//
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(), vtkSmartPointer<vtkActor>::New() )
{
  this->Actor()->GetProperty()->SetLineWidth(2.0);
}

//! Sets input data for the pipeline.
//! \param[in] DP Data Provider.
void asiVisu_CurvatureCombsPipeline::SetInput(const Handle(asiVisu_DataProvider)& DP)
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
    // Curvature combs source
    double f, l;
    vtkSmartPointer<asiVisu_CurvatureCombsSource>
      src = vtkSmartPointer<asiVisu_CurvatureCombsSource>::New();
    //
    if ( curve_type->SubType( STANDARD_TYPE(Geom_Curve) ) )
    {
      Handle(Geom_Curve) curve = dp->GetCurve(f, l);
      src->SetInputCurve(curve, f, l);
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
void asiVisu_CurvatureCombsPipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void asiVisu_CurvatureCombsPipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void asiVisu_CurvatureCombsPipeline::callback_update()
{}
