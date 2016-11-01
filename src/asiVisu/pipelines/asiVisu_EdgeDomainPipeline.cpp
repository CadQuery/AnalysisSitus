//-----------------------------------------------------------------------------
// Created on: 18 August 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_EdgeDomainPipeline.h>

// Common includes
#include <common_facilities.h>

// Visualization includes
#include <asiVisu_EdgeDataProvider.h>

// VTK includes
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>

//-----------------------------------------------------------------------------

//! Creates new Edge Domain Pipeline initialized by default VTK mapper and actor.
asiVisu_EdgeDomainPipeline::asiVisu_EdgeDomainPipeline()
//
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                 vtkSmartPointer<vtkActor>::New() )
{}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void asiVisu_EdgeDomainPipeline::SetInput(const Handle(asiVisu_DataProvider)& DP)
{
  Handle(asiVisu_EdgeDataProvider)
    edgeProvider = Handle(asiVisu_EdgeDataProvider)::DownCast(DP);

  /* =================
   *  Validate inputs
   * ================= */

  const int edge_idx = edgeProvider->GetEdgeIndexAmongSubshapes();
  if ( !edge_idx )
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

  if ( edgeProvider->MustExecute( this->GetMTime() ) )
  {
    // TODO: NYI
    //this->SetInputConnection( source->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_EdgeDomainPipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void asiVisu_EdgeDomainPipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void asiVisu_EdgeDomainPipeline::callback_update()
{}
