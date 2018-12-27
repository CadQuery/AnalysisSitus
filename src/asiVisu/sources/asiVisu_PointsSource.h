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

#ifndef asiVisu_PointsSource_h
#define asiVisu_PointsSource_h

// asiVisu includes
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_BaseCloud.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

//-----------------------------------------------------------------------------

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

  //! Sets input points to visualize.
  //! \param[in] points point cloud to visualize.
  asiVisu_EXPORT void
    SetInputPoints(const Handle(asiAlgo_BaseCloud<REAL_TYPE>)& points);

  //! Sets input points to visualize.
  //! \param[in] points vector of coordinate triples to visualize.
  asiVisu_EXPORT void
    SetInputPoints(const std::vector<gp_XYZ>& points);

  //! Sets filter on the indices. The filter will have effect only if the passed
  //! map of indices is not null. Otherwise, the filter is assumed non-existing.
  //! \param[in] filter selected point indices to visualize.
  asiVisu_EXPORT void
    SetFilter(const Handle(TColStd_HPackedMapOfInteger)& filter);

  //! Sets a filter to contain only one index.
  //! \param[in] pidx zero-based index of a point to add into filter.
  asiVisu_EXPORT void
    SetOnePointFilter(const int pidx);

protected:

  //! This method (called by superclass) performs conversion of OCCT
  //! data structures to VTK polygonal representation.
  //!
  //! \param[in]  request      describes "what" algorithm should do. This is
  //!                          typically just one key such as REQUEST_INFORMATION.
  //! \param[in]  inputVector  inputs of the algorithm.
  //! \param[out] outputVector outputs of the algorithm.
  asiVisu_EXPORT virtual int
    RequestData(vtkInformation*        request,
                vtkInformationVector** inputVector,
                vtkInformationVector*  outputVector);

protected:

  //! Adds the given point to the passed polygonal data set.
  //! \param[in]     point    point to add.
  //! \param[in,out] polyData polygonal data set being populated.
  //! \return ID of the just added VTK point.
  asiVisu_EXPORT vtkIdType
    registerGridPoint(const gp_Pnt& point,
                      vtkPolyData*  polyData);

  //! Adds a vertex cell into the polygonal data set.
  //! \param[in]     n        index of the corresponding geometric point.
  //! \param[in,out] polyData polygonal data set being populated.
  //! \return ID of the just added VTK cell.
  asiVisu_EXPORT vtkIdType
    registerVertex(const vtkIdType n,
                   vtkPolyData*    polyData);

protected:

  asiVisu_EXPORT asiVisu_PointsSource(); //!< Default ctor.
  asiVisu_EXPORT ~asiVisu_PointsSource(); //!< Dtor.

private:

  asiVisu_PointsSource(const asiVisu_PointsSource&) = delete;
  asiVisu_PointsSource& operator=(const asiVisu_PointsSource&) = delete;

private:

  Handle(asiAlgo_BaseCloud<REAL_TYPE>) m_points;  //!< Points to visualize.
  Handle(TColStd_HPackedMapOfInteger)  m_indices; //!< Point indices to keep (if filter is set).

};

#endif
