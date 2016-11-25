//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_PointsSource_h
#define asiVisu_PointsSource_h

// Visualization includes
#include <asiVisu_Utils.h>

// Qr includes
#include <QrGeom3D_PositionCloud.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

//! Source of polygonal data representing a point cloud.
class asiVisu_PointsSource : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_PointsSource, vtkPolyDataAlgorithm);
  static asiVisu_PointsSource* New();

// Kernel methods:
public:

  void SetInputPoints(const QrPtr<pcloud>& points);

protected:

  virtual int RequestData(vtkInformation*        request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector*  outputVector);

protected:

  vtkIdType
    registerGridPoint(const gp_Pnt& point,
                      vtkPolyData*  polyData);

  vtkIdType
    registerVertex(const vtkIdType n,
                   vtkPolyData*    polyData);

protected:

  asiVisu_PointsSource();
  ~asiVisu_PointsSource();

private:

  asiVisu_PointsSource(const asiVisu_PointsSource&);
  asiVisu_PointsSource& operator=(const asiVisu_PointsSource&);

private:

  QrPtr<pcloud> m_points; //!< Points to visualize.

};

#endif
