//-----------------------------------------------------------------------------
// Created on: 06 April 2017
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
#include <asiVisu_ShapeDisplayModeFilter.h>

// OCCT includes
#include <TColStd_PackedMapOfInteger.hxx>

// VTK includes
#include <vtkCellData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>

vtkStandardNewMacro(asiVisu_ShapeDisplayModeFilter)

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiVisu_ShapeDisplayModeFilter::asiVisu_ShapeDisplayModeFilter()
: vtkPolyDataAlgorithm (),
  m_displayMode        (ShapeDisplayMode_Undefined),
  m_bAllowExtraScalars (false)
{}

//-----------------------------------------------------------------------------

void asiVisu_ShapeDisplayModeFilter::SetDisplayMode(const asiVisu_ShapeDisplayMode mode)
{
  // Get primitive types employed in the display mode
  TColStd_PackedMapOfInteger
    modePrimitiveTypes = asiVisu_ShapeDisplayModeProvider::GetPrimitivesForMode(mode);

  if ( !m_modePrimitiveTypes.IsEqual(modePrimitiveTypes) )
  {
    m_displayMode        = mode;
    m_modePrimitiveTypes = modePrimitiveTypes;

    this->Modified();
  }
}

//-----------------------------------------------------------------------------

asiVisu_ShapeDisplayMode asiVisu_ShapeDisplayModeFilter::GetDisplayMode() const
{
  return m_displayMode;
}

//-----------------------------------------------------------------------------

void asiVisu_ShapeDisplayModeFilter::AddPrimitive(const asiVisu_ShapePrimitive prim)
{
  if ( !m_modePrimitiveTypes.Contains(prim) )
  {
    m_modePrimitiveTypes.Add(prim);
    //
    this->Modified();
  }
}

//-----------------------------------------------------------------------------

void asiVisu_ShapeDisplayModeFilter::RemovePrimitive(const asiVisu_ShapePrimitive prim)
{
  if ( m_modePrimitiveTypes.Contains(prim) )
  {
    m_modePrimitiveTypes.Remove(prim);
    //
    this->Modified();
  }
}

//-----------------------------------------------------------------------------

int asiVisu_ShapeDisplayModeFilter::RequestData(vtkInformation*        pInfo,
                                                vtkInformationVector** pInputVector,
                                                vtkInformationVector*  pOutputVector)
{
  if ( m_displayMode == ShapeDisplayMode_Undefined )
  {
    vtkErrorMacro( << "asiVisu_ShapeDisplayModeFilter executed with undefined display mode" );
    return 0;
  }

  // Get the input and output
  vtkInformation* pInInfo  = pInputVector[0]->GetInformationObject(0);
  vtkInformation* pOutInfo = pOutputVector->GetInformationObject(0);

  // Input
  vtkPolyData*
    pInputPolyData = vtkPolyData::SafeDownCast( pInInfo->Get(vtkDataObject::DATA_OBJECT() ) );
  vtkCellData*
    pInputCellData = pInputPolyData->GetCellData();
  vtkIdTypeArray*
    pInputCellTypeArray = vtkIdTypeArray::SafeDownCast( pInputCellData->GetArray(ARRNAME_PART_CELL_TYPES) );

  // Output
  vtkPolyData*
    pOutputPolyData = vtkPolyData::SafeDownCast( pOutInfo->Get(vtkDataObject::DATA_OBJECT() ) );
  vtkCellData*
    pOutputCellData = pOutputPolyData->GetCellData();

  // Number of cells in the input
  vtkIdType numOfInputCells = pInputCellData->GetNumberOfTuples();

  // List of cell ids to be passed. We allocate as much memory as many
  // cells we have in the input. This gonna be more than enough
  // because usually not all cells are passed though the display mode
  // filter
  vtkSmartPointer<vtkIdList> cellIdsToPass = vtkSmartPointer<vtkIdList>::New();
  cellIdsToPass->Allocate(numOfInputCells); // Allocate the list of ids

  // Collect cell IDs which will be preserved in the output. We need to put
  // the preserved IDs to the list and then call CopyCells() method of VTK
  // which will transfer only necessary cells to the output
  for ( vtkIdType cellId = 0; cellId < numOfInputCells; ++cellId )
  {
    const int primTypeOfACell = (int) pInputCellTypeArray->GetValue(cellId);

    // Notice that the second condition makes the cells with the custom scalars
    // pass just any display mode filtering. That's probably not good, but
    // we keep it like this for now...
    if ( m_modePrimitiveTypes.Contains(primTypeOfACell) ||
         (m_bAllowExtraScalars && primTypeOfACell >= ShapePrimitive_LAST) )
    {
      // Add a cell id to output if it's value is in the set
      cellIdsToPass->InsertNextId(cellId);
    }
  }
  const vtkIdType numOfRemainingCells = cellIdsToPass->GetNumberOfIds();

  // Allocate space for output data arrays
  pOutputPolyData->Allocate( pInputPolyData, numOfRemainingCells );
  pOutputPolyData->GetCellData()->AllocateArrays( pInputPolyData->GetCellData()->GetNumberOfArrays() );

  // Pass data arrays
  for ( int i = 0; i < pInputCellData->GetNumberOfArrays(); ++i )
  {
    vtkDataArray* pInArr = pInputCellData->GetArray(i);

    if ( !strcasecmp( pInArr->GetName(), ARRNAME_PART_SUBSHAPE_IDS ) )
      continue;

    vtkDataArray* pOutArr = vtkDataArray::CreateDataArray( pInArr->GetDataType() );
    //
    pOutArr->SetName( pInArr->GetName() );
    pOutArr->Allocate( numOfRemainingCells * pInArr->GetNumberOfComponents() );
    pOutArr->SetNumberOfTuples( numOfRemainingCells );
    pOutArr->SetNumberOfComponents( pInArr->GetNumberOfComponents() );
    //
    pOutputCellData->AddArray(pOutArr);

    // Copy elements
    for ( vtkIdType outIdx = 0; outIdx < numOfRemainingCells; ++outIdx )
    {
      vtkIdType cellId = cellIdsToPass->GetId(outIdx);
      pOutArr->SetTuple(outIdx, cellId, pInArr);
    }
  }

  // Transfer pedigree IDs
  vtkIdTypeArray*
    pInPedigreeArr = vtkIdTypeArray::SafeDownCast( pInputCellData->GetPedigreeIds() );
  //
  if ( pInPedigreeArr )
  {
    vtkSmartPointer<vtkIdTypeArray>
      outPedigreeArr = vtkSmartPointer<vtkIdTypeArray>::New();
    //
    outPedigreeArr->SetName(ARRNAME_PART_SUBSHAPE_IDS);
    outPedigreeArr->SetNumberOfComponents(1);
    //
    for ( vtkIdType outIdx = 0; outIdx < numOfRemainingCells; ++outIdx )
    {
      vtkIdType cellId = cellIdsToPass->GetId(outIdx);

      double pedigreeIdDbl;
      pInPedigreeArr->GetTuple(cellId, &pedigreeIdDbl);
      outPedigreeArr->InsertNextTuple(&pedigreeIdDbl);
    }
    pOutputCellData->SetPedigreeIds(outPedigreeArr);
  }

  //###################################
  // Copy cells with ids from our list
  pOutputPolyData->CopyCells(pInputPolyData, cellIdsToPass);

#if defined COUT_DEBUG
  std::cout << "Number of arrays in the input cell data: " << pInputCellData->GetNumberOfArrays() << std::endl;
  std::cout << "Number of arrays in the output cell data: " << pOutputCellData->GetNumberOfArrays() << std::endl;
#endif

  // Return non-zero value in case of success
  return vtkPolyDataAlgorithm::RequestData(pInfo, pInputVector, pOutputVector);
}
