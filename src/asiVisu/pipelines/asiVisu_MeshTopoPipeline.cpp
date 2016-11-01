//-----------------------------------------------------------------------------
// Created on: 31 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_MeshTopoPipeline.h>

// Visualization includes
#include <visu_mesh_topo_data_provider.h>
#include <asiVisu_MeshTopoSource.h>

// VTK includes
#include <vtkPolyDataMapper.h>

//! Creates new Pipeline instance.
asiVisu_MeshTopoPipeline::asiVisu_MeshTopoPipeline()
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                 vtkSmartPointer<vtkActor>::New() )
{
}

//! Sets input data for the pipeline.
//! This method performs translation of DOMAIN data to VTK polygonal data.
//! \param dataProvider [in] Data Provider.
void asiVisu_MeshTopoPipeline::SetInput(const Handle(asiVisu_DataProvider)& dataProvider)
{
  Handle(visu_mesh_topo_data_provider)
    prv = Handle(visu_mesh_topo_data_provider)::DownCast(dataProvider);

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( prv->MustExecute( this->GetMTime() ) )
  {
    vtkSmartPointer<asiVisu_MeshTopoSource> src = vtkSmartPointer<asiVisu_MeshTopoSource>::New();
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
void asiVisu_MeshTopoPipeline::callback_add_to_renderer(vtkRenderer*)
{
}

//! Callback for RemoveFromRenderer() base routine.
void asiVisu_MeshTopoPipeline::callback_remove_from_renderer(vtkRenderer*)
{
}

//! Callback for Update() routine.
void asiVisu_MeshTopoPipeline::callback_update()
{
}
