//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <visu_mesh_E_vector_pipeline.h>

// Visualization includes
#include <visu_mesh_E_vector_filter.h>
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
#include <vtkGlyph3D.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

//-----------------------------------------------------------------------------
// Pipeline
//-----------------------------------------------------------------------------

//! Creates new Pipeline instance.
visu_mesh_E_vector_pipeline::visu_mesh_E_vector_pipeline()
  : visu_pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                   vtkSmartPointer<vtkActor>::New() )
{
  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Allocate filter populating vector data
  vtkSmartPointer<visu_mesh_E_vector_filter>
    aVecFilter = vtkSmartPointer<visu_mesh_E_vector_filter>::New();

  // Allocate Transformation filter
  vtkSmartPointer<vtkTransformPolyDataFilter>
    aTrsfFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();

  // Allocate Glyph filter
  vtkSmartPointer<vtkGlyph3D>
    aGlyphFilter = vtkSmartPointer<vtkGlyph3D>::New();

  /* =========================
   *  Register custom filters
   * ========================= */

  m_filterMap.Bind(Filter_EVector, aVecFilter);
  m_filterMap.Bind(Filter_Trsf,    aTrsfFilter);
  m_filterMap.Bind(Filter_Glyph3D, aGlyphFilter);

  // Append custom filters to the pipeline
  this->append( m_filterMap.Find(Filter_EVector) );
  this->append( m_filterMap.Find(Filter_Glyph3D) );
}

//! Sets input data for the pipeline accepting Active Data entities.
//! Actually this method performs translation of DOMAIN data to VTK POLYGONAL
//! DATA.
//! \param theDataProvider [in] Data Provider.
void visu_mesh_E_vector_pipeline::SetInput(const Handle(visu_data_provider)& theDataProvider)
{
  Handle(visu_mesh_E_vector_data_provider)
    aMeshPrv = Handle(visu_mesh_E_vector_data_provider)::DownCast(theDataProvider);

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( aMeshPrv->MustExecute( this->GetMTime() ) )
  {
    vtkSmartPointer<visu_mesh_source> aMeshSource = vtkSmartPointer<visu_mesh_source>::New();
    aMeshSource->SetInputMesh( aMeshPrv->GetMeshDS() );

    Handle(HIntArray) anElemIDs = aMeshPrv->GetElementIDs();
    Handle(HRealArray) anElemVectors = aMeshPrv->GetElementVectors();

    // Pass vector array
    visu_mesh_E_vector_filter*
      aVecFilter = visu_mesh_E_vector_filter::SafeDownCast( m_filterMap.Find(Filter_EVector) );
    aVecFilter->SetVectorArrays( anElemIDs,
                                 anElemVectors,
                                 aMeshPrv->GetMaxModulus() );

    // Transform glyphs
    vtkTransformPolyDataFilter*
      aTrsfFilter = vtkTransformPolyDataFilter::SafeDownCast( m_filterMap.Find(Filter_Trsf) );
    aTrsfFilter->SetInputConnection( visu_mesh_result_utils::GetVectorGlyph()->GetOutputPort() );
    aTrsfFilter->SetTransform( visu_mesh_result_utils::GetVectorGlyphTransform() );

    // Adjust glyph filter
    vtkGlyph3D* aGlyphFilter = vtkGlyph3D::SafeDownCast( m_filterMap.Find(Filter_Glyph3D) );
    aGlyphFilter->SetSourceConnection( aTrsfFilter->GetOutputPort() );
    aGlyphFilter->SetVectorModeToUseVector();
    aGlyphFilter->SetScaleModeToScaleByVector();
    aGlyphFilter->SetColorModeToColorByScale();
    aGlyphFilter->SetInputArrayToProcess(1, 0, 0, 0, ARRNAME_MESH_E_VECTORS);
    aGlyphFilter->SetScaleFactor(1.0); // No global scaling

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
void visu_mesh_E_vector_pipeline::addToRendererCallback(vtkRenderer*)
{
}

//! Callback for RemoveFromRenderer base routine.
void visu_mesh_E_vector_pipeline::removeFromRendererCallback(vtkRenderer*)
{
}

//! Callback for Update routine.
void visu_mesh_E_vector_pipeline::updateCallback()
{
  m_mapper->ScalarVisibilityOff();
}
