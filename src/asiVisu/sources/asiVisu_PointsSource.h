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
#include <asiAlgo_BaseCloud.h>

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

  asiVisu_EXPORT void
    SetInputPoints(const Handle(asiAlgo_BaseCloud<REAL_TYPE>)& points);

  asiVisu_EXPORT void
    SetFilter(const Handle(TColStd_HPackedMapOfInteger)& filter);

protected:

  asiVisu_EXPORT virtual int
    RequestData(vtkInformation*        request,
                vtkInformationVector** inputVector,
                vtkInformationVector*  outputVector);

protected:

  asiVisu_EXPORT vtkIdType
    registerGridPoint(const gp_Pnt& point,
                      vtkPolyData*  polyData);

  asiVisu_EXPORT vtkIdType
    registerVertex(const vtkIdType n,
                   vtkPolyData*    polyData);

protected:

  asiVisu_EXPORT asiVisu_PointsSource();
  asiVisu_EXPORT ~asiVisu_PointsSource();

private:

  asiVisu_PointsSource(const asiVisu_PointsSource&);
  asiVisu_PointsSource& operator=(const asiVisu_PointsSource&);

private:

  Handle(asiAlgo_BaseCloud<REAL_TYPE>) m_points;  //!< Points to visualize.
  Handle(TColStd_HPackedMapOfInteger)  m_indices; //!< Point indices to keep (if filter is set).

};

#endif
