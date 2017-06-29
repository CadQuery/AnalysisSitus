//-----------------------------------------------------------------------------
// Created on: 06 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef asiVisu_PointsSource_h
#define asiVisu_PointsSource_h

// Visualization includes
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_BaseCloud.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

//! Source of polygonal data representing a point cloud.
template <typename REAL_TYPE>
class asiVisu_PointsSource : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_PointsSource, vtkPolyDataAlgorithm);
  static asiVisu_PointsSource* New();

// Kernel methods:
public:

  asiVisu_EXPORT void
    SetInputPoints(const Handle(asiAlgo_BaseCloud<REAL_TYPE>)& points);

  asiVisu_EXPORT void
    SetFilter(const Handle(TColStd_HPackedMapOfInteger)& filter);

protected:

  asiVisu_EXPORT virtual int
    RequestData(vtkInformation*        request,
                vtkInformationVector** inputVector,
                vtkInformationVector*  outputVector);

protected:

  asiVisu_EXPORT vtkIdType
    registerGridPoint(const gp_Pnt& point,
                      vtkPolyData*  polyData);

  asiVisu_EXPORT vtkIdType
    registerVertex(const vtkIdType n,
                   vtkPolyData*    polyData);

protected:

  asiVisu_EXPORT asiVisu_PointsSource();
  asiVisu_EXPORT ~asiVisu_PointsSource();

private:

  asiVisu_PointsSource(const asiVisu_PointsSource&);
  asiVisu_PointsSource& operator=(const asiVisu_PointsSource&);

private:

  Handle(asiAlgo_BaseCloud<REAL_TYPE>) m_points;  //!< Points to visualize.
  Handle(TColStd_HPackedMapOfInteger)  m_indices; //!< Point indices to keep (if filter is set).

};

#endif
