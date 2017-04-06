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

//-----------------------------------------------------------------------------

asiVisu_DisplayModeFilter::asiVisu_DisplayModeFilter()
: vtkPolyDataAlgorithm(),
  m_displayMode(DisplayMode_Undefined)
{
  // SHADED
  {
    TColStd_PackedMapOfInteger mode;
    //
    mode.Add(ShapePrimitive_FreeVertex);
    mode.Add(ShapePrimitive_DanglingEdge);
    mode.Add(ShapePrimitive_NonManifoldEdge);
    mode.Add(ShapePrimitive_Facet);
    //
    m_modeMap.Bind(DisplayMode_Shaded, mode);
  }

  // WIREFRAME
  {
    TColStd_PackedMapOfInteger mode;
    //
    mode.Add(ShapePrimitive_FreeVertex);
    mode.Add(ShapePrimitive_DanglingEdge);
    mode.Add(ShapePrimitive_FreeEdge);
    mode.Add(ShapePrimitive_ManifoldEdge);
    mode.Add(ShapePrimitive_NonManifoldEdge);
    //
    m_modeMap.Bind(DisplayMode_Wireframe, mode);
  }

  // VERTICES
  {
    TColStd_PackedMapOfInteger mode;
    //
    mode.Add(ShapePrimitive_FreeVertex);
    mode.Add(ShapePrimitive_SharedVertex);
    //
    m_modeMap.Bind(DisplayMode_Vertices, mode);
  }
}

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
  const TColStd_PackedMapOfInteger& modePrimitiveTypes = m_modeMap(m_displayMode);

  // Get the input and output
  vtkInformation* pInInfo  = pInputVector[0]->GetInformationObject(0);
  vtkInformation* pOutInfo = pOutputVector->GetInformationObject(0);

  // Input
  vtkPolyData*
    pInputPolyData = vtkPolyData::SafeDownCast( pInInfo->Get(vtkDataObject::DATA_OBJECT() ) );
  vtkCellData*
    pInputCellData = pInputPolyData->GetCellData();
  vtkIdTypeArray*
    pInputDataArray = vtkIdTypeArray::SafeDownCast( pInputCellData->GetArray(ARRNAME_PART_CELL_TYPES) );

  // Output
  vtkPolyData*
    pOutputPolyData = vtkPolyData::SafeDownCast( pOutInfo->Get(vtkDataObject::DATA_OBJECT() ) );
  vtkCellData*
    pOutputCellData = pOutputPolyData->GetCellData();

  // List of cell ids to be passed
  vtkSmartPointer<vtkIdList> idList = vtkSmartPointer<vtkIdList>::New();
  idList->Allocate( modePrimitiveTypes.Extent() ); // Allocate the list of ids

  vtkIdType numOfTuples = 0;
  if ( pInputDataArray )
  {
    numOfTuples = pInputDataArray->GetNumberOfTuples();
    idList->Allocate(numOfTuples); // Allocate the list of ids
  }

  // Filtering
  for ( vtkIdType t = 0; t < numOfTuples; ++t )
  {
    int primTypeOfACell = (int) pInputDataArray->GetValue(t);

    if ( modePrimitiveTypes.Contains(primTypeOfACell) )
    {
      // Add a cell id to output if it's value is in the set.
      idList->InsertNextId(t);
    }
  }

  // Copy cells with their points according to the prepared list of cell ids.
  pOutputPolyData->Allocate( pInputPolyData, idList->GetNumberOfIds() );  // Allocate output cells
  pOutputPolyData->GetCellData()->AllocateArrays( pInputPolyData->GetCellData()->GetNumberOfArrays() );

  // Pass data arrays
  for ( int i = 0; i < pInputCellData->GetNumberOfArrays(); ++i )
  {
    vtkDataArray* pInArr  = pInputCellData->GetArray(i);
    vtkDataArray* pOutArr = vtkDataArray::CreateDataArray( pInArr->GetDataType() );
    //
    pOutArr->SetName( pInArr->GetName() );
    pOutArr->Allocate( idList->GetNumberOfIds() * pInArr->GetNumberOfComponents() );
    pOutArr->SetNumberOfTuples( idList->GetNumberOfIds() );
    pOutArr->SetNumberOfComponents( pInArr->GetNumberOfComponents() );
    //
    pOutputCellData->AddArray(pOutArr);
  }

  // Copy cells with ids from our list
  pOutputPolyData->CopyCells(pInputPolyData, idList);

  // Copy filtered arrays data
  vtkIdType outId, inId;
  for ( int i = 0; i < pInputCellData->GetNumberOfArrays(); ++i )
  {
    vtkDataArray* pInArr  = pInputCellData->GetArray(i);
    vtkDataArray* pOutArr = pOutputCellData->GetArray(i);

    for ( outId = 0; outId < idList->GetNumberOfIds(); ++outId )
    {
      inId = idList->GetId(outId);
      pOutArr->SetTuple(outId, inId, pInArr);
    }
  }

  // Return non-zero value in case of success
  return vtkPolyDataAlgorithm::RequestData(pInfo, pInputVector, pOutputVector);
}
