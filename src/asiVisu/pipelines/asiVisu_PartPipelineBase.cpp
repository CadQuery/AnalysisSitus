//-----------------------------------------------------------------------------
// Created on: 15 April 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_PartPipelineBase.h>

// asiVisu includes
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

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiVisu_PartPipelineBase::asiVisu_PartPipelineBase(const vtkSmartPointer<asiVisu_ShapeRobustSource>& source)
//
: asiVisu_Pipeline   ( vtkSmartPointer<vtkPolyDataMapper>::New(), vtkSmartPointer<vtkActor>::New() ),
  m_bScalarsOn       ( true ),
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
  m_dmFilter = vtkSmartPointer<asiVisu_ShapeDisplayModeFilter>::New();

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
//! \param[in] elementIds element IDs.
//! \param[in] selNature  selection nature (detection or selection).
void asiVisu_PartPipelineBase::SetPickedElements(const TColStd_PackedMapOfInteger& elementIds,
                                                 const asiVisu_SelectionNature     selNature)
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

  // Get array of pedigree IDs
  vtkIdTypeArray*
    pPedigreeArr = vtkIdTypeArray::SafeDownCast( pCellData->GetPedigreeIds() );

  // Get array of cell types
  vtkIdTypeArray*
    pShapePrimArr = vtkIdTypeArray::SafeDownCast( pCellData->GetArray(ARRNAME_PART_CELL_TYPES) );

  // Choose proper scalar
  asiVisu_ShapePrimitive hiliPrim;
  if ( selNature == SelectionNature_Detection )
    hiliPrim = ShapePrimitive_Detected;
  else
    hiliPrim = ShapePrimitive_Selected;

  // Choose cache
  NCollection_DataMap<vtkIdType, int>&
    cache = ( selNature == SelectionNature_Detection ? m_detectedCells : m_selectedCells );

  // Mark cells
  for ( vtkIdType cellId = 0; cellId < numOfInputCells; ++cellId )
  {
    // Check pedigree ID of the cell.
    double pedigreeIdDbl;
    pPedigreeArr->GetTuple(cellId, &pedigreeIdDbl);
    const int pedigreeId = (int) pedigreeIdDbl;

    // Special treatment for detection.
    if ( !elementIds.Contains(pedigreeId) && (selNature == SelectionNature_Detection) )
      continue;

    // Substitute cell scalar.
    asiVisu_ShapePrimitive prim2Set;
    //
    if ( !elementIds.Contains(pedigreeId) )
    {
      if ( !cache.IsBound(cellId) )
        continue;

      // If the current cell is not requested, let's restore its original
      // scalar. Thanks to this mechanics, we allow unselection on repeated
      // click.
      prim2Set = (asiVisu_ShapePrimitive) cache(cellId);
    }
    else
    {
      prim2Set = hiliPrim;

      // Get the current scalar.
      const int
        currentPrim = (int) pShapePrimArr->GetTuple1(cellId);

      // Save the original scalar in cache.
      if ( currentPrim != ShapePrimitive_Detected && currentPrim != ShapePrimitive_Selected )
      {
        if ( cache.IsBound(cellId) )
          cache.UnBind(cellId);
        //
        cache.Bind(cellId, currentPrim);
      }

      if ( currentPrim == ShapePrimitive_Selected && hiliPrim == ShapePrimitive_Detected )
        continue; // Detection must not override selection.
    }

    // Set scalar value in array.
    pShapePrimArr->SetTuple1(cellId, prim2Set);
  }

  // Set modification status for data and update.
  pData->Modified();
  this->Mapper()->Update();
}

//-----------------------------------------------------------------------------

//! Resets initial scalars for the detected/selected cells.
//! \param[in] selNature selection nature (detection or selection).
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
  if ( m_bScalarsOn && !m_bMapperColorsSet )
  {
    asiVisu_Utils::InitShapeMapper(m_mapper);

    m_bMapperColorsSet = true;
  }
}
