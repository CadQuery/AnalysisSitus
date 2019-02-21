//-----------------------------------------------------------------------------
// Created on: 13 November 2015
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
#include <asiVisu_MeshNScalarFilter.h>

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

vtkStandardNewMacro(asiVisu_MeshNScalarFilter);

//! Default constructor.
asiVisu_MeshNScalarFilter::asiVisu_MeshNScalarFilter()
{
  m_scalarMap = new HScalarMap(100);

  m_fMinScalar =  VTK_FLOAT_MAX;
  m_fMaxScalar = -VTK_FLOAT_MAX;

  //this->GetInformation()->Set(vtkAlgorithm::PRESERVES_RANGES(), 1);
  //this->GetInformation()->Set(vtkAlgorithm::PRESERVES_BOUNDS(), 1);
}

//! Destructor.
asiVisu_MeshNScalarFilter::~asiVisu_MeshNScalarFilter()
{
}

//! Specifies the applicable input types.
//! \param thePort [not used] connection port.
//! \param theInfo [in/out] information object.
//! \return result code.
int asiVisu_MeshNScalarFilter::FillInputPortInformation(int, vtkInformation* info)
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
int asiVisu_MeshNScalarFilter::RequestData(vtkInformation*,
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

  // Skip execution if there are no input scalars
  if ( m_scalarMap->IsEmpty() )
  {
    m_fMaxScalar = m_fMinScalar = 0.0;
    vtkDebugMacro("No scalars to process!");
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

  // Get arrays of IDs
  vtkIntArray* aPointIDs =
    vtkIntArray::SafeDownCast( anInput->GetPointData()->GetArray(ARRNAME_MESH_NODE_IDS) );

  // Allocate space for data associated with the new set of points
  vtkPointData* anInputPD = anInput->GetPointData();
  vtkPointData* anOutputPD = anOutput->GetPointData();
  anOutputPD->CopyAllocate(anInputPD, aNbPoints, aNbPoints);

  // Allocate an array for point scalars
  vtkSmartPointer<vtkDoubleArray>
    aNewNodeScalars = asiVisu_Utils::InitDoubleArray(ARRNAME_MESH_N_SCALARS);

  // Map for shared VTK points
  NCollection_DataMap<int, vtkIdType> aRegPoints;

  // Traverse all cells
  for ( vtkIdType aCellId = 0; aCellId < aNbCells; ++aCellId )
  {
    // Get the list of points for this cell
    anInput->GetCellPoints(aCellId, anOldPointIds);
    vtkIdType aNbCellPoints = anOldPointIds->GetNumberOfIds();

    // Create new points for this cell
    aNewPointIds->Reset();

    bool isCellScalared = true;
    for ( vtkIdType aNodeLocalID = 0; aNodeLocalID < aNbCellPoints; ++aNodeLocalID )
    {
      vtkIdType anOldPid = anOldPointIds->GetId(aNodeLocalID);
      int aNodeID = aPointIDs->GetValue(anOldPid);

      double aScalar = 0.0;
      bool hasScalar = this->scalarForNode(aNodeID, aScalar);
      this->adjustMinMax(aScalar);

      if ( !hasScalar )
      {
        isCellScalared = false;
        break;
      }

      // Create the new point for this cell
      vtkIdType aNewPid;
      if ( !aRegPoints.IsBound(aNodeID) )
      {
        double aCoords[3];
        anInput->GetPoint(anOldPid, aCoords);
        aNewPid = aNewPoints->InsertNextPoint(aCoords);
        aRegPoints.Bind(aNodeID, aNewPid);
      }
      else
        aNewPid = aRegPoints.Find(aNodeID);

      aNewPointIds->InsertId(aNodeLocalID, aNewPid);

      // Retrieve and associate scalar
      double aScalarTuple[1] = {aScalar};
      aNewNodeScalars->InsertTypedTuple(aNewPid, aScalarTuple);

      anOutputPD->CopyData(anInputPD, anOldPid, aNewPid);
    }

    // Store the new cell in the output
    if ( isCellScalared )
      anOutput->InsertNextCell(anInput->GetCellType(aCellId), aNewPointIds);
  }

  // Set scalars to point data
  anOutputPD->SetScalars(aNewNodeScalars);

  // Store the new set of points in the output
  anOutput->SetPoints(aNewPoints);

  // Just pass cell data through because we still have the same number
  // and type of cells
  anOutput->GetCellData()->PassData( anInput->GetCellData() );

  // Avoid keeping extra memory around
  anOutput->Squeeze();

  return 1;
}

//! Retrieves scalar value for the node with the given ID.
//! \param theNodeID [in]  ID of the mesh node to access scalar value for.
//! \param theScalar [out] requested scalar if any.
//! \return true if scalar value has been found, false -- otherwise.
bool
  asiVisu_MeshNScalarFilter::scalarForNode(const int theNodeID,
                                           double&   theScalar)
{
  if ( !m_scalarMap->IsBound(theNodeID) )
    return false;

  theScalar = m_scalarMap->Find(theNodeID);
  return true;
}

//! Adjusts min & max scalar values against the passed scalar value.
//! \param theScalar [in] scalar value to adjust the min & max
//!        values against.
void asiVisu_MeshNScalarFilter::adjustMinMax(double theScalar)
{
  if ( theScalar > m_fMaxScalar )
    m_fMaxScalar = theScalar;
  if ( theScalar < m_fMinScalar )
    m_fMinScalar = theScalar;
}
