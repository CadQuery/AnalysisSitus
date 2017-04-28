//-----------------------------------------------------------------------------
// Created on: 28 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_FuncLevelsSource_h
#define asiVisu_FuncLevelsSource_h

// asiVisu includes
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_Function.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

//! Source of polygonal data representing a function levels.
template <typename T_VARIABLE>
class asiVisu_FuncLevelsSource : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_FuncLevelsSource, vtkPolyDataAlgorithm);
  static asiVisu_FuncLevelsSource* New();

// Kernel methods:
public:

  asiVisu_EXPORT void
    SetInputFunction(const Handle(asiAlgo_Function<T_VARIABLE>)& func,
                     const T_VARIABLE&                           minCorner,
                     const T_VARIABLE&                           maxCorner,
                     const int                                   numIntervals = 100);

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

  asiVisu_EXPORT asiVisu_FuncLevelsSource();
  asiVisu_EXPORT ~asiVisu_FuncLevelsSource();

private:

  asiVisu_FuncLevelsSource(const asiVisu_FuncLevelsSource&);
  asiVisu_FuncLevelsSource& operator=(const asiVisu_FuncLevelsSource&);

private:

  Handle(asiAlgo_Function<T_VARIABLE>) m_func;          //!< Function to visualize.
  T_VARIABLE                           m_minCorner;     //!< Min corner of domain hypercube.
  T_VARIABLE                           m_maxCorner;     //!< Max corner of domain hypercube.
  int                                  m_iNumIntervals; //!< Number of intervals.
  double                               m_fFuncMin;      //!< Min function value.
  double                               m_fFuncMax;      //!< Max function value.

};

#endif
