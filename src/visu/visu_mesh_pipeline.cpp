//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <visu_mesh_pipeline.h>

// Visualization includes
#include <visu_mesh_data_provider.h>
#include <visu_mesh_source.h>
#include <visu_mesh_utils.h>
#include <visu_node_info.h>
#include <visu_shrink_poly_filter.h>

// Active Data includes
#include <ActData_MeshParameter.h>
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <Standard_ProgramError.hxx>

// VTK includes
#include <vtkActor.h>
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

  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Allocate SHRINK filter
  vtkSmartPointer<visu_shrink_poly_filter>
    shrinkFilter = vtkSmartPointer<visu_shrink_poly_filter>::New();
  shrinkFilter->SetShrinkFactor(1.0);

  /* =========================
   *  Register custom filters
   * ========================= */

  m_filterMap.Bind(Filter_Shrink, shrinkFilter);

  // Append custom filters to the pipeline
  this->append( m_filterMap.Find(Filter_Shrink) );

  /* ===========================
   *  Initialize custom filters
   * =========================== */

  // Initialize SHRINK filter
  this->ShrinkModeOff();
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

    if ( aMeshPrv->HasMeshGroup() )
    {
      Handle(OMFDS_MeshGroup) aGrp = aMeshPrv->GetMeshGroup();
      if ( !aGrp->IsEmpty() ) // Do not use empty groups for filtering
        aMeshSource->SetInputElemGroup( aMeshPrv->GetMeshGroup() );
    }

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

//! Returns true if the pipeline is configured to work in SHRINK mode
//! currently, false -- otherwise.
//! \return true/false.
bool visu_mesh_pipeline::IsShrinkMode() const
{
  visu_shrink_poly_filter*
    aShrinkFilter = visu_shrink_poly_filter::SafeDownCast( m_filterMap.Find(Filter_Shrink) );
  return Abs(aShrinkFilter->GetShrinkFactor() - 1.0) > RealEpsilon();
}

//! Switches ON SHRINK visualization mode.
void visu_mesh_pipeline::ShrinkModeOn()
{
  visu_shrink_poly_filter*
    aShrinkFilter = visu_shrink_poly_filter::SafeDownCast( m_filterMap.Find(Filter_Shrink) );

  aShrinkFilter->SetShrinkFactor( visu_mesh_utils::DefaultShrinkFactor() );
}

//! Switches OFF SHRINK visualization mode.
void visu_mesh_pipeline::ShrinkModeOff()
{
  visu_shrink_poly_filter*
    aShrinkFilter = visu_shrink_poly_filter::SafeDownCast( m_filterMap.Find(Filter_Shrink) );

  aShrinkFilter->SetShrinkFactor(1.0);
}

//! Callback for AddToRenderer base routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void visu_mesh_pipeline::addToRendererCallback(vtkRenderer*)
{
  this->Actor()->GetProperty()->SetPointSize( visu_mesh_utils::DefaultPointSize() );
  this->Actor()->GetProperty()->SetInterpolationToGouraud();
}

//! Callback for RemoveFromRenderer base routine.
void visu_mesh_pipeline::removeFromRendererCallback(vtkRenderer*)
{
}

//! Callback for Update routine.
void visu_mesh_pipeline::updateCallback()
{
  if ( !m_bMapperColorsSet )
  {
    visu_mesh_utils::InitMapper(m_mapper, ARRNAME_MESH_ITEM_TYPE);
    m_bMapperColorsSet = true;
  }
}
