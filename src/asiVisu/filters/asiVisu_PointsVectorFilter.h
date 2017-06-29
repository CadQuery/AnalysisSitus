//-----------------------------------------------------------------------------
// Created on: 09 December 2016
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
  void SetNormals(const Handle(asiAlgo_BaseCloud<float>)& normals,
                  const double                            maxModulus = 1.0)
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
  Handle(asiAlgo_BaseCloud<float>) m_normals;

  //! Maximum scalar value obtained during the filtering process.
  double m_fMaxScalar;

  //! Minimal scalar value obtained during the filtering process.
  double m_fMinScalar;

};

#endif
