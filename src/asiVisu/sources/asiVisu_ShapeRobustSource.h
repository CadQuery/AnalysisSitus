//-----------------------------------------------------------------------------
// Created on: 21 March 2017
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

#ifndef asiVisu_ShapeRobustSource_h
#define asiVisu_ShapeRobustSource_h

// asiVisu includes
#include <asiVisu_ShapeData.h>

// asiAlgo includes
#include <asiAlgo_AAG.h>

// Active Data includes
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

// VTK includes
#include <vtkInformationIdTypeKey.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkType.h>
#include <vtkSmartPointer.h>

//! Source of polygonal VTK data representing OCCT shape.
class asiVisu_ShapeRobustSource : public vtkPolyDataAlgorithm
{
public:

  vtkTypeMacro(asiVisu_ShapeRobustSource, vtkPolyDataAlgorithm);

  asiVisu_EXPORT static asiVisu_ShapeRobustSource*
    New();

public:

  asiVisu_EXPORT void
    SetAAG(const Handle(asiAlgo_AAG)& aag);

  asiVisu_EXPORT const Handle(asiAlgo_AAG)&
    GetAAG() const;

  asiVisu_EXPORT void
    SetShape(const TopoDS_Shape& shape);

  asiVisu_EXPORT const TopoDS_Shape&
    GetShape() const;

  asiVisu_EXPORT void
    SetDiagnosticTools(ActAPI_ProgressEntry progress,
                       ActAPI_PlotterEntry  plotter);

  asiVisu_EXPORT void
    SetTessellationParams(const double linDefl,
                          const double angDefl);

protected:

  //! Builds output polygonal data set from the B-Rep shape.
  asiVisu_EXPORT virtual int
    RequestData(vtkInformation*        pInfo,
                vtkInformationVector** pInputVector,
                vtkInformationVector*  pOutputVector);

protected:

  asiVisu_EXPORT
    asiVisu_ShapeRobustSource();

  asiVisu_EXPORT
    ~asiVisu_ShapeRobustSource();

private:

  asiVisu_EXPORT
    asiVisu_ShapeRobustSource(const asiVisu_ShapeRobustSource&);

  asiVisu_EXPORT asiVisu_ShapeRobustSource&
    operator=(const asiVisu_ShapeRobustSource&);

protected:

  Handle(asiAlgo_AAG)       m_aag;               //!< (Optional) AAG of the original B-Rep shape.
  TopoDS_Shape              m_shape;             //!< Original B-Rep shape.
  Handle(asiVisu_ShapeData) m_shapeData;         //!< Polygonal data container.
  double                    m_fLinDeflection;    //!< Linear deflection.
  double                    m_fAngDeflectionDeg; //!< Angular deflection.
  ActAPI_ProgressEntry      m_progress;          //!< Progress notifier.
  ActAPI_PlotterEntry       m_plotter;           //!< Imperative plotter.

};

#endif
