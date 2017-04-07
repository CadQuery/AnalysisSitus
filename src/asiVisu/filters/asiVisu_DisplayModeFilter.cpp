//-----------------------------------------------------------------------------
// Created on: 06 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_DisplayModeFilter.h>

// asiVisu includes
#include <asiVisu_ShapePrimitive.h>

// OCCT includes
#include <TColStd_PackedMapOfInteger.hxx>

// VTK includes
#include <vtkCellData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>

vtkStandardNewMacro(asiVisu_DisplayModeFilter)

#define COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiVisu_DisplayModeFilter::asiVisu_DisplayModeFilter()
: vtkPolyDataAlgorithm(),
  m_displayMode(DisplayMode_Undefined)
{}

//-----------------------------------------------------------------------------

void asiVisu_DisplayModeFilter::SetDisplayMode(const asiVisu_DisplayMode mode)
{
  if ( m_displayMode != mode )
  {
    m_displayMode = mode;

    this->Modified();
  }
}

//-----------------------------------------------------------------------------

asiVisu_DisplayMode asiVisu_DisplayModeFilter::GetDisplayMode() const
{
  return m_displayMode;
}

//-----------------------------------------------------------------------------

int asiVisu_DisplayModeFilter::RequestData(vtkInformation*        pInfo,
                                           vtkInformationVector** pInputVector,
                                           vtkInformationVector*  pOutputVector)
{
  if ( m_displayMode == DisplayMode_Undefined )
  {
    vtkErrorMacro( << "asiVisu_DisplayModeFilter executed with undefined display mode" );
    return 0;
  }

  // Get primitive types employed in the display mode
  TColStd_PackedMapOfInteger
    modePrimitiveTypes = asiVisu_DisplayModeProvider::GetPrimitivesForMode(m_displayMode);

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

    if ( modePrimitiveTypes.Contains(primTypeOfACell) )
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
    vtkDataArray* pOutArr = vtkDataArray::CreateDataArray( pInArr->GetDataType() );
    //
    pOutArr->SetName( pInArr->GetName() );
    pOutArr->Allocate( numOfRemainingCells * pInArr->GetNumberOfComponents() );
    pOutArr->SetNumberOfTuples( numOfRemainingCells );
    pOutArr->SetNumberOfComponents( pInArr->GetNumberOfComponents() );
    //
    pOutputCellData->AddArray(pOutArr);

    // Set array with sub-shape IDs as pedigree array
    if ( !_strcmpi( pInArr->GetName(), ARRNAME_PART_SUBSHAPE_IDS ) )
      pOutputCellData->SetPedigreeIds(pInArr);
  }

  //###################################
  // Copy cells with ids from our list
  pOutputPolyData->CopyCells(pInputPolyData, cellIdsToPass);

  //###################################
  // Copy data arrays
  for ( int i = 1; i < pInputCellData->GetNumberOfArrays(); ++i )
  {
    vtkDataArray* pInArr  = pInputCellData->GetArray(i);
    vtkDataArray* pOutArr = pOutputCellData->GetArray(i);

    for ( vtkIdType outIdx = 0; outIdx < numOfRemainingCells; ++outIdx )
    {
      vtkIdType cellId = cellIdsToPass->GetId(outIdx);
      pOutArr->SetTuple(outIdx, cellId, pInArr);
    }
  }

#if defined COUT_DEBUG
  std::cout << "Number of arrays in the input cell data: " << pInputCellData->GetNumberOfArrays() << std::endl;
  std::cout << "Number of arrays in the output cell data: " << pOutputCellData->GetNumberOfArrays() << std::endl;
#endif

  // Return non-zero value in case of success
  return vtkPolyDataAlgorithm::RequestData(pInfo, pInputVector, pOutputVector);
}
