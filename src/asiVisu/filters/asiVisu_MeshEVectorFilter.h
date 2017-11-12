//-----------------------------------------------------------------------------
// Created on: 13 November 2015
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

#ifndef asiVisu_MeshEVectorFilter_h
#define asiVisu_MeshEVectorFilter_h

// Visualization includes
#include <asiVisu_Common.h>

// Active Data (auxiliary) includes
#include <ActAux_Common.h>

// VTK includes
#include <vtkDoubleArray.h>
#include <vtkIdList.h>
#include <vtkPolyDataAlgorithm.h>

//! Filter populating VTK cell vectors with domain data.
class asiVisu_MeshEVectorFilter : public vtkPolyDataAlgorithm
{
public:

  vtkTypeMacro(asiVisu_MeshEVectorFilter, vtkPolyDataAlgorithm);

public:

  static asiVisu_MeshEVectorFilter* New();

  //! Sets elemental vectors.
  //! \param theElemIDs    [in] involved mesh element IDs.
  //! \param theElemData   [in] vectors for the involved mesh elements.
  //! \param theMaxModulus [in] maximum modulus.
  void SetVectorArrays(const Handle(HIntArray)&  theElemIDs,
                       const Handle(HRealArray)& theElemData,
                       const double              theMaxModulus = 1.0)
  {
    m_vectorMap->Clear();

    if ( !theElemIDs.IsNull() && !theElemData.IsNull() )
    {
      for ( int i = theElemIDs->Lower(); i <= theElemIDs->Upper(); i++ )
      {
        int anID = theElemIDs->Value(i);
        if ( anID == -1 )
          continue;

        asiVisu_VectorTuple aVecTuple( theElemData->Value(3 * i),
                                       theElemData->Value(3 * i + 1),
                                       theElemData->Value(3 * i + 2) );
        m_vectorMap->Bind(anID, aVecTuple);
      }
    }

    m_fMaxModulus = theMaxModulus;
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

  asiVisu_MeshEVectorFilter();

  ~asiVisu_MeshEVectorFilter();

public:

  virtual int FillInputPortInformation(int thePort, vtkInformation* theInfo);

  virtual int RequestData(vtkInformation*        theInfo,
                          vtkInformationVector** theInput,
                          vtkInformationVector*  theOutput);

private:

  bool vectorForElem (const int theElemID, asiVisu_VectorTuple& theVecTuple);
  void adjustMinMax  (const asiVisu_VectorTuple& theVecTuple);

private:

  //! Prohibited copy constructor.
  //! \param theFilter [in] another filter.
  asiVisu_MeshEVectorFilter(const asiVisu_MeshEVectorFilter& theFilter); // Not implemented.

  //! Prohibited assignment operator.
  //! \param theFilter [in] another filter.
  void operator=(const asiVisu_MeshEVectorFilter& theFilter); // Not implemented.

private:

  //! Maximum modulus.
  double m_fMaxModulus;

  //! Vectors associated with their owning IDs.
  Handle(asiVisu_HVectorMap) m_vectorMap;

  //! Maximum scalar value obtained during the filtering process.
  double m_fMaxScalar;

  //! Minimal scalar value obtained during the filtering process.
  double m_fMinScalar;

};

#endif
