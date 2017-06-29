//-----------------------------------------------------------------------------
// Created on: 06 May 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef asiVisu_FuncLevelsSourceBase_h
#define asiVisu_FuncLevelsSourceBase_h

// asiVisu includes
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_Function.h>

// VTK includes
#include <vtkCellData.h>
#include <vtkDataObject.h>
#include <vtkLookupTable.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataAlgorithm.h>

//! Source of polygonal data representing a function levels.
template <typename T_VARIABLE>
class asiVisu_FuncLevelsSourceBase : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_FuncLevelsSourceBase, vtkPolyDataAlgorithm);

// Kernel methods:
public:

  //! Sets input function to visualize. This method also accepts the function
  //! domain bounds which are used to limit the sampled are.
  //! \param func         [in] functions to visualize.
  //! \param minCorner    [in] min corner of domain hypercube.
  //! \param maxCorner    [in] max corner of domain hypercube.
  //! \param numIntervals [in] number of sampling intervals.
  void SetInputFunction(const Handle(asiAlgo_Function<T_VARIABLE>)& func,
                        const T_VARIABLE&                           minCorner,
                        const T_VARIABLE&                           maxCorner,
                        const int                                   numIntervals = 100)
  {
    m_func          =  func;
    m_minCorner     =  minCorner;
    m_maxCorner     =  maxCorner;
    m_iNumIntervals =  numIntervals;
    m_fFuncMin      =  RealLast();
    m_fFuncMax      = -RealLast();
    //
    this->Modified();
  }

protected:

  //! Adds the given point to the passed polygonal data set.
  //! \param point    [in]     point to add.
  //! \param polyData [in/out] polygonal data set being populated.
  //! \return ID of the just added VTK point.
  vtkIdType registerGridPoint(const gp_Pnt& point,
                              vtkPolyData*  polyData)
  {
    // Access necessary arrays
    vtkPoints* points = polyData->GetPoints();

    // Push the point into VTK data set
    vtkIdType pid = points->InsertNextPoint( point.X(),
                                             point.Y(),
                                             point.Z() );

    return pid;
  }

  //! Adds a vertex cell into the polygonal data set.
  //! \param n        [in]     index of the corresponding geometric point.
  //! \param polyData [in/out] polygonal data set being populated.
  //! \return ID of the just added VTK cell.
  vtkIdType registerVertex(const vtkIdType n,
                           vtkPolyData*    polyData)
  {
    std::vector<vtkIdType> nodes;
    nodes.push_back(n);
    //
    vtkIdType cellID = polyData->InsertNextCell(VTK_VERTEX, 1, &nodes[0]);
    //
    return cellID;
  }

protected:

  Handle(asiAlgo_Function<T_VARIABLE>) m_func;          //!< Function to visualize.
  T_VARIABLE                           m_minCorner;     //!< Min corner of domain hypercube.
  T_VARIABLE                           m_maxCorner;     //!< Max corner of domain hypercube.
  int                                  m_iNumIntervals; //!< Number of intervals.
  double                               m_fFuncMin;      //!< Min function value.
  double                               m_fFuncMax;      //!< Max function value.

};

#endif
