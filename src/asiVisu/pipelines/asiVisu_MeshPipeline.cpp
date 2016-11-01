//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_MeshPipeline.h>

// Visualization includes
#include <asiVisu_MeshDataProvider.h>
#include <asiVisu_MeshSource.h>
#include <asiVisu_MeshUtils.h>
#include <asiUI_NodeInfo.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------
// Mesh Pipeline
//-----------------------------------------------------------------------------

//! Creates new Mesh Pipeline instance.
asiVisu_MeshPipeline::asiVisu_MeshPipeline()
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                 vtkSmartPointer<vtkActor>::New() )
{
  this->EmptyGroupForAllModeOn();

  /* ===========================
   *  Initialize custom filters
   * =========================== */

  asiVisu_Utils::ApplyLightingRules( this->Actor() );
}

//! Sets input data for the pipeline.
//! This method performs translation of DOMAIN data to VTK polygonal data.
//! \param dataProvider [in] Data Provider.
void asiVisu_MeshPipeline::SetInput(const Handle(asiVisu_DataProvider)& dataProvider)
{
  Handle(asiVisu_MeshDataProvider)
    aMeshPrv = Handle(asiVisu_MeshDataProvider)::DownCast(dataProvider);

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( aMeshPrv->MustExecute( this->GetMTime() ) )
  {
    vtkSmartPointer<asiVisu_MeshSource> aMeshSource = vtkSmartPointer<asiVisu_MeshSource>::New();
    aMeshSource->SetInputMesh( aMeshPrv->GetMeshDS() );
    if ( m_bIsEmptyGroupForAll )
      aMeshSource->EmptyGroupForAllModeOn();
    else
      aMeshSource->EmptyGroupForAllModeOff();

    // Bind actor to owning Node ID. Thus we set back reference from VTK
    // entity to data object
    asiUI_NodeInfo::Store( aMeshPrv->GetNodeID(), this->Actor() );

    // Initialize pipeline
    this->SetInputConnection( aMeshSource->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//! Switches ON inverse mode of filtering.
void asiVisu_MeshPipeline::EmptyGroupForAllModeOn()
{
  m_bIsEmptyGroupForAll = true;
}

//! Switches OFF inverse mode of filtering.
void asiVisu_MeshPipeline::EmptyGroupForAllModeOff()
{
  m_bIsEmptyGroupForAll = false;
}

//! Callback for AddToRenderer base routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_MeshPipeline::callback_add_to_renderer(vtkRenderer*)
{
  this->Actor()->GetProperty()->SetPointSize( asiVisu_MeshUtils::DefaultPointSize() );
  this->Actor()->GetProperty()->SetInterpolationToGouraud();
}

//! Callback for RemoveFromRenderer base routine.
void asiVisu_MeshPipeline::callback_remove_from_renderer(vtkRenderer*)
{
}

//! Callback for Update routine.
void asiVisu_MeshPipeline::callback_update()
{
  if ( !m_bMapperColorsSet )
  {
    asiVisu_MeshUtils::InitMapper(m_mapper, ARRNAME_MESH_ITEM_TYPE);
    m_bMapperColorsSet = true;
  }
}
