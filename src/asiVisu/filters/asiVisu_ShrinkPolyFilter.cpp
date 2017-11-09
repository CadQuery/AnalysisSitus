//-----------------------------------------------------------------------------
// Created on: 20 November 2015
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
#include <asiVisu_ShrinkPolyFilter.h>

// VTK includes
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkShrinkFilter.h>

vtkStandardNewMacro(asiVisu_ShrinkPolyFilter);

//! Default constructor.
asiVisu_ShrinkPolyFilter::asiVisu_ShrinkPolyFilter()
: vtkPolyDataAlgorithm(),
  ShrinkFactor(0.8)
{}

//! Destructor.
asiVisu_ShrinkPolyFilter::~asiVisu_ShrinkPolyFilter()
{}

//! This method (called by superclass) performs conversion of OCCT
//! data structures to VTK polygonal representation.
//!
//! \param request      [in]  describes "what" algorithm should do. This is
//!                           typically just one key such as REQUEST_INFORMATION.
//! \param inputVector  [in]  inputs of the algorithm.
//! \param outputVector [out] outputs of the algorithm.
int asiVisu_ShrinkPolyFilter::RequestData(vtkInformation*        vtkNotUsed(request),
                                          vtkInformationVector** inputVector,
                                          vtkInformationVector*  outputVector)
{
  /* ========================================
   *  Access input & output VTK data objects
   * ======================================== */

  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkPolyData *output= vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  /* ==============================================
   *  Skip execution if there is no input geometry
   * ============================================== */

  vtkIdType numCells = input->GetNumberOfCells();
  vtkIdType numPts = input->GetNumberOfPoints();
  if ( numCells < 1 || numPts < 1 )
  {
    vtkDebugMacro("No data to shrink");
    return 1;
  }

  /* ==============================
   *  Allocate working collections
   * ============================== */

  vtkSmartPointer<vtkIdList> ptIds = vtkSmartPointer<vtkIdList>::New();
  vtkSmartPointer<vtkIdList> newPtIds = vtkSmartPointer<vtkIdList>::New();
  ptIds->Allocate(VTK_CELL_SIZE);
  newPtIds->Allocate(VTK_CELL_SIZE);

  // Allocate space needed for the output cells
  output->Allocate(numCells);

  // Allocate space for the new set of points
  vtkSmartPointer<vtkPoints> newPts = vtkSmartPointer<vtkPoints>::New();
  newPts->Allocate(numPts*8, numPts);

  // Allocate space for data associated with the new set of points
  vtkPointData* inPD = input->GetPointData();
  vtkPointData* outPD = output->GetPointData();
  outPD->CopyAllocate(inPD, numPts*8, numPts);

  /* ================================================================
   *  Traverse all cells, obtaining node coordinates. Compute center
   *  of gravity for each cell, then create new vertices shrunk
   *  towards that center
   * ================================================================ */

  for ( vtkIdType cellId = 0; cellId < numCells; ++cellId )
  {
    int cellType = input->GetCellType(cellId);

    // Get the list of points for this cell
    input->GetCellPoints(cellId, ptIds);
    vtkIdType numIds = ptIds->GetNumberOfIds();

    // Compute the center of mass of the cell by its points
    double center[3] = {0.0, 0.0, 0.0};
    for ( vtkIdType i = 0; i < numIds; ++i )
    {
      double p[3];
      input->GetPoint(ptIds->GetId(i), p);
      for ( int j = 0; j < 3; ++j )
      {
        center[j] += p[j];
      }
    }
    for ( int j = 0; j < 3; ++j )
    {
      center[j] /= numIds;
    }

    double shrinkFactor;
    if ( cellType == VTK_VERTEX || cellType == VTK_POLY_VERTEX )
      shrinkFactor = 1.0; // No shrinking for vertices
    else
      shrinkFactor = ShrinkFactor;

    // Create new points for the current cell
    newPtIds->Reset();
    for ( vtkIdType i = 0; i < numIds; ++i )
    {
      // Get the old point's location
      double p[3];
      input->GetPoint(ptIds->GetId(i), p);

      // Compute the new point's location.
      double newPt[3];
      for ( int j = 0; j < 3; ++j )
      {
        newPt[j] = center[j] + shrinkFactor*(p[j] - center[j]);
      }

      // Create the new point for the current cell
      vtkIdType newId = newPts->InsertNextPoint(newPt);
      newPtIds->InsertId(i, newId);

      // Copy point data from the old point
      vtkIdType oldId = ptIds->GetId(i);
      outPD->CopyData(inPD, oldId, newId);
    }

    // Store the new cell in the output
    output->InsertNextCell(input->GetCellType(cellId), newPtIds);
  }

  // Store the new set of points in the output
  output->SetPoints(newPts);

  // Just pass cell data through because we still have the same number
  // and types of cells
  output->GetCellData()->PassData(input->GetCellData());

  // Avoid keeping extra memory around
  output->Squeeze();

  return 1;
}
