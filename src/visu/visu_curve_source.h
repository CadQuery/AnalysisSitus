//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef visu_curve_source_h
#define visu_curve_source_h

// Visualization includes
#include <visu_utils.h>

// Active Data (auxiliary) includes
#include <ActAux_Common.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

// OCCT includes
#include <NCollection_Sequence.hxx>

//! Source of polygonal data representing a 3D curve.
class visu_curve_source : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(visu_curve_source, vtkPolyDataAlgorithm);
  ASitus_EXPORT static visu_curve_source* New();

// Kernel methods:
public:

  ASitus_EXPORT void
    SetInputArrays(const Handle(HRealArray)& theXCoords,
                   const Handle(HRealArray)& theYCoords,
                   const Handle(HRealArray)& theZCoords,
                   const visu_orientation    theOri = VisuOri_Undefined);

protected:

  virtual int RequestData(vtkInformation*        theRequest,
                          vtkInformationVector** theInputVector,
                          vtkInformationVector*  theOutputVector);

private:

  vtkIdType
    registerGridPoint(const int    thePairIndex,
                      vtkPolyData* thePolyData);

  vtkIdType
    registerLine(const int    thePairIndex,
                 vtkPolyData* thePolyData);

  vtkIdType
    registerVertex(const int    thePairIndex,
                   vtkPolyData* thePolyData);

protected:

  visu_curve_source();
  ~visu_curve_source();

private:

  visu_curve_source(const visu_curve_source&);
  visu_curve_source& operator=(const visu_curve_source&);

private:

  Handle(HRealArray) m_XCoords; //!< X coordinates.
  Handle(HRealArray) m_YCoords; //!< Y coordinates.
  Handle(HRealArray) m_ZCoords; //!< Z coordinates.
  visu_orientation   m_ori;     //!< Curve orientation.

};

#endif
