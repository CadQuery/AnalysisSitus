//-----------------------------------------------------------------------------
// Created on: 09 December 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_PointsVectorFilter.h>

// Visualization includes
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkIdTypeArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkMath.h>

vtkStandardNewMacro(asiVisu_PointsVectorFilter);

//! Default constructor.
asiVisu_PointsVectorFilter::asiVisu_PointsVectorFilter()
{
  m_fMaxModulus =  1.0;
  m_fMinScalar  =  VTK_FLOAT_MAX;
  m_fMaxScalar  = -VTK_FLOAT_MAX;
}

//! Destructor.
asiVisu_PointsVectorFilter::~asiVisu_PointsVectorFilter()
{}

//! Specifies the applicable input types.
//! \param port [not used] connection port.
//! \param info [in/out] information object.
//! \return result code.
int asiVisu_PointsVectorFilter::FillInputPortInformation(int, vtkInformation* info)
{
  // This filter uses the vtkDataSet cell traversal methods so it
  // supports any data set type as input
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}

//! Main processing routine.
//! \param info   [not used] information object.
//! \param input  [in]       input data.
//! \param output [out]      output data.
//! \return result code.
int asiVisu_PointsVectorFilter::RequestData(vtkInformation*,
                                            vtkInformationVector** inputVector,
                                            vtkInformationVector*  outputVector)
{
  // Get input and output data sets
  vtkDataSet*  input  = vtkDataSet::GetData(inputVector[0]);
  vtkPolyData* output = vtkPolyData::GetData(outputVector);

  const vtkIdType nbCells  = input->GetNumberOfCells();
  const vtkIdType nbPoints = output->GetNumberOfPoints();

  // Skip execution if there is no input topology / geometry
  if ( nbCells < 1 || nbPoints < 1 )
  {
    vtkDebugMacro("No points to process");
    return 1;
  }

  // Skip execution if there are no vectors in the field
  if ( m_normals.IsNull() || m_normals->GetNumberOfPoints() < 1 )
  {
    m_fMaxScalar = m_fMinScalar = 0.0;
    vtkDebugMacro("No vectors to process");
    return 1;
  }

  // Reset min & max scalar values
  m_fMaxScalar = -VTK_FLOAT_MAX;
  m_fMinScalar =  VTK_FLOAT_MAX;

  // Allocate working space for the new and old cell point lists
  vtkSmartPointer<vtkIdList> oldPointIds = vtkSmartPointer<vtkIdList>::New();
  vtkSmartPointer<vtkIdList> newPointIds = vtkSmartPointer<vtkIdList>::New();
  //
  oldPointIds->Allocate(VTK_CELL_SIZE);
  newPointIds->Allocate(VTK_CELL_SIZE);

  // Allocate space needed for the output cells
  output->Allocate(nbCells);

  // Allocate space for the new collection of points
  vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();
  newPoints->Allocate(nbPoints, nbPoints);

  // Allocate space for data associated with the new set of points
  vtkPointData* outputPD = output->GetPointData();

  // Allocate space for data associated with the new set of cells
  vtkCellData* inputCD  = input->GetCellData();
  vtkCellData* outputCD = output->GetCellData();
  //
  outputCD->CopyAllocate(inputCD, nbCells, nbCells);

  // Allocate an array for cell vectors
  vtkSmartPointer<vtkDoubleArray>
    aNewCellVectors = asiVisu_Utils::InitDoubleVectorArray(ARRNAME_MESH_E_VECTORS);

  // Traverse all cells
  for ( vtkIdType aCellId = 0; aCellId < nbCells; ++aCellId )
  {
    // Get the list of points for this cell
    input->GetCellPoints(aCellId, oldPointIds);
    vtkIdType aNbCellPoints = oldPointIds->GetNumberOfIds();

    // Create new points for this cell
    newPointIds->Reset();

    // Access scalar value
    float nx, ny, nz;
    m_normals->GetPoint( (int) aCellId, nx, ny, nz );
    asiVisu_VectorTuple aVecTuple(nx, ny, nz);
    this->adjustMinMax(aVecTuple);

    double aCenterCoords[3] = {0.0, 0.0, 0.0};
    for ( vtkIdType aNodeLocalID = 0; aNodeLocalID < aNbCellPoints; ++aNodeLocalID )
    {
      vtkIdType anOldPid = oldPointIds->GetId(aNodeLocalID);
      double aCurrentCoords[3];
      input->GetPoint(anOldPid, aCurrentCoords);

      for ( int k = 0; k < 3; k++ )
        aCenterCoords[k] += aCurrentCoords[k];
    }

    for ( int k = 0; k < 3; k++ )
      aCenterCoords[k] /= aNbCellPoints;

    vtkIdType aNewPntPid = newPoints->InsertNextPoint(aCenterCoords);
    newPointIds->InsertId(0, aNewPntPid);

    // Store the new cell in the output as vertex
    vtkIdType aNewCellPid = output->InsertNextCell(VTK_VERTEX, newPointIds);

    // Associate scalar with cell data
    double aVecCoords[3] = {aVecTuple.F[0], aVecTuple.F[1], aVecTuple.F[2]};
    aNewCellVectors->InsertTypedTuple(aNewCellPid, aVecCoords);
  }

  // Loop over all vectors calibrating their magnitudes by the maximum one
  for ( vtkIdType vecId = 0; vecId < aNewCellVectors->GetNumberOfTuples(); ++vecId )
  {
    double* aVecCoords = aNewCellVectors->GetTuple(vecId);

    double aVecModulus = 0.0;
    for ( int k = 0; k < 3; k++ )
      aVecModulus += aVecCoords[k] * aVecCoords[k];
    aVecModulus = Sqrt(aVecModulus);

    double aModFactor = (m_fMaxScalar ? m_fMaxModulus * (aVecModulus / m_fMaxScalar) : 0.0);
    for ( int k = 0; k < 3; k++ )
      aVecCoords[k] *= aModFactor;

    aNewCellVectors->SetTuple(vecId, aVecCoords);
  }

  // Set vectors to cell data
  outputPD->SetScalars(aNewCellVectors);

  // Store the new set of points in the output
  output->SetPoints(newPoints);

  // Avoid keeping extra memory around
  output->Squeeze();

  return 1;
}

//! Adjusts min & max scalar values against the passed vectorial data.
//! \param vecTuple [in] vectorial data.
void asiVisu_PointsVectorFilter::adjustMinMax(const asiVisu_VectorTuple& vecTuple)
{
  double aVecModulus = 0.0;
  for ( int k = 0; k < 3; k++ )
    aVecModulus += vecTuple.F[k] * vecTuple.F[k];
  aVecModulus = Sqrt(aVecModulus);

  if ( aVecModulus > m_fMaxScalar )
    m_fMaxScalar = aVecModulus;
  if ( aVecModulus < m_fMinScalar )
    m_fMinScalar = aVecModulus;
}
