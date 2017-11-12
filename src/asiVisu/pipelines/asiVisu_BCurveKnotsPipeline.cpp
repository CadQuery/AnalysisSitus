//-----------------------------------------------------------------------------
// Created on: 02 September 2016
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
