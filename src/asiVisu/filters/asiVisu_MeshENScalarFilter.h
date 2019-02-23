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

#ifndef asiVisu_MeshENScalarFilter_h
#define asiVisu_MeshENScalarFilter_h

// Visualization includes
#include <asiVisu.h>

// Active Data (auxiliary) includes
#include <ActAux_Common.h>

// VTK includes
#include <vtkDoubleArray.h>
#include <vtkIdList.h>
#include <vtkPolyDataAlgorithm.h>

// OCCT includes
#include <NCollection_SparseArray.hxx>

//! Elementary data chunk representing EN scalar values.
struct ScalarTuple
{
  bool IsTriangle;

  double F[4];

  ScalarTuple() : IsTriangle(true)
  {
    F[0] = 0.0; F[1] = 0.0; F[2] = 0.0; F[3] = 0.0;
  }

  ScalarTuple(const bool   isTriangle,
              const double theF1,
              const double theF2,
              const double theF3,
              const double theF4 = 0.0) : IsTriangle(isTriangle)
  {
    F[0] = theF1; F[1] = theF2; F[2] = theF3; F[3] = theF4;
  }
};

//! Collection of scalar values associated with sparse integer IDs.
typedef NCollection_SparseArray<ScalarTuple> ScalarMap;
typedef NCollection_Shared<ScalarMap>        HScalarMap;

//! Filter splitting the cells by their shared points in order to associate
//! the charged nodal scalars for each one.
class asiVisu_MeshENScalarFilter : public vtkPolyDataAlgorithm
{
public:

  vtkTypeMacro(asiVisu_MeshENScalarFilter, vtkPolyDataAlgorithm);

public:

  static asiVisu_MeshENScalarFilter* New();

  //! Sets the shrink factor for nodal splitting.
  //! \param theShrinkFactor [in] shrink factor to set
  void SetShrinkFactor(const double theShrinkFactor)
  {
    m_fShrinkFactor = theShrinkFactor;
  }

  //! Returns the shrink factor used for nodal splitting.
  //! \return shrink factor
  double GetShrinkFactor() const
  {
    return m_fShrinkFactor;
  }

  //! Sets nodal scalars distributed by triangle/quadrangle elements.
  //! \param theNodalTriIDs   [in] involved triangle element IDs.
  //! \param theNodalTriData  [in] nodal scalars for the involved triangles.
  //! \param theNodalQuadIDs  [in] involved quadrangle element IDs.
  //! \param theNodalQuadData [in] nodal scalars for the involved quadrangles.
  void SetScalarArrays(const Handle(HIntArray)&  theNodalTriIDs,
                       const Handle(HRealArray)& theNodalTriData,
                       const Handle(HIntArray)&  theNodalQuadIDs,
                       const Handle(HRealArray)& theNodalQuadData)
  {
    m_scalarMap->Clear();
    for ( int i = theNodalTriIDs->Lower(); i <= theNodalTriIDs->Upper(); i++ )
    {
      int anID = theNodalTriIDs->Value(i);
      if ( anID == -1 )
        continue;

      double aTriScalars[3];
      for ( int k = 0; k < 3; k++ )
        aTriScalars[k] = theNodalTriData->Value(3 * i + k);

      m_scalarMap->Bind( anID, ScalarTuple(true, aTriScalars[0], aTriScalars[1], aTriScalars[2]) );
    }
    for ( int i = theNodalQuadIDs->Lower(); i <= theNodalQuadIDs->Upper(); i++ )
    {
      int anID = theNodalQuadIDs->Value(i);
      if ( anID == -1 )
        continue;

      double aQuadScalars[4];
      for ( int k = 0; k < 4; k++ )
        aQuadScalars[k] = theNodalQuadData->Value(4 * i + k);

      m_scalarMap->Bind( anID, ScalarTuple(false, aQuadScalars[0], aQuadScalars[1], aQuadScalars[2], aQuadScalars[3]) );
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

  asiVisu_MeshENScalarFilter();

  ~asiVisu_MeshENScalarFilter();

public:

  virtual int FillInputPortInformation(int thePort, vtkInformation* theInfo);

  virtual int RequestData(vtkInformation*        theInfo,
                          vtkInformationVector** theInput,
                          vtkInformationVector*  theOutput);

private:

  void applyShrinkFactor(vtkDataSet* theInput,
                         vtkIdList*  thePointIds,
                         double*     thePointCoords);

  bool scalarsForElem(const int theID,
                      ScalarTuple& theScTuple);

  void adjustMinMax(const ScalarTuple& theScTuple);

private:

  //! Prohibited copy constructor.
  //! \param theFilter [in] another filter.
  asiVisu_MeshENScalarFilter(const asiVisu_MeshENScalarFilter& theFilter); // Not implemented.

  //! Prohibited assignment operator.
  //! \param theFilter [in] another filter.
  void operator=(const asiVisu_MeshENScalarFilter& theFilter); // Not implemented.

private:

  //! Shrink factor used for nodal splitting.
  double m_fShrinkFactor;

  //! Scalar data.
  Handle(HScalarMap) m_scalarMap;

  //! Maximum scalar value obtained during the filtering process.
  double m_fMaxScalar;

  //! Minimal scalar value obtained during the filtering process.
  double m_fMinScalar;

};

#endif
