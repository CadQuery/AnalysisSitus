//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_re_contours_pipeline.h>

// Visualization includes
#include <visu_re_contours_data_provider.h>
#include <visu_curve_source.h>

// VTK includes
#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

// OCCT includes
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

//! Creates new Pipeline initialized by default VTK mapper and actor.
visu_re_contours_pipeline::visu_re_contours_pipeline()
: visu_pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                 vtkSmartPointer<vtkActor>::New() )
{
  this->Actor()->GetProperty()->SetLineWidth(2.0);
  this->Actor()->GetProperty()->SetColor(1.0, 0.0, 0.0);
  this->Actor()->GetProperty()->SetOpacity(0.5);
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void visu_re_contours_pipeline::SetInput(const Handle(visu_data_provider)& DP)
{
  Handle(visu_re_contours_data_provider)
    provider = Handle(visu_re_contours_data_provider)::DownCast(DP);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  const int numContours = provider->GetNumOfContours();
  //
  if ( !numContours )
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
    for ( int c = 1; c <= numContours; ++c )
    {
      TopoDS_Wire W = provider->GetContour(c);

      // Add edges one by one
      for ( TopExp_Explorer exp(W, TopAbs_EDGE); exp.More(); exp.Next() )
      {
        vtkSmartPointer<visu_curve_source>
          src = vtkSmartPointer<visu_curve_source>::New();
        //
        src->SetInputEdge( TopoDS::Edge( exp.Current() ) );
        //
        appendFilter->AddInputConnection( src->GetOutputPort() );
      }
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
void visu_re_contours_pipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void visu_re_contours_pipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void visu_re_contours_pipeline::callback_update()
{}
