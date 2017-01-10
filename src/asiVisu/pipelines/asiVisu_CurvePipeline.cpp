//-----------------------------------------------------------------------------
// Created on: 10 February 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_CurvePipeline.h>

// Visualization includes
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
      src->SetTipSize(tipSize);
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
