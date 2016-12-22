//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_MeshEVectorPipeline.h>

// Visualization includes
#include <asiVisu_MeshEVectorFilter.h>
#include <asiVisu_MeshResultUtils.h>
#include <asiVisu_MeshSource.h>
#include <asiVisu_MeshUtils.h>
#include <asiVisu_NodeInfo.h>

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
asiVisu_MeshEVectorPipeline::asiVisu_MeshEVectorPipeline()
  : asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                   vtkSmartPointer<vtkActor>::New() )
{
  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Allocate filter populating vector data
  vtkSmartPointer<asiVisu_MeshEVectorFilter>
    aVecFilter = vtkSmartPointer<asiVisu_MeshEVectorFilter>::New();

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
void asiVisu_MeshEVectorPipeline::SetInput(const Handle(asiVisu_DataProvider)& theDataProvider)
{
  Handle(asiVisu_MeshEVectorDataProvider)
    aMeshPrv = Handle(asiVisu_MeshEVectorDataProvider)::DownCast(theDataProvider);

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( aMeshPrv->MustExecute( this->GetMTime() ) )
  {
    vtkSmartPointer<asiVisu_MeshSource> aMeshSource = vtkSmartPointer<asiVisu_MeshSource>::New();
    aMeshSource->SetInputMesh( aMeshPrv->GetMeshDS() );

    Handle(HIntArray) anElemIDs = aMeshPrv->GetElementIDs();
    Handle(HRealArray) anElemVectors = aMeshPrv->GetElementVectors();

    // Pass vector array
    asiVisu_MeshEVectorFilter*
      aVecFilter = asiVisu_MeshEVectorFilter::SafeDownCast( m_filterMap.Find(Filter_EVector) );
    aVecFilter->SetVectorArrays( anElemIDs,
                                 anElemVectors,
                                 aMeshPrv->GetMaxModulus() );

    // Transform glyphs
    vtkTransformPolyDataFilter*
      aTrsfFilter = vtkTransformPolyDataFilter::SafeDownCast( m_filterMap.Find(Filter_Trsf) );
    aTrsfFilter->SetInputConnection( asiVisu_MeshResultUtils::GetVectorGlyph()->GetOutputPort() );
    aTrsfFilter->SetTransform( asiVisu_MeshResultUtils::GetVectorGlyphTransform() );

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
    asiVisu_NodeInfo::Store( aMeshPrv->GetNodeID(), this->Actor() );
  }

  // Update modification timestamp
  this->Modified();
}

//! Callback for AddToRenderer base routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_MeshEVectorPipeline::addToRendererCallback(vtkRenderer*)
{
}

//! Callback for RemoveFromRenderer base routine.
void asiVisu_MeshEVectorPipeline::removeFromRendererCallback(vtkRenderer*)
{
}

//! Callback for Update routine.
void asiVisu_MeshEVectorPipeline::updateCallback()
{
  m_mapper->ScalarVisibilityOff();
}
