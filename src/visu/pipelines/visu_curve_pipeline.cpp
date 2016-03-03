//-----------------------------------------------------------------------------
// Created on: 10 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_curve_pipeline.h>

// Visualization includes
#include <visu_curve_source.h>
#include <visu_curve_data_provider.h>

// VTK includes
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//! Creates new Curve Pipeline initialized by default VTK mapper and actor.
visu_curve_pipeline::visu_curve_pipeline()
//
: visu_pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(), vtkSmartPointer<vtkActor>::New() )
{
  this->Actor()->GetProperty()->SetLineWidth(2.0);
}

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void visu_curve_pipeline::SetInput(const Handle(visu_data_provider)& DP)
{
  Handle(visu_curve_data_provider) dp = Handle(visu_curve_data_provider)::DownCast(DP);

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
    vtkSmartPointer<visu_curve_source> src = vtkSmartPointer<visu_curve_source>::New();
    //
    if ( curve_type->SubType( STANDARD_TYPE(Geom2d_Curve) ) )
    {
      src->SetInputCurve2d( dp->GetCurve2d() );
      src->SetTipSize(0.1);
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
void visu_curve_pipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void visu_curve_pipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void visu_curve_pipeline::callback_update()
{}
