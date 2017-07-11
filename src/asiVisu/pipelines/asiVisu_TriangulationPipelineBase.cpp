//-----------------------------------------------------------------------------
// Created on: 11 July 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_TriangulationPipelineBase.h>

// asiVisu includes
#include <asiVisu_MeshUtils.h>
#include <asiVisu_PartDataProvider.h>
#include <asiVisu_PartNodeInfo.h>

// VTK includes
#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkInformation.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiVisu_TriangulationPipelineBase::asiVisu_TriangulationPipelineBase(const vtkSmartPointer<asiVisu_TriangulationSource>& source)
//
: asiVisu_Pipeline   ( vtkSmartPointer<vtkPolyDataMapper>::New(), vtkSmartPointer<vtkActor>::New() ),
  m_bMapperColorsSet ( false )
{
  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Initialize Data Source
  if ( !source.GetPointer() )
    m_source = vtkSmartPointer<asiVisu_TriangulationSource>::New();
  else
    m_source = source;

  // Display mode filter
  m_dmFilter = vtkSmartPointer<asiVisu_TriangulationDisplayModeFilter>::New();

  // Compose pipeline
  this->append(m_dmFilter);
}

//-----------------------------------------------------------------------------

void asiVisu_TriangulationPipelineBase::SetDiagnosticTools(ActAPI_ProgressEntry progress,
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
void asiVisu_TriangulationPipelineBase::SetPickedElements(const TColStd_PackedMapOfInteger& elementIds,
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
  asiVisu_MeshPrimitive requestedPrim;
  if ( selNature == SelectionNature_Detection )
    requestedPrim = MeshPrimitive_Detected;
  else
    requestedPrim = MeshPrimitive_Selected;

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
    if ( currentPrim != MeshPrimitive_Detected && currentPrim != MeshPrimitive_Selected )
    {
      if ( cache.IsBound(cellId) )
        cache.UnBind(cellId);
      //
      cache.Bind( cellId, (int) pShapePrimArr->GetTuple1(cellId) );
    }

    if ( currentPrim == MeshPrimitive_Selected && requestedPrim == MeshPrimitive_Detected )
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
void asiVisu_TriangulationPipelineBase::ResetPickedElements(const asiVisu_SelectionNature selNature)
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
    if ( currentPrim == MeshPrimitive_Selected && selNature == SelectionNature_Detection )
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
void asiVisu_TriangulationPipelineBase::callback_add_to_renderer(vtkRenderer*)
{
  this->Actor()->GetProperty()->SetInterpolationToPhong();
}

//-----------------------------------------------------------------------------

//! Callback for RemoveFromRenderer() routine.
void asiVisu_TriangulationPipelineBase::callback_remove_from_renderer(vtkRenderer*)
{}

//-----------------------------------------------------------------------------

//! Callback for Update() routine.
void asiVisu_TriangulationPipelineBase::callback_update()
{
  if ( !m_bMapperColorsSet )
  {
    asiVisu_MeshUtils::InitMapper(m_mapper, ARRNAME_MESH_ITEM_TYPE);
    m_bMapperColorsSet = true;
  }
}
