//-----------------------------------------------------------------------------
// Created on: 20 August 2018
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
#include <asiVisu_BSurfAxesSource.h>

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

vtkStandardNewMacro(asiVisu_BSurfAxesSource);

//-----------------------------------------------------------------------------

//! Default constructor.
asiVisu_BSurfAxesSource::asiVisu_BSurfAxesSource() : vtkPolyDataAlgorithm()
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//-----------------------------------------------------------------------------

//! Destructor.
asiVisu_BSurfAxesSource::~asiVisu_BSurfAxesSource()
{}

//-----------------------------------------------------------------------------

//! Initialize data source from a b-surface.
//! \param[in] bsurf B-spline surface to extract axes for.
//! \return true if poly data has been produced.
bool asiVisu_BSurfAxesSource::SetInputSurface(const Handle(Geom_BSplineSurface)& bsurf)
{
  if ( bsurf.IsNull() )
    return false;

  m_surf = bsurf;
  this->Modified();
  return true;
}

//-----------------------------------------------------------------------------

//! This method (called by superclass) performs conversion of OCCT
//! data structures to VTK polygonal representation.
//!
//! \param[in]  request      describes "what" algorithm should do. This is
//!                          typically just one key such as REQUEST_INFORMATION.
//! \param[in]  inputVector  inputs of the algorithm.
//! \param[out] outputVector outputs of the algorithm.
int asiVisu_BSurfAxesSource::RequestData(vtkInformation*        request,
                                         vtkInformationVector** inputVector,
                                         vtkInformationVector*  outputVector)
{
  /* ==============================
   *  Prepare involved collections
   * ============================== */

  vtkPolyData* polyOutput = vtkPolyData::GetData(outputVector);
  polyOutput->Allocate();

  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  polyOutput->SetPoints(points);

  // Add array for scalars.
  vtkCellData* cellData = polyOutput->GetCellData();
  //
  vtkSmartPointer<vtkIntArray> typeArr = asiVisu_Utils::InitIntArray(ARRNAME_CURVIAXES_SCALARS);
  cellData->AddArray(typeArr);

  /* =======================
   *  Populate VTK data set
   * ======================= */

  const TColStd_Array1OfReal& uKnots = m_surf->UKnots();
  const TColStd_Array1OfReal& vKnots = m_surf->VKnots();

  const double u0 = uKnots( uKnots.Lower() );
  const double u1 = uKnots( uKnots.Lower() + 1 );
  const double v0 = vKnots( vKnots.Lower() );
  const double v1 = vKnots( vKnots.Lower() + 1 );

  // Register glyphs representing images of two first knot intervals.
  this->registerAxis(u0, v0, u1, v0, true, polyOutput);  // U axis.
  this->registerAxis(u0, v0, u0, v1, false, polyOutput); // V axis.

  return Superclass::RequestData(request, inputVector, outputVector);
}

//-----------------------------------------------------------------------------

//! Adds the given point to the passed polygonal data set.
//! \param[in]     P        point to add.
//! \param[in,out] polyData polygonal data set being populated.
//! \return ID of the just added VTK point.
vtkIdType asiVisu_BSurfAxesSource::registerPoint(const gp_Pnt& P,
                                                 vtkPolyData*  polyData)
{
  // Access necessary arrays
  vtkPoints* points = polyData->GetPoints();

  // Push the point into VTK data set
  vtkIdType pid = points->InsertNextPoint( P.X(), P.Y(), P.Z() );

  return pid;
}

//-----------------------------------------------------------------------------

vtkIdType
  asiVisu_BSurfAxesSource::registerAxis(const double u0,
                                        const double v0,
                                        const double u1,
                                        const double v1,
                                        const bool   isUAxis,
                                        vtkPolyData* polyData)
{
  // Choose scalar for coloring.
  asiVisu_CurviAxis axisType;
  if ( isUAxis )
    axisType = VisuCurviAxis_U;
  else
    axisType = VisuCurviAxis_V;

  // Register points.
  gp_Pnt P0 = m_surf->Value(u0, v0);
  gp_Pnt P1 = m_surf->Value(u1, v1);
  //
  std::vector<vtkIdType> pids;
  pids.push_back( this->registerPoint(P0, polyData) );
  pids.push_back( this->registerPoint(P1, polyData) );

  // Add cell.
  vtkIdType cellID =
    polyData->InsertNextCell( VTK_LINE, (int) pids.size(), &pids[0] );

  // Set scalar.
  vtkIntArray*
    typeArr = vtkIntArray::SafeDownCast( polyData->GetCellData()->GetArray(ARRNAME_CURVIAXES_SCALARS) );
  //
  typeArr->InsertNextValue(axisType);

  return cellID;
}
