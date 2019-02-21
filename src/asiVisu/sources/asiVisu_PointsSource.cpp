//-----------------------------------------------------------------------------
// Created on: 06 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Sergey Slyadnev
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
#include <asiVisu_PointsSource.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_PointCloudUtils.h>

// VTK includes
#include <vtkCellData.h>
#include <vtkDataObject.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

// Instantiate for allowed types
template class asiVisu_PointsSource<double>;
template class asiVisu_PointsSource<float>;

//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_PointsSource<double>);
vtkStandardNewMacro(asiVisu_PointsSource<float>);

//-----------------------------------------------------------------------------

template <typename REAL_TYPE>
asiVisu_PointsSource<REAL_TYPE>::asiVisu_PointsSource() : vtkPolyDataAlgorithm ()
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline.
}

//-----------------------------------------------------------------------------

template <typename REAL_TYPE>
asiVisu_PointsSource<REAL_TYPE>::~asiVisu_PointsSource()
{}

//-----------------------------------------------------------------------------

template <typename REAL_TYPE>
void asiVisu_PointsSource<REAL_TYPE>::SetInputPoints(const Handle(asiAlgo_BaseCloud<REAL_TYPE>)& points)
{
  m_points = points;

  // Set source modified.
  this->Modified();
}


//-----------------------------------------------------------------------------

template <typename REAL_TYPE>
void asiVisu_PointsSource<REAL_TYPE>::SetInputPoints(const std::vector<gp_XYZ>& points)
{
  // Create and populate a point cloud.
  m_points = new asiAlgo_BaseCloud<REAL_TYPE>;
  //
  for ( size_t k = 0; k < points.size(); ++k )
    m_points->AddElement( points[k].X(), points[k].Y(), points[k].Z() );

  // Set source modified.
  this->Modified();
}

//-----------------------------------------------------------------------------

template <typename REAL_TYPE>
void asiVisu_PointsSource<REAL_TYPE>::SetFilter(const Handle(TColStd_HPackedMapOfInteger)& filter)
{
  m_indices = filter;

  // Set source modified.
  this->Modified();
}

//-----------------------------------------------------------------------------

template <typename REAL_TYPE>
void asiVisu_PointsSource<REAL_TYPE>::SetOnePointFilter(const int pidx)
{
  // Prepare the collection of indices.
  if ( m_indices.IsNull() )
    m_indices = new TColStd_HPackedMapOfInteger;
  else
    m_indices->ChangeMap().Clear();

  // Set single element.
  m_indices->ChangeMap().Add(pidx);

  // Set source modified.
  this->Modified();
}

//-----------------------------------------------------------------------------

template <typename REAL_TYPE>
int asiVisu_PointsSource<REAL_TYPE>::RequestData(vtkInformation*        request,
                                                 vtkInformationVector** inputVector,
                                                 vtkInformationVector*  outputVector)
{
  if ( m_points.IsNull() )
  {
    vtkErrorMacro( << "Invalid input: NULL point cloud" );
    return 0;
  }

  Handle(TColStd_HArray1OfReal) coords = asiAlgo_PointCloudUtils::AsRealArray(m_points);
  //
  if ( coords.IsNull() )
  {
    vtkErrorMacro( << "Invalid input: NULL point cloud" );
    return 0;
  }

  /* ==============================
   *  Prepare involved collections
   * ============================== */

  vtkPolyData* polyOutput = vtkPolyData::GetData(outputVector);
  polyOutput->Allocate();

  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  polyOutput->SetPoints(points);

  //---------------------------------------------------------------------------

  for ( int i = coords->Lower(); i <= coords->Upper() - 2; i += 3 )
  {
    gp_Pnt P( coords->Value(i), coords->Value(i + 1), coords->Value(i + 2) );

    vtkIdType pointIndex = this->registerGridPoint(P, polyOutput);

    // NOTE: this is for sure not the best practice to register all points
    //       and create cells only for those which are not filtered out.
    //       Though, this looks as a simplest possible solution.

    if (  m_indices.IsNull() || // Filter is not set, so pass any point
         !m_indices.IsNull() && m_indices->Map().Contains(pointIndex) )
      this->registerVertex( pointIndex, polyOutput );
  }

  return Superclass::RequestData(request, inputVector, outputVector);
}

//-----------------------------------------------------------------------------

template <typename REAL_TYPE>
vtkIdType asiVisu_PointsSource<REAL_TYPE>::registerGridPoint(const gp_Pnt& point,
                                                             vtkPolyData*  polyData)
{
  // Access necessary arrays.
  vtkPoints* points = polyData->GetPoints();

  // Push the point into VTK data set.
  vtkIdType pid = points->InsertNextPoint( point.X(),
                                           point.Y(),
                                           point.Z() );

  return pid;
}

//-----------------------------------------------------------------------------

template <typename REAL_TYPE>
vtkIdType asiVisu_PointsSource<REAL_TYPE>::registerVertex(const vtkIdType n,
                                                          vtkPolyData*    polyData)
{
  std::vector<vtkIdType> nodes;
  nodes.push_back(n);
  //
  vtkIdType cellID = polyData->InsertNextCell(VTK_VERTEX, 1, &nodes[0]);
  //
  return cellID;
}
