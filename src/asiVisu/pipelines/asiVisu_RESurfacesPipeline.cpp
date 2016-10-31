//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_re_surfaces_pipeline.h>

// Visualization includes
#include <visu_re_surfaces_data_provider.h>
#include <visu_surface_source.h>

// VTK includes
#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

//! Creates new Pipeline initialized by default VTK mapper and actor.
visu_re_surfaces_pipeline::visu_re_surfaces_pipeline()
: visu_pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                 vtkSmartPointer<vtkActor>::New() ),
  m_iStepsNumber(20)
{
  this->Actor()->GetProperty()->SetLineWidth(1.0);
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void visu_re_surfaces_pipeline::SetInput(const Handle(visu_data_provider)& DP)
{
  if ( !m_iStepsNumber )
    return;

  Handle(visu_re_surfaces_data_provider)
    provider = Handle(visu_re_surfaces_data_provider)::DownCast(DP);

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

      vtkSmartPointer<visu_surface_source>
        src = vtkSmartPointer<visu_surface_source>::New();
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
void visu_re_surfaces_pipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void visu_re_surfaces_pipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void visu_re_surfaces_pipeline::callback_update()
{}
