//-----------------------------------------------------------------------------
// Created on: 30 November 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_PartPipeline.h>

// Visualization includes
#include <asiVisu_PartDataProvider.h>
#include <asiVisu_PartNodeInfo.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkInformation.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

#define COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiVisu_PartPipeline::asiVisu_PartPipeline()
//
: asiVisu_Pipeline   ( vtkSmartPointer<vtkPolyDataMapper>::New(), vtkSmartPointer<vtkActor>::New() ),
  m_bMapperColorsSet ( false )
{
  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Initialize Data Source
  m_source = vtkSmartPointer<asiVisu_ShapeRobustSource>::New();

  // Filter for normals
  m_normalsFilter = vtkSmartPointer<vtkPolyDataNormals>::New();

  // Display mode filter
  m_dmFilter = vtkSmartPointer<asiVisu_DisplayModeFilter>::New();
  m_dmFilter->SetDisplayMode(DisplayMode_Shaded);

  // Set line width
  this->Actor()->GetProperty()->SetLineWidth(1);
  this->Actor()->GetProperty()->SetPointSize(8);
  //
  asiVisu_Utils::ApplyLightingRules( this->Actor() );

  // Compose pipeline
  this->append(m_dmFilter);
  //
  if ( m_dmFilter->GetDisplayMode() == DisplayMode_Shaded )
    this->append(m_normalsFilter);
}

//-----------------------------------------------------------------------------

void asiVisu_PartPipeline::SetDiagnosticTools(ActAPI_ProgressEntry progress,
                                              ActAPI_PlotterEntry  plotter)
{
  m_progress = progress;
  m_plotter  = plotter;
  //
  m_source->SetDiagnosticTools(m_progress, m_plotter);
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param dataProvider [in] Data Provider.
void asiVisu_PartPipeline::SetInput(const Handle(asiVisu_DataProvider)& dataProvider)
{
  Handle(asiVisu_PartDataProvider)
    DP = Handle(asiVisu_PartDataProvider)::DownCast(dataProvider);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  TopoDS_Shape shape = DP->GetShape();
  if ( shape.IsNull() )
  {
    // Pass empty data set in order to have valid pipeline
    vtkSmartPointer<vtkPolyData> dummyData = vtkSmartPointer<vtkPolyData>::New();
    this->SetInputData(dummyData);
    this->Modified(); // Update modification timestamp
    return; // Do nothing
  }

  /* ====================
   *  Configure pipeline
   * ==================== */

  if ( DP->MustExecute( this->GetMTime() ) )
  {
    // Clear cached data which is by design actual for the current state of
    // source only. The source changes, so the cache needs nullification
    this->clearCache();

    // Configure data source
    m_source->SetAAG( DP->GetAAG() );
    m_source->SetTessellationParams( DP->GetLinearDeflection(),
                                     DP->GetAngularDeflection() );

    // Bind to a Data Node using information key
    asiVisu_PartNodeInfo::Store( DP->GetNodeID(), this->Actor() );

    // Initialize pipeline
    this->SetInputConnection( m_source->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Highlights the passed elements according to the given selection nature.
//! \param elementIds [in] element IDs.
//! \param selNature  [in] selection nature (detection or selection).
void asiVisu_PartPipeline::SetPickedElements(const TColStd_PackedMapOfInteger& elementIds,
                                             const asiVisu_SelectionNature     selNature)
{
  if ( !elementIds.Extent() )
    return;

  // Get polygonal data
  vtkPolyData*
    pData = vtkPolyData::SafeDownCast( this->Mapper()->GetInputDataObject(0, 0) );
  //
  if ( !pData )
    return;

  // Get cell data
  vtkCellData* pCellData       = pData->GetCellData();
  const int    numOfInputCells = pCellData->GetNumberOfTuples();

  // Get array of pedigree IDs
  vtkIdTypeArray*
    pPedigreeArr = vtkIdTypeArray::SafeDownCast( pCellData->GetPedigreeIds() );

  // Get array of cell types
  vtkIdTypeArray*
    pShapePrimArr = vtkIdTypeArray::SafeDownCast( pCellData->GetArray(ARRNAME_PART_CELL_TYPES) );

  // Choose proper scalar
  asiVisu_ShapePrimitive prim;
  if ( selNature == SelectionNature_Detection )
    prim = ShapePrimitive_Detected;
  else
    prim = ShapePrimitive_Selected;

  // Choose cache
  NCollection_DataMap<vtkIdType, int>&
    cache = ( selNature == SelectionNature_Detection ? m_detectedCells : m_selectedCells );

  // Mark cells
  for ( vtkIdType cellId = 0; cellId < numOfInputCells; ++cellId )
  {
    // Check pedigree ID of the cell. Continue only for those which are
    // requested by the caller code
    double pedigreeIdDbl;
    pPedigreeArr->GetTuple(cellId, &pedigreeIdDbl);
    const int pedigreeId = (int) pedigreeIdDbl;
    //
    if ( !elementIds.Contains(pedigreeId) )
      continue;

    // Get the current scalar
    const int
      sPrim = (int) pShapePrimArr->GetTuple1(cellId);

    // Save the original scalar in cache
    if ( sPrim != ShapePrimitive_Detected && sPrim != ShapePrimitive_Selected )
    {
      if ( cache.IsBound(cellId) )
        cache.UnBind(cellId);
      //
      cache.Bind( cellId, (int) pShapePrimArr->GetTuple1(cellId) );
    }

    // Change scalar
    pShapePrimArr->SetTuple1(cellId, prim);
  }

  // Set modification status for data and update
  pData->Modified();
  this->Mapper()->Update();
}

//-----------------------------------------------------------------------------

//! Resets initial scalars for the detected/selected cells.
//! \param selNature [in] selection nature (detection or selection).
void asiVisu_PartPipeline::ResetPickedElements(const asiVisu_SelectionNature selNature)
{
  // Get polygonal data
  vtkPolyData*
    pData = vtkPolyData::SafeDownCast( this->Mapper()->GetInputDataObject(0, 0) );
  //
  if ( !pData )
    return;

  // Get cell data
  vtkCellData* pCellData       = pData->GetCellData();
  const int    numOfInputCells = pCellData->GetNumberOfTuples();

  // Get array of cell types
  vtkIdTypeArray*
    pShapePrimArr = vtkIdTypeArray::SafeDownCast( pCellData->GetArray(ARRNAME_PART_CELL_TYPES) );

  // Choose cache
  NCollection_DataMap<vtkIdType, int>&
    cache = ( selNature == SelectionNature_Detection ? m_detectedCells : m_selectedCells );

  // Reset cell scalars
  for ( vtkIdType cellId = 0; cellId < numOfInputCells; ++cellId )
  {
    // Proceed for the cached cells only
    if ( !cache.IsBound(cellId) )
      continue;

    // Reset scalar
    pShapePrimArr->SetTuple1( cellId, cache.Find(cellId) );
  }

  // Set modification status for data and update
  pData->Modified();
  this->Mapper()->Update();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_PartPipeline::callback_add_to_renderer(vtkRenderer*)
{
  this->Actor()->GetProperty()->SetInterpolationToPhong();
}

//-----------------------------------------------------------------------------

//! Callback for RemoveFromRenderer() routine.
void asiVisu_PartPipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//-----------------------------------------------------------------------------

//! Callback for Update() routine.
void asiVisu_PartPipeline::callback_update()
{
  if ( !m_bMapperColorsSet )
  {
    asiVisu_Utils::InitShapeMapper(m_mapper);
    m_bMapperColorsSet = true;
  }
}
