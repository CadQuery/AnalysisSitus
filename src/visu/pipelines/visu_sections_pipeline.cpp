//-----------------------------------------------------------------------------
// Created on: 09 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <visu_sections_pipeline.h>

// Visualization includes
#include <visu_sections_data_provider.h>
#include <visu_shape_data_source.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// VTK includes
#include <vtkPolyDataMapper.h>

// VIS includes
#include <IVtkOCC_Shape.hxx>
#include <IVtkTools_DisplayModeFilter.hxx>

//! Creates new Sections Pipeline initialized by default VTK mapper and actor.
visu_sections_pipeline::visu_sections_pipeline()
: visu_pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(), vtkSmartPointer<vtkActor>::New() )
{}

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void visu_sections_pipeline::SetInput(const Handle(visu_data_provider)& DP)
{
  Handle(visu_sections_data_provider)
    sDP = Handle(visu_sections_data_provider)::DownCast(DP);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  TopoDS_Shape shape = sDP->GetTransformedSections();
  if ( shape.IsNull() )
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

  if ( sDP->MustExecute( this->GetMTime() ) )
  {
    static int ShapeID = 0; ++ShapeID;
    Handle(IVtkOCC_Shape) shapeIntoVtk = new IVtkOCC_Shape(shape);
    shapeIntoVtk->SetId(ShapeID);

    // Prepare DS
    vtkSmartPointer<visu_shape_data_source> sDS = vtkSmartPointer<visu_shape_data_source>::New();
    sDS->SetShape(shapeIntoVtk);

    // Allocate Display Mode filter
    vtkSmartPointer<IVtkTools_DisplayModeFilter>
      DMFilter = vtkSmartPointer<IVtkTools_DisplayModeFilter>::New();
    //
    DMFilter->SetInputConnection( sDS->GetOutputPort() );
    DMFilter->SetDisplayMode(DM_Wireframe); // Wires are visible in wireframe mode only

    // Initialize pipeline
    this->SetInputConnection( DMFilter->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void visu_sections_pipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void visu_sections_pipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void visu_sections_pipeline::callback_update()
{}
