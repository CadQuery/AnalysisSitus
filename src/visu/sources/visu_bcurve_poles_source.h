//-----------------------------------------------------------------------------
// Created on: 15 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_bcurve_poles_source_h
#define visu_bcurve_poles_source_h

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
#include <Geom2d_BSplineCurve.hxx>
#include <TColgp_HArray1OfPnt.hxx>

//! Data source giving a control polygon of a b-curve.
class visu_bcurve_poles_source : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(visu_bcurve_poles_source, vtkPolyDataAlgorithm);
  static visu_bcurve_poles_source* New();

// Kernel methods:
public:

  bool SetInputCurve   (const Handle(Geom_BSplineCurve)& bcurve);
  bool SetInputCurve2d (const Handle(Geom2d_BSplineCurve)& bcurve);

protected:

  virtual int RequestData(vtkInformation*        request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector*  outputVector);

protected:

  vtkIdType
    registerGridPoint(const int    index,
                      vtkPolyData* polyData);

  vtkIdType
    registerLine(const vtkIdType pid0,
                 const vtkIdType pid1,
                 vtkPolyData*    polyData);

  vtkIdType
    registerVertex(const vtkIdType pid,
                   vtkPolyData*    polyData);

protected:

  visu_bcurve_poles_source();
  ~visu_bcurve_poles_source();

private:

  visu_bcurve_poles_source(const visu_bcurve_poles_source&);
  visu_bcurve_poles_source& operator=(const visu_bcurve_poles_source&);

private:

  Handle(TColgp_HArray1OfPnt) m_poles; //!< Poles to visualize.

};

#endif
