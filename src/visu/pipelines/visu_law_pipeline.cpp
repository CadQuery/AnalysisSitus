//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_law_pipeline.h>

// Visualization includes
#include <visu_law_data_provider.h>
#include <visu_law_source.h>
#include <visu_utils.h>

// VTK includes
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

//! Creates new pipeline initialized by default VTK mapper and actor.
visu_law_pipeline::visu_law_pipeline()
: visu_pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                 vtkSmartPointer<vtkActor>::New() )
{
  this->Actor()->GetProperty()->SetLineWidth(2.0);
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void visu_law_pipeline::SetInput(const Handle(visu_data_provider)& DP)
{
  Handle(visu_law_data_provider)
    provider = Handle(visu_law_data_provider)::DownCast(DP);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  Handle(calculus_design_law) law = provider->GetLaw();
  if ( law.IsNull() )
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
    // Allocate Data Source
    vtkSmartPointer<visu_law_source> src = vtkSmartPointer<visu_law_source>::New();

    // Initialize data source
    src->SetLaw(law);

    // Initialize pipeline
    this->SetInputConnection( src->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void visu_law_pipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void visu_law_pipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void visu_law_pipeline::callback_update()
{}
