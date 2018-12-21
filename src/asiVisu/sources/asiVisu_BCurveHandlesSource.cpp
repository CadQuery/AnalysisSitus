//-----------------------------------------------------------------------------
// Created on: 21 December 2018
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
//    * Neither the name of Sergey Slyadnev nor the
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
#include <asiVisu_BCurveHandlesSource.h>

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

vtkStandardNewMacro(asiVisu_BCurveHandlesSource);

//-----------------------------------------------------------------------------

asiVisu_BCurveHandlesSource::asiVisu_BCurveHandlesSource() : vtkPolyDataAlgorithm()
{
  m_iActiveHandleId = -1;

  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//-----------------------------------------------------------------------------

asiVisu_BCurveHandlesSource::~asiVisu_BCurveHandlesSource()
{}

//-----------------------------------------------------------------------------

void asiVisu_BCurveHandlesSource::SetInputs(const Handle(Geom_BSplineCurve)& bcurve,
                                            const Handle(HRealArray)&        handles)
{
  m_curve   = bcurve;
  m_handles = handles;

  this->Modified();
}

//-----------------------------------------------------------------------------

void asiVisu_BCurveHandlesSource::SetActiveHandle(const int handleId)
{
  m_iActiveHandleId = handleId;

  this->Modified();
}

//-----------------------------------------------------------------------------

int asiVisu_BCurveHandlesSource::RequestData(vtkInformation*        request,
                                             vtkInformationVector** inputVector,
                                             vtkInformationVector*  outputVector)
{
  if ( m_curve.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: b-curve is not set" );
    return 0;
  }

  if ( m_handles.IsNull() || !m_handles->Length() )
  {
    // No handles to visualize.
    return Superclass::RequestData(request, inputVector, outputVector);
  }

  /* ==============================
   *  Prepare involved collections
   * ============================== */

  const int nHandles = m_handles->Length();

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
  for ( int handleId = 0; handleId < nHandles; ++handleId )
  {
    // If active handle is specified, skip all but this handle.
    if ( m_iActiveHandleId != -1 && handleId != m_iActiveHandleId )
      continue;

    // Register geometry (point).
    const vtkIdType
      pid = this->registerGridPoint(handleId, polyOutput);

    // Register topology (vertex).
    this->registerVertex(pid, handleId, polyOutput);
  }

  return Superclass::RequestData(request, inputVector, outputVector);
}

//-----------------------------------------------------------------------------

vtkIdType asiVisu_BCurveHandlesSource::registerGridPoint(const int    index,
                                                         vtkPolyData* polyData)
{
  // Access necessary arrays
  vtkPoints* points = polyData->GetPoints();

  // Push the point into VTK data set
  gp_Pnt P = m_curve->Value( m_handles->Value(index) );
  //
  vtkIdType pid = points->InsertNextPoint( P.X(), P.Y(), P.Z() );

  return pid;
}

//-----------------------------------------------------------------------------

vtkIdType asiVisu_BCurveHandlesSource::registerVertex(const vtkIdType pid,
                                                      const int       handleId,
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
  gidsArray->InsertNextValue(handleId);

  return cellID;
}
