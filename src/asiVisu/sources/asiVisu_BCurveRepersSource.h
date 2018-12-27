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

#ifndef asiVisu_BCurveRepersSource_h
#define asiVisu_BCurveRepersSource_h

// asiVisu includes
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

//-----------------------------------------------------------------------------

//! Data source for reper points associated with a B-spline curve.
class asiVisu_BCurveRepersSource : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_BCurveRepersSource, vtkPolyDataAlgorithm);
  static asiVisu_BCurveRepersSource* New();

public:

  //! Initialize data source with reper points.
  //! \param[in] pts points to set.
  void SetInputPoints(const std::vector<gp_XYZ>& pts);

  //! Sets ID of the active point.
  //! \param[in] reperId ID of the reper to set.
  void SetActiveReper(const int reperId);

protected:

  //! This method (called by superclass) performs conversion of OCCT
  //! data structures to VTK polygonal representation.
  //!
  //! \param[in]  request      describes "what" algorithm should do. This is
  //!                          typically just one key such as REQUEST_INFORMATION.
  //! \param[in]  inputVector  inputs of the algorithm.
  //! \param[out] outputVector outputs of the algorithm.
  //! \return execution result code (0 in case of error).
  virtual int RequestData(vtkInformation*        request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector*  outputVector);

protected:

  //! Adds the grid point with the given index to the passed polygonal data set.
  //! \param[in]     index    0-based index of handle.
  //! \param[in,out] polyData polygonal data set being populated.
  //! \return ID of the just added VTK point.
  vtkIdType
    registerGridPoint(const int    index,
                      vtkPolyData* polyData);

  //! Adds a vertex cell into the polygonal data set.
  //! \param[in]     pid      point index.
  //! \param[in]     reperId  0-based ID of the reper point.
  //! \param[in,out] polyData polygonal data set being populated.
  //! \return ID of the just added VTK cell.
  vtkIdType
    registerVertex(const vtkIdType pid,
                   const int       reperId,
                   vtkPolyData*    polyData);

protected:

  asiVisu_BCurveRepersSource();  //!< Ctor.
  ~asiVisu_BCurveRepersSource(); //!< Dtor.

private:

  asiVisu_BCurveRepersSource(const asiVisu_BCurveRepersSource&) = delete;
  asiVisu_BCurveRepersSource& operator=(const asiVisu_BCurveRepersSource&) = delete;

private:

  std::vector<gp_XYZ> m_repers;         //!< Reper points.
  int                 m_iActiveReperId; //!< ID of the currently active reper.

};

#endif
