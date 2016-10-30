//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_mesh_pipeline.h>

// Visualization includes
#include <visu_mesh_data_provider.h>
#include <visu_mesh_source.h>
#include <visu_mesh_utils.h>
#include <visu_node_info.h>
#include <visu_utils.h>

// VTK includes
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------
// Mesh Pipeline
//-----------------------------------------------------------------------------

//! Creates new Mesh Pipeline instance.
visu_mesh_pipeline::visu_mesh_pipeline()
: visu_pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                 vtkSmartPointer<vtkActor>::New() )
{
  this->EmptyGroupForAllModeOn();

  /* ===========================
   *  Initialize custom filters
   * =========================== */

  visu_utils::ApplyLightingRules( this->Actor() );
}

//! Sets input data for the pipeline.
//! This method performs translation of DOMAIN data to VTK polygonal data.
//! \param dataProvider [in] Data Provider.
void visu_mesh_pipeline::SetInput(const Handle(visu_data_provider)& dataProvider)
{
  Handle(visu_mesh_data_provider)
    aMeshPrv = Handle(visu_mesh_data_provider)::DownCast(dataProvider);

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( aMeshPrv->MustExecute( this->GetMTime() ) )
  {
    vtkSmartPointer<visu_mesh_source> aMeshSource = vtkSmartPointer<visu_mesh_source>::New();
    aMeshSource->SetInputMesh( aMeshPrv->GetMeshDS() );
    if ( m_bIsEmptyGroupForAll )
      aMeshSource->EmptyGroupForAllModeOn();
    else
      aMeshSource->EmptyGroupForAllModeOff();

    // Bind actor to owning Node ID. Thus we set back reference from VTK
    // entity to data object
    visu_node_info::Store( aMeshPrv->GetNodeID(), this->Actor() );

    // Initialize pipeline
    this->SetInputConnection( aMeshSource->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//! Switches ON inverse mode of filtering.
void visu_mesh_pipeline::EmptyGroupForAllModeOn()
{
  m_bIsEmptyGroupForAll = true;
}

//! Switches OFF inverse mode of filtering.
void visu_mesh_pipeline::EmptyGroupForAllModeOff()
{
  m_bIsEmptyGroupForAll = false;
}

//! Callback for AddToRenderer base routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void visu_mesh_pipeline::callback_add_to_renderer(vtkRenderer*)
{
  this->Actor()->GetProperty()->SetPointSize( visu_mesh_utils::DefaultPointSize() );
  this->Actor()->GetProperty()->SetInterpolationToGouraud();
}

//! Callback for RemoveFromRenderer base routine.
void visu_mesh_pipeline::callback_remove_from_renderer(vtkRenderer*)
{
}

//! Callback for Update routine.
void visu_mesh_pipeline::callback_update()
{
  if ( !m_bMapperColorsSet )
  {
    visu_mesh_utils::InitMapper(m_mapper, ARRNAME_MESH_ITEM_TYPE);
    m_bMapperColorsSet = true;
  }
}
