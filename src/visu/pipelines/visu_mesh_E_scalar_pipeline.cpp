//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <visu_mesh_E_scalar_pipeline.h>

// Visualization includes
#include <visu_mesh_E_scalar_filter.h>
#include <visu_mesh_result_utils.h>
#include <visu_mesh_source.h>
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
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------
// Pipeline
//-----------------------------------------------------------------------------

//! Creates new Pipeline instance.
visu_mesh_E_scalar_pipeline::visu_mesh_E_scalar_pipeline()
  : visu_pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                   vtkSmartPointer<vtkActor>::New() )
{
  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Allocate filter populating scalar arrays
  vtkSmartPointer<visu_mesh_E_scalar_filter>
    aScFilter = vtkSmartPointer<visu_mesh_E_scalar_filter>::New();

  /* =========================
   *  Register custom filters
   * ========================= */

  m_filterMap.Bind(Filter_EScalar, aScFilter);

  // Append custom filters to the pipeline
  this->append( m_filterMap.Find(Filter_EScalar) );
}

//! Sets input data for the pipeline accepting Active Data entities.
//! Actually this method performs translation of DOMAIN data to VTK POLYGONAL
//! DATA.
//! \param theDataProvider [in] Data Provider.
void visu_mesh_E_scalar_pipeline::SetInput(const Handle(visu_data_provider)& theDataProvider)
{
  Handle(visu_mesh_E_scalar_data_provider)
    aMeshPrv = Handle(visu_mesh_E_scalar_data_provider)::DownCast(theDataProvider);

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( aMeshPrv->MustExecute( this->GetMTime() ) )
  {
    vtkSmartPointer<visu_mesh_source> aMeshSource = vtkSmartPointer<visu_mesh_source>::New();
    aMeshSource->SetInputMesh( aMeshPrv->GetMeshDS() );

    visu_mesh_E_scalar_filter*
      aScFilter = visu_mesh_E_scalar_filter::SafeDownCast( m_filterMap.Find(Filter_EScalar) );

    aScFilter->SetScalarArrays( aMeshPrv->GetElementIDs(),
                                aMeshPrv->GetElementScalars() );

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
void visu_mesh_E_scalar_pipeline::addToRendererCallback(vtkRenderer*)
{
  this->Actor()->GetProperty()->SetInterpolationToFlat();
}

//! Callback for RemoveFromRenderer base routine.
void visu_mesh_E_scalar_pipeline::removeFromRendererCallback(vtkRenderer*)
{
}

//! Callback for Update routine.
void visu_mesh_E_scalar_pipeline::updateCallback()
{
  visu_mesh_E_scalar_filter*
    aScFilter = visu_mesh_E_scalar_filter::SafeDownCast( m_filterMap.Find(Filter_EScalar) );

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

  visu_mesh_result_utils::InitCellScalarMapper(m_mapper, ARRNAME_MESH_E_SCALARS,
                                               aMinScalar, aMaxScalar,
                                               doScalarInterpolation);
}
