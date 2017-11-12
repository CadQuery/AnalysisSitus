//-----------------------------------------------------------------------------
// Created on: 06 May 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sergey Slyadnev nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
