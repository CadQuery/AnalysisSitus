//-----------------------------------------------------------------------------
// Created on: 13 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_MeshEVectorFilter.h>

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

vtkStandardNewMacro(asiVisu_MeshEVectorFilter);

//! Default constructor.
asiVisu_MeshEVectorFilter::asiVisu_MeshEVectorFilter()
{
  m_vectorMap = new asiVisu_HVectorMap(100);

  m_fMaxModulus =  1.0;
  m_fMinScalar  =  VTK_FLOAT_MAX;
  m_fMaxScalar  = -VTK_FLOAT_MAX;

  //this->GetInformation()->Set(vtkAlgorithm::PRESERVES_RANGES(), 1);
  //this->GetInformation()->Set(vtkAlgorithm::PRESERVES_BOUNDS(), 1);
}

//! Destructor.
asiVisu_MeshEVectorFilter::~asiVisu_MeshEVectorFilter()
{
}

//! Specifies the applicable input types.
//! \param thePort [not used] connection port.
//! \param theInfo [in/out] information object.
//! \return result code.
int asiVisu_MeshEVectorFilter::FillInputPortInformation(int, vtkInformation* info)
{
  // This filter uses the vtkDataSet cell traversal methods so it
  // supports any data set type as input
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}

//! This method (called by superclass) performs conversion of OCCT
//! data structures to VTK polygonal representation.
//!
//! \param request      [in]  describes "what" algorithm should do. This is
//!                           typically just one key such as REQUEST_INFORMATION.
//! \param inputVector  [in]  inputs of the algorithm.
//! \param outputVector [out] outputs of the algorithm.
int asiVisu_MeshEVectorFilter::RequestData(vtkInformation*,
                                           vtkInformationVector** theInputVector,
                                           vtkInformationVector*  theOutputVector)
{
  // Get input and output data sets
  vtkDataSet* anInput = vtkDataSet::GetData(theInputVector[0]);
  vtkPolyData* anOutput = vtkPolyData::GetData(theOutputVector);

  vtkIdType aNbCells = anInput->GetNumberOfCells();
  vtkIdType aNbPoints = anInput->GetNumberOfPoints();

  // Skip execution if there is no input geometry
  if ( aNbCells < 1 || aNbPoints < 1 )
  {
    vtkDebugMacro("No data to process!");
    return 1;
  }

  // Skip execution if there are no input vectors
  if ( m_vectorMap->IsEmpty() )
  {
    m_fMaxScalar = m_fMinScalar = 0.0;
    vtkDebugMacro("No vectors to process!");
    return 1;
  }

  // Reset min & max scalar values
  m_fMaxScalar = -VTK_FLOAT_MAX;
  m_fMinScalar =  VTK_FLOAT_MAX;

  // Allocate working space for the new and old cell point lists
  vtkSmartPointer<vtkIdList> anOldPointIds = vtkSmartPointer<vtkIdList>::New();
  vtkSmartPointer<vtkIdList> aNewPointIds = vtkSmartPointer<vtkIdList>::New();
  anOldPointIds->Allocate(VTK_CELL_SIZE);
  aNewPointIds->Allocate(VTK_CELL_SIZE);

  // Allocate roughly the space needed for the output cells
  anOutput->Allocate(aNbCells);

  // Allocate roughly the space for the new collection of points
  vtkSmartPointer<vtkPoints> aNewPoints = vtkSmartPointer<vtkPoints>::New();
  aNewPoints->Allocate(aNbPoints, aNbPoints);

  // Allocate space for data associated with the new set of points
  vtkPointData* anOutputPD = anOutput->GetPointData();

  // Get arrays of IDs
  vtkIntArray* aCellIDs =
    vtkIntArray::SafeDownCast( anInput->GetCellData()->GetArray(ARRNAME_MESH_ELEM_IDS) );

  // Allocate space for data associated with the new set of cells
  vtkCellData* anInputCD = anInput->GetCellData();
  vtkCellData* anOutputCD = anOutput->GetCellData();
  anOutputCD->CopyAllocate(anInputCD, aNbCells, aNbCells);

  // Allocate an array for cell vectors
  vtkSmartPointer<vtkDoubleArray>
    aNewCellVectors = asiVisu_Utils::InitDoubleVectorArray(ARRNAME_MESH_E_VECTORS);

  // Traverse all cells
  for ( vtkIdType aCellId = 0; aCellId < aNbCells; ++aCellId )
  {
    // Get the list of points for this cell
    anInput->GetCellPoints(aCellId, anOldPointIds);
    vtkIdType aNbCellPoints = anOldPointIds->GetNumberOfIds();

    // Create new points for this cell
    aNewPointIds->Reset();

    // Get ID of the mesh element
    int anElemID = aCellIDs->GetValue(aCellId);

    // Access scalar value
    asiVisu_VectorTuple aVecTuple;
    bool isCellVectored = this->vectorForElem(anElemID, aVecTuple);
    this->adjustMinMax(aVecTuple);

    if ( !isCellVectored )
      continue;

    double aCenterCoords[3] = {0.0, 0.0, 0.0};
    for ( vtkIdType aNodeLocalID = 0; aNodeLocalID < aNbCellPoints; ++aNodeLocalID )
    {
      vtkIdType anOldPid = anOldPointIds->GetId(aNodeLocalID);
      double aCurrentCoords[3];
      anInput->GetPoint(anOldPid, aCurrentCoords);

      for ( int k = 0; k < 3; k++ )
        aCenterCoords[k] += aCurrentCoords[k];
    }

    for ( int k = 0; k < 3; k++ )
      aCenterCoords[k] /= aNbCellPoints;

    vtkIdType aNewPntPid = aNewPoints->InsertNextPoint(aCenterCoords);
    aNewPointIds->InsertId(0, aNewPntPid);

    // Store the new cell in the output as vertex
    vtkIdType aNewCellPid = anOutput->InsertNextCell(VTK_VERTEX, aNewPointIds);

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
  anOutputPD->SetScalars(aNewCellVectors);

  // Store the new set of points in the output
  anOutput->SetPoints(aNewPoints);

  // Avoid keeping extra memory around
  anOutput->Squeeze();

  return 1;
}

//! Retrieves vector components for the element with the given ID.
//! \param theElemID   [in]  ID of the mesh element to access data for.
//! \param theVecTuple [out] requested vectorial data if any.
//! \return true if vectorial data has been found, false -- otherwise.
bool
  asiVisu_MeshEVectorFilter::vectorForElem(const int    theElemID,
                                           asiVisu_VectorTuple& theVecTuple)
{
  if ( !m_vectorMap->IsBound(theElemID) )
    return false;

  theVecTuple = m_vectorMap->Find(theElemID);
  return true;
}

//! Adjusts min & max scalar values against the passed vectorial data.
//! \param theVecTuple [in] vectorial data.
void asiVisu_MeshEVectorFilter::adjustMinMax(const asiVisu_VectorTuple& theVecTuple)
{
  double aVecModulus = 0.0;
  for ( int k = 0; k < 3; k++ )
    aVecModulus += theVecTuple.F[k] * theVecTuple.F[k];
  aVecModulus = Sqrt(aVecModulus);

  if ( aVecModulus > m_fMaxScalar )
    m_fMaxScalar = aVecModulus;
  if ( aVecModulus < m_fMinScalar )
    m_fMinScalar = aVecModulus;
}
