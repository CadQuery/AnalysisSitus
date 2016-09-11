//-----------------------------------------------------------------------------
// Created on: 02 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_bcurve_knots_pipeline.h>

// Visualization includes
#include <visu_bcurve_knots_source.h>
#include <visu_curve_data_provider.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// VTK includes
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//! Creates new Pipeline initialized by default VTK mapper and actor.
visu_bcurve_knots_pipeline::visu_bcurve_knots_pipeline()
//
: visu_pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(), vtkSmartPointer<vtkActor>::New() )
{
  this->Actor()->GetProperty()->SetPointSize(5.0f);
}

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void visu_bcurve_knots_pipeline::SetInput(const Handle(visu_data_provider)& DP)
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
    // Access curve (3d or 2d)
    double f, l;
    Handle(Geom_Curve) c3d = dp->GetCurve(f, l);

    // B-curve knots
    if ( !c3d.IsNull() && c3d->IsKind( STANDARD_TYPE(Geom_BSplineCurve) ) )
    {
      Handle(Geom_BSplineCurve) BC = Handle(Geom_BSplineCurve)::DownCast(c3d);
      //
      vtkSmartPointer<visu_bcurve_knots_source>
        bpoles_src = vtkSmartPointer<visu_bcurve_knots_source>::New();
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
void visu_bcurve_knots_pipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void visu_bcurve_knots_pipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void visu_bcurve_knots_pipeline::callback_update()
{}
