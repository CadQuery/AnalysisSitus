//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <visu_mesh_EN_scalar_pipeline.h>

// Visualization includes
#include <visu_mesh_EN_scalar_filter.h>
#include <visu_mesh_result_utils.h>
#include <visu_mesh_source.h>
#include <visu_mesh_utils.h>
#include <visu_node_info.h>

// Active Data includes
#include <ActData_MeshParameter.h>
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>

// VTK includes
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------
// Pipeline
//-----------------------------------------------------------------------------

//! Creates new Pipeline instance.
visu_mesh_EN_scalar_pipeline::visu_mesh_EN_scalar_pipeline()
  : visu_pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                   vtkSmartPointer<vtkActor>::New() )
{
  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Allocate filter populating scalar arrays
  vtkSmartPointer<visu_mesh_EN_scalar_filter>
    aScFilter = vtkSmartPointer<visu_mesh_EN_scalar_filter>::New();

  /* =========================
   *  Register custom filters
   * ========================= */

  m_filterMap.Bind(Filter_ENScalar, aScFilter);

  // Append custom filters to the pipeline
  this->append( m_filterMap.Find(Filter_ENScalar) );
}

//! Sets input data for the pipeline accepting Active Data entities.
//! Actually this method performs translation of DOMAIN data to VTK POLYGONAL
//! DATA.
//! \param theDataProvider [in] Data Provider.
void visu_mesh_EN_scalar_pipeline::SetInput(const Handle(visu_data_provider)& theDataProvider)
{
  Handle(visu_mesh_EN_scalar_data_provider)
    aMeshPrv = Handle(visu_mesh_EN_scalar_data_provider)::DownCast(theDataProvider);

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( aMeshPrv->MustExecute( this->GetMTime() ) )
  {
    vtkSmartPointer<visu_mesh_source> aMeshSource = vtkSmartPointer<visu_mesh_source>::New();
    aMeshSource->SetInputMesh( aMeshPrv->GetMeshDS() );

    if ( aMeshPrv->HasMeshGroup() )
    {
      Handle(OMFDS_MeshGroup) aGrp = aMeshPrv->GetMeshGroup();
      if ( !aGrp->IsEmpty() ) // Do not use empty groups for filtering
        aMeshSource->SetInputElemGroup( aMeshPrv->GetMeshGroup() );
    }

    visu_mesh_EN_scalar_filter*
      aScFilter = visu_mesh_EN_scalar_filter::SafeDownCast( m_filterMap.Find(Filter_ENScalar) );

    aScFilter->SetScalarArrays( aMeshPrv->GetTriIDs(),
                                aMeshPrv->GetTriScalars(),
                                aMeshPrv->GetQuadIDs(),
                                aMeshPrv->GetQuadScalars() );

    // Complete pipeline
    this->SetInputConnection( aMeshSource->GetOutputPort() );

    // Bind actor to owning Node ID. Thus we set back reference from VTK
    // entity to data object
    visu_node_info::Store( aMeshPrv->GetNodeID(), this->Actor() );
  }

  // Update modification timestamp
  this->Modified();
}

//! Callback for AddToRenderer base routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void visu_mesh_EN_scalar_pipeline::addToRendererCallback(vtkRenderer*)
{
  this->Actor()->GetProperty()->SetInterpolationToGouraud();
}

//! Callback for RemoveFromRenderer base routine.
void visu_mesh_EN_scalar_pipeline::removeFromRendererCallback(vtkRenderer*)
{
}

//! Callback for Update routine.
void visu_mesh_EN_scalar_pipeline::updateCallback()
{
  visu_mesh_EN_scalar_filter*
    aScFilter = visu_mesh_EN_scalar_filter::SafeDownCast( m_filterMap.Find(Filter_ENScalar) );
  aScFilter->Update();
  double aMinScalar = aScFilter->GetMinScalar(),
         aMaxScalar = aScFilter->GetMaxScalar();

  if ( Abs(aMinScalar) == VTK_FLOAT_MAX && Abs(aMaxScalar) == VTK_FLOAT_MAX )
    aMinScalar = aMaxScalar = 0.0;

  bool doScalarInterpolation;
  if ( Abs(aMaxScalar - aMinScalar) < Precision::Confusion() )
    doScalarInterpolation = false;
  else
    doScalarInterpolation = true;

  visu_mesh_result_utils::InitPointScalarMapper(m_mapper, ARRNAME_MESH_EN_SCALARS,
                                                aMinScalar, aMaxScalar,
                                                doScalarInterpolation);
}
