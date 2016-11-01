//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_LawPipeline.h>

// Visualization includes
#include <asiVisu_LawDataProvider.h>
#include <asiVisu_LawSource.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

//! Creates new pipeline initialized by default VTK mapper and actor.
asiVisu_LawPipeline::asiVisu_LawPipeline()
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                 vtkSmartPointer<vtkActor>::New() )
{
  this->Actor()->GetProperty()->SetLineWidth(2.0);
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void asiVisu_LawPipeline::SetInput(const Handle(asiVisu_DataProvider)& DP)
{
  Handle(asiVisu_LawDataProvider)
    provider = Handle(asiVisu_LawDataProvider)::DownCast(DP);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  Handle(asiAlgo_DesignLaw) law = provider->GetLaw();
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
    vtkSmartPointer<asiVisu_LawSource> src = vtkSmartPointer<asiVisu_LawSource>::New();

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
void asiVisu_LawPipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void asiVisu_LawPipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void asiVisu_LawPipeline::callback_update()
{}
