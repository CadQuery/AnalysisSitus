//-----------------------------------------------------------------------------
// Created on: 31 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_mesh_topo_pipeline.h>

// Visualization includes
#include <visu_mesh_topo_data_provider.h>
#include <visu_mesh_topo_source.h>

// VTK includes
#include <vtkPolyDataMapper.h>

//! Creates new Pipeline instance.
visu_mesh_topo_pipeline::visu_mesh_topo_pipeline()
: visu_pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                 vtkSmartPointer<vtkActor>::New() )
{
}

//! Sets input data for the pipeline.
//! This method performs translation of DOMAIN data to VTK polygonal data.
//! \param dataProvider [in] Data Provider.
void visu_mesh_topo_pipeline::SetInput(const Handle(visu_data_provider)& dataProvider)
{
  Handle(visu_mesh_topo_data_provider)
    prv = Handle(visu_mesh_topo_data_provider)::DownCast(dataProvider);

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( prv->MustExecute( this->GetMTime() ) )
  {
    vtkSmartPointer<visu_mesh_topo_source> src = vtkSmartPointer<visu_mesh_topo_source>::New();
    src->SetInputMesh( prv->GetMeshDS() );
    src->SetBaseNodes( prv->GetBaseNodes() );

    // Initialize pipeline
    this->SetInputConnection( src->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//! Callback for AddToRenderer() base routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void visu_mesh_topo_pipeline::callback_add_to_renderer(vtkRenderer*)
{
}

//! Callback for RemoveFromRenderer() base routine.
void visu_mesh_topo_pipeline::callback_remove_from_renderer(vtkRenderer*)
{
}

//! Callback for Update() routine.
void visu_mesh_topo_pipeline::callback_update()
{
}
