//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_iv_surface_pipeline.h>

// Visualization includes
#include <visu_iv_surface_data_provider.h>
#include <visu_surface_source.h>

// VTK includes
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

//! Creates new Pipeline initialized by default VTK mapper and actor.
visu_iv_surface_pipeline::visu_iv_surface_pipeline()
: visu_pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                 vtkSmartPointer<vtkActor>::New() ),
  m_iStepsNumber(20)
{
  this->Actor()->GetProperty()->SetLineWidth(1.0);
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void visu_iv_surface_pipeline::SetInput(const Handle(visu_data_provider)& DP)
{
  if ( !m_iStepsNumber )
    return;

  Handle(visu_iv_surface_data_provider)
    provider = Handle(visu_iv_surface_data_provider)::DownCast(DP);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  double uLimit, vLimit;
  Handle(Geom_Surface) surface = provider->GetSurface(uLimit, vLimit);
  //
  if ( surface.IsNull() )
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
    vtkSmartPointer<visu_surface_source>
      src = vtkSmartPointer<visu_surface_source>::New();
    //
    src->SetInputSurface  (surface);
    src->SetNumberOfSteps (m_iStepsNumber);
    src->SetTrimValues    (uLimit, vLimit);

    // Initialize pipeline
    this->SetInputConnection( src->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void visu_iv_surface_pipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void visu_iv_surface_pipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void visu_iv_surface_pipeline::callback_update()
{}
