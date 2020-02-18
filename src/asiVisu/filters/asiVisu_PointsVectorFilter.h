//-----------------------------------------------------------------------------
// Created on: 09 December 2016
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

#ifndef asiVisu_PointsVectorFilter_h
#define asiVisu_PointsVectorFilter_h

// asiVisu includes
#include <asiVisu_Common.h>

// asiAlgo includes
#include <asiAlgo_BaseCloud.h>

// Active Data (auxiliary) includes
#include <ActAux_Common.h>

// VTK includes
#include <vtkDoubleArray.h>
#include <vtkIdList.h>
#include <vtkPolyDataAlgorithm.h>

//! Filter to associate points with normals.
class asiVisu_PointsVectorFilter : public vtkPolyDataAlgorithm
{
public:

  vtkTypeMacro(asiVisu_PointsVectorFilter, vtkPolyDataAlgorithm);

public:

  asiVisu_EXPORT static asiVisu_PointsVectorFilter*
    New();

  //! Sets normals.
  //! \param normals    [in] normals to associate.
  //! \param maxModulus [in] max modulus of a normal vector.
  void SetNormals(const Handle(asiAlgo_BaseCloud<double>)& normals,
                  const double                             maxModulus = 1.0)
  {
    m_normals     = normals;
    m_fMaxModulus = maxModulus;
  }

  //! Gets maximum scalar value obtained during the filtering process.
  //! \return max scalar value
  double GetMaxScalar() const
  {
    return m_fMaxScalar;
  }

  //! Gets minimal scalar value obtained during the filtering process.
  //! \return min scalar value
  double GetMinScalar() const
  {
    return m_fMinScalar;
  }

private:

  asiVisu_EXPORT
    asiVisu_PointsVectorFilter();

  asiVisu_EXPORT
    ~asiVisu_PointsVectorFilter();

public:

  asiVisu_EXPORT virtual int
    FillInputPortInformation(int port, vtkInformation* info);

  asiVisu_EXPORT virtual int
    RequestData(vtkInformation*        info,
                vtkInformationVector** input,
                vtkInformationVector*  output);

private:

  void adjustMinMax(const asiVisu_VectorTuple& vecTuple);

private:

  //! Prohibited copy constructor.
  asiVisu_PointsVectorFilter(const asiVisu_PointsVectorFilter&); // Not implemented.

  //! Prohibited assignment operator.
  void operator=(const asiVisu_PointsVectorFilter&); // Not implemented.

private:

  //! Maximum modulus.
  double m_fMaxModulus;

  //! Normal field.
  Handle(asiAlgo_BaseCloud<double>) m_normals;

  //! Maximum scalar value obtained during the filtering process.
  double m_fMaxScalar;

  //! Minimal scalar value obtained during the filtering process.
  double m_fMinScalar;

};

#endif
