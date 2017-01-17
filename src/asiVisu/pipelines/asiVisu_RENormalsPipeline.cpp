//-----------------------------------------------------------------------------
// Created on: 13 January 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_RENormalsPipeline.h>

// Visualization includes
#include <asiVisu_PointsSource.h>
#include <asiVisu_PointsVectorFilter.h>
#include <asiVisu_RENormalsDataProvider.h>

// VTK includes
#include <vtkActor.h>
#include <vtkGlyph3D.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkTransformPolyDataFilter.h>

//-----------------------------------------------------------------------------
// Pipeline
//-----------------------------------------------------------------------------

//! Creates new Pipeline instance.
asiVisu_RENormalsPipeline::asiVisu_RENormalsPipeline()
  : asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                      vtkSmartPointer<vtkActor>::New() )
{
  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Allocate filter populating vector data
  vtkSmartPointer<asiVisu_PointsVectorFilter>
    vecFilter = vtkSmartPointer<asiVisu_PointsVectorFilter>::New();

  // Allocate Transformation filter
  vtkSmartPointer<vtkTransformPolyDataFilter>
    trsfFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();

  // Allocate Glyph filter
  vtkSmartPointer<vtkGlyph3D>
    glyphFilter = vtkSmartPointer<vtkGlyph3D>::New();

  /* =========================
   *  Register custom filters
   * ========================= */

  m_filterMap.Bind(Filter_Vector,  vecFilter);
  m_filterMap.Bind(Filter_Trsf,    trsfFilter);
  m_filterMap.Bind(Filter_Glyph3D, glyphFilter);

  // Append custom filters to the pipeline
  this->append( m_filterMap.Find(Filter_Vector) );
  this->append( m_filterMap.Find(Filter_Glyph3D) );
}

//! Sets input data for the pipeline accepting Active Data entities.
//! Actually this method performs translation of DOMAIN data to
//! VTK POLYGONAL DATA.
//! \param dataProvider [in] Data Provider.
void asiVisu_RENormalsPipeline::SetInput(const Handle(asiVisu_DataProvider)& dataProvider)
{
  Handle(asiVisu_RENormalsDataProvider)
    DP = Handle(asiVisu_RENormalsDataProvider)::DownCast(dataProvider);

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( DP->MustExecute( this->GetMTime() ) )
  {
    vtkSmartPointer<asiVisu_PointsSource>
      pointsSource = vtkSmartPointer<asiVisu_PointsSource>::New();
    //
    pointsSource->SetInputPoints( DP->GeGetMeshDS() );

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
void asiVisu_RENormalsPipeline::addToRendererCallback(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer base routine.
void asiVisu_RENormalsPipeline::removeFromRendererCallback(vtkRenderer*)
{}

//! Callback for Update routine.
void asiVisu_RENormalsPipeline::updateCallback()
{
  m_mapper->ScalarVisibilityOff();
}
