//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_MeshNScalarFilter_h
#define asiVisu_MeshNScalarFilter_h

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

//! Collection of scalar values associated with sparse integer IDs.
typedef NCollection_SparseArray<double> ScalarMap;
typedef NCollection_Shared<ScalarMap>   HScalarMap;

//! Filter populating VTK point scalars with domain data.
class asiVisu_MeshNScalarFilter : public vtkPolyDataAlgorithm
{
public:

  vtkTypeMacro(asiVisu_MeshNScalarFilter, vtkPolyDataAlgorithm);

public:

  static asiVisu_MeshNScalarFilter* New();

  //! Sets nodal scalars.
  //! \param theNodeIDs  [in] involved mesh node IDs.
  //! \param theNodeData [in] nodal scalars for the involved mesh nodes.
  void SetScalarArrays(const Handle(HIntArray)&  theNodeIDs,
                       const Handle(HRealArray)& theNodeData)
  {
    m_scalarMap->Clear();
    for ( int i = theNodeIDs->Lower(); i <= theNodeIDs->Upper(); i++ )
    {
      int anID = theNodeIDs->Value(i);
      if ( anID == -1 )
        continue;

      double aScalar = theNodeData->Value(i);
      m_scalarMap->Bind(anID, aScalar);
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

  asiVisu_MeshNScalarFilter();

  ~asiVisu_MeshNScalarFilter();

public:

  virtual int FillInputPortInformation(int thePort, vtkInformation* theInfo);

  virtual int RequestData(vtkInformation*        theInfo,
                          vtkInformationVector** theInput,
                          vtkInformationVector*  theOutput);

private:

  bool scalarForNode(const int theNodeID,
                     double&   theScalar);

  void adjustMinMax(double theScalar);

private:

  //! Prohibited copy constructor.
  //! \param theFilter [in] another filter.
  asiVisu_MeshNScalarFilter(const asiVisu_MeshNScalarFilter& theFilter); // Not implemented.

  //! Prohibited assignment operator.
  //! \param theFilter [in] another filter.
  void operator=(const asiVisu_MeshNScalarFilter& theFilter); // Not implemented.

private:

  //! Scalar associated with their owning IDs.
  Handle(HScalarMap) m_scalarMap;

  //! Maximum scalar value obtained during the filtering process.
  double m_fMaxScalar;

  //! Minimal scalar value obtained during the filtering process.
  double m_fMinScalar;

};

#endif
