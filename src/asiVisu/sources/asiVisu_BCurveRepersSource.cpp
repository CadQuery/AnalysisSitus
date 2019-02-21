//-----------------------------------------------------------------------------
// Created on: 27 December 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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
#include <asiVisu_BCurveRepersSource.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkCellData.h>
#include <vtkDataObject.h>
#include <vtkIdTypeArray.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_BCurveRepersSource);

//-----------------------------------------------------------------------------

asiVisu_BCurveRepersSource::asiVisu_BCurveRepersSource() : vtkPolyDataAlgorithm()
{
  m_iActiveReperId = -1;

  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline.
}

//-----------------------------------------------------------------------------

asiVisu_BCurveRepersSource::~asiVisu_BCurveRepersSource()
{}

//-----------------------------------------------------------------------------

void asiVisu_BCurveRepersSource::SetInputPoints(const std::vector<gp_XYZ>& pts)
{
  m_repers = pts;

  this->Modified();
}

//-----------------------------------------------------------------------------

void asiVisu_BCurveRepersSource::SetActiveReper(const int reperId)
{
  m_iActiveReperId = reperId;

  this->Modified();
}

//-----------------------------------------------------------------------------

int asiVisu_BCurveRepersSource::RequestData(vtkInformation*        request,
                                            vtkInformationVector** inputVector,
                                            vtkInformationVector*  outputVector)
{
  if ( !m_repers.size() )
  {
    // No repers to visualize.
    return Superclass::RequestData(request, inputVector, outputVector);
  }

  /* ==============================
   *  Prepare involved collections
   * ============================== */

  const int nRepers = int( m_repers.size() );

  vtkPolyData* polyOutput = vtkPolyData::GetData(outputVector);
  polyOutput->Allocate();

  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  polyOutput->SetPoints(points);

  // Global IDs for selection.
  vtkSmartPointer<vtkIdTypeArray> gidsArr = vtkSmartPointer<vtkIdTypeArray>::New();
  gidsArr->SetNumberOfComponents(1);
  //
  polyOutput->GetCellData()->SetGlobalIds(gidsArr);

  // Populate poly data with vertices.
  for ( int reperId = 0; reperId < nRepers; ++reperId )
  {
    // If active reper is specified, skip all but this handle.
    if ( m_iActiveReperId != -1 && reperId != m_iActiveReperId )
      continue;

    // Register geometry (point).
    const vtkIdType
      pid = this->registerGridPoint(reperId, polyOutput);

    // Register topology (vertex).
    this->registerVertex(pid, reperId, polyOutput);
  }

  return Superclass::RequestData(request, inputVector, outputVector);
}

//-----------------------------------------------------------------------------

vtkIdType asiVisu_BCurveRepersSource::registerGridPoint(const int    index,
                                                        vtkPolyData* polyData)
{
  // Access necessary arrays.
  vtkPoints* points = polyData->GetPoints();

  // Push the point into VTK data set.
  gp_Pnt P = m_repers[index];
  //
  vtkIdType pid = points->InsertNextPoint( P.X(), P.Y(), P.Z() );

  return pid;
}

//-----------------------------------------------------------------------------

vtkIdType asiVisu_BCurveRepersSource::registerVertex(const vtkIdType pid,
                                                     const int       reperId,
                                                     vtkPolyData*    polyData)
{
  std::vector<vtkIdType> pids;
  pids.push_back(pid);
  //
  vtkIdType cellID =
    polyData->InsertNextCell( VTK_VERTEX, (int) pids.size(), &pids[0] );

  // Assign a global ID.
  vtkSmartPointer<vtkIdTypeArray>
    gidsArray = vtkIdTypeArray::SafeDownCast( polyData->GetCellData()->GetGlobalIds() );
  //
  gidsArray->InsertNextValue(reperId);

  return cellID;
}
