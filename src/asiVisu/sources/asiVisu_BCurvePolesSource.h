//-----------------------------------------------------------------------------
// Created on: 15 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_BCurvePolesSource_h
#define asiVisu_BCurvePolesSource_h

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
#include <Geom2d_BSplineCurve.hxx>
#include <TColgp_HArray1OfPnt.hxx>

//! Data source giving a control polygon of a b-curve.
class asiVisu_BCurvePolesSource : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_BCurvePolesSource, vtkPolyDataAlgorithm);
  static asiVisu_BCurvePolesSource* New();

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

  asiVisu_BCurvePolesSource();
  ~asiVisu_BCurvePolesSource();

private:

  asiVisu_BCurvePolesSource(const asiVisu_BCurvePolesSource&);
  asiVisu_BCurvePolesSource& operator=(const asiVisu_BCurvePolesSource&);

private:

  Handle(TColgp_HArray1OfPnt) m_poles; //!< Poles to visualize.

};

#endif
