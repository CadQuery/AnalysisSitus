//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_mesh_E_scalar_filter_h
#define visu_mesh_E_scalar_filter_h

// Visualization includes
#include <visu_common.h>

// VTK includes
#include <vtkDoubleArray.h>
#include <vtkIdList.h>
#include <vtkPolyDataAlgorithm.h>

// OCCT includes
#pragma warning(push, 0)
#include <NCollection_SparseArray.hxx>
#pragma warning(pop)

//! Collection of scalar values associated with sparse integer IDs.
typedef NCollection_SparseArray<double> ScalarMap;
typedef NCollection_Shared<ScalarMap>   HScalarMap;

//! Filter populating VTK cell scalars with domain data.
class visu_mesh_E_scalar_filter : public vtkPolyDataAlgorithm
{
public:

  vtkTypeMacro(visu_mesh_E_scalar_filter, vtkPolyDataAlgorithm);

public:

  static visu_mesh_E_scalar_filter* New();

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

  visu_mesh_E_scalar_filter();

  ~visu_mesh_E_scalar_filter();

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
  visu_mesh_E_scalar_filter(const visu_mesh_E_scalar_filter& theFilter); // Not implemented.

  //! Prohibited assignment operator.
  //! \param theFilter [in] another filter.
  void operator=(const visu_mesh_E_scalar_filter& theFilter); // Not implemented.

private:

  //! Scalar values associated with their owning IDs.
  Handle(HScalarMap) m_scalarMap;

  //! Maximum scalar value obtained during the filtering process.
  double m_fMaxScalar;

  //! Minimal scalar value obtained during the filtering process.
  double m_fMinScalar;

};

#endif
