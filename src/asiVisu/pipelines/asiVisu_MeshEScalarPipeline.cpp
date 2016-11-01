//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_MeshEScalarPipeline.h>

// Visualization includes
#include <asiVisu_MeshEScalarFilter.h>
#include <asiVisu_MeshResultUtils.h>
#include <asiVisu_MeshSource.h>
#include <asiUI_NodeInfo.h>

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
asiVisu_MeshEScalarPipeline::asiVisu_MeshEScalarPipeline()
  : asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                   vtkSmartPointer<vtkActor>::New() )
{
  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Allocate filter populating scalar arrays
  vtkSmartPointer<asiVisu_MeshEScalarFilter>
    aScFilter = vtkSmartPointer<asiVisu_MeshEScalarFilter>::New();

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
void asiVisu_MeshEScalarPipeline::SetInput(const Handle(asiVisu_DataProvider)& theDataProvider)
{
  Handle(asiVisu_MeshEScalarDataProvider)
    aMeshPrv = Handle(asiVisu_MeshEScalarDataProvider)::DownCast(theDataProvider);

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( aMeshPrv->MustExecute( this->GetMTime() ) )
  {
    vtkSmartPointer<asiVisu_MeshSource> aMeshSource = vtkSmartPointer<asiVisu_MeshSource>::New();
    aMeshSource->SetInputMesh( aMeshPrv->GetMeshDS() );

    asiVisu_MeshEScalarFilter*
      aScFilter = asiVisu_MeshEScalarFilter::SafeDownCast( m_filterMap.Find(Filter_EScalar) );

    aScFilter->SetScalarArrays( aMeshPrv->GetElementIDs(),
                                aMeshPrv->GetElementScalars() );

    // Complete pipeline
    this->SetInputConnection( aMeshSource->GetOutputPort() );

    // Bind actor to owning Node ID. Thus we set back reference from VTK
    // entity to data object
    asiUI_NodeInfo::Store( aMeshPrv->GetNodeID(), this->Actor() );
  }

  // Update modification timestamp
  this->Modified();
}

//! Callback for AddToRenderer base routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_MeshEScalarPipeline::addToRendererCallback(vtkRenderer*)
{
  this->Actor()->GetProperty()->SetInterpolationToFlat();
}

//! Callback for RemoveFromRenderer base routine.
void asiVisu_MeshEScalarPipeline::removeFromRendererCallback(vtkRenderer*)
{
}

//! Callback for Update routine.
void asiVisu_MeshEScalarPipeline::updateCallback()
{
  asiVisu_MeshEScalarFilter*
    aScFilter = asiVisu_MeshEScalarFilter::SafeDownCast( m_filterMap.Find(Filter_EScalar) );

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

  asiVisu_MeshResultUtils::InitCellScalarMapper(m_mapper, ARRNAME_MESH_E_SCALARS,
                                               aMinScalar, aMaxScalar,
                                               doScalarInterpolation);
}
