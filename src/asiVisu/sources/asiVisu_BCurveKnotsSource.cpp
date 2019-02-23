//-----------------------------------------------------------------------------
// Created on: 02 September 2016
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
#include <asiVisu_BCurveKnotsSource.h>

// Visualization includes
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkCellData.h>
#include <vtkDataObject.h>
#include <vtkIdTypeArray.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

// OCCT includes
#include <TColgp_Array1OfPnt2d.hxx>

//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_BCurveKnotsSource);

//! Default constructor.
asiVisu_BCurveKnotsSource::asiVisu_BCurveKnotsSource() : vtkPolyDataAlgorithm()
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//! Destructor.
asiVisu_BCurveKnotsSource::~asiVisu_BCurveKnotsSource()
{
}

//-----------------------------------------------------------------------------
// Kernel methods
//-----------------------------------------------------------------------------

//! Initialize data source from a b-curve.
//! \param curve [in] curve to extract a knot sequence from.
void asiVisu_BCurveKnotsSource::SetInputCurve(const Handle(Geom_BSplineCurve)& bcurve)
{
  m_curve = bcurve;

  this->Modified();
}

//! This method (called by superclass) performs conversion of OCCT
//! data structures to VTK polygonal representation.
//!
//! \param request      [in]  describes "what" algorithm should do. This is
//!                           typically just one key such as REQUEST_INFORMATION.
//! \param inputVector  [in]  inputs of the algorithm.
//! \param outputVector [out] outputs of the algorithm.
int asiVisu_BCurveKnotsSource::RequestData(vtkInformation*        request,
                                          vtkInformationVector** inputVector,
                                          vtkInformationVector*  outputVector)
{
  if ( m_curve.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: b-curve is not set" );
    return 0;
  }

  /* ==============================
   *  Prepare involved collections
   * ============================== */

  vtkPolyData* polyOutput = vtkPolyData::GetData(outputVector);
  polyOutput->Allocate();

  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  polyOutput->SetPoints(points);

  const int nKnots = m_curve->Knots().Length();
  if ( !nKnots )
    return 0;

  // Populate poly data with vertices
  for ( int i = 0; i < nKnots; ++i )
  {
    // Register geometry (point) and topology (vertex)
    const vtkIdType pid = this->registerGridPoint(i, polyOutput);
    this->registerVertex(pid, polyOutput);
  }

  return Superclass::RequestData(request, inputVector, outputVector);
}

//! Adds the grid point with the given index to the passed polygonal data set.
//! \param index    [in]     0-based index of knot.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK point.
vtkIdType asiVisu_BCurveKnotsSource::registerGridPoint(const int    index,
                                                      vtkPolyData* polyData)
{
  // Access necessary arrays
  vtkPoints* points = polyData->GetPoints();

  // Push the point into VTK data set
  gp_Pnt P = m_curve->Value( m_curve->Knots()(index + 1) );
  //
  vtkIdType pid = points->InsertNextPoint( P.X(), P.Y(), P.Z() );

  return pid;
}

//! Adds a vertex cell into the polygonal data set.
//! \param pid      [in]     point index.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType asiVisu_BCurveKnotsSource::registerVertex(const vtkIdType pid,
                                                   vtkPolyData*    polyData)
{
  std::vector<vtkIdType> pids;
  pids.push_back(pid);
  //
  vtkIdType cellID =
    polyData->InsertNextCell( VTK_VERTEX, (int) pids.size(), &pids[0] );

  return cellID;
}
