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
#include <asiVisu_TriangulationDisplayModeFilter.h>

// OCCT includes
#include <TColStd_PackedMapOfInteger.hxx>

// VTK includes
#include <vtkCellData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>

vtkStandardNewMacro(asiVisu_TriangulationDisplayModeFilter)

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiVisu_TriangulationDisplayModeFilter::asiVisu_TriangulationDisplayModeFilter()
: vtkPolyDataAlgorithm(),
  m_displayMode(MeshDisplayMode_Undefined)
{}

//-----------------------------------------------------------------------------

void asiVisu_TriangulationDisplayModeFilter::SetDisplayMode(const asiVisu_MeshDisplayMode mode)
{
  // Get primitive types employed in the display mode
  TColStd_PackedMapOfInteger
    modePrimitiveTypes = asiVisu_MeshDisplayModeProvider::GetPrimitivesForMode(mode);

  if ( !m_modePrimitiveTypes.IsEqual(modePrimitiveTypes) )
  {
    m_displayMode        = mode;
    m_modePrimitiveTypes = modePrimitiveTypes;

    this->Modified();
  }
}

//-----------------------------------------------------------------------------

asiVisu_MeshDisplayMode asiVisu_TriangulationDisplayModeFilter::GetDisplayMode() const
{
  return m_displayMode;
}

//-----------------------------------------------------------------------------

void asiVisu_TriangulationDisplayModeFilter::AddPrimitive(const asiVisu_MeshPrimitive prim)
{
  if ( !m_modePrimitiveTypes.Contains(prim) )
  {
    m_modePrimitiveTypes.Add(prim);
    //
    this->Modified();
  }
}

//-----------------------------------------------------------------------------

void asiVisu_TriangulationDisplayModeFilter::RemovePrimitive(const asiVisu_MeshPrimitive prim)
{
  if ( m_modePrimitiveTypes.Contains(prim) )
  {
    m_modePrimitiveTypes.Remove(prim);
    //
    this->Modified();
  }
}

//-----------------------------------------------------------------------------

int asiVisu_TriangulationDisplayModeFilter::RequestData(vtkInformation*        pInfo,
                                                        vtkInformationVector** pInputVector,
                                                        vtkInformationVector*  pOutputVector)
{
  if ( m_displayMode == MeshDisplayMode_Undefined )
  {
    vtkErrorMacro( << "asiVisu_TriangulationDisplayModeFilter executed with undefined display mode" );
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
    pInputCellTypeArray = vtkIdTypeArray::SafeDownCast( pInputCellData->GetArray(ARRNAME_MESH_ITEM_TYPE) );

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

    if ( m_modePrimitiveTypes.Contains(primTypeOfACell) )
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
    vtkDataArray* pInArr  = pInputCellData->GetArray(i);

    if ( !_strcmpi( pInArr->GetName(), ARRNAME_MESH_ELEM_IDS ) )
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
    outPedigreeArr->SetName(ARRNAME_MESH_ELEM_IDS);
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
