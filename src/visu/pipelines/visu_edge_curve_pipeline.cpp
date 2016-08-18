//-----------------------------------------------------------------------------
// Created on: 18 August 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_edge_curve_pipeline.h>

// Common includes
#include <common_facilities.h>

// Visualization includes
#include <visu_curve_source.h>
#include <visu_edge_data_provider.h>
#include <visu_utils.h>

// VTK includes
#include <vtkAppendPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

// OCCT includes
#include <BRep_Tool.hxx>

//-----------------------------------------------------------------------------

//! Creates new Edge Curve Pipeline initialized by default VTK mapper and actor.
visu_edge_curve_pipeline::visu_edge_curve_pipeline()
: visu_pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                 vtkSmartPointer<vtkActor>::New() ),
  m_iStepsNumber(20)
{
  this->Actor()->GetProperty()->SetLineWidth(1.0);
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void visu_edge_curve_pipeline::SetInput(const Handle(visu_data_provider)& DP)
{
  if ( !m_iStepsNumber )
    return;

  Handle(visu_edge_data_provider)
    edgeProvider = Handle(visu_edge_data_provider)::DownCast(DP);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

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
    TopoDS_Edge E = edgeProvider->ExtractEdge();
    //
    if ( E.IsNull() )
      return;

    // Allocate Data Source
    vtkSmartPointer<visu_curve_source>
      curveSource = vtkSmartPointer<visu_curve_source>::New();

    // Set geometry to be converted to VTK polygonal DS
    if ( !curveSource->SetInputEdge(E) )
      return; // No poly data produced

    // Initialize pipeline
    this->SetInputConnection( curveSource->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void visu_edge_curve_pipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void visu_edge_curve_pipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void visu_edge_curve_pipeline::callback_update()
{}
