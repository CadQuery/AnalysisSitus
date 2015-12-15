//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef visu_mesh_E_vector_filter_h
#define visu_mesh_E_vector_filter_h

// Visualization includes
#include <visu_common.h>

// Active Data (auxiliary) includes
#include <ActAux_Common.h>

// VTK includes
#include <vtkDoubleArray.h>
#include <vtkIdList.h>
#include <vtkPolyDataAlgorithm.h>

// OCCT includes
#pragma warning(push, 0)
#include <NCollection_SparseArray.hxx>
#pragma warning(pop)

//! Elementary data chunk representing vector value.
struct VectorTuple
{
  double F[3];

  VectorTuple()
  {
    F[0] = 0.0; F[1] = 0.0; F[2] = 0.0;
  }

  VectorTuple(const double theF1,
              const double theF2,
              const double theF3)
  {
    F[0] = theF1; F[1] = theF2; F[2] = theF3;
  }
};

//! Collection of vector values associated with sparse integer IDs.
typedef NCollection_SparseArray<VectorTuple> VectorMap;
typedef NCollection_Shared<VectorMap>        HVectorMap;

//! Filter populating VTK cell vectors with domain data.
class visu_mesh_E_vector_filter : public vtkPolyDataAlgorithm
{
public:

  vtkTypeMacro(visu_mesh_E_vector_filter, vtkPolyDataAlgorithm);

public:

  static visu_mesh_E_vector_filter* New();

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

  visu_mesh_E_vector_filter();

  ~visu_mesh_E_vector_filter();

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
  visu_mesh_E_vector_filter(const visu_mesh_E_vector_filter& theFilter); // Not implemented.

  //! Prohibited assignment operator.
  //! \param theFilter [in] another filter.
  void operator=(const visu_mesh_E_vector_filter& theFilter); // Not implemented.

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
