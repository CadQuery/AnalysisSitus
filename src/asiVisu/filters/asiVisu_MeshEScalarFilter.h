//-----------------------------------------------------------------------------
// Created on: 13 November 2015
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

#ifndef asiVisu_MeshEScalarFilter_h
#define asiVisu_MeshEScalarFilter_h

// Visualization includes
#include <asiVisu.h>

// VTK includes
#include <vtkDoubleArray.h>
#include <vtkIdList.h>
#include <vtkPolyDataAlgorithm.h>

// Active Data includes
#include <ActAPI_Common.h>

// OCCT includes
#include <NCollection_SparseArray.hxx>

//! Collection of scalar values associated with sparse integer IDs.
typedef NCollection_SparseArray<double> ScalarMap;
typedef NCollection_Shared<ScalarMap>   HScalarMap;

//! Filter populating VTK cell scalars with domain data.
class asiVisu_MeshEScalarFilter : public vtkPolyDataAlgorithm
{
public:

  vtkTypeMacro(asiVisu_MeshEScalarFilter, vtkPolyDataAlgorithm);

public:

  static asiVisu_MeshEScalarFilter* New();

  //! Sets elemental scalars.
  //! \param theElemIDs  [in] involved mesh element IDs.
  //! \param theElemData [in] scalars for the involved mesh elements.
  void SetScalarArrays(const Handle(HIntArray)&  theElemIDs,
                       const Handle(HRealArray)& theElemData)
  {
    m_scalarMap->Clear();

    if ( !theElemIDs.IsNull() && !theElemData.IsNull() )
    {
      for ( int i = theElemIDs->Lower(); i <= theElemIDs->Upper(); i++ )
      {
        int anID = theElemIDs->Value(i);
        if ( anID == -1 )
          continue;

        const double aScalar = theElemData->Value(i);
        m_scalarMap->Bind(anID, aScalar);
      }
    }
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

  asiVisu_MeshEScalarFilter();

  ~asiVisu_MeshEScalarFilter();

public:

  virtual int FillInputPortInformation(int thePort, vtkInformation* theInfo);

  virtual int RequestData(vtkInformation*        theInfo,
                          vtkInformationVector** theInput,
                          vtkInformationVector*  theOutput);

private:

  bool scalarForElem(const int theElemID,
                     double&   theScalar);

  void adjustMinMax(double theScalar);

private:

  //! Prohibited copy constructor.
  //! \param theFilter [in] another filter.
  asiVisu_MeshEScalarFilter(const asiVisu_MeshEScalarFilter& theFilter); // Not implemented.

  //! Prohibited assignment operator.
  //! \param theFilter [in] another filter.
  void operator=(const asiVisu_MeshEScalarFilter& theFilter); // Not implemented.

private:

  //! Scalar values associated with their owning IDs.
  Handle(HScalarMap) m_scalarMap;

  //! Maximum scalar value obtained during the filtering process.
  double m_fMaxScalar;

  //! Minimal scalar value obtained during the filtering process.
  double m_fMinScalar;

};

#endif
