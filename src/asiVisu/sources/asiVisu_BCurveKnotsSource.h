//-----------------------------------------------------------------------------
// Created on: 02 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_BCurveKnotsSource_h
#define asiVisu_BCurveKnotsSource_h

// Visualization includes
#include <asiVisu_Utils.h>

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
class asiVisu_BCurveKnotsSource : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_BCurveKnotsSource, vtkPolyDataAlgorithm);
  static asiVisu_BCurveKnotsSource* New();

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

  asiVisu_BCurveKnotsSource();
  ~asiVisu_BCurveKnotsSource();

private:

  asiVisu_BCurveKnotsSource(const asiVisu_BCurveKnotsSource&);
  asiVisu_BCurveKnotsSource& operator=(const asiVisu_BCurveKnotsSource&);

private:

  Handle(Geom_BSplineCurve) m_curve; //!< B-curve as a data container.

};

#endif
