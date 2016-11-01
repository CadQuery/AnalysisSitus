//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_PointsPipeline.h>

// Visualization includes
#include <asiVisu_PointsSource.h>
#include <asiVisu_PointsDataProvider.h>

// VTK includes
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

//! Creates new Pipeline initialized by default VTK mapper and actor.
asiVisu_PointsPipeline::asiVisu_PointsPipeline()
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                 vtkSmartPointer<vtkActor>::New() )
{
  this->Actor()->GetProperty()->SetPointSize(5.0);
  this->Actor()->GetProperty()->SetColor(0.0, 0.0, 1.0);
  this->Actor()->GetProperty()->SetOpacity(0.5);
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void asiVisu_PointsPipeline::SetInput(const Handle(asiVisu_DataProvider)& DP)
{
  const Handle(asiVisu_PointsDataProvider)&
    provider = Handle(asiVisu_PointsDataProvider)::DownCast(DP);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  Handle(asiAlgo_PointCloud) points = provider->GetPoints();
  //
  if ( points.IsNull() )
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
    // Source for points
    vtkSmartPointer<asiVisu_PointsSource>
      src = vtkSmartPointer<asiVisu_PointsSource>::New();
    //
    src->SetInputPoints(points);

    // Initialize pipeline
    this->SetInputConnection( src->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_PointsPipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void asiVisu_PointsPipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void asiVisu_PointsPipeline::callback_update()
{}
