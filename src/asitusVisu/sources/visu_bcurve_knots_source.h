//-----------------------------------------------------------------------------
// Created on: 02 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_bcurve_knots_source_h
#define visu_bcurve_knots_source_h

// Visualization includes
#include <visu_utils.h>

// Active Data (auxiliary) includes
#include <ActAux_Common.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

// OCCT includes
#include <Geom_BSplineCurve.hxx>
#include <TColgp_HArray1OfPnt.hxx>

//! Data source giving a visual feedback on the distribution of knots
//! in a b-curve.
class visu_bcurve_knots_source : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(visu_bcurve_knots_source, vtkPolyDataAlgorithm);
  static visu_bcurve_knots_source* New();

// Kernel methods:
public:

  bool SetInputCurve(const Handle(Geom_BSplineCurve)& bcurve);

protected:

  virtual int RequestData(vtkInformation*        request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector*  outputVector);

protected:

  vtkIdType
    registerGridPoint(const int    index,
                      vtkPolyData* polyData);

  vtkIdType
    registerVertex(const vtkIdType pid,
                   vtkPolyData*    polyData);

protected:

  visu_bcurve_knots_source();
  ~visu_bcurve_knots_source();

private:

  visu_bcurve_knots_source(const visu_bcurve_knots_source&);
  visu_bcurve_knots_source& operator=(const visu_bcurve_knots_source&);

private:

  Handle(Geom_BSplineCurve) m_curve; //!< B-curve as a data container.

};

#endif
