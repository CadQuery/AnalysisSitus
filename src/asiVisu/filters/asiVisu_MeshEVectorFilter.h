//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_MeshEVectorFilter_h
#define asiVisu_MeshEVectorFilter_h

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

        VectorTuple aVecTuple( theElemData->Value(3 * i),
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

  bool vectorForElem (const int theElemID, VectorTuple& theVecTuple);
  void adjustMinMax  (const VectorTuple& theVecTuple);

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
  Handle(HVectorMap) m_vectorMap;

  //! Maximum scalar value obtained during the filtering process.
  double m_fMaxScalar;

  //! Minimal scalar value obtained during the filtering process.
  double m_fMinScalar;

};

#endif
