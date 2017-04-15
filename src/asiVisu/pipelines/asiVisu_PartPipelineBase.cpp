//-----------------------------------------------------------------------------
// Created on: 15 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_PartPipelineBase.h>

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

asiVisu_PartPipelineBase::asiVisu_PartPipelineBase(const vtkSmartPointer<asiVisu_ShapeRobustSource>& source)
//
: asiVisu_Pipeline   ( vtkSmartPointer<vtkPolyDataMapper>::New(), vtkSmartPointer<vtkActor>::New() ),
  m_bMapperColorsSet ( false )
{
  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Initialize Data Source
  if ( !source.GetPointer() )
    m_source = vtkSmartPointer<asiVisu_ShapeRobustSource>::New();
  else
    m_source = source;

  // Filter for normals
  m_normalsFilter = vtkSmartPointer<vtkPolyDataNormals>::New();

  // Display mode filter
  m_dmFilter = vtkSmartPointer<asiVisu_DisplayModeFilter>::New();

  // Compose pipeline
  this->append(m_dmFilter);
  this->append(m_normalsFilter);
}

//-----------------------------------------------------------------------------

void asiVisu_PartPipelineBase::SetDiagnosticTools(ActAPI_ProgressEntry progress,
                                                  ActAPI_PlotterEntry  plotter)
{
  m_progress = progress;
  m_plotter  = plotter;
  //
  m_source->SetDiagnosticTools(m_progress, m_plotter);
}

//-----------------------------------------------------------------------------

//! Highlights the passed elements according to the given selection nature.
//! \param elementIds [in] element IDs.
//! \param selNature  [in] selection nature (detection or selection).
void asiVisu_PartPipelineBase::SetPickedElements(const TColStd_PackedMapOfInteger& elementIds,
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
  asiVisu_ShapePrimitive requestedPrim;
  if ( selNature == SelectionNature_Detection )
    requestedPrim = ShapePrimitive_Detected;
  else
    requestedPrim = ShapePrimitive_Selected;

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
      currentPrim = (int) pShapePrimArr->GetTuple1(cellId);

    // Save the original scalar in cache
    if ( currentPrim != ShapePrimitive_Detected && currentPrim != ShapePrimitive_Selected )
    {
      if ( cache.IsBound(cellId) )
        cache.UnBind(cellId);
      //
      cache.Bind( cellId, (int) pShapePrimArr->GetTuple1(cellId) );
    }

    if ( currentPrim == ShapePrimitive_Selected && requestedPrim == ShapePrimitive_Detected )
      continue; // Detection must not override selection

    // Change scalar
    pShapePrimArr->SetTuple1(cellId, requestedPrim);
  }

  // Set modification status for data and update
  pData->Modified();
  this->Mapper()->Update();
}

//-----------------------------------------------------------------------------

//! Resets initial scalars for the detected/selected cells.
//! \param selNature [in] selection nature (detection or selection).
void asiVisu_PartPipelineBase::ResetPickedElements(const asiVisu_SelectionNature selNature)
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

    // Get the current scalar
    const int
      currentPrim = (int) pShapePrimArr->GetTuple1(cellId);

    // Do not restore the original scalar if the cell is in selected state,
    // given that the request on reset is yielded for detection
    if ( currentPrim == ShapePrimitive_Selected && selNature == SelectionNature_Detection )
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
void asiVisu_PartPipelineBase::callback_add_to_renderer(vtkRenderer*)
{
  this->Actor()->GetProperty()->SetInterpolationToPhong();
}

//-----------------------------------------------------------------------------

//! Callback for RemoveFromRenderer() routine.
void asiVisu_PartPipelineBase::callback_remove_from_renderer(vtkRenderer*)
{}

//-----------------------------------------------------------------------------

//! Callback for Update() routine.
void asiVisu_PartPipelineBase::callback_update()
{
  if ( !m_bMapperColorsSet )
  {
    asiVisu_Utils::InitShapeMapper(m_mapper);
    m_bMapperColorsSet = true;
  }
}
