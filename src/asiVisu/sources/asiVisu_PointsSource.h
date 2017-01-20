//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_PointsSource_h
#define asiVisu_PointsSource_h

// Visualization includes
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_PointCloud.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

//! Source of polygonal data representing a point cloud.
template <typename REAL_TYPE>
class asiVisu_PointsSource : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_PointsSource, vtkPolyDataAlgorithm);
  static asiVisu_PointsSource* New();

// Kernel methods:
public:

  void SetInputPoints(const Handle(asiAlgo_PointCloud<REAL_TYPE>)& points);

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

  Handle(asiAlgo_PointCloud<REAL_TYPE>) m_points; //!< Points to visualize.

};

#endif
