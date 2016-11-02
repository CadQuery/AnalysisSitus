//-----------------------------------------------------------------------------
// Created on: 09 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_SectionsPipeline.h>

// Visualization includes
#include <asiVisu_SectionsDataProvider.h>
#include <asiVisu_ShapeDataSource.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// VTK includes
#include <vtkPolyDataMapper.h>

// VIS includes
#include <IVtkOCC_Shape.hxx>
#include <IVtkTools_DisplayModeFilter.hxx>

//! Creates new Sections Pipeline initialized by default VTK mapper and actor.
asiVisu_SectionsPipeline::asiVisu_SectionsPipeline()
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(), vtkSmartPointer<vtkActor>::New() )
{}

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void asiVisu_SectionsPipeline::SetInput(const Handle(asiVisu_DataProvider)& DP)
{
  Handle(asiVisu_SectionsDataProvider)
    sDP = Handle(asiVisu_SectionsDataProvider)::DownCast(DP);

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
    vtkSmartPointer<asiVisu_ShapeDataSource> sDS = vtkSmartPointer<asiVisu_ShapeDataSource>::New();
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
void asiVisu_SectionsPipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void asiVisu_SectionsPipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void asiVisu_SectionsPipeline::callback_update()
{}
