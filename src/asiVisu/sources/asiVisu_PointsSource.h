//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_points_source_h
#define visu_points_source_h

// Visualization includes
#include <visu_utils.h>

// Geometry includes
#include <asiAlgo_PointCloud.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

//! Source of polygonal data representing a point cloud.
class visu_points_source : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(visu_points_source, vtkPolyDataAlgorithm);
  static visu_points_source* New();

// Kernel methods:
public:

  void SetInputPoints(const Handle(asiAlgo_PointCloud)& points);

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

  visu_points_source();
  ~visu_points_source();

private:

  visu_points_source(const visu_points_source&);
  visu_points_source& operator=(const visu_points_source&);

private:

  Handle(asiAlgo_PointCloud) m_points; //!< Points to visualize.

};

#endif
